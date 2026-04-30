# Install F1 do Gordo in FL Studio

This guide explains how to install the packaged **F1 do Gordo** VST3 release ZIP on Windows.

## Install from the Release ZIP

1. Download the release ZIP from GitHub.
2. Extract the ZIP to a normal folder, such as your Downloads folder or Desktop.
3. Find the VST3 bundle folder:

```text
F1 do Gordo.vst3
```

4. Copy the whole `F1 do Gordo.vst3` folder to:

```text
C:\Program Files\Common Files\VST3
```

5. If Windows asks for administrator permission, allow it. The `Program Files` folder is protected by Windows.
6. Open FL Studio.
7. Go to `Options > Manage plugins`.
8. Click `Find plugins`.
9. Load `F1 do Gordo` on a Mixer Insert.

## If the Plugin Does Not Appear

- Confirm that the full `F1 do Gordo.vst3` folder was copied, not only a file inside it.
- Confirm that `C:\Program Files\Common Files\VST3` is enabled in FL Studio's plugin search paths.
- Run `Options > Manage plugins > Find plugins` again.
- Restart FL Studio and rescan if needed.

## Prototype Notice

This is an experimental development checkpoint. Test it in a new FL Studio session before using it in important projects.
