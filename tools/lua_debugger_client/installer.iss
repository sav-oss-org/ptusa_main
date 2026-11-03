#define AppName "ptusa Lua debugger"
#define AppVersion "0.1.0"
#define AppExeName "PtusaLuaDebugger.exe"

[Setup]
AppId={{E70A5C5A-A1B0-43B8-A17E-CF77099DD855}
AppName={#AppName}
AppVersion={#AppVersion}
DefaultDirName={autopf}\PtusaLuaDebugger
DefaultGroupName={#AppName}
OutputDir=installer
OutputBaseFilename=PtusaLuaDebuggerSetup
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Files]
Source: "dist\{#AppExeName}"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExeName}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Создать ярлык на рабочем столе"

[Run]
Filename: "{app}\{#AppExeName}"; Description: "Запустить {#AppName}"; Flags: nowait postinstall skipifsilent
