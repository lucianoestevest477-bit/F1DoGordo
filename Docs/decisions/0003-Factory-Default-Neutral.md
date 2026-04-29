# ADR 0003: Factory default neutro

## Status

Aceita.

## Contexto

O usuario confirmou que uma instancia nova estava alterando o audio ao ser inserida no mixer. Para um plugin profissional, o default deve ser seguro e previsivel.

## Decisao

Factory default deve ser neutro:

- `inputGainDb = 0 dB`
- `outputGainDb = 0 dB`
- `masterWidth = 100%`
- `globalBypass = false`
- Modulos off por default.
- Mixes/sends em 0% quando necessario para garantir pass-through.

## Consequencias

- Inserir o plugin deve soar praticamente igual ao bypass do slot.
- O usuario ativa os modulos manualmente.
- LEDs da UI devem refletir modulos OFF em nova instancia.
- Testes de regressao devem sempre incluir instancia nova no FL Studio.

