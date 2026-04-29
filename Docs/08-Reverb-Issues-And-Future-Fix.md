# Reverb Issues And Future Fix

## Estado atual

O ReverbModule esta funcional e possui parametros completos, mas o usuario indicou que o resultado sonoro atual ainda nao esta no nivel desejado.

## Problema percebido

- Resultado ruim/fraco em qualidade.
- Necessidade de reverb mais simples, musical, claro e controlavel.
- Tail deve ser mais previsivel.
- Early reflections devem ajudar sem embolar.
- Wet deve ter melhor separacao do dry.

## Decisao temporaria

Nao mexer no Reverb nesta fase de documentacao.

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

## Objetivo sonoro da V2

- Digital limpo.
- Facil de controlar.
- Bom em vocal e instrumental.
- Mix 15% a 30% ja deve soar util.
- OFF e MIX 0% devem permanecer neutros.

