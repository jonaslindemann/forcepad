@echo off
setlocal EnableDelayedExpansion
rem ===========================================================================
rem  Publish the built wasm app (bin\wasm) into the documentation site
rem  (docs\docs\app). cmd.exe equivalent of scripts\wasm-deploy.ps1.
rem
rem  The site serves docs\docs\app\index.html at /forcepad/app/. That page is
rem  the shell Qt generates next to the build, and it names the payload it
rem  loads ("ForcePAD.js", window.ForcePAD). Shell and payload must therefore
rem  be copied as one set - a half-copy leaves the published page asking for
rem  files that are no longer there, which is how a deploy breaks. This script
rem  copies the whole set and writes index.html from the generated shell, so
rem  the two cannot drift apart.
rem
rem  The previous release's files (qtforcepad.js/.wasm/.data, from before the
rem  ForcePAD rename) are left in place unless "prunelegacy" is given: they are
rem  already committed, so keeping them costs nothing. (Git history is the real
rem  backup: git checkout <sha> -- docs/docs/app.)
rem
rem  Every deploy also publishes bin\release\samples\*.fp2 as
rem  docs\docs\app\models\, so ?model=models/<name> links work straight away.
rem
rem  Usage:  wasm-deploy.cmd [force] [prunelegacy] [nomodels] [model.fp2 ...]
rem            force        deploy even if the wasm looks like a Debug build
rem            prunelegacy  delete the superseded qtforcepad.* payload
rem            nomodels     publish only the app, no models
rem            model.fp2    an extra model to publish alongside the samples
rem  Override defaults via env vars DEPLOY_SOURCE / DEPLOY_DEST /
rem  DEPLOY_MODELDIR.
rem
rem  Example:  wasm-deploy.cmd tests\beam_example.fp2
rem ===========================================================================

rem Repo root = parent of this script's directory
for %%I in ("%~dp0..") do set "REPO=%%~fI"
if not defined DEPLOY_SOURCE set "DEPLOY_SOURCE=%REPO%\bin\wasm"
if not defined DEPLOY_DEST   set "DEPLOY_DEST=%REPO%\docs\docs\app"

if not defined DEPLOY_MODELDIR set "DEPLOY_MODELDIR=%REPO%\bin\release\samples"

set "FORCE=0"
set "PRUNE=0"
set "NOMODELS=0"
set "MODELS="

:parse
if "%~1"=="" goto parsed
if /i "%~1"=="force" (
    set "FORCE=1"
) else if /i "%~1"=="prunelegacy" (
    set "PRUNE=1"
) else if /i "%~1"=="nomodels" (
    set "NOMODELS=1"
) else (
    set MODELS=!MODELS! "%~1"
)
shift
goto parse
:parsed

set "PAYLOAD=ForcePAD.html ForcePAD.js ForcePAD.wasm ForcePAD.data qtloader.js qtlogo.svg"

rem "exit /b" from inside a for loop terminates the script without setting the
rem exit code, so every loop below flags the failure and exits after the loop.
set "FAILED="
for %%F in (%PAYLOAD%) do (
    if not exist "%DEPLOY_SOURCE%\%%F" (
        echo ERROR: %%F missing from "%DEPLOY_SOURCE%" - run scripts\wasm-build.cmd first.
        set "FAILED=1"
    )
)
if defined FAILED exit /b 1

rem A Debug wasm is ~66 MB against ~15 MB for Release; publishing one by
rem accident is a slow download for every visitor and a large commit.
for %%I in ("%DEPLOY_SOURCE%\ForcePAD.wasm") do set "WASMSIZE=%%~zI"
set /a WASMMB=%WASMSIZE% / 1048576
if %WASMMB% GTR 30 if "%FORCE%"=="0" (
    echo ERROR: ForcePAD.wasm is %WASMMB% MB - that is a Debug build.
    echo        Rebuild as Release, or pass "force".
    exit /b 1
)

if not exist "%DEPLOY_DEST%" mkdir "%DEPLOY_DEST%"
for %%F in (%PAYLOAD%) do (
    copy /y "%DEPLOY_SOURCE%\%%F" "%DEPLOY_DEST%\%%F" >nul
    if errorlevel 1 (
        echo ERROR: failed to copy %%F
        set "FAILED=1"
    )
)
if defined FAILED exit /b 1

rem The site's entry point. Copied from the generated shell rather than edited
rem by hand so it always references the payload that just shipped.
copy /y "%DEPLOY_SOURCE%\ForcePAD.html" "%DEPLOY_DEST%\index.html" >nul
if errorlevel 1 ( echo ERROR: failed to write index.html & exit /b 1 )

rem Sample models ship with every deploy so ?model=models/<name> links work
rem straight away. The .fp2 format embeds its image data, so a model is one
rem self-contained file - the .rgb files next to the samples are not needed.
if "%NOMODELS%"=="0" (
    if not exist "%DEPLOY_MODELDIR%" (
        echo WARNING: model directory not found, publishing app only: "%DEPLOY_MODELDIR%"
    ) else (
        if not exist "%DEPLOY_DEST%\models" mkdir "%DEPLOY_DEST%\models"
        for %%M in ("%DEPLOY_MODELDIR%\*.fp2") do (
            copy /y "%%~M" "%DEPLOY_DEST%\models\%%~nxM" >nul
            if errorlevel 1 (
                echo ERROR: failed to copy %%~nxM
                set "FAILED=1"
            ) else (
                echo     model: %%~nxM  -^> ?model=models/%%~nxM
            )
        )
    )
)

if defined MODELS (
    if not exist "%DEPLOY_DEST%\models" mkdir "%DEPLOY_DEST%\models"
    for %%M in (!MODELS!) do (
        if not exist "%%~M" (
            echo ERROR: model not found: %%~M
            set "FAILED=1"
        ) else (
            copy /y "%%~M" "%DEPLOY_DEST%\models\%%~nxM" >nul
            if errorlevel 1 (
                echo ERROR: failed to copy %%~nxM
                set "FAILED=1"
            ) else (
                echo     model: %%~nxM  -^> ?model=models/%%~nxM
            )
        )
    )
)
if defined FAILED exit /b 1

if "%PRUNE%"=="1" (
    for %%F in (qtforcepad.js qtforcepad.wasm qtforcepad.data qtforcepad.html) do (
        if exist "%DEPLOY_DEST%\%%F" (
            del /q "%DEPLOY_DEST%\%%F"
            echo     removed: %%F
        )
    )
)

echo ==^> Deployed %WASMMB% MB wasm to "%DEPLOY_DEST%"
echo     Review with 'git status docs/docs/app' before committing.
endlocal
