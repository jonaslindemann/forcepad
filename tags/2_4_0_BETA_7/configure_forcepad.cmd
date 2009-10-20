@echo off

set BUILD_TYPE=%1
set FLTK_ROOT=c:\users\jonas\libraries\fltk-1.1.9
set GLEW_ROOT=c:\users\jonas\libraries\glew

if %BUILD_TYPE%==Debug (
	set BUILD_DIR=win32_build_debug
) else (
	set BUILD_DIR=win32_build_release
)	

cmake --version 1>NUL 2>NUL
if errorlevel 1 goto needcmake

if exist %BUILD_DIR% (
	rmdir /Q /S %BUILD_DIR%
)

mkdir %BUILD_DIR%
pushd %BUILD_DIR%
cmake -DCMAKE_BUILD_TYPE="%BUILD_TYPE%" -DFLTK_ROOT=%FLTK_ROOT% -DGLEW_ROOT=%GLEW_ROOT% ..
popd
goto end

:needcmake
echo ------------------------------------------------
echo CMAKE is required for building this application.
echo ------------------------------------------------
:end