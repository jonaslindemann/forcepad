@echo off
rem ===========================================================================
rem  gen-uml.cmd - generate ForcePAD UML class diagrams with clang-uml and
rem                render them to images with PlantUML.
rem
rem  Three steps:
rem    1. Configure build-uml\ (Ninja, configure only, nothing is compiled) to
rem       get a compile_commands.json. The Visual Studio generator used for the
rem       normal build cannot produce one, and clang-uml needs it.
rem    2. Run clang-uml over the diagrams defined in .clang-uml -> docs\uml\*.puml
rem    3. Run PlantUML over those -> docs\uml\*.svg
rem
rem  The database is written by MSVC (cl.exe) by default, since that is the
rem  compiler this project already builds with and clang-uml 0.6.1 handles MSVC
rem  compilation flags. If the parse fails with flag-dialect errors, there are
rem  two fallbacks:
rem    --rewrite-db   post-process the database into clang-cl form
rem                   (scripts\msvc-db-to-clang.ps1; installs nothing)
rem    --clang-cl     configure with clang-cl instead, which needs the Visual
rem                   Studio component "C++ Clang tools for Windows" or LLVM
rem                   from https://releases.llvm.org
rem
rem  Requirements:
rem    * Visual Studio with the C++ workload (cl.exe) and "C++ CMake tools for
rem      Windows" (supplies ninja)
rem    * clang-uml       https://github.com/bkryza/clang-uml/releases
rem                      on PATH, or point CLANG_UML at the exe
rem    * PlantUML for step 3 only. The Microsoft Store package "WinPlantUml"
rem      (50760EliasAE.PlantUml) is detected automatically and is enough on its
rem      own - it bundles plantuml.jar, a private JRE and its own Graphviz.
rem      Otherwise: PLANTUML_JAR, or --fetch-plantuml, or --no-render.
rem ===========================================================================

setlocal EnableExtensions EnableDelayedExpansion

for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "BUILD_DIR=%ROOT%\build-uml"
set "OUT_DIR=%ROOT%\docs\uml"
set "IMG_FORMAT=svg"
set "DO_RENDER=1"
set "FRESH=0"
set "DIAGRAM="
set "VERBOSE=0"
set "FETCH=0"
set "LIST_ONLY=0"
set "USE_CLANGCL=0"
set "REWRITE_DB=0"

:parse_args
if "%~1"=="" goto args_done
if /I "%~1"=="--help"            goto usage
if /I "%~1"=="-h"                goto usage
if /I "%~1"=="--no-render"       ( set "DO_RENDER=0"  & shift & goto parse_args )
if /I "%~1"=="--fresh"           ( set "FRESH=1"      & shift & goto parse_args )
if /I "%~1"=="--verbose"         ( set "VERBOSE=1"    & shift & goto parse_args )
if /I "%~1"=="--fetch-plantuml"  ( set "FETCH=1"      & shift & goto parse_args )
if /I "%~1"=="--list"            ( set "LIST_ONLY=1"  & shift & goto parse_args )
if /I "%~1"=="--clang-cl"        ( set "USE_CLANGCL=1" & shift & goto parse_args )
if /I "%~1"=="--rewrite-db"      ( set "REWRITE_DB=1" & shift & goto parse_args )
if /I "%~1"=="--png"             ( set "IMG_FORMAT=png" & shift & goto parse_args )
if /I "%~1"=="--diagram"         ( set "DIAGRAM=%~2"  & shift & shift & goto parse_args )
echo [gen-uml] Unknown option: %~1
echo.
goto usage

:args_done

rem Step 1 is skipped when a database already exists, so switching compilers has
rem to force a reconfigure - otherwise --clang-cl would silently reuse the
rem database cl.exe wrote (and vice versa).
if "%USE_CLANGCL%"=="1" set "FRESH=1"

pushd "%ROOT%" || exit /b 1

rem --- locate clang-uml ------------------------------------------------------
if not defined CLANG_UML call :which CLANG_UML clang-uml.exe
if not defined CLANG_UML (
    for %%D in ("%ProgramFiles%\clang-uml\bin" "%LOCALAPPDATA%\Programs\clang-uml\bin" "%ROOT%\tools\clang-uml\bin") do (
        if not defined CLANG_UML if exist "%%~D\clang-uml.exe" set "CLANG_UML=%%~D\clang-uml.exe"
    )
)
if not defined CLANG_UML (
    echo [gen-uml] ERROR: clang-uml not found.
    echo           Install a Windows build from
    echo             https://github.com/bkryza/clang-uml/releases
    echo           then put it on PATH or run:  set CLANG_UML=C:\path\to\clang-uml.exe
    goto fail
)

if "%LIST_ONLY%"=="1" (
    "%CLANG_UML%" --config .clang-uml --list-diagrams
    goto done
)

rem --- MSVC environment (headers/libs for clang-cl, plus VS's cmake/ninja) ---
set "PF86=%ProgramFiles(x86)%"
if not defined PF86 set "PF86=%ProgramFiles%"
set "VSWHERE=%PF86%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSDIR="
if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VSDIR=%%I"
)
if not defined VCINSTALLDIR if defined VSDIR (
    if exist "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" (
        echo [gen-uml] Setting up MSVC environment from "%VSDIR%"
        call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul
    )
)

rem --- locate cmake / ninja / clang-cl --------------------------------------
call :which CMAKE cmake.exe
if not defined CMAKE (
    echo [gen-uml] ERROR: cmake not found on PATH.
    goto fail
)

call :which NINJA ninja.exe
if not defined NINJA if defined VSDIR (
    set "_vsninja=%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
    if exist "!_vsninja!" set "NINJA=!_vsninja!"
)
if not defined NINJA (
    echo [gen-uml] ERROR: ninja not found. Install the Visual Studio component
    echo           "C++ CMake tools for Windows", or put ninja.exe on PATH.
    goto fail
)

rem clang-cl is only needed for --clang-cl; the default uses cl.exe from the
rem MSVC environment set up above.
if "%USE_CLANGCL%"=="1" (
    call :which CLANGCL clang-cl.exe
    if not defined CLANGCL (
        for %%D in ("%ProgramFiles%\LLVM\bin" "%VSDIR%\VC\Tools\Llvm\x64\bin" "%VSDIR%\VC\Tools\Llvm\bin") do (
            if not defined CLANGCL if exist "%%~D\clang-cl.exe" set "CLANGCL=%%~D\clang-cl.exe"
        )
    )
    if not defined CLANGCL (
        echo [gen-uml] ERROR: --clang-cl given but clang-cl was not found.
        echo           Install the Visual Studio component "C++ Clang tools for
        echo           Windows", or LLVM from https://releases.llvm.org - or drop
        echo           the flag to use cl.exe, optionally with --rewrite-db.
        goto fail
    )
)

rem --- step 1: compile_commands.json ---------------------------------------
if "%FRESH%"=="1" if exist "%BUILD_DIR%" (
    echo [gen-uml] Removing "%BUILD_DIR%"
    rmdir /s /q "%BUILD_DIR%"
)

if not exist "%BUILD_DIR%\compile_commands.json" (
    rem Reuse the normal build's vcpkg packages when they are there, so this
    rem tree does not install a second copy of Qt/Eigen/spdlog.
    set "VCPKG_ARGS="
    for %%V in (build-debug build-release) do (
        if not defined VCPKG_ARGS if exist "%ROOT%\%%V\vcpkg_installed\x64-windows\include" (
            set "_vi=%ROOT:\=/%/%%V/vcpkg_installed"
            set "VCPKG_ARGS=-DVCPKG_INSTALLED_DIR=!_vi! -DVCPKG_MANIFEST_INSTALL=OFF"
            echo [gen-uml] Reusing vcpkg packages from %%V\vcpkg_installed
        )
    )
    if not defined VCPKG_ARGS echo [gen-uml] No existing vcpkg_installed found - vcpkg will install into build-uml ^(slow, once^).

    set "NINJA_FWD=%NINJA:\=/%"
    set "COMPILER_ARGS="
    set "COMPILER_LABEL=cl.exe"
    if "%USE_CLANGCL%"=="1" (
        set "CLANGCL_FWD=!CLANGCL:\=/!"
        set "COMPILER_ARGS=-DCMAKE_C_COMPILER=!CLANGCL_FWD! -DCMAKE_CXX_COMPILER=!CLANGCL_FWD!"
        set "COMPILER_LABEL=clang-cl"
    )

    echo [gen-uml] Configuring "%BUILD_DIR%" ^(Ninja + !COMPILER_LABEL!, configure only^)...
    "%CMAKE%" -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja ^
        -DCMAKE_BUILD_TYPE=Debug ^
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
        -DCMAKE_MAKE_PROGRAM="!NINJA_FWD!" ^
        -DBUILD_FLTK_APP=OFF ^
        !COMPILER_ARGS! !VCPKG_ARGS!
    if errorlevel 1 (
        echo [gen-uml] ERROR: CMake configure failed.
        goto fail
    )
)

if not exist "%BUILD_DIR%\compile_commands.json" (
    echo [gen-uml] ERROR: "%BUILD_DIR%\compile_commands.json" was not created.
    goto fail
)

if "%REWRITE_DB%"=="1" (
    echo [gen-uml] Rewriting the compilation database into clang-cl form...
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0msvc-db-to-clang.ps1" ^
        -Path "%BUILD_DIR%\compile_commands.json"
    if errorlevel 1 (
        echo [gen-uml] ERROR: rewriting "%BUILD_DIR%\compile_commands.json" failed.
        goto fail
    )
)

rem --- step 2: clang-uml ---------------------------------------------------
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

set "CU_ARGS=--config .clang-uml"
if defined DIAGRAM set "CU_ARGS=!CU_ARGS! -n %DIAGRAM%"
if "%VERBOSE%"=="1" set "CU_ARGS=!CU_ARGS! -V"

echo [gen-uml] Running: "%CLANG_UML%" !CU_ARGS!
"%CLANG_UML%" !CU_ARGS!
if errorlevel 1 (
    echo [gen-uml] ERROR: clang-uml failed. Re-run with --verbose for details.
    goto fail
)

if "%DO_RENDER%"=="0" (
    echo [gen-uml] Diagram sources written to "%OUT_DIR%" ^(rendering skipped^).
    goto done
)

rem --- step 3: PlantUML ----------------------------------------------------
if "%FETCH%"=="1" (
    if not exist "%ROOT%\tools" mkdir "%ROOT%\tools"
    echo [gen-uml] Downloading plantuml.jar from github.com/plantuml/plantuml ...
    curl -fL -o "%ROOT%\tools\plantuml.jar" ^
        https://github.com/plantuml/plantuml/releases/latest/download/plantuml.jar
    if errorlevel 1 (
        echo [gen-uml] ERROR: download failed.
        goto fail
    )
    set "PLANTUML_JAR=%ROOT%\tools\plantuml.jar"
)

set "PLANTUML_CMD="
if not defined PLANTUML_JAR (
    for %%F in ("%ROOT%\tools\plantuml.jar" "%~dp0plantuml.jar" "%LOCALAPPDATA%\plantuml\plantuml.jar") do (
        if not defined PLANTUML_JAR if exist "%%~F" set "PLANTUML_JAR=%%~F"
    )
)
rem A launcher on PATH (chocolatey/scoop shims); "where" honours PATHEXT, so
rem this picks up plantuml.exe, .cmd and .bat alike.
if not defined PLANTUML_JAR call :which PLANTUML_CMD plantuml
if not defined PLANTUML_JAR if not defined PLANTUML_CMD call :find_msix_plantuml

if not defined PLANTUML_JAR if not defined PLANTUML_CMD (
    echo [gen-uml] Diagram sources written to "%OUT_DIR%", but PlantUML was not found.
    echo           Options:  scripts\gen-uml.cmd --fetch-plantuml
    echo                     set PLANTUML_JAR=C:\path\to\plantuml.jar
    echo                     https://plantuml.com/download
    goto done
)

if not defined PLANTUML_CMD (
    call :which JAVA java.exe
    if not defined JAVA if defined JAVA_HOME if exist "%JAVA_HOME%\bin\java.exe" set "JAVA=%JAVA_HOME%\bin\java.exe"
    if not defined JAVA (
        echo [gen-uml] Diagram sources written to "%OUT_DIR%", but java was not found
        echo           to run "%PLANTUML_JAR%". Install a JRE/JDK, or set JAVA_HOME.
        goto done
    )
)

rem PlantUML shells out to Graphviz for class diagrams.
if not defined GRAPHVIZ_DOT (
    call :which GRAPHVIZ_DOT dot.exe
    if not defined GRAPHVIZ_DOT (
        echo [gen-uml] WARNING: Graphviz "dot" not found - PlantUML cannot lay out
        echo           class diagrams without it. See https://graphviz.org/download/
    )
)

set "RENDERED=0"
for /f "delims=" %%F in ('dir /b /a-d "%OUT_DIR%\*.puml" "%OUT_DIR%\*.plantuml" 2^>nul') do (
    echo [gen-uml] Rendering %%F -^> %%~nF.%IMG_FORMAT%
    if defined PLANTUML_CMD (
        call "%PLANTUML_CMD%" -t%IMG_FORMAT% -o "%OUT_DIR%" "%OUT_DIR%\%%F"
    ) else (
        "%JAVA%" -Xmx2g -DPLANTUML_LIMIT_SIZE=16384 -jar "%PLANTUML_JAR%" ^
            -t%IMG_FORMAT% -o "%OUT_DIR%" "%OUT_DIR%\%%F"
    )
    if errorlevel 1 (
        echo [gen-uml] WARNING: PlantUML failed on %%F
    ) else (
        set /a RENDERED+=1
    )
)

if "%RENDERED%"=="0" (
    echo [gen-uml] No diagram sources found in "%OUT_DIR%".
    goto fail
)

echo [gen-uml] Done - %RENDERED% diagram^(s^) in "%OUT_DIR%".
goto done

rem --- helpers -------------------------------------------------------------
:which
rem %1 = variable to set, %2 = executable to look for on PATH
for /f "delims=" %%I in ('where %2 2^>nul') do (
    if not defined %1 set "%1=%%I"
)
goto :eof

:find_msix_plantuml
rem The Microsoft Store package "WinPlantUml" (50760EliasAE.PlantUml) ships
rem Java\plantuml.jar together with a private JRE and its own Graphviz build,
rem so it needs no separate Java installation.
rem Its install path contains the package version and would change on every
rem update, and C:\Program Files\WindowsApps cannot be enumerated by a normal
rem user (a known full path inside it can still be read), so ask the package
rem manager where it is rather than globbing for it.
set "_msix="
for /f "usebackq delims=" %%I in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "(Get-AppxPackage -Name 50760EliasAE.PlantUml).InstallLocation" 2^>nul`) do set "_msix=%%I"
if not defined _msix goto :eof
if not exist "%_msix%\Java\plantuml.jar" goto :eof
set "PLANTUML_JAR=%_msix%\Java\plantuml.jar"
if exist "%_msix%\Java\jre\x64\bin\java.exe" set "JAVA=%_msix%\Java\jre\x64\bin\java.exe"
if not defined GRAPHVIZ_DOT if exist "%_msix%\Graphviz\bin\dot.exe" set "GRAPHVIZ_DOT=%_msix%\Graphviz\bin\dot.exe"
echo [gen-uml] Using PlantUML from the Microsoft Store package (WinPlantUml).
goto :eof

:usage
echo Usage: scripts\gen-uml.cmd [options]
echo.
echo   Generates the class diagrams defined in .clang-uml into docs\uml\ and
echo   renders them with PlantUML.
echo.
echo   --diagram NAME     generate only one diagram (see --list)
echo   --list             list the diagram names defined in .clang-uml
echo   --png              render PNG instead of SVG
echo   --no-render        stop after writing the .puml sources
echo   --fresh            delete and reconfigure build-uml\ first
echo   --rewrite-db       rewrite the MSVC compilation database into clang-cl
echo                      form; try this if clang-uml reports flag errors
echo   --clang-cl         configure with clang-cl instead of cl.exe; needs
echo                      clang-cl installed, and implies --fresh because the
echo                      existing database came from a different compiler
echo   --fetch-plantuml   download plantuml.jar into tools\ (needs network)
echo   --verbose          pass -V to clang-uml
echo   --help
goto done_no_pop

:fail
popd
endlocal
exit /b 1

:done
popd
:done_no_pop
endlocal
exit /b 0
