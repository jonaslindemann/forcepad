@echo off

if not exist win32_build (
	call configure.cmd
)

pushd win32_build
EriViz.sln
popd
