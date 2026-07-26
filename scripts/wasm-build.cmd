@echo off
setlocal EnableDelayedExpansion
rem ===========================================================================
rem  Configure + build the ForcePAD Qt-for-WebAssembly (single-threaded) target.
rem  cmd.exe equivalent of scripts\wasm-build.ps1.
rem
rem  Output (html/js/wasm/data + loader) lands in bin\wasm\, servable with
rem  scripts\wasm-serve.cmd.
rem
rem  Prerequisites (see docs\wasm-setup.md):
rem    * Qt 6.9.x WebAssembly (single-threaded) kit installed
rem    * emsdk with Emscripten 3.1.70 activated  (Qt 6.9 pins 3.1.70)
rem
rem  Usage:  wasm-build.cmd [clean]
rem  Override defaults via env vars QT_WASM / EMSDK_DIR / EMSDK_PYTHON before
rem  calling.
rem ===========================================================================

if not defined QT_WASM      set "QT_WASM=E:\Qt\6.9.3\wasm_singlethread"
if not defined EMSDK_DIR    set "EMSDK_DIR=E:\Users\Jonas\Development\emsdk"
if not defined EMSDK_PYTHON set "EMSDK_PYTHON=C:\Python312\python.exe"

rem Repo root = parent of this script's directory
for %%I in ("%~dp0..") do set "REPO=%%~fI"
set "BUILD=%REPO%\build-wasm"
set "QTCMAKE=%QT_WASM%\bin\qt-cmake.bat"

if not exist "%QTCMAKE%" (
    echo ERROR: qt-cmake not found at "%QTCMAKE%"
    echo        Is the Qt WebAssembly kit installed? See docs\wasm-setup.md
    exit /b 1
)

rem Set up the Emscripten environment (EMSDK + PATH for emcc/clang/node).
call "%EMSDK_DIR%\emsdk_env.bat" >nul
if errorlevel 1 ( echo ERROR: emsdk_env.bat failed & exit /b 1 )
if not exist "%EMSDK_PYTHON%" set "EMSDK_PYTHON="

if /i "%~1"=="clean" (
    if exist "%BUILD%" (
        echo ==^> Removing "%BUILD%"
        rmdir /s /q "%BUILD%"
    )
)

if not exist "%BUILD%\CMakeCache.txt" (
    echo ==^> Configuring wasm build ^(%QT_WASM%^)
    call "%QTCMAKE%" -G Ninja -S "%REPO%" -B "%BUILD%"
    if errorlevel 1 ( echo ERROR: configure failed & exit /b 1 )
)

echo ==^> Building qtforcepad ^(wasm^)
cmake --build "%BUILD%"
if errorlevel 1 ( echo ERROR: build failed & exit /b 1 )

echo ==^> Done. App in "%REPO%\bin\wasm"  ^(serve with scripts\wasm-serve.cmd^)
endlocal
