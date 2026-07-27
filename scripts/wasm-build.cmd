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
rem  Usage:  wasm-build.cmd [clean] [debug|release|relwithdebinfo|minsizerel]
rem            (default: debug)
rem
rem  Ninja is single-config, so each build type gets its own tree - but they all
rem  write the app to bin\wasm, so the last build wins there. Rebuild minsizerel
rem  before deploying, or bin\wasm still holds a Debug app. Debug is ~66 MB (-g);
rem  minsizerel (-Os) is ~15 MB and is what the site should serve.
rem
rem  Override defaults via env vars QT_WASM / EMSDK_DIR / EMSDK_PYTHON before
rem  calling.
rem ===========================================================================

if not defined QT_WASM      set "QT_WASM=E:\Qt\6.9.3\wasm_singlethread"
if not defined EMSDK_DIR    set "EMSDK_DIR=E:\Users\Jonas\Development\emsdk"
if not defined EMSDK_PYTHON set "EMSDK_PYTHON=C:\Python312\python.exe"

set "CLEAN=0"
set "CONFIG=Debug"

rem Parse up to two positional args in any order ("clean" and a build type).
for %%A in (%1 %2) do (
    if /i "%%~A"=="clean" (
        set "CLEAN=1"
    ) else if /i "%%~A"=="debug" (
        set "CONFIG=Debug"
    ) else if /i "%%~A"=="release" (
        set "CONFIG=Release"
    ) else if /i "%%~A"=="relwithdebinfo" (
        set "CONFIG=RelWithDebInfo"
    ) else if /i "%%~A"=="minsizerel" (
        set "CONFIG=MinSizeRel"
    ) else if "%%~A" NEQ "" (
        echo ERROR: unknown argument "%%~A"
        echo        Usage: wasm-build.cmd [clean] [debug^|release^|relwithdebinfo^|minsizerel]
        exit /b 1
    )
)

rem Repo root = parent of this script's directory
for %%I in ("%~dp0..") do set "REPO=%%~fI"

rem Existing trees keep their established names (build-wasm is Debug,
rem build-wasm-rel is MinSizeRel) so switching to this argument does not throw
rem away a configured tree and force a full rebuild.
if /i "%CONFIG%"=="Debug" (
    set "BUILD=%REPO%\build-wasm"
) else if /i "%CONFIG%"=="MinSizeRel" (
    set "BUILD=%REPO%\build-wasm-rel"
) else (
    set "BUILD=%REPO%\build-wasm-%CONFIG%"
)
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

if "%CLEAN%"=="1" (
    if exist "%BUILD%" (
        echo ==^> Removing "%BUILD%"
        rmdir /s /q "%BUILD%"
    )
)

if not exist "%BUILD%\CMakeCache.txt" (
    echo ==^> Configuring wasm build, %CONFIG% ^(%QT_WASM%^)
    call "%QTCMAKE%" -G Ninja -S "%REPO%" -B "%BUILD%" -DCMAKE_BUILD_TYPE=%CONFIG%
    if errorlevel 1 ( echo ERROR: configure failed & exit /b 1 )
)

echo ==^> Building qtforcepad ^(wasm, %CONFIG%^)
cmake --build "%BUILD%"
if errorlevel 1 ( echo ERROR: build failed & exit /b 1 )

set "WASMMB="
for %%I in ("%REPO%\bin\wasm\ForcePAD.wasm") do set /a WASMMB=%%~zI / 1048576
echo ==^> Done. %CONFIG% app in "%REPO%\bin\wasm" ^(%WASMMB% MB^)  ^(serve with scripts\wasm-serve.cmd^)
endlocal
