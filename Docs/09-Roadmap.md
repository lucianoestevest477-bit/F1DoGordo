# Roadmap

## Fase 9: Documentacao

Status: concluida inicialmente em `7f0a6b9 Add Obsidian documentation vault`; este vault deve ser mantido vivo a cada fase.

- Criar Obsidian vault em `Docs/`.
- Documentar arquitetura, parametros, modulos, UI, hardware mapping e decisoes.
- Revisar documentacao sempre que parametros, fluxo, UI ou comportamento mudarem.

## Fase 10: Hardware Mapping

- Definir estrategia de mapeamento.
- Validar nomes de automacao no FL Studio.
- Criar guia de mapeamento para volante/controlador.

## Fase 11: Presets

- Criar arquitetura inicial de presets.
- Presets devem respeitar factory default neutro.
- Evitar nomes/copias de produtos comerciais.

## Fase 12: Reverb V2

- Revisar ReverbModule.
- Corrigir o ponto principal: o usuario nao gostou do Reverb atual.
- Buscar um reverb digital simples, claro e estavel.
- Preservar IDs existentes sempre que possivel.
- Documentar qualquer mudanca em [[08-Reverb-Issues-And-Future-Fix]].

## Fase 13: Polimento final UI

- Ajustar legibilidade.
- Conferir 1200x700.
- Refinar contraste e estados ON/OFF.
- Verificar que todos os controles principais seguem hardware mapping friendly.

## Fase 14: Installer / Release package

- Definir empacotamento VST3.
- README de instalacao.
- Versao e changelog de release.
- Testes finais no FL Studio.
