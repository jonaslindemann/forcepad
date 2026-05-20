[Setup]
AppName=ForcePAD
AppVersion=2.6.0
AppVerName=ForcePAD 2.6.0 BETA
AppPublisher=Division of Structural Mechanics, LTH
AppPublisherURL=https://www.byggmek.lth.se
AppSupportURL=https://www.byggmek.lth.se
AppUpdatesURL=https://www.byggmek.lth.se
DefaultDirName={autopf}\Structural Mechanics\ForcePAD2
DefaultGroupName=Structural Mechanics
UsePreviousAppDir=no
UsePreviousGroup=no
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=output
OutputBaseFilename=ForcePAD-2.6.0-setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
MinVersion=10.0
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:

[Files]
Source: "..\..\bin\release\qtforcepad.exe";  DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\release\*.dll";           DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\release\brushes\*";       DestDir: "{app}\brushes";   Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\samples\*";       DestDir: "{app}\samples";   Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\colormaps\*";     DestDir: "{app}\colormaps"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\icons\*";         DestDir: "{app}\icons";     Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\platforms\*";     DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\styles\*";        DestDir: "{app}\styles";    Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\ForcePAD 2";     Filename: "{app}\qtforcepad.exe"; WorkingDir: "{app}"
Name: "{userdesktop}\ForcePAD 2"; Filename: "{app}\qtforcepad.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\qtforcepad.exe"; WorkingDir: "{app}"; Description: "Launch ForcePAD"; Flags: nowait postinstall skipifsilent
