# F1 do Gordo Docs

This vault documents **F1 do Gordo** for technical continuity, release checkpoints, architecture, testing and future phases.

## Current Checkpoint

- Version: `v0.1.0-wheel-prototype`.
- Status: experimental public prototype.
- Main host target: FL Studio on Windows.
- Local release ZIP: `Release/F1-do-Gordo-v0.1.0-wheel-prototype.zip`.
- Manual install target: `C:\Program Files\Common Files\VST3`.

## What Exists Now

- Interactive Wheel UI.
- Rotating pointer controls.
- Direct APVTS mapping for automation and future controller mapping.
- Basic Compressor module.
- Basic Air module.
- Basic Delay module.
- Basic Reverb module.
- Global Bypass.
- Public README, INSTALL, RELEASE_NOTES and NOTICE files.
- Release ZIP workflow for GitHub Releases.

## Documentation Index

- [[01-Project-Overview]]
- [[02-Build-And-Test]]
- [[03-Architecture]]
- [[04-Parameters-APVTS]]
- [[05-DSP-Modules]]
- [[06-UI-Cockpit-Design]]
- [[07-Hardware-Mapping-Plan]]
- [[08-Reverb-Issues-And-Future-Fix]]
- [[09-Roadmap]]
- [[10-Changelog]]

## Modules

- [[modules/Channel-EQ]]
- [[modules/FET-Compressor]]
- [[modules/Air-Exciter]]
- [[modules/Delay]]
- [[modules/Reverb]]

## Decisions

- [[decisions/0001-Use-JUCE-CMake-VST3]]
- [[decisions/0002-Delay-No-DryWet-Output-Analog]]
- [[decisions/0003-Factory-Default-Neutral]]
- [[decisions/0004-F1-Wheel-UI-And-Hardware-Mapping]]

## Release Hygiene

- `build/` is local build output and must not be committed.
- `Release/` is local packaging output and must not be committed.
- `ReferenceAudio/` is local-only and must not be published.
- WAV/MP3/FLAC reference files must not be committed.

## Next Steps

- Replace temporary wheel artwork with a fully original design.
- Continue Reverb A/B polish.
- Refine Delay, Air and Compressor voicing.
- Add real A/B.
- Add Save/preset manager.
- Build installer.
- Test additional hosts.
- Optimize CPU usage.
