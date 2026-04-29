# Reverb Issues And Future Fix

## Estado atual

O ReverbModule esta funcional e possui parametros completos, mas o usuario indicou explicitamente que nao gostou do resultado sonoro atual.

Conclusao: tratar o Reverb atual como **funcional/prototipo**, nao como modulo aprovado para release final.

## Problema percebido

- Resultado ruim/fraco em qualidade.
- Necessidade de reverb mais simples, musical, claro e controlavel.
- Tail deve ser mais previsivel.
- Early reflections devem ajudar sem embolar.
- Wet deve ter melhor separacao do dry.

## Decisao temporaria

Nao mexer no Reverb nesta fase de documentacao. A correcao deve ser planejada como fase propria, preferencialmente [[09-Roadmap|Fase 12: Reverb V2]].

## Ideia futura: Reverb V2

Criar uma versao V2 simples e estavel, possivelmente usando:

- `juce::Reverb` ou `juce::dsp::Reverb` como base.
- Predelay proprio.
- Early reflections simples.
- Filtros wet de low cut e high cut.
- Damping simples.
- Ducking suave.
- Freeze controlado.
- Width e mono bass.

Preservar IDs APVTS existentes sempre que possivel. Se algum parametro novo for inevitavel, documentar antes em [[04-Parameters-APVTS]] e criar uma ADR.

## Objetivo sonoro da V2

- Digital limpo.
- Facil de controlar.
- Bom em vocal e instrumental.
- Mix 15% a 30% ja deve soar util.
- OFF e MIX 0% devem permanecer neutros.

## Links relacionados

- [[modules/Reverb]]
- [[05-DSP-Modules]]
- [[04-Parameters-APVTS]]
- [[decisions/0003-Factory-Default-Neutral]]
