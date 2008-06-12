@echo off
if exist win32_build_release (
	rmdir /Q /S win32_build_release
)
if exist win32_build_debug (
	rmdir /Q /S win32_build_debug
)
del /Q .\bin\release\*.exe
del /Q .\bin\release\*.ilk
del /Q .\bin\debug\*.exe
del /Q .\bin\debug\*.ilk
del /Q .\lib\release\*.*
del /Q .\lib\debug\*.*
del /Q .\install\win32\Output\*.*
