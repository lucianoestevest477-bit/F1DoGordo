# Channel / EQ

## Objetivo

Modulo de entrada tonal para ganho, fase, filtros, EQ de 4 bandas, drive leve e mix paralelo.

## Parametros

- `channelEnabled`
- `channelInputTrimDb`
- `phaseInvert`
- `highPassHz`
- `lowPassHz`
- `lowGainDb`
- `lowFreqHz`
- `lowMidGainDb`
- `lowMidFreqHz`
- `highMidGainDb`
- `highMidFreqHz`
- `highGainDb`
- `highFreqHz`
- `drive`
- `channelMix`

## Estado atual

Funcional. Implementado com filtros e EQ via `juce::dsp` onde apropriado, drive suave e mix paralelo.

## Testes no FL

- Channel OFF retorna sinal limpo.
- MIX 0% retorna sinal original.
- HPF alto corta graves.
- LPF baixo corta agudos.
- LOW/HIGH alteram graves/agudos.
- DRIVE adiciona saturacao leve.

## Pendencias

- Refinar Q/curvas se necessario.
- Criar presets de Channel.
- Testar em material vocal, baixo e master bus.

