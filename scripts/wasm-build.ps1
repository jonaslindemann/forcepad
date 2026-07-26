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

.PARAMETER Clean
    Delete build-wasm first for a fresh configure.
#>
param(
    [string]$QtWasm  = 'E:\Qt\6.9.3\wasm_singlethread',
    [string]$Emsdk   = 'E:\Users\Jonas\Development\emsdk',
    [string]$Python  = 'C:\Python312\python.exe',
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
$repo    = Split-Path -Parent $PSScriptRoot
$build   = Join-Path $repo 'build-wasm'
$qtcmake = Join-Path $QtWasm 'bin\qt-cmake.bat'

if (-not (Test-Path $qtcmake)) { throw "qt-cmake not found at $qtcmake - is the Qt WebAssembly kit installed?" }

# emsdk env (sets EMSDK + PATH for emcc/clang/node). emsdk's own bundled Python
# is fine now, but EMSDK_PYTHON lets an older emsdk fall back to a modern Python.
. (Join-Path $Emsdk 'emsdk_env.ps1') | Out-Null
if (Test-Path $Python) { $env:EMSDK_PYTHON = $Python }

if ($Clean -and (Test-Path $build)) { Remove-Item -Recurse -Force $build }

if (-not (Test-Path (Join-Path $build 'CMakeCache.txt'))) {
    Write-Host "==> Configuring wasm build ($QtWasm)" -ForegroundColor Cyan
    & $qtcmake -G Ninja -S $repo -B $build
    if ($LASTEXITCODE -ne 0) { throw "configure failed" }
}

Write-Host "==> Building qtforcepad (wasm)" -ForegroundColor Cyan
cmake --build $build
if ($LASTEXITCODE -ne 0) { throw "build failed" }

Write-Host "==> Done. App in $(Join-Path $repo 'bin\wasm')  (serve with scripts/wasm-serve.ps1)" -ForegroundColor Green
