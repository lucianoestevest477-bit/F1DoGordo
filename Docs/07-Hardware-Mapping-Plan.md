# Hardware Mapping Plan

## Objetivo

Permitir que o F1 do Gordo seja controlado de forma natural por botoes, knobs e seletores fisicos de um volante/controlador.

## Macros principais

Estes devem permanecer faceis de automatizar e mapear:

- INPUT
- TONE
- PUNCH
- AIR
- ECHO
- SPACE
- WIDTH
- OUTPUT

## Botoes principais

- CHANNEL ON
- COMP ON
- AIR ON
- DELAY ON
- REVERB ON
- BYPASS

## Regras de mapeamento

- IDs de parametros estaveis.
- Todos os controles funcionais devem usar APVTS.
- Host automation friendly.
- Um botao fisico = uma funcao clara.
- Evitar submenus para funcoes essenciais.
- Evitar controles decorativos que parecam funcionais.
- Labels de UI podem mudar, mas IDs APVTS nao devem mudar. Ver [[04-Parameters-APVTS]].

## Possivel mapeamento inicial

| Controle fisico | Parametro / acao |
|---|---|
| Knob 1 | `inputGainDb` |
| Knob 2 | `channelMix` / TONE |
| Knob 3 | `compMix` / PUNCH |
| Knob 4 | `airMix` / AIR |
| Knob 5 | `delaySend` / ECHO |
| Knob 6 | `reverbMix` / SPACE |
| Knob 7 | `masterWidth` / WIDTH |
| Knob 8 | `outputGainDb` |
| Button 1 | `globalBypass` |
| Button 2 | `channelEnabled` |
| Button 3 | `compEnabled` |
| Button 4 | `airEnabled` |
| Button 5 | `delayEnabled` |
| Button 6 | `reverbEnabled` |

Nota atual: o macro SPACE da UI global controla `reverbMix`. `reverbSend` continua no APVTS como ID legado, mas nao deve ser usado para criar confusao de mapeamento.

## Pendencias futuras

- Definir estrategia MIDI Learn ou depender de automacao do host.
- Criar presets de mapeamento para FL Studio.
- Testar com hardware real.
- Documentar ranges recomendados para performance ao vivo.
