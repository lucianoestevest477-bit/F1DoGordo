# F1 do Gordo

Esqueleto inicial de plugin de audio profissional em C++20 usando JUCE, CMake e VST3.

Fabricante: **Gordo Audio**  
Projeto: **F1DoGordo**  
Plugin: **F1 do Gordo**

## Estado atual

- Base compilavel em C++20 com JUCE + CMake.
- Formato VST3.
- Audio pass-through limpo por padrao.
- Parametros expostos via `AudioProcessorValueTreeState`.
- Estrutura modular preparada para:
  - Channel / EQ
  - FET Compressor
  - Air Exciter
  - Delay
  - Reverb
- UI custom inicial inspirada em cockpit/volante tecnico de F1, sem copiar interfaces, logos, presets ou layouts comerciais.
- Janela minima de 1200x700 e redimensionavel.

## Compilar no Windows

Requisitos:

- Visual Studio 2022 com workload "Desktop development with C++"
- CMake 3.22 ou superior
- Git

Passos:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

O CMake baixa o JUCE automaticamente via `FetchContent`.

O VST3 final sera gerado em uma pasta parecida com:

```text
build/F1DoGordo_artefacts/Release/VST3/F1 do Gordo.vst3
```

## Testar no FL Studio

1. Compile em `Release`.
2. Copie `F1 do Gordo.vst3` para:

```text
C:\Program Files\Common Files\VST3
```

3. Abra o FL Studio.
4. Va em `Options > Manage plugins`.
5. Confirme que `C:\Program Files\Common Files\VST3` esta na lista de busca.
6. Clique em `Find plugins`.
7. Abra o plugin `F1 do Gordo` em um mixer insert.
8. Toque audio atraves do insert e confirme que o sinal passa limpo.

## Parametros globais

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

Observacao: o Delay nesta etapa nao possui parametros `dryWet`, `output` ou `analog`.

## Proxima etapa sugerida

Implementar DSP real por modulo mantendo a ordem:

```text
Input -> Channel -> FET Compressor -> Air Exciter -> Delay -> Reverb -> Output
```
