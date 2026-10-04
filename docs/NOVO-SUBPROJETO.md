# Como iniciar um novo subprojeto (NFC-e, MDF-e, CT-e...)

Roteiro para começar a biblioteca de um novo documento fiscal sobre a libnfe, tirado do que aconteceu na [libnfc](https://github.com/icaroraci/libnfc) (NFC-e, modelo 65) e na [libmdf](https://github.com/icaroraci/libmdf) (MDF-e, modelo 58). Cada fase diz o que entra, o que sai e quem faz: algumas coisas só o mantenedor consegue fazer (criar repositório, publicar versão, homologar com o certificado da empresa).

## O que os documentos têm em comum, e o que não têm

Os documentos da família do Portal Nacional (NF-e, NFC-e, CT-e, CT-e OS, MDF-e, BP-e, NF3e, NFCom) seguem o mesmo padrão: leiaute publicado em XSD por pacotes de liberação, chave de acesso de 44 posições, assinatura XMLDSig com certificado ICP-Brasil, webservices SOAP 1.2 com TLS e certificado cliente, eventos com o mesmo envelope e notas técnicas que mudam o leiaute com frequência. É esse padrão que a libnfe já resolve e que cada subprojeto reaproveita.

As diferenças que mudam o projeto aparecem nos detalhes e precisam ser levantadas antes de escrever código:

| Ponto | Exemplos do que já mudou |
|---|---|
| Autorizador | NF-e e NFC-e: um por UF; MDF-e: SVRS para todas as UFs |
| Endereços | Os da NFC-e não são os da NF-e, mesmo na mesma UF (`nfce-homologacao.svrs...`) |
| Autorização | NF-e em lote assíncrono ou síncrono; MDF-e síncrona, com a mensagem em gzip e base64 |
| Grupo suplementar | QR Code com CSC na NFC-e; QR Code com assinatura RSA-SHA1 em contingência no MDF-e |
| Contingência | SVC na NF-e; offline (`tpEmis` 9) na NFC-e |
| Eventos | Cada documento tem os seus (encerramento e inclusão de condutor no MDF-e, cancelamento por substituição na NFC-e) |
| Pré-requisitos do contribuinte | CSC do ambiente de teste na NFC-e; credenciamento e RNTRC no transporte |

Dois documentos fogem do padrão e pedem um levantamento mais cuidadoso antes de seguir este roteiro: a **NFS-e do padrão nacional**, que usa uma API própria do Sistema Nacional NFS-e em vez dos webservices SOAP da SEFAZ (conferir na documentação técnica antes de planejar), e o **CF-e SAT/MF-e**, em que quem assina e transmite é um equipamento (ver a opção B do levantamento feito antes da libmdf).

## Princípios

1. **O que é comum vai para a libnfe; o subprojeto só tem o que é do documento.** Assinatura, envio SOAP, validação contra XSD, certificado e chave de acesso ficam no núcleo. Quando o novo documento precisa de algo que poderia servir a outro, a mudança é um PR no tooldoce, que gera uma versão nova da libnfe, e o subprojeto passa a exigir essa versão (foi assim com `nfe_certificado_assinar`, em #270, e com `nfe_assinar_elemento`, `nfe_sefaz_enviar_ws` e `nfe_validador_xsd`, em #273).
2. **O XSD é a fonte da verdade: gere, não digite.** Padrões, tamanhos, valores permitidos, a estrutura dos grupos, os diagramas e a lista do que falta saem dos schemas oficiais por scripts (`tools/gerar_*.py`). O que é digitado à mão desatualiza na primeira nota técnica.
3. **Fatia vertical primeiro.** Um documento mínimo autorizado na homologação vale mais que todos os grupos prontos e nunca transmitidos. A NFC-e foi autorizada na homologação real logo no segundo PR, e o que veio depois foi construído sobre um caminho que já funcionava.
4. **Teste em cada PR, não no fim.** Cada função entra com teste, validação contra o XSD e CI verde com gcc e clang. A homologação real é um marco, não uma fase de testes.
5. **Toda regra que o MOC não deixa clara vira documentação.** Exemplo: o cancelamento por substituição da NFC-e só é aceito com a substituta em contingência offline; com outra, a SEFAZ devolve `cStat` 920 (registrado em `docs/HOMOLOGACAO.md` da libnfc).

## Fase 0: levantamento (antes do repositório)

Um documento curto, como o levantamento feito antes da libmdf (MDF-e comparado ao MF-e), que responde:

- **Qual documento, exatamente.** O nome engana: "libmfe" virou MDF-e (modelo 58) e não o MF-e do Ceará depois do levantamento.
- **Leiaute e schemas:** versão do leiaute, pacote de liberação vigente (PL_...), notas técnicas em vigor ou com data marcada.
- **Webservices:** autorizador de cada UF, lista de serviços, síncrono ou assíncrono, compressão, versão SOAP, endereços de homologação e produção.
- **Eventos, QR Code e contingência** do documento.
- **O que a libnfe já faz e o que falta nela**, numa tabela com o arquivo e a função (é a primeira parte do `docs/ROTEIRO.md` da libnfc e da libmdf).
- **Pré-requisitos do contribuinte para homologar:** certificado A1 da empresa, credenciamento no ambiente de homologação da UF, CSC de **teste** (o de produção dá `cStat` 462), cadastro de transportador etc. Esses itens dependem de terceiros e costumam ser o que mais atrasa.
- **Fora do escopo:** a representação gráfica (DANFE, DAMDFE, DACTE) fica com o programa emissor.
- **Nome e prefixo:** `libmdf` e `mdf_`, `libnfc` e `nfc_`; o prefixo não pode colidir com o da libnfe (`nfe_`).

Valores tirados de memória ou de terceiros ficam marcados como "(conferir)" até serem confirmados no portal oficial. A rede das sessões na nuvem não alcança os portais da SEFAZ; quem confere é o mantenedor.

**Saída:** levantamento aprovado e decisão de criar o subprojeto.

## Fase 1: repositório (mantenedor)

O repositório é criado pelo mantenedor, porque as sessões na nuvem não têm permissão para isso:

```sh
gh repo create icaroraci/libXXX --public --license lgpl-3.0 \
  --description "Biblioteca C para emissão de XXX (modelo NN)" --add-readme
```

Depois: incluir o repositório no projeto do Claude, ligá-lo ao GitHub Project "tooldoce" (campo Documento) e criar o milestone `libXXX 0.1`.

## Fase 2: estrutura inicial (um PR)

Copiar a estrutura da libmdf, que é a mais recente, trocando nome e prefixo:

| Arquivo | Conteúdo |
|---|---|
| `Makefile` | `lib/libXXX.so` com SONAME `libXXX.so.0`, `make test` com AddressSanitizer e UBSan, `make install`/`uninstall` com `DESTDIR` e `PREFIX`, `verificar-formato` |
| `libXXX.pc.in` | `Requires: libnfe >= 1.0` (sem `-rc`: no pkg-config `1.0.0-rc2` é maior que `1.0.0`) |
| `include/libXXX/versao.h` | `XXX_VERSAO` e `xxx_versao()` |
| `tests/` | `test_versao.c` e `test_libnfe.c` (confere que a libnfe certa foi ligada) |
| `.github/workflows/ci.yml` | gcc e clang, formatação, testes, exemplos e o teste de instalação; `LIBNFE_REF` com a versão da libnfe usada |
| `.github/scripts/` | `instalar_libnfe.sh` (compila a libnfe do tooldoce num prefixo) e `compilar.sh` |
| `.clang-format`, `docs/CONVENCOES.md`, `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, `pull_request_template.md` | Iguais aos da libnfc |
| `COPYING` e `LICENSE` | GPLv3 e LGPLv3 |
| `CHANGELOG.md` | Keep a Changelog, seção "Não lançado" |
| `README.md` | O que é, o que vem da libnfe, situação, dependências, compilação, badges de CI e licença |
| `docs/ROTEIRO.md` | Tabela "o que a libnfe já faz" e lista numerada "o que falta", saída da fase 0 |

No tooldoce, atualizar a tabela de documentos em [`VISAO.md`](VISAO.md) com o link do novo repositório.

**Saída:** CI verde, `make install` gerando `libXXX.pc`. Nada do documento ainda.

## Fase 3: schemas

- Baixar o pacote de liberação oficial e guardar os XSD **sem alteração** em `tests/schemas/<documento>/`, com um `README.md` dizendo pacote, versão, data e origem (modelo: [`tests/schemas/README.md`](../tests/schemas/README.md) do tooldoce). Eventos em pastas próprias, um pacote por pasta.
- Atualização de schema é sempre um commit separado, para a diferença do leiaute ficar visível no histórico.
- Escrever a configuração do documento e gerar os diagramas e a lista de estruturas (`TODO.md`) a partir dos schemas, com `gerar_diagramas.py --config` ([`ESQUEMAS.md`](ESQUEMAS.md)).

**Saída:** schemas versionados e o mapa do leiaute.

## Fase 4: o que falta no núcleo (PR no tooldoce)

Tudo o que a fase 0 marcou como "falta na libnfe" vira um PR no tooldoce, antes do código do documento que depende dele. O mantenedor publica a versão da libnfe e o CI do subprojeto passa a compilar contra essa tag (`LIBNFE_REF`), não contra o `master`.

O motor genérico de grupos (`<libnfe/esquema.h>` e `<libnfe/grupo.h>`) e os geradores (`gerar_esquemas.py`, `gerar_padroes.py`, `gerar_diagramas.py`, `gerar_issues.py`) servem a qualquer documento: a biblioteca descreve os seus schemas num arquivo de configuração e gera as tabelas com os geradores instalados pela libnfe ([`ESQUEMAS.md`](ESQUEMAS.md)).

## Fase 5: fatia vertical

O menor caminho completo, do XML à autorização:

1. Montar um documento mínimo, só com os grupos obrigatórios e o caso mais comum (por exemplo, o modal rodoviário no MDF-e).
2. Validar contra o XSD, gerar a chave, assinar.
3. Transmitir a um servidor falso no CI (como `tests/servidor_sefaz.py` da libnfc, com TLS e certificado cliente de teste).
4. Na homologação real, pelo mantenedor: status do serviço, depois autorização. Registrar em `docs/HOMOLOGACAO.md` só chave, protocolo e `cStat`, nunca o XML (tem dados do certificado e da empresa).

**Saída:** primeiro documento autorizado em homologação.

## Fase 6: completar o leiaute

Com o caminho funcionando, os demais grupos entram pelo motor de grupos gerado do XSD, preenchidos pelo caminho do campo (`grupo_set(g, "ICMS10/vBC", ...)`). Estruturas opacas próprias, com funções específicas, só para a raiz do documento e para grupos com lógica além do leiaute (chave de acesso, totais calculados, QR Code). Uma estrutura opaca por grupo foi o caminho original da NF-e e não escalou: os grupos de tributos do item só ficaram todos prontos quando passaram para o motor genérico.

As issues seguem funcionalidades ("autorização síncrona", "evento de encerramento", "modal aéreo"), não grupos do XSD. A lista de grupos já existe no `TODO.md` gerado; se for útil dividir um grupo grande em issues, `tools/gerar_issues.py` gera o texto delas a partir do XSD.

**Definição de pronto** de cada grupo ou funcionalidade: setters com as validações do leiaute, XML gerado válido contra o XSD, teste no CI e, se transmite algo, teste contra o servidor falso.

## Fase 7: serviços, eventos e contingência

- **Endereços:** tabela por UF e ambiente gerada por script a partir de uma captura das fontes oficiais (portal do documento, ENCAT), com as fontes em `docs/ENDERECOS.md` (modelo: libnfc, `tools/gerar_enderecos.py`). Como a nuvem não alcança os portais, a captura é feita pelo mantenedor.
- **Consulta, eventos e contingência**, cada um com teste no servidor falso e, depois, na homologação real.
- Exemplos em `examples/`, executados no CI.

## Fase 8: homologação completa

`docs/HOMOLOGACAO.md` com uma linha por serviço e evento (serviço, webservice, `cStat`, protocolo) e, no fim, a lista do que ainda não foi testado. As rejeições inesperadas e as regras que o MOC não deixa claras são registradas ali, com o `cStat`.

A homologação roda no computador do mantenedor (no WSL), com o certificado da empresa e a senha numa variável de ambiente; a rede da nuvem não alcança a SEFAZ.

## Fase 9: versão candidata

Critérios para a `1.0.0-rc1`:

- itens do `docs/ROTEIRO.md` feitos ou explicitamente adiados para depois da 1.0;
- autorização, consulta, cancelamento e os eventos principais aceitos na homologação real;
- CI verde contra uma **tag** publicada da libnfe;
- `versao.h` e SONAME `libXXX.so.1`, `CHANGELOG.md` com a seção da versão, notas da versão escritas.

A tag e a release são publicadas pelo mantenedor no GitHub, com as notas preparadas no PR. A `1.0.0` final vem depois de uso real com a rc, sem mudança de API.

## O roteiro proposto originalmente, comparado

| Passo proposto | Neste roteiro |
|---|---|
| 1. Criar o repositório | Fase 1, mas depois do levantamento (fase 0): o nome, o escopo e o que falta no núcleo se decidem antes |
| 2. Configuração inicial | Fase 2, copiando a libmdf |
| 3. Schemas e XSD | Fase 3, guardados sem alteração e usados para gerar código e documentação |
| 4. Uma issue por grupo (estruturas opacas) | Fase 6: issues por funcionalidade; grupos pelo motor genérico; estrutura opaca só onde há lógica |
| 5. Funções para cada grupo | Fases 5 e 6, começando por uma fatia vertical, não grupo a grupo |
| 6. Documentar | Em todas as fases: README, ROTEIRO, HOMOLOGACAO, ENDERECOS e CHANGELOG mudam junto com o código |
| 7. Testar gerar, validar, assinar, transmitir, cancelar | Em cada PR (CI) e como marco na homologação real (fases 5 e 8), não no fim |
| 8. Postar v1.0.0-rc | Fase 9, com critérios de entrada |
| (faltava) | Fase 4: o que falta no núcleo vai para a libnfe antes |
| (faltava) | Pré-requisitos do contribuinte (certificado, credenciamento, CSC), levantados na fase 0 |
