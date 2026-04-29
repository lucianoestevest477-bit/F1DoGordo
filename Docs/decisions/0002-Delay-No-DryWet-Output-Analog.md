# ADR 0002: Delay sem dryWet, output ou analog

## Status

Aceita.

## Contexto

O Delay deve funcionar como send interno controlado por ECHO / `delaySend`, e nao como um modulo com dry/wet tradicional.

## Decisao

Nao criar parametros ou controles de Delay chamados:

- `dryWet`
- `output`
- `analog`

O comportamento desejado e:

```text
output = dry + wetDelay * delaySend
```

## Consequencias

- A quantidade do delay e sempre mapeada por `delaySend`.
- O macro ECHO continua simples para hardware mapping.
- Evita conflito conceitual com outros delays comerciais.
- Presets e automacao devem depender de `delaySend`, nao de dry/wet.

