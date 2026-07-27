<#
.SYNOPSIS
    Configure + build the ForcePAD Qt-for-WebAssembly (single-threaded) target.

.DESCRIPTION
    Wraps the emsdk env + Qt wasm qt-cmake so a wasm build is one command.
    Output (html/js/wasm/data + loader) lands in bin/wasm/, ready to serve with
    scripts/wasm-serve.ps1.

    Prerequisites (see docs/wasm-setup.md):
      * Qt 6.9.x WebAssembly (single-threaded) kit installed
      * emsdk with Emscripten 3.1.70 activated  (Qt 6.9 pins 3.1.70)

.PARAMETER Config
    CMake build type. Ninja is single-config, so each one gets its own build
    tree - but they all write the app to bin/wasm, so the last build wins there.
    Rebuild MinSizeRel before deploying, or bin/wasm still holds a Debug app.

    Debug is ~66 MB (-g) and instantiates in a few seconds locally; MinSizeRel
    (-Os) is ~15 MB and is what the site should serve.

.PARAMETER Clean
    Delete the build tree for this config first, for a fresh configure.
#>
param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$Config  = 'Debug',
    [string]$QtWasm  = 'E:\Qt\6.9.3\wasm_singlethread',
    [string]$Emsdk   = 'E:\Users\Jonas\Development\emsdk',
    [string]$Python  = 'C:\Python312\python.exe',
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
$repo    = Split-Path -Parent $PSScriptRoot

# Existing trees keep their established names (build-wasm is Debug,
# build-wasm-rel is MinSizeRel) so switching to this parameter does not throw
# away a configured tree and force a full rebuild.
$build = switch ($Config) {
    'Debug'      { Join-Path $repo 'build-wasm' }
    'MinSizeRel' { Join-Path $repo 'build-wasm-rel' }
    default      { Join-Path $repo "build-wasm-$($Config.ToLower())" }
}
$qtcmake = Join-Path $QtWasm 'bin\qt-cmake.bat'

if (-not (Test-Path $qtcmake)) { throw "qt-cmake not found at $qtcmake - is the Qt WebAssembly kit installed?" }

# emsdk env (sets EMSDK + PATH for emcc/clang/node). emsdk's own bundled Python
# is fine now, but EMSDK_PYTHON lets an older emsdk fall back to a modern Python.
. (Join-Path $Emsdk 'emsdk_env.ps1') | Out-Null
if (Test-Path $Python) { $env:EMSDK_PYTHON = $Python }

if ($Clean -and (Test-Path $build)) { Remove-Item -Recurse -Force $build }

if (-not (Test-Path (Join-Path $build 'CMakeCache.txt'))) {
    Write-Host "==> Configuring wasm build, $Config ($QtWasm)" -ForegroundColor Cyan
    & $qtcmake -G Ninja -S $repo -B $build -DCMAKE_BUILD_TYPE=$Config
    if ($LASTEXITCODE -ne 0) { throw "configure failed" }
}

Write-Host "==> Building qtforcepad (wasm, $Config)" -ForegroundColor Cyan
cmake --build $build
if ($LASTEXITCODE -ne 0) { throw "build failed" }

$wasm = Join-Path $repo 'bin\wasm\ForcePAD.wasm'
$size = if (Test-Path $wasm) { " ($([math]::Round((Get-Item $wasm).Length / 1MB, 1)) MB)" } else { "" }
Write-Host "==> Done. $Config app in $(Join-Path $repo 'bin\wasm')$size  (serve with scripts/wasm-serve.ps1)" -ForegroundColor Green
