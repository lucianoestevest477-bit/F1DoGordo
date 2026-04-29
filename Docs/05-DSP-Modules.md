# DSP Modules

## Resumo atual

- Channel/EQ: funcional.
- FET Compressor: funcional.
- Air Exciter: funcional.
- Delay: funcional.
- Reverb: funcional/prototipo, mas o usuario nao gostou do resultado sonoro atual. Precisa Reverb V2 ou revisao profunda de qualidade/voicing.

## Validacao no FL Studio

Ao longo das fases, o usuario validou:

- Plugin abre no FL Studio.
- VST3 e gerado.
- Channel/EQ altera graves, agudos, HPF, LPF, drive e mix.
- Compressor responde a threshold/input/ratio e mostra GR.
- Air Exciter funciona com AMOUNT, MID AIR, HIGH AIR, INTENSITY, DENSITY, DYNAMIC, DE-ESS, MIX e OUTPUT.
- Delay funciona com SEND, TIME, FEEDBACK, SYNC, modos, filtros, ducking, freeze e width.
- Factory default neutro evita coloracao ao inserir nova instancia.
- Reverb abre e processa, mas nao deve ser tratado como aprovado/final.

## Detalhes por modulo

- [[modules/Channel-EQ]]
- [[modules/FET-Compressor]]
- [[modules/Air-Exciter]]
- [[modules/Delay]]
- [[modules/Reverb]]

## Pronto versus futuro

Pronto para continuidade: Channel/EQ, FET Compressor, Air Exciter, Delay, APVTS e factory default neutro.

Futuro/prioridade: Reverb V2, presets, hardware mapping pratico e polimento final da UI. Ver [[09-Roadmap]].
