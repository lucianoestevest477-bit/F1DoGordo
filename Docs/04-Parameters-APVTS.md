# Parameters APVTS

Todos os parametros do plugin sao criados em `Parameters::createParameterLayout()` e expostos por `AudioProcessorValueTreeState`.

## Regra principal

IDs de parametros devem permanecer estaveis. Eles sao o contrato com:

- Automacao do host.
- Sessoes antigas.
- Presets futuros.
- Mapeamento para hardware/controladores.

## Globais

- `inputGainDb`
- `outputGainDb`
- `globalBypass`
- `channelEnabled`
- `compEnabled`
- `airEnabled`
- `delayEnabled`
- `reverbEnabled`
- `channelMix`
- `compMix`
- `airMix`
- `delaySend`
- `reverbSend`
- `masterWidth`

Default de fabrica deve ser neutro:

- input/output em 0 dB.
- masterWidth em 100%.
- modulos desligados.
- mixes/sends em 0%, exceto controles internos cujo default musical nao afete audio enquanto o modulo esta off/mix zero.

## Channel / EQ

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
- `channelEnabled`

## FET Compressor

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
- `compEnabled`

## Air Exciter

- `airAmount`
- `airMidAir`
- `airHighAir`
- `airFrequencyHz`
- `airDrive`
- `airDensity`
- `airDynamic`
- `airDeEss`
- `airTone`
- `airOutputDb`
- `airMix`
- `airEnabled`

Nota: `airDrive` e o ID estavel. Na UI ele e tratado como **INTENSITY**.

## Delay

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
- `delayEnabled`

Restricao permanente nesta linha de design: Delay nao deve receber parametros `dryWet`, `output` ou `analog`. A quantidade do delay e controlada por `delaySend` / macro ECHO.

## Reverb

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
- `reverbEnabled`

`reverbSend` permanece como macro legado, mas a UI global SPACE esta ligada ao `reverbMix` ativo.

