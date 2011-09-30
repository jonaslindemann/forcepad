@echo off
cls
echo Starting ForcePAD Kiosk.
:restart
start /WAIT forcepad_kiosk
echo Restarting ForcePAD Kiosk.
goto restart