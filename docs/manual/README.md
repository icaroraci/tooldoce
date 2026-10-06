# Manual da libnfe

Este manual documenta o tooldoce/libnfe: estruturas XML, montagem, validação, assinatura e comunicação. Base conferida em **05/10/2026**: libnfe **1.0.0-rc4**, commit `1dd412eebca80dd80d884b3f03a1a31cfaf42279`, leiaute NF-e/NFC-e 4.00, pacote oficial **PL010f v1.04**, de 31/08/2026.

Há páginas para as **171 estruturas do índice de diagramas**, com finalidade, campos/atributos, ocorrências, escolhas, API, C executável, XML correspondente, navegação e fontes oficiais. [NFref](NFe/infNFe/ide/NFref.md) e [refNF](NFe/infNFe/ide/NFref/refNF.md) permanecem como primeira referência interna conferida segundo [Como documentar](../COMO_DOCUMENTAR.md).

## Começar pelo fluxo de uso

| Guia | Conteúdo |
|---|---|
| [Bases oficiais e transições](BASES.md) | XSD, MOC, NT, vigência e diferenças entre anúncio e implementação |
| [Instalação](INSTALACAO.md) | Dependências, compilação, instalação, pkg-config e schemas |
| [API e memória](API.md) | Cópia, empréstimo, posse, caminhos e erros |
| [Montagem da nota](EMISSAO.md) | Pais/filhos, itens, tributos, totais e XML |
| [Validação](VALIDACAO.md) | XSD, regras locais e limites de cobertura |
| [Diagnósticos XSD](DIAGNOSTICOS.md) | Tag, caminho, valor recebido e restrição extraída automaticamente do validador |
| [Assinatura](ASSINATURA.md) | A1, XMLDSig e verificação |
| [Serviços](SERVICOS.md) | Endpoints, SOAP/TLS, recibos, cStat e protocolos |
| [Eventos](EVENTOS.md) | Cancelamento, substituição e CC-e |
| [Inutilização](INUTILIZACAO.md) | Faixas, pedido, assinatura e ProcInutNFe |
| [Contingência](CONTINGENCIA.md) | Modalidades, SVC, NFC-e offline e conciliação |
| [Exemplos](EXEMPLOS.md) | C, XML, catálogo e reprodução |
| [Ferramentas](FERRAMENTAS.md) | Regeneração, atualização e obsolescência |

## Consultar uma estrutura ou função

O [índice XML completo](INDICE.md) segue a árvore dos grupos. Comece por [NFe](NFe.md) → [infNFe](NFe/infNFe.md) → [ide](NFe/infNFe/ide.md), ou consulte o item em [det](NFe/infNFe/det.md). As páginas dos pais explicam sequências/escolhas; as dos filhos detalham seus campos e acesso.

O [índice da API](FUNCOES.md) cobre os 35 headers instalados, com contratos públicos, tipos e enumerações. Símbolos internos não são API de integração. O [índice de diagramas](../diagramas/README.md) liga às páginas editoriais preservando os SVGs.

## Cobertura e estado

O [estado verificável](ESTADO.md) e o [manifesto](schema-manifest.json) registram as bases. Hashes iguais aparecem uma vez; bases distintas mostram dois pares de rótulo/hash. A detecção estrutural é conservadora por conjunto de dependências: mudanças de XSD podem marcar todas as páginas como obsoletas, preservando o texto. Regenerar não equivale a revisar; veja [atualização](FERRAMENTAS.md).

[BASES.md](BASES.md) identifica campos anunciados ainda ausentes do XSD/API adotados, especialmente a NT 2026.008. Regras novas não são verificadas automaticamente pelo validador local. Exemplos sintéticos comprovam estrutura, sem enquadramento fiscal, assinatura real ou autorização.

O manual cobre o projeto implementado. NFC-e tem fluxo próprio na [libnfc](https://github.com/icaroraci/libnfc), MDF-e na [libmdf](https://github.com/icaroraci/libmdf); CT-e/NFS-e seguem o [roteiro](../VISAO.md). A infraestrutura compartilhada é descrita em [Ferramentas](FERRAMENTAS.md).

## Documentação de apoio

- [Convenções](../CONVENCOES.md), [contribuição](../../CONTRIBUTING.md) e [dúvidas](../DUVIDAS.md).
- [Visão](../VISAO.md) e [histórico de versões](../../CHANGELOG.md).
- [Homologação](../HOMOLOGACAO.md), [TLS](../TLS.md) e [webservices](../WEBSERVICES.md).
- [Motor de grupos](../ESQUEMAS.md) e [novo subprojeto](../NOVO-SUBPROJETO.md).

Próximo: [Bases oficiais](BASES.md)
