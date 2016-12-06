; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{715DDC95-B7DD-4D56-B564-F36527D2CCC7}
AppName=VScope
AppVersion=0.99.2
;AppVerName=VScope 0.99.2
AppPublisher=Daniel Wagenaar
AppPublisherURL=http://www.danielwagenaar.net
AppSupportURL=http://www.danielwagenaar.net
AppUpdatesURL=http://www.danielwagenaar.net
DefaultDirName={pf}\VScope
DisableProgramGroupPage=yes
LicenseFile=C:\Users\Daniel Wagenaar\Documents\Progs\vscope\gpl-3.0.txt
OutputDir=../build
OutputBaseFilename=vscope-0.99.2-setup
SetupIconFile=C:\Users\Daniel Wagenaar\Documents\Progs\vscope\microscopeicon.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\Daniel Wagenaar\Documents\Progs\vscope\build-vscope-Desktop_Qt_5_7_0_MSVC2013_32bit-Release\release\vscope-w32\*"; DestDir: "{app}"; Flags: ignoreversion  recursesubdirs createallsubdirs

[Icons]
Name: "{commonprograms}\VScope"; Filename: "{app}\vscope.exe"
Name: "{commondesktop}\VScope"; Filename: "{app}\vscope.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\vscope.exe"; Description: "{cm:LaunchProgram,VScope}"; Flags: nowait postinstall skipifsilent

