[Setup]
AppName=ForcePAD
AppId={{26BA5D59-E5B8-4A71-BDA9-750E53EF5C0B}
AppVersion=2.6.b3
AppVerName=ForcePAD 2.6.b3 BETA
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
OutputBaseFilename=ForcePAD-2.6.b3-setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
MinVersion=10.0
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
SignTool=signtool_lu
SignedUninstaller=yes

[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:

[Files]
Source: "vc_redist\VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
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
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/install /passive /quiet /norestart";
Filename: "{app}\qtforcepad.exe"; WorkingDir: "{app}"; Description: "Launch ForcePAD"; Flags: nowait postinstall skipifsilent