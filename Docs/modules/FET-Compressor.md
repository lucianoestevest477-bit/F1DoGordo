# FET Compressor

## Objetivo

Compressor rapido inspirado em comportamento FET generico, sem copiar algoritmo proprietario. Deve oferecer controle de punch, threshold, ratio, attack/release, mix paralelo e GR.

## Parametros

- `compEnabled`
- `compInputDb`
- `compThresholdDb`
- `compOutputDb`
- `compAttack`
- `compRelease`
- `compRatio`
- `compMix`
- `compSidechainHpHz`
- `compRevision`
- `compNoiseMode`

## Estado atual

Funcional. Usa detector estavel, link stereo, threshold, input/output gain, mix paralelo, sidechain HPF e medidor de gain reduction.

## Testes no FL

- COMP OFF retorna sinal original.
- MIX 0% retorna sinal original.
- Threshold baixo comprime mais.
- Input alto comprime mais.
- Ratio maior comprime mais.
- ALL e mais agressivo.
- GR responde quando ha compressao.

## Pendencias

- Refinar carater das revisoes Clean/Blue/Black/Hot.
- Avaliar ruido opcional, mantendo desligavel.
- Criar presets de uso: vocal, drum bus, parallel crush.

