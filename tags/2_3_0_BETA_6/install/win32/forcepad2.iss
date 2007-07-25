; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=ForcePAD
AppVerName=ForcePAD 2.3.0 (BETA6)
AppPublisher=Division of Structural Mechanics
AppPublisherURL=http://www.byggmek.lth.se
AppSupportURL=http://www.byggmek.lth.se/resources/forcepad/forcepad.htm
AppUpdatesURL=http://www.byggmek.lth.se/resources/forcepad/forcepad.htm
UsePreviousAppDir=no
UsePreviousGroup=no
DefaultDirName={pf}\Structural Mechanics\ForcePAD2
DefaultGroupName=Structural Mechanics
;AlwaysCreateUninstallIcon=yes
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; MinVersion: 4,4

[Run]
Filename: {tmp}\vcredist_x86-sp1.exe; Flags: hidewizard;

[Files]
Source: "..\..\..\forcepad-redist\vcredist_x86-sp1.exe"; DestDir: {tmp}; Flags: deleteafterinstall
Source: "..\..\bin\release\forcepad.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\..\bin\release\brushes\*.*"; DestDir: "{app}\brushes"; CopyMode: alwaysoverwrite
Source: "..\..\bin\release\samples\*.*"; DestDir: "{app}\samples"; CopyMode: alwaysoverwrite
Source: "..\..\bin\release\colormaps\*.*"; DestDir: "{app}\colormaps"; CopyMode: alwaysoverwrite
;Source: "c:\windows\system32\msvcp60.dll"; DestDir: "{sys}"; CopyMode: onlyifdoesntexist; Flags: uninsneveruninstall sharedfile

[Icons]
Name: "{group}\ForcePAD 2"; WorkingDir: "{app}"; Filename: "{app}\forcepad.exe"
Name: "{userdesktop}\ForcePAD 2"; WorkingDir: "{app}"; Filename: "{app}\forcepad.exe"; MinVersion: 4,4; Tasks: desktopicon

[Run]
Filename: "{app}\forcepad.exe"; WorkingDir: "{app}"; Description: "Launch ForcePAD"; Flags: nowait postinstall skipifsilent

