# Changelog

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

## Marcos principais

- Wheel prototype release checkpoint: versao documentada para GitHub Release e teste no FL Studio.
- Wheel UI final integrada ao prototipo atual, com hotspots e ponteiros interativos.
- Mapeamento direto da Wheel para APVTS, mantendo IDs estaveis para automacao.
- Ajustes DSP acumulados no Compressor, Air e Reverb.
- Delay corrigido para usar RETURN/LEVEL via `delaySend` e TIMING via `delayNoteDivision`.
- Reverb refinado por A/B com referencia local em `ReferenceAudio/`, sem publicar WAVs.
- Initial skeleton: base C++20/JUCE/CMake/VST3.
- Build metadata: bundle ID e arquivos gerados ignorados.
- APVTS controls: knobs e botoes ligados a parametros reais.
- Channel EQ: HPF, LPF, EQ de 4 bandas, trim, phase, drive e mix.
- Compressor: FET compressor com threshold, ratio, attack, release, mix e GR.
- Air: exciter com amount, mid/high air, intensity, density, dynamic, de-ess e output.
- Delay: delay send interno com sync, modos, feedback, filtros, ducking, freeze e width.
- Reverb: algoritmo funcional, porem marcado para revisao futura.
- Factory default neutral: instancia nova deve abrir em unity/pass-through.
- UI cockpit fases 8 a 8.4: evolucao visual para volante F1 interativo escuro/premium.
- Documentation vault: organizacao Obsidian Markdown em `Docs/`, com arquitetura, APVTS, modulos, UI, hardware mapping, roadmap, changelog e ADRs.
