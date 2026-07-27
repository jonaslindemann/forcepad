@echo off
setlocal EnableDelayedExpansion
rem ===========================================================================
rem  Serve the built ForcePAD wasm app (bin\wasm) over HTTP and open it.
rem  cmd.exe equivalent of scripts\wasm-serve.ps1.
rem
rem  The single-threaded build needs no special headers, so a plain static
rem  server works. (The multi-threaded build will need COOP/COEP headers.)
rem
rem  Usage:  wasm-serve.cmd [port] [nobrowser]
rem            port       TCP port (default 8137)
rem            nobrowser  just serve; do not open a browser
rem  Override the interpreter via env var SERVE_PYTHON (default: python on PATH).
rem ===========================================================================

set "PORT=8137"
set "OPEN=1"

rem Parse up to two positional args in any order (a number = port, "nobrowser").
for %%A in (%1 %2) do (
    if /i "%%~A"=="nobrowser" (
        set "OPEN=0"
    ) else if "%%~A" NEQ "" (
        set "PORT=%%~A"
    )
)

if not defined SERVE_PYTHON set "SERVE_PYTHON=python"

for %%I in ("%~dp0..") do set "REPO=%%~fI"
set "DIR=%REPO%\bin\wasm"
set "URL=http://localhost:%PORT%/ForcePAD.html"

if not exist "%DIR%\ForcePAD.html" (
    echo ERROR: No wasm app in "%DIR%" - run scripts\wasm-build.cmd first.
    exit /b 1
)

echo ==^> Serving "%DIR%" at %URL%  ^(Ctrl+C to stop^)
if "%OPEN%"=="1" (
    rem Open the browser shortly after the server comes up.
    start "" cmd /c "timeout /t 2 >nul & start "" "%URL%""
)

pushd "%DIR%"
"%SERVE_PYTHON%" -m http.server %PORT%
popd
endlocal
