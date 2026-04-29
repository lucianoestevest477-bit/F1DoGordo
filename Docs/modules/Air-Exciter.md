# Air Exciter

## Objetivo

Adicionar brilho e harmonicos controlados, com separacao entre ar medio-alto e ar superior, mantendo defaults neutros.

## Parametros

- `airEnabled`
- `airAmount`
- `airMidAir`
- `airHighAir`
- `airFrequencyHz`
- `airDrive` (UI: INTENSITY)
- `airDensity`
- `airDynamic`
- `airDeEss`
- `airTone`
- `airOutputDb`
- `airMix`

## Estado atual

Funcional e refinado. `airDrive` permanece como ID interno estavel, mas o label de usuario e INTENSITY.

## Testes no FL

- AIR OFF retorna sinal original.
- MIX 0% retorna sinal original.
- AMOUNT aumenta ar geral.
- MID AIR aumenta presenca.
- HIGH AIR aumenta brilho superior.
- INTENSITY/DENSITY aumentam harmonicos.
- DE-ESS segura excesso de agudo/sibilancia.
- OUTPUT compensa volume.

## Pendencias

- Ajustar voicing fino por tipo de fonte.
- Criar presets seguros.
- Avaliar medicao visual de energia high band no futuro.

