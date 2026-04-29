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
- Factory default neutro: uma instancia nova deve soar praticamente igual ao bypass do slot.
- Modulos funcionais: Channel/EQ, FET Compressor, Air Exciter, Delay e Reverb.
- Reverb esta funcional, mas o usuario marcou a qualidade atual como ponto de revisao futura.
- UI esta em direcao de volante/cockpit F1 interativo, escura, com carbono/metal, hub central, botoes neon e foco em hardware mapping.

## Proximas prioridades

1. Fase 10: plano pratico de hardware mapping / controller mapping.
2. Fase 11: sistema inicial de presets.
3. Fase 12: Reverb V2 simples, estavel e musical.
4. Fase 13: polimento final de UI e legibilidade.
5. Fase 14: pacote de release / installer.

