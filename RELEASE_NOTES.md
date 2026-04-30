# v0.1.0-wheel-prototype

This is an experimental checkpoint release for the **F1 do Gordo** wheel-first VST3 prototype. It is intended for FL Studio testing and development continuity, not as a final commercial release.

Please test this build in a new FL Studio session before using it in important projects.

## Added

- Wheel UI prototype with interactive hotspots.
- Rotating pointer graphics for the main controls.
- Direct APVTS-backed parameter control for automation and future hardware mapping.
- Manual release ZIP workflow.
- Public installation documentation.
- Public prototype notices.

## Changed

- Compressor, Air, Delay and Reverb are available through simplified Wheel controls.
- Delay Wheel behavior uses RETURN/LEVEL and synced TIMING.
- Reverb Wheel behavior uses ON/OFF and MIX.
- Documentation now describes the prototype status, install flow and release hygiene.

## Known Issues

- A/B is a placeholder.
- SAVE is a placeholder.
- Reverb is improved but still under refinement.
- Delay will be refined further.
- Compressor will be refined further.
- Visual artwork is temporary and must be replaced with original artwork before any final public or commercial release.

## Installation

Copy the full VST3 bundle folder:

```text
F1 do Gordo.vst3
```

to:

```text
C:\Program Files\Common Files\VST3
```

Then open FL Studio, go to `Options > Manage plugins`, click `Find plugins`, and load `F1 do Gordo` on a Mixer Insert.

## Next Steps

- Create original artwork with no dependency on real-world brands.
- Improve Delay with analog/tape-inspired behavior.
- Improve Compressor behavior and gain staging.
- Polish Air.
- Continue Reverb polish using A/B references.
- Add real presets.
- Add real A/B.
- Add SAVE/preset manager.
- Build an official installer.
- Test in additional hosts.
- Optimize CPU usage.
