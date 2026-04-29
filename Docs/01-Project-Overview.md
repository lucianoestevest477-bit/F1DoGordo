# Project Overview

## Identidade

- Nome do plugin: **F1 do Gordo**
- Nome do projeto: **F1DoGordo**
- Fabricante: **Gordo Audio**
- Stack: **C++20 + JUCE + CMake + VST3**
- Host principal: **FL Studio**

## Conceito

O F1 do Gordo e um rack de efeitos profissional com interface inspirada em volante/cockpit de Formula 1. A proposta e transformar controles de mixagem e efeitos em uma superficie visual que pareca um volante interativo usado para controlar um rack de audio.

O projeto tambem considera uma futura integracao com botoes e controles fisicos de volante/controlador. Por isso, os nomes de controles devem permanecer curtos, claros e amigaveis para automacao do host.

## Estado funcional

- Audio passa limpo quando os modulos estao desligados/default neutro.
- Parametros sao expostos via APVTS.
- Automacao do host deve permanecer estavel por meio de IDs de parametros imutaveis.
- UI e customizada, sem visual padrao do JUCE.
- VST3 abre e foi testado no FL Studio ao longo das fases.

## Principios

- Nao copiar logos, marcas, textos, simbolos, presets ou layout exato de produtos comerciais.
- Preservar compatibilidade de sessoes: IDs de parametros nao devem ser renomeados sem uma razao extrema.
- Cada controle importante deve mapear uma funcao clara.
- Foco sonoro: profissional, musical, controlavel e seguro.

