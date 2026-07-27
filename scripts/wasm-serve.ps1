<#
.SYNOPSIS
    Serve the built ForcePAD wasm app (bin/wasm) over HTTP and open it.

.DESCRIPTION
    The single-threaded build needs no special headers, so a plain static
    server works. (The multi-threaded build will need COOP/COEP cross-origin
    isolation headers - a different server; add when that phase lands.)

.PARAMETER Port
    TCP port to serve on (default 8137).

.PARAMETER NoBrowser
    Just serve; do not open a browser.
#>
param(
    [int]$Port = 8137,
    [string]$Python = 'C:\Python312\python.exe',
    [switch]$NoBrowser
)

$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$dir  = Join-Path $repo 'bin\wasm'
if (-not (Test-Path (Join-Path $dir 'ForcePAD.html'))) {
    throw "No wasm app in $dir - run scripts/wasm-build.ps1 first."
}

$url = "http://localhost:$Port/ForcePAD.html"
Write-Host "==> Serving $dir at $url  (Ctrl+C to stop)" -ForegroundColor Cyan
if (-not $NoBrowser) {
    Start-Job { param($u) Start-Sleep 2; Start-Process $u } -ArgumentList $url | Out-Null
}
Push-Location $dir
try   { & $Python -m http.server $Port }
finally { Pop-Location }
