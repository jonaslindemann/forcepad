; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=ForcePAD
AppVerName=ForcePAD 2.4.3
AppPublisher=Division of Structural Mechanics
AppPublisherURL=http://www.byggmek.lth.se
AppSupportURL=http://www.byggmek.lth.se/resources/forcepad/forcepad.htm
AppUpdatesURL=http://www.byggmek.lth.se/resources/forcepad/forcepad.htm
UsePreviousAppDir=false
UsePreviousGroup=false
DefaultDirName={pf}\Structural Mechanics\ForcePAD2
DefaultGroupName=Structural Mechanics
;AlwaysCreateUninstallIcon=yes
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51
WizardImageBackColor=clBlack
WizardImageFile=Styles\Office2007Gray.bmp
WizardSmallImageFile=compiler:wizmodernsmallimage-IS.bmp

[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:; MinVersion: 4,4

[Run]
Filename: {tmp}\vcredist_x86.exe; Flags: hidewizard

Filename: {app}\forcepad.exe; WorkingDir: {app}; Description: Launch ForcePAD; Flags: nowait postinstall skipifsilent

[Files]
Source: ..\..\..\forcepad-redist\vc2010\vcredist_x86.exe; DestDir: {tmp}; Flags: deleteafterinstall
Source: ..\..\bin\release\forcepad.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\..\bin\release\forcepad_kiosk.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\..\docs\eng_manual_2_4.pdf; DestDir: {app}; Flags: ignoreversion
;Source: ..\..\bin\release\glew32.dll; DestDir: {app}; Flags: ignoreversion
Source: ..\..\bin\release\brushes\*.*; DestDir: {app}\brushes; Flags: ignoreversion
Source: ..\..\bin\release\samples\*.*; DestDir: {app}\samples; Flags: ignoreversion
Source: ..\..\bin\release\colormaps\*.*; DestDir: {app}\colormaps; Flags: ignoreversion
Source: ..\..\bin\release\kiosk\*.*; DestDir: {app}\kiosk; Flags: ignoreversion
;Source: "c:\windows\system32\msvcp60.dll"; DestDir: "{sys}"; CopyMode: onlyifdoesntexist; Flags: uninsneveruninstall sharedfile
Source: ISSkin.dll; DestDir: {app}; Flags: dontcopy
Source: Styles\Office2007.cjstyles; DestDir: {tmp}; Flags: dontcopy

[Icons]
Name: {group}\ForcePAD 2; WorkingDir: {app}; Filename: {app}\forcepad.exe
Name: {group}\ForcePAD 2 Kiosk; WorkingDir: {app}; Filename: {app}\forcepad_kiosk.exe
Name: {group}\ForcePAD Users Guide; WorkingDir: {app}; Filename: {app}\eng_manual_2_4.pdf
Name: {userdesktop}\ForcePAD 2; WorkingDir: {app}; Filename: {app}\forcepad.exe; MinVersion: 4,4; Tasks: desktopicon

[Code]
// Importing LoadSkin API from ISSkin.DLL
procedure LoadSkin(lpszPath: String; lpszIniFileName: String);
external 'LoadSkin@files:isskin.dll stdcall';

// Importing UnloadSkin API from ISSkin.DLL
procedure UnloadSkin();
external 'UnloadSkin@files:isskin.dll stdcall';

// Importing ShowWindow Windows API from User32.DLL
function ShowWindow(hWnd: Integer; uType: Integer): Integer;
external 'ShowWindow@user32.dll stdcall';

function InitializeSetup(): Boolean;
begin
	ExtractTemporaryFile('Office2007.cjstyles');
	LoadSkin(ExpandConstant('{tmp}\Office2007.cjstyles'), 'NormalBlack.ini');
	Result := True;
end;

procedure DeinitializeSetup();
begin
	// Hide Window before unloading skin so user does not get
	// a glimse of an unskinned window before it is closed.
	ShowWindow(StrToInt(ExpandConstant('{wizardhwnd}')), 0);
	UnloadSkin();
end;
