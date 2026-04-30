# F1 do Gordo Docs

This vault documents **F1 do Gordo** for technical continuity, release checkpoints, architecture, testing and future phases.

## Index

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

## Current Checkpoint

Current checkpoint: `v0.1.0-wheel-prototype`.

This version documents the current public prototype state:

- Interactive Wheel UI.
- Rotating pointer graphics.
- Direct APVTS mapping for automation and future controller mapping.
- Basic Compressor, Air, Delay and Reverb modules.
- Public README, install guide, release notes and notices.
- Release ZIP workflow for GitHub Releases.

## Current Status

- Plugin format: VST3.
- Main validation host: FL Studio on Windows.
- Factory default remains intended to be neutral/pass-through.
- IDs APVTS are treated as a compatibility contract.
- Reverb has been refined by local A/B work, but will continue to evolve.
- `ReferenceAudio/` is local-only, ignored by Git and must not be published.

## Next Major Phase

The next major public-facing phase is replacing temporary/prototype visual artwork with fully original artwork that has no dependency on real-world brands or protected visual identities.

## Critical Notes

- This is a development checkpoint, not a final commercial release.
- The project will continue to evolve in DSP, presets, artwork, release packaging and host compatibility.
- Delay amount is controlled by `delaySend` / RETURN, not a separate `dryWet`, `output` or `analog` parameter. See [[decisions/0002-Delay-No-DryWet-Output-Analog]].
- The UI must remain host-automation/controller-mapping friendly. See [[decisions/0004-F1-Wheel-UI-And-Hardware-Mapping]].
