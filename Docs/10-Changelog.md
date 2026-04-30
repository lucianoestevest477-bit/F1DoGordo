# Changelog

## v0.1.0-wheel-prototype

- Public prototype documentation polished for GitHub.
- Wheel UI integration documented.
- Interactive pointers documented.
- Simplified module controls documented.
- Reverb and delay behavior corrections documented.
- Release ZIP created for manual GitHub Release attachment.
- README, INSTALL, RELEASE_NOTES and NOTICE prepared for public repository use.

## Previous Checkpoint Work

- Wheel UI final integrada ao prototipo atual.
- Ponteiros interativos.
- APVTS direto para automacao e futuro hardware mapping.
- Compressor, Air, Delay e Reverb simplificados para uso na Wheel.
- Delay com RETURN/LEVEL e TIMING corrigidos.
- Reverb refinado por A/B local.
- ZIP de release gerado para GitHub Release.
- Documentacao publica adicionada.

## Timeline de fases e commits

- `v0.1.0-wheel-prototype` Prepare wheel prototype release
- `a7b77d0` Initial JUCE VST3 plugin skeleton
- `1b07141` Fix build metadata and ignore generated files
- `17d4d8b` Wire UI controls to APVTS parameters
- `08dda69` Implement Channel EQ DSP
- `977db5b` Fix Channel UI layout and parameter mapping
- `9bed123` Implement FET compressor DSP
- `3d2b518` Add compressor threshold and fix metering
- `de821a8` Implement Air exciter DSP
- `4db17a4` Refine Air exciter controls
- `bd1a3db` Rename Air drive label to intensity
- `8908ba5` Implement Delay DSP
- `97f3aee` Implement Reverb DSP
- `a437530` Make factory default neutral
- `a7a59a9` Refine reverb voicing
- `318f7fa` Improve F1 cockpit visual design
- `0296c5a` Refine F1 wheel cockpit layout
- `f67ff11` Push UI toward literal interactive F1 wheel
- `dfeb0bf` Make UI resemble full interactive F1 wheel
- `63e816e` Make cockpit UI dark oval and neon premium
- `7f0a6b9` Add Obsidian documentation vault

## Release Notes

- Initial skeleton: base C++20/JUCE/CMake/VST3.
- APVTS controls: knobs and buttons linked to real parameters.
- Channel EQ: HPF, LPF, 4-band EQ, trim, phase, drive and mix.
- Compressor: FET-style compressor controls and metering.
- Air: exciter module with mid/high air controls.
- Delay: send-based delay with sync, timing, feedback, filters, ducking, freeze and width.
- Reverb: functional prototype reverb, still in polish.
- Factory default neutral: new instance should open close to unity/pass-through.
- UI Wheel: interactive wheel with hotspots and rotating pointers.
- Documentation vault: Markdown docs for architecture, APVTS, modules, UI, mapping, roadmap and changelog.
