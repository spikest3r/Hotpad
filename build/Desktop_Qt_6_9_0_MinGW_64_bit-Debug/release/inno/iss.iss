[Setup]
AppName=Hotpad
AppVersion=1.0
DefaultDirName={autopf}\Hotpad
DefaultGroupName=Hotpad
OutputDir=output
OutputBaseFilename=HotpadInstaller
Compression=lzma
SolidCompression=yes

[Files]
Source: "D:\Programming Stuff\IDEs\Qt\Hotpad\build\Desktop_Qt_6_9_0_MinGW_64_bit-Debug\release\ship\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\Hotpad"; Filename: "{app}\Hotpad.exe"

[Run]
Filename: "{app}\Hotpad.exe"; Description: "Launch Hotpad"; Flags: nowait postinstall skipifsilent

[Tasks]
Name: autostart; Description: "Add Hotpad to Windows startup"; GroupDescription: "Additional tasks:"; Flags: unchecked

[Icons]
Name: "{userstartup}\Hotpad"; Filename: "{app}\Hotpad.exe"; Tasks: autostart
