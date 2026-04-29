# ADR 0004: UI de volante F1 e hardware mapping

## Status

Aceita.

## Contexto

A direcao visual evoluiu de plugin com tema de F1 para volante de F1 interativo que controla um rack de audio. O usuario tambem pretende integrar controles fisicos de volante/controlador.

## Decisao

A UI deve:

- Parecer uma peca fisica de volante/cockpit.
- Usar preto denso, carbono, metal escuro e botoes neon.
- Manter macros globais visiveis e faceis de mapear.
- Manter botoes de modulo claros.
- Evitar controles funcionais sem APVTS.
- Nao copiar logos, marcas, textos, simbolos ou layout exato de referencias.

## Consequencias

- `F1Dashboard`, `F1TopBar`, `GordoKnob`, `GordoButton`, `GordoLED` e `GordoMeter` sao areas centrais de evolucao visual.
- Design deve preservar legibilidade em 1200x700.
- Um botao fisico deve mapear uma funcao clara.
- IDs APVTS devem permanecer estaveis para automacao e mapeamento.

