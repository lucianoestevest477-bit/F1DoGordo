# Delay

## Objetivo

Delay interno estilo send: adiciona repeticoes ao sinal seco sem criar controles de dry/wet, output ou analog.

## Parametros

- `delayEnabled`
- `delayTimeMs`
- `delaySyncEnabled`
- `delayNoteDivision`
- `delayMode`
- `delayFeedback`
- `delayLeftTime`
- `delayRightTime`
- `delayLink`
- `delayLoFi`
- `delayModDepth`
- `delayModRate`
- `delayHighPassHz`
- `delayLowPassHz`
- `delayDucking`
- `delayFreeze`
- `delayStereoWidth`
- `delaySend`

## Estado atual

Funcional. Opera como send interno:

```text
output = dry + wetDelay * delaySend
```

## Testes no FL

- DELAY OFF retorna original.
- SEND 0% retorna original.
- SEND acima de 0% adiciona delay.
- FEEDBACK aumenta repeticoes.
- TIME muda espacamento.
- SYNC acompanha BPM quando disponivel.
- PingPong alterna L/R.
- HPF/LPF filtram o wet/feedback.
- DUCKING limpa o delay durante sinal direto.
- FREEZE segura a cauda.
- WIDTH altera largura.

## Pendencias

- Testar em BPMs extremos.
- Refinar LoFi e modulacao se necessario.
- Criar presets de slap, dotted, ping-pong e ambience.

## Restricao

Nao adicionar parametros ou controles chamados:

- `dryWet`
- `output`
- `analog`

