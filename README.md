# F1 do Gordo

Experimental VST3 audio plugin by **Gordo Audio** with an interactive wheel-first UI.

![Status](https://img.shields.io/badge/status-prototype-orange)
![Platform](https://img.shields.io/badge/platform-Windows-blue)
![Format](https://img.shields.io/badge/format-VST3-blue)
![Host](https://img.shields.io/badge/host-FL%20Studio-green)
![Built with](https://img.shields.io/badge/built%20with-JUCE%20%2F%20CMake%20%2F%20C%2B%2B20-lightgrey)

## Overview

**F1 do Gordo** is an experimental VST3 audio plugin built around a wheel-first interactive interface. The goal is to explore a fast, visual, performance-oriented control surface for mixing and effects, with simplified controls for gain, compression, air, delay, reverb and bypass.

This repository is currently a development prototype. It is useful as a checkpoint for testing in FL Studio, documenting the architecture, and continuing DSP/UI development.

## Current Prototype Status

- Version: `v0.1.0-wheel-prototype`.
- Status: experimental prototype.
- Main test environment: Windows + FL Studio.
- UI: interactive wheel prototype with rotating pointers.
- DSP: functional but still being refined.
- Distribution: local ZIP package intended for manual GitHub Release attachment.

## Features

- Wheel-first interactive UI.
- Rotating pointer controls.
- Input and output gain.
- Compressor: Threshold, Ratio, Attack, Release, Gain.
- Air: Mid Air, High Air.
- Delay: Return/Level and Timing.
- Reverb: ON/OFF and Mix.
- Global Bypass.

## Controls

| Visual Control | Function | Status |
| --- | --- | --- |
| INPUT | Input gain | Working |
| OUTPUT | Output gain | Working |
| THRESHOLD | Compressor threshold | Working |
| RATIO | Compressor ratio | Working |
| ATTACK | Compressor attack | Working |
| RELEASE | Compressor release | Working |
| GAIN | Compressor output/makeup gain | Working |
| MID AIR | Mid/high presence | Working |
| HIGH AIR | High-air sheen | Working |
| DELAY RETURN / FEEDBACK visual | Delay return level | Working |
| TIMING | Delay note division | Working |
| N / REVERB | Reverb on/off | Working |
| DIF IN scroll | Reverb mix | Working |
| BYPASS | Global bypass | Working |
| A/B | A/B comparison | Placeholder |
| SAVE | Save/preset action | Placeholder |

## Installation for FL Studio

1. Download the release ZIP from GitHub.
2. Extract the ZIP.
3. Copy the full folder named `F1 do Gordo.vst3` to:

```text
C:\Program Files\Common Files\VST3
```

4. Open FL Studio.
5. Go to `Options > Manage plugins`.
6. Confirm that `C:\Program Files\Common Files\VST3` is included in the scan paths.
7. Click `Find plugins`.
8. Open `F1 do Gordo` on a Mixer Insert.

For a focused install guide, see [INSTALL.md](INSTALL.md).

## Build from Source

Requirements:

- Visual Studio 2022 Build Tools with the C++ desktop workload.
- CMake 3.22 or newer.
- Git.

Build commands:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The built VST3 bundle is generated at:

```text
build\F1DoGordo_artefacts\Release\VST3\F1 do Gordo.vst3
```

## Documentation

- [INSTALL.md](INSTALL.md)
- [RELEASE_NOTES.md](RELEASE_NOTES.md)
- [NOTICE.md](NOTICE.md)
- [Docs/00-Home.md](Docs/00-Home.md)
- [Docs/09-Roadmap.md](Docs/09-Roadmap.md)
- [Docs/10-Changelog.md](Docs/10-Changelog.md)

## Release Package

The local release ZIP is generated at:

```text
Release/F1-do-Gordo-v0.1.0-wheel-prototype.zip
```

`Release/` is intentionally ignored by Git. The ZIP should be attached manually to the GitHub Release and should not be committed to the repository.

## Known Limitations

- Prototype build.
- Visual artwork is temporary/prototypical.
- A/B and SAVE are placeholders.
- Sound modules are still being refined.
- Windows/FL Studio is the main tested target.
- Not ready for commercial distribution.

## Roadmap

- Replace temporary wheel artwork with fully original design.
- Reverb A/B polish.
- Delay voicing inspired by analog/tape-style references.
- Air polish.
- Compressor polish.
- Real A/B system.
- Save/preset manager.
- Official installer.
- CPU optimization.
- Multi-host testing.

## Legal / Trademark Notice

This project is independent and experimental. It is not affiliated with, sponsored by, endorsed by, or associated with Ferrari, Formula 1, FIA, Thrustmaster, Waves, Valhalla DSP, Slate Digital, Image-Line, or any other referenced brand.

Brand names are used only as descriptive references for study, development direction, and quality targets. The current visual artwork is temporary/prototypical and must be replaced with original artwork before any commercial release.

No proprietary code, algorithm, preset, trademark, or official asset is included intentionally.
