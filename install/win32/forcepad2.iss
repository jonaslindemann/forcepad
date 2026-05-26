#define MyAppName "ForcePAD2"
#define MyAppVersion "2.6.b4"
#define MyAppPublisher "Division of Structural Mechanics"
#define MyAppURL "https://jonaslindemann.github.io/forcepad/"
#define MyAppExeName "qtforcepad.exe"
#define MyAppAssocName MyAppName + " File"
#define MyAppAssocExt ".fp2"
#define MyAppAssocKey StringChange(MyAppAssocName, " ", "") + MyAppAssocExt

[Setup]
AppName={#MyAppName}
AppId={{26BA5D59-E5B8-4A71-BDA9-750E53EF5C0B}
AppVersion={#MyAppVersion}
;AppVerName=ForcePAD 2.6.b3 BETA
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\Structural Mechanics\{#MyAppName}
DefaultGroupName=Structural Mechanics
UsePreviousAppDir=no
UsePreviousGroup=no
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=output
OutputBaseFilename={#MyAppName}-{#MyAppVersion}-setup-signed
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
Source: "..\..\bin\release\{#MyAppExeName}";  DestDir: "{app}"; Flags: ignoreversion signonce
Source: "..\..\bin\release\*.dll";           DestDir: "{app}"; Flags: ignoreversion signonce
Source: "..\..\bin\release\brushes\*";       DestDir: "{app}\brushes";   Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\samples\*";       DestDir: "{app}\samples";   Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\colormaps\*";     DestDir: "{app}\colormaps"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\icons\*";         DestDir: "{app}\icons";     Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\platforms\*";     DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\bin\release\styles\*";        DestDir: "{app}\styles";    Flags: ignoreversion recursesubdirs createallsubdirs

[Registry]
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocExt}\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppAssocKey}"; ValueData: ""; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocKey}"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocKey}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocKey}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: ".myp"; ValueData: ""

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/install /passive /quiet /norestart";
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
