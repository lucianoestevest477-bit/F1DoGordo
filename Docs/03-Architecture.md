# Architecture

## Estrutura geral

```text
F1DoGordo/
  CMakeLists.txt
  README.md
  Source/
    PluginProcessor.*
    PluginEditor.*
    Parameters.*
    DSP/
    UI/
  Docs/
```

## Fluxo de audio

```text
Input -> Channel -> FET Compressor -> Air Exciter -> Delay -> Reverb -> Output
```

Este fluxo e fixo e deve ser preservado ate existir uma decisao explicita para muda-lo.

## PluginProcessor

`PluginProcessor` e responsavel por:

- Criar e manter o `AudioProcessorValueTreeState`.
- Obter ponteiros atomicos para parametros usados no processamento.
- Preparar, resetar e chamar os modulos DSP.
- Ler BPM do host para Delay e Reverb quando necessario.
- Atualizar medidores de input/output e gain reduction.

## PluginEditor

`PluginEditor` e responsavel por:

- Criar a janela redimensionavel.
- Instanciar `F1TopBar` e `F1Dashboard`.
- Conectar sliders e botoes ao APVTS com `SliderAttachment` e `ButtonAttachment`.
- Atualizar medidores e estados visuais via timer.

## Parameters

`Parameters.h/cpp` centralizam os IDs e a criacao de parametros APVTS. Os IDs devem permanecer estaveis para:

- Automacao no FL Studio.
- Compatibilidade de sessoes.
- Futura integracao com controles fisicos.

## DSP modules

Todos os modulos seguem a ideia de `prepare`, `reset`, `process` e ponteiros para parametros:

- `ChannelModule`
- `FETCompressorModule`
- `AirExciterModule`
- `DelayModule`
- `ReverbModule`

## UI components

Componentes principais:

- `F1Theme`: cores e fundo carbono.
- `F1TopBar`: ponte/status superior.
- `F1Dashboard`: shell visual, paginas e layout de controles.
- `GordoKnob`: seletor rotativo custom.
- `GordoButton`: botao fisico custom.
- `GordoLED`: LED de status.
- `GordoMeter`: medidores IN/OUT/GR.

