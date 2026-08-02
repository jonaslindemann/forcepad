# Rewrites a compile_commands.json produced by MSVC so that clang tooling
# (clang-uml, clang-tidy, clangd) parses it in MSVC-compatible mode.
#
# clang picks its driver mode from argv[0] of each command, so entries that
# start with cl.exe can end up parsed as if they were GCC-style command lines,
# where /-prefixed flags are meaningless. This replaces the compiler token with
# "clang-cl.exe --driver-mode=cl" - no clang-cl binary needs to exist, the name
# only selects the flag dialect - and drops /showIncludes, which otherwise
# makes the parse print a "Note: including file:" line for every header.
#
# The MSVC and Windows SDK header directories still have to be findable. Run
# this from a Developer Command Prompt, or after vcvars64.bat, so that INCLUDE
# is set; gen-uml.cmd does that for you.
#
# Only needed as a fallback: clang-uml 0.6.1 has its own handling of MSVC
# compilation flags, so try without this first (gen-uml.cmd default), and use
# gen-uml.cmd --rewrite-db if the parse produces flag-related errors.
#
# Idempotent - entries that have already been rewritten are left alone.

param([Parameter(Mandatory = $true)][string]$Path)

if (-not (Test-Path -LiteralPath $Path)) {
    Write-Error "compile_commands.json not found: $Path"
    exit 1
}

$db = @(Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)

$rewritten = 0
foreach ($entry in $db) {
    if (-not $entry.command) { continue }
    if ($entry.command -match '^\s*"?clang-cl') { continue }

    # Strip the leading executable token, quoted ("C:\...\cl.exe" ...) or bare.
    $rest = $null
    if ($entry.command -match '^\s*"([^"]+)"\s+(.*)$') { $rest = $Matches[2] }
    elseif ($entry.command -match '^\s*(\S+)\s+(.*)$') { $rest = $Matches[2] }
    if ($null -eq $rest) { continue }

    $rest = $rest -replace '(?<=^|\s)[/-]showIncludes(?=\s|$)', ''
    $entry.command = 'clang-cl.exe --driver-mode=cl ' + $rest.Trim()
    $rewritten++
}

$json = $db | ConvertTo-Json -Depth 10
# A single-entry database unrolls to a JSON object in Windows PowerShell 5.1;
# the compilation database format requires an array.
if ($json -notmatch '^\s*\[') { $json = "[$json]" }

# No BOM: clang's JSON parser does not accept one.
[System.IO.File]::WriteAllText(
    (Resolve-Path -LiteralPath $Path).Path,
    $json,
    (New-Object System.Text.UTF8Encoding($false)))

Write-Host "[msvc-db-to-clang] rewrote $rewritten of $($db.Count) entries in $Path"
