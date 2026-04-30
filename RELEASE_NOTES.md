# F1 do Gordo v0.1.0-wheel-prototype

## Summary

This is the first public prototype checkpoint for **F1 do Gordo**, an experimental VST3 plugin by Gordo Audio with a wheel-first interactive UI.

The build is intended for Windows/FL Studio testing and development feedback. It is not a final commercial release.

## Added

- Interactive wheel-first UI prototype.
- Rotating pointer controls.
- Input and output gain.
- Compressor controls: Threshold, Ratio, Attack, Release, Gain.
- Air controls: Mid Air and High Air.
- Delay controls: Return/Level and Timing.
- Reverb controls: ON/OFF and Mix.
- Global Bypass.
- Public README, install guide, release notes and notice file.
- Local release ZIP workflow for manual GitHub Release attachment.

## Changed

- Documentation now presents the project as a public prototype.
- README now includes controls, installation, build instructions, roadmap and legal notice.
- INSTALL now focuses on end-user FL Studio installation.
- NOTICE now summarizes prototype/legal/reference-audio boundaries.

## Fixed

- Delay Wheel behavior uses delay return level instead of internal feedback as the main visible amount control.
- Delay timing is documented as note-division based.
- Reverb Wheel behavior is documented as ON/OFF plus Mix.
- Release hygiene is documented so `build/`, `Release/`, `ReferenceAudio/` and reference audio files stay out of Git.

## Known Issues

- A/B is a placeholder.
- SAVE is a placeholder.
- Reverb is still being refined.
- Delay will be refined further.
- Compressor will be refined further.
- Visual artwork is temporary/prototypical and must be replaced with original artwork before final public or commercial release.
- Windows/FL Studio is the main tested target.

## Installation

Copy the full folder:

```text
F1 do Gordo.vst3
```

to:

```text
C:\Program Files\Common Files\VST3
```

Then open FL Studio, go to `Options > Manage plugins`, click `Find plugins`, and load `F1 do Gordo` on a Mixer Insert.

## Notes for Testers

- Test in a new FL Studio session first.
- Close FL Studio before replacing an older VST3 build.
- If the plugin does not appear, rescan with `Verify plugins` enabled.
- Report host, Windows version, FL Studio version and a short reproduction path for bugs.

## Legal / Trademark Notice

This project is independent and experimental. It is not affiliated with, sponsored by, endorsed by, or associated with Ferrari, Formula 1, FIA, Thrustmaster, Waves, Valhalla DSP, Slate Digital, Image-Line, or any other referenced brand.

Brand names are used only as descriptive references for study, development direction, and quality targets. No proprietary code, algorithm, preset, trademark, or official asset is included intentionally.
