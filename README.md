# F1 do Gordo

**F1 do Gordo** is a VST3 audio plugin by **Gordo Audio**.

Current checkpoint: **v0.1.0-wheel-prototype**.

This is a development build. The plugin is usable as a stable checkpoint for testing in FL Studio, but the DSP, UI artwork, installer and release flow will continue to evolve.

## Current State

- JUCE/CMake/C++20 VST3 plugin.
- Custom Wheel UI with interactive pointers.
- APVTS-backed controls for automation and host mapping.
- Functional input/output gain, bypass, compressor, air exciter, delay and reverb.
- Delay Wheel mapping uses `delaySend` as return/level and `delayNoteDivision` for synced timing.
- Reverb Wheel mapping uses `reverbEnabled` and `reverbMix`, with an internal musical plate/hall-style prototype voicing.
- Main validation host: FL Studio on Windows.

## Requirements

- Visual Studio 2022 Build Tools, including the C++ desktop workload.
- CMake 3.22 or newer.
- Git.
- FL Studio for host testing.

## Build on Windows

From a Developer PowerShell or a terminal with Visual Studio tools available:

```powershell
git clone <repo-url>
cd F1DoGordo-functional
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

CMake downloads JUCE automatically through `FetchContent`.

The built VST3 bundle is generated here:

```text
build\F1DoGordo_artefacts\Release\VST3\F1 do Gordo.vst3
```

## Install in FL Studio

Copy the whole VST3 bundle folder:

```text
build\F1DoGordo_artefacts\Release\VST3\F1 do Gordo.vst3
```

to:

```text
C:\Program Files\Common Files\VST3
```

You may need administrator permission to copy into `Program Files`.

Then open FL Studio:

1. Go to `Options > Manage plugins`.
2. Confirm `C:\Program Files\Common Files\VST3` is included in the scan paths.
3. Click `Find plugins`.
4. Load `F1 do Gordo` on a Mixer Insert.

## Release ZIP Install

For packaged releases, download the ZIP from the GitHub Release page, extract it, and follow [INSTALL.md](INSTALL.md).

## Documentation

Project documentation lives in `Docs/`:

- [Docs/00-Home.md](Docs/00-Home.md)
- [Docs/09-Roadmap.md](Docs/09-Roadmap.md)
- [Docs/10-Changelog.md](Docs/10-Changelog.md)

## Development Notes

- `build/`, `Release/` and local reference audio are not committed.
- `ReferenceAudio/` is local-only and must not be published.
- APVTS parameter IDs are treated as a compatibility contract for automation and hardware mapping.
- Before any public release, replace prototype/reference artwork with original final artwork.
