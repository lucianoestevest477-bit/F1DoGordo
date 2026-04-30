#define MyAppName "F1 do Gordo"
#define MyAppPublisher "Gordo Audio"
#define MyAppVersion "0.1.0-wheel-prototype"
#define MyAppId "{{E0FC7D5B-3D85-48B8-8D74-6C0E4B2D4747}"

[Setup]
AppId={#MyAppId}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL=https://github.com/lucianoestevest477-bit/F1DoGordo
AppSupportURL=https://github.com/lucianoestevest477-bit/F1DoGordo
AppUpdatesURL=https://github.com/lucianoestevest477-bit/F1DoGordo/releases
DefaultDirName={commoncf}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
UninstallDisplayName={#MyAppName} VST3
UninstallFilesDir={commonappdata}\Gordo Audio\F1 do Gordo\Uninstall
LicenseFile=..\NOTICE.md
OutputDir=..\Release
OutputBaseFilename=F1-do-Gordo-v0.1.0-wheel-prototype-installer
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\build\F1DoGordo_artefacts\Release\VST3\F1 do Gordo.vst3\*"; DestDir: "{app}\F1 do Gordo.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\README.md"; DestDir: "{commonappdata}\Gordo Audio\F1 do Gordo"; Flags: ignoreversion
Source: "..\INSTALL.md"; DestDir: "{commonappdata}\Gordo Audio\F1 do Gordo"; Flags: ignoreversion
Source: "..\RELEASE_NOTES.md"; DestDir: "{commonappdata}\Gordo Audio\F1 do Gordo"; Flags: ignoreversion
Source: "..\NOTICE.md"; DestDir: "{commonappdata}\Gordo Audio\F1 do Gordo"; Flags: ignoreversion

[UninstallDelete]
Type: filesandordirs; Name: "{app}\F1 do Gordo.vst3"
Type: dirifempty; Name: "{commonappdata}\Gordo Audio\F1 do Gordo"
Type: dirifempty; Name: "{commonappdata}\Gordo Audio"

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
    MsgBox('Installation complete.' + #13#10 + #13#10 +
           'Open FL Studio > Options > Manage plugins > Find plugins.',
           mbInformation, MB_OK);
end;
