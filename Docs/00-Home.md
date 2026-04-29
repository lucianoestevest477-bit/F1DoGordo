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
- Modulos funcionais/validados: [[modules/Channel-EQ]], [[modules/FET-Compressor]], [[modules/Air-Exciter]] e [[modules/Delay]].
- [[modules/Reverb]] existe e processa audio, mas o usuario nao gostou do resultado sonoro atual. Tratar como funcional/prototipo, nao como modulo aprovado.
- UI esta em direcao de volante/cockpit F1 interativo, escura, com carbono/metal, hub central, botoes neon e foco em hardware mapping.
- IDs APVTS sao contrato de automacao/hardware mapping e devem permanecer estaveis. Ver [[04-Parameters-APVTS]] e [[07-Hardware-Mapping-Plan]].

## Proximas prioridades

1. Fase 10: plano pratico de hardware mapping / controller mapping.
2. Fase 11: sistema inicial de presets.
3. Fase 12: Reverb V2 simples, estavel e musical para substituir/refinar o Reverb atual.
4. Fase 13: polimento final de UI e legibilidade.
5. Fase 14: pacote de release / installer.

## Notas criticas

- Delay nao deve ter `dryWet`, `output` ou `analog`; a quantidade e controlada por `delaySend` / ECHO. Ver [[decisions/0002-Delay-No-DryWet-Output-Analog]].
- A UI deve continuar host-automation/controller-mapping friendly. Ver [[decisions/0004-F1-Wheel-UI-And-Hardware-Mapping]].
- Reverb V2 e pendencia prioritaria porque o resultado atual nao foi aprovado pelo usuario. Ver [[08-Reverb-Issues-And-Future-Fix]].
