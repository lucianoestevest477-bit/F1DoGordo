# Reverb

## Objetivo

Adicionar ambiencia algorithmic stereo com predelay, early/late, damping, filtros wet, modos, cores, ducking, freeze, width e mono bass.

## Parametros

- `reverbEnabled`
- `reverbMix`
- `reverbPredelayMs`
- `reverbDecaySec`
- `reverbSize`
- `reverbAttack`
- `reverbEarly`
- `reverbLate`
- `reverbLowCutHz`
- `reverbHighCutHz`
- `reverbLowDampHz`
- `reverbHighDampDb`
- `reverbDiffusionEarly`
- `reverbDiffusionLate`
- `reverbModRate`
- `reverbModDepth`
- `reverbWidth`
- `reverbMode`
- `reverbColor`
- `reverbFreeze`
- `reverbDucking`
- `reverbTempoSyncPredelay`
- `reverbMonoBass`

## Estado atual

Funcional/prototipo. O usuario nao gostou do resultado sonoro atual, entao este modulo nao deve ser considerado aprovado para release final.

## Testes no FL

- REVERB OFF retorna original.
- MIX 0% retorna original.
- MIX acima de 0% adiciona reverb.
- DECAY aumenta cauda.
- PREDELAY altera atraso inicial.
- HIGH CUT e LOW CUT afetam wet.
- WIDTH altera stereo.
- DUCKING reduz wet durante sinal direto.
- FREEZE segura cauda.

## Pendencias

- Criar Reverb V2 ou refatorar profundamente o voicing atual.
- Priorizar som digital simples, claro e musical.
- Considerar `juce::Reverb` ou `juce::dsp::Reverb` como base.
- Preservar IDs existentes se possivel.

## Links relacionados

- [[../08-Reverb-Issues-And-Future-Fix]]
- [[../04-Parameters-APVTS]]
- [[../09-Roadmap]]
