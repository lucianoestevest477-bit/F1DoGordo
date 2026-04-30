# F1 do Gordo Docs

Este vault documenta o projeto **F1 do Gordo** para continuidade tecnica, historico de decisoes, arquitetura, testes e proximas fases.

## Indice

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

## Modulos

- [[modules/Channel-EQ]]
- [[modules/FET-Compressor]]
- [[modules/Air-Exciter]]
- [[modules/Delay]]
- [[modules/Reverb]]

## Decisoes

- [[decisions/0001-Use-JUCE-CMake-VST3]]
- [[decisions/0002-Delay-No-DryWet-Output-Analog]]
- [[decisions/0003-Factory-Default-Neutral]]
- [[decisions/0004-F1-Wheel-UI-And-Hardware-Mapping]]

## Status atual

- Plugin VST3 compila em Release.
- Host principal de validacao: FL Studio.
- Factory default neutro: uma instancia nova deve soar praticamente igual ao bypass do slot. Ver [[decisions/0003-Factory-Default-Neutral]].
- Checkpoint atual: `v0.1.0-wheel-prototype`.
- Modulos funcionais/validados para checkpoint: [[modules/Channel-EQ]], [[modules/FET-Compressor]], [[modules/Air-Exciter]], [[modules/Delay]] e [[modules/Reverb]].
- [[modules/Reverb]] foi refinado por A/B local e esta musical para prototipo, mas ainda continua em evolucao.
- UI Wheel esta integrada com ponteiros interativos, hotspots e foco em hardware mapping.
- IDs APVTS sao contrato de automacao/hardware mapping e devem permanecer estaveis. Ver [[04-Parameters-APVTS]] e [[07-Hardware-Mapping-Plan]].
- `ReferenceAudio/` e local, ignorado pelo Git e nao deve ser publicado.

## Proximas prioridades

1. Melhorar Delay inspirado em delay analogico/tape.
2. Melhorar Compressor.
3. Melhorar Air.
4. Substituir arte de referencia/prototipo por arte original antes de release publico.
5. Melhorar instalador e fluxo de GitHub Release.

## Notas criticas

- Delay nao deve ter `dryWet`, `output` ou `analog`; a quantidade e controlada por `delaySend` / ECHO. Ver [[decisions/0002-Delay-No-DryWet-Output-Analog]].
- A UI deve continuar host-automation/controller-mapping friendly. Ver [[decisions/0004-F1-Wheel-UI-And-Hardware-Mapping]].
- Esta versao e um checkpoint documentado; o projeto ainda vai evoluir bastante antes de um release publico final.
