# F1 do Gordo

**F1 do Gordo** is an experimental VST3 audio plugin by **Gordo Audio**.

Current version: **v0.1.0-wheel-prototype**.

This checkpoint captures the current wheel-first interactive prototype: a custom visual control surface with rotating pointers, APVTS-backed automation, and early DSP modules for mixing, color and space. It is intended for development testing in FL Studio, not as a final commercial release.

## Features

- INPUT and OUTPUT gain.
- Compressor controls: THRESHOLD, RATIO, ATTACK, RELEASE, GAIN.
- AIR controls: MID AIR and HIGH AIR.
- DELAY controls: RETURN/LEVEL and TIMING.
- REVERB controls: ON/OFF and MIX.
- BYPASS.
- Wheel UI with interactive hotspots and pointer graphics.
- Direct APVTS parameter mapping for host automation and future controller mapping.

## Stack

- C++20.
- JUCE.
- CMake.
- VST3.
- Windows.
- FL Studio as the main validation host.

## Prototype Status

This is a public development checkpoint, not a final product.

- DSP is functional but still being refined.
- Delay, compressor, air and reverb will continue to evolve.
- A/B and SAVE are currently placeholders.
- The current visual artwork is temporary/prototype artwork.
- Before any commercial distribution or final public release, the project needs fully original final artwork and additional host testing.

## Build on Windows

Requirements:

- Visual Studio 2022 Build Tools with the C++ desktop workload.
- CMake 3.22 or newer.
- Git.

Build commands:

```powershell
git clone <repo-url>
cd F1DoGordo-functional
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

CMake downloads JUCE automatically through `FetchContent`.

The VST3 bundle is generated at:

```text
build\F1DoGordo_artefacts\Release\VST3\F1 do Gordo.vst3
```

## Manual Install in FL Studio

Copy the full VST3 bundle folder:

```text
F1 do Gordo.vst3
```

to:

```text
C:\Program Files\Common Files\VST3
```

Then open FL Studio:

1. Go to `Options > Manage plugins`.
2. Confirm `C:\Program Files\Common Files\VST3` is in the plugin search paths.
3. Click `Find plugins`.
4. Load `F1 do Gordo` on a Mixer Insert.

For release ZIP installation, see [INSTALL.md](INSTALL.md).

## Release Notes and Docs

- [INSTALL.md](INSTALL.md)
- [RELEASE_NOTES.md](RELEASE_NOTES.md)
- [Docs/00-Home.md](Docs/00-Home.md)

## Legal / Trademark Notice

This project is independent. It is not affiliated with, sponsored by, endorsed by, approved by or associated with Ferrari, Formula 1, FIA, Thrustmaster, Waves, Valhalla DSP, Slate Digital or any other brand mentioned in the project.

Any brand, product or plugin names mentioned in repository notes, discussions or development context are used only as descriptive references for study, sound-quality targets or artistic direction.

The current visual artwork is temporary/prototype artwork. Fully original artwork will be required before any commercial distribution or final public release.

No proprietary code, proprietary algorithm or commercial preset has been copied into this project.

See [NOTICE.md](NOTICE.md) for the public notice summary.
