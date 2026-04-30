# Install F1 do Gordo

This guide is for installing the packaged **F1 do Gordo** VST3 release in FL Studio on Windows.

## Requirements

- Windows.
- FL Studio.
- The release ZIP containing `F1 do Gordo.vst3`.
- Administrator permission may be required to copy files into `Program Files`.

## Download

1. Open the GitHub Release page.
2. Download `F1-do-Gordo-v0.1.0-wheel-prototype.zip`.
3. Extract the ZIP to a normal folder such as Desktop or Downloads.

## Manual Installation

1. Find the extracted folder named:

```text
F1 do Gordo.vst3
```

2. Copy the whole `F1 do Gordo.vst3` folder to:

```text
C:\Program Files\Common Files\VST3
```

3. If Windows asks for permission, allow it. `Program Files` is a protected system folder.

## FL Studio Scan

1. Open FL Studio.
2. Go to `Options > Manage plugins`.
3. Confirm that `C:\Program Files\Common Files\VST3` is included in the scan paths.
4. Enable `Verify plugins` if you want a deeper scan.
5. Click `Find plugins`.
6. Open `F1 do Gordo` on a Mixer Insert.

## Troubleshooting

- Plugin does not appear: run `Find plugins` again with `Verify plugins` enabled.
- Windows asks permission: this is expected because `Program Files` requires administrator permission.
- Old version still loads: close FL Studio before replacing the VST3 bundle.
- Copy fails because the file is in use: close FL Studio / `FL64.exe`, then copy again.
- Updated plugin does not seem to change: open a new plugin instance after replacing the VST3.
- Still not found: confirm that the full `F1 do Gordo.vst3` folder was copied, not only a file inside it.

## Prototype Notice

This is an experimental prototype build. Test it in a new FL Studio session before using it in important projects.
