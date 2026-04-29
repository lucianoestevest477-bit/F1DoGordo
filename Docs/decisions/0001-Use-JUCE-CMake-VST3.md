# ADR 0001: Use JUCE + CMake + VST3

## Status

Aceita.

## Contexto

O projeto precisava de uma base profissional em C++20 para plugin de audio, com build reproduzivel e formato compatibile com FL Studio.

## Decisao

Usar:

- C++20
- JUCE
- CMake
- VST3

## Consequencias

- Build pode ser configurado via CMake.
- JUCE fornece infraestrutura de audio, GUI e plugin wrappers.
- VST3 e o formato principal de teste no Windows/FL Studio.
- O projeto deve preservar compatibilidade de CMake e Visual Studio Build Tools.

