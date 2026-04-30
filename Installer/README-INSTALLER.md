# F1 do Gordo Windows Installer

This folder contains the Inno Setup script used to build the Windows installer for **F1 do Gordo**.

The installer is for the VST3 plugin only. It is not a standalone desktop application.

## What the Installer Does

- Requires administrator permission.
- Copies the full `F1 do Gordo.vst3` bundle to:

```text
C:\Program Files\Common Files\VST3\F1 do Gordo.vst3
```

- Adds a normal Windows uninstall entry.
- Shows `NOTICE.md` during setup.
- Shows a final message telling the user to scan plugins in FL Studio.

## Build Requirements

- Inno Setup 6.
- A Release VST3 build at:

```text
build\F1DoGordo_artefacts\Release\VST3\F1 do Gordo.vst3
```

## Compile Manually

Install Inno Setup:

```powershell
winget install --id JRSoftware.InnoSetup
```

Then compile:

```powershell
& "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" "Installer\F1DoGordo.iss"
```

The generated installer is:

```text
Release\F1-do-Gordo-v0.1.0-wheel-prototype-installer.exe
```

Do not commit the generated `.exe`; attach it manually to the GitHub Release.

## After Installing

Open FL Studio and run:

```text
Options > Manage plugins > Find plugins
```

If the plugin does not appear, close FL Studio, reinstall, then scan again with `Verify plugins` enabled.
