# Manual da libnfe

Este manual reúne o significado dos grupos XML, as restrições do XSD e o uso efetivo da API C. Comece pela página do pai e prossiga ao filho: [NFref](NFe/infNFe/ide/NFref.md) → [refNF](NFe/infNFe/ide/NFref/refNF.md). Estas duas páginas constituem a primeira referência interna conferida segundo [Como documentar](../COMO_DOCUMENTAR.md).

## Base e cobertura

Revisão de 05/10/2026: libnfe `1.0.0-rc4`, commit `94aca18ee130df3070f8fe421c88d7f30b632a2d`; leiaute NF-e 4.00, pacote oficial **PL010f v1.04**, publicado em 31/08/2026. Os cinco arquivos oficiais adotados no projeto foram comparados byte a byte com o pacote disponibilizado pelo Portal Nacional nessa revisão. Consulte o [estado verificável das páginas](ESTADO.md) e o [inventário da base revisada](schema-manifest.json).

| Caminho XML | Conteúdo | Cobertura |
|---|---|---|
| `NFe/infNFe/ide/NFref` | Escolha do documento, repetição e ligação à identificação | [Página completa](NFe/infNFe/ide/NFref.md) |
| `NFe/infNFe/ide/NFref/refNF` | Campos da nota modelo 1/1A e nota modelo 2, API e serialização | [Página completa](NFe/infNFe/ide/NFref/refNF.md) |
| Demais estruturas | Diagramas gerados do XSD | [Índice de diagramas](../diagramas/README.md); páginas editoriais pendentes |

A presença de um diagrama não indica documentação completa nem implementação da API. O [TODO](../../TODO.md) acompanha implementação; o estado editorial é independente.

## Uso da biblioteca

- [Instalação e dependências](../../README.md): preparação do ambiente.
- [Convenções da API e código](../CONVENCOES.md): tipos, retornos e estilo.
- [Exemplo compilável de referências](../../examples/referenciar_nf.c): duas ocorrências de NFref, API específica, genérica e serialização isolada.
- [Exemplo de montagem da NF-e](../../examples/gerar_nfe.c): integração do ide à nota completa.
- [Exemplo de assinatura](../../examples/assinar_nfe.c): etapa posterior à montagem.

## Atualização e revisão

Execute `python3 tools/gerar_diagramas.py --todo` após atualizar os schemas. O gerador mantém os caminhos dos SVGs existentes, preserva o texto em `docs/manual/` e atualiza os avisos e o relatório [ESTADO.md](ESTADO.md).

Nesta primeira entrega, a detecção é **conservadora por conjunto de XSDs**: qualquer mudança no leiaute ou numa dependência transitiva marca as duas páginas como obsoletas, mesmo que a alteração esteja em outro grupo. Comentários, indentação e prefixos de namespace equivalentes não mudam a assinatura; mudanças em tipos compartilhados, anotações, atributos, padrões e contexto ficam cobertas. O relatório identifica os arquivos alterados, sem afirmar que localizou a diferença em uma tag específica. Nós removidos preservam a página e deixam de incorporar o SVG atual.

A geração atualiza `base_atual`, nunca `assinatura_revisada` ou `fontes_revisadas`. Depois de conferir texto, API, normas, exemplos e XML contra a nova base, o revisor atualiza esses dois valores no inventário com os da base atual e registra pacote, biblioteca e data. Regenerar novamente não remove um aviso de obsolescência por si só. Mudanças de API, MOC ou Notas Técnicas exigem revisão própria, mesmo quando o XSD permanece igual.

Para acompanhar novas publicações, consulte [schemas oficiais](https://www.nfe.fazenda.gov.br/portal/listaConteudo.aspx?tipoConteudo=Qd+N9D0fbCI=) e [Notas Técnicas](https://www.nfe.fazenda.gov.br/portal/listaConteudo.aspx?tipoConteudo=04BIflQt1aY=). O XSD mais recente determina a estrutura; as normas e seus cronogramas determinam aplicação e vigência. A consulta das NT 2026.007 v1.10 e 2026.008 v1.00, publicadas em 01/10/2026, não identificou alterações em NFref/refNF; a atualização pertinente de referenciamento está na NT 2025.002 v1.52.

Os exemplos são fragmentos sintéticos validados estruturalmente, sem assinatura ou autorização pela SEFAZ.
