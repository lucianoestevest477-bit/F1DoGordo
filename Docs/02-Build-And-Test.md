# Build And Test

## Requisitos

- Visual Studio 2022 Build Tools
- CMake
- Git
- FL Studio
- Obsidian, opcional, para abrir este vault de documentacao

## Configurar build

No PowerShell, a partir da raiz do projeto:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

## Build Release

```powershell
cmake --build build --config Release
```

Tambem e possivel usar MSBuild diretamente:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe' build\F1DoGordo.sln /p:Configuration=Release /p:Platform=x64 /m
```

## Copiar VST3

```powershell
Copy-Item -Recurse -Force "C:\Users\b\Desktop\F1DoGordo\build\F1DoGordo_artefacts\Release\VST3\F1 do Gordo.vst3" "C:\Program Files\Common Files\VST3\"
```

## Testar no FL Studio

1. Compile em Release.
2. Copie o VST3 para `C:\Program Files\Common Files\VST3\`.
3. Abra o FL Studio.
4. Va em `Options > Manage plugins`.
5. Confirme que `C:\Program Files\Common Files\VST3` esta na lista de busca.
6. Clique em `Find plugins`.
7. Abra `F1 do Gordo` em um mixer insert.
8. Teste uma instancia nova antes de mexer nos controles.
9. Compare slot bypass ON/OFF para confirmar default neutro.

## Testes rapidos recomendados

- Instancia nova: deve soar praticamente igual ao bypass do slot.
- Channel OFF: passa limpo.
- Channel ON + HPF/LPF/EQ/Drive: altera o som.
- COMP ON + threshold/input/ratio: comprime e GR responde.
- AIR ON + amount/intensity/mix: adiciona brilho/harmonicos.
- DELAY ON + send/time/feedback: adiciona repeticoes sem dryWet/output/analog.
- REVERB ON + mix/decay/size: adiciona ambiencia, mas sera revisado futuramente.

