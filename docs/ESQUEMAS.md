# Motor de grupos e geradores para outros documentos

A libnfe monta os grupos do leiaute por um motor genérico (`<libnfe/esquema.h>` e `<libnfe/grupo.h>`): a estrutura de cada grupo (elementos, sequências, escolhas, listas, atributos, padrões, valores e tamanhos) vem de tabelas C geradas dos schemas oficiais por `tools/gerar_esquemas.py`. O mesmo motor e os mesmos geradores servem a qualquer documento no padrão da SEFAZ (MDF-e, CT-e, NFC-e...): basta descrever onde estão os schemas do documento num arquivo de configuração.

## Configuração do documento

Um JSON por documento, guardado no repositório da biblioteca dele (ex.: `tools/documento.json` na libmdf). Os caminhos são relativos à pasta de onde os geradores são chamados, a raiz do projeto. A descrição completa das chaves está em [`tools/documento.py`](../tools/documento.py); a da NF-e, que vale quando nenhuma é informada, está em [`tools/documentos/nfe.json`](../tools/documentos/nfe.json).

Esboço para o MDF-e (nomes de arquivos a conferir no pacote de liberação vigente):

```json
{
  "documento": "MDF-e",
  "versao": "3.00",
  "biblioteca": "libmdf",
  "schemas": "tests/schemas/mdfe",
  "tipos": ["tiposGeralMDFe_v3.00.xsd", "mdfeModalRodoviario_v3.00.xsd"],
  "leiaute": "mdfeTiposBasico_v3.00.xsd",
  "raiz": {"elemento": "MDFe", "tipo": "TMDFe"},
  "repositorio": "icaroraci/libmdf",
  "ramo": "main",
  "padroes": {"saida": "src/libmdf/padroes.h", "prefixo": "MDF_"},
  "esquemas": {
    "saida": "src/libmdf/esquemas.c",
    "cabecalho": "src/libmdf/esquemas.h",
    "prefixo": "mdf_esq_",
    "raizes": ["ide", "emit", "infDoc", "seg", "prodPred", "tot", "rodo"]
  },
  "diagramas": {"saida": "docs/diagramas", "todo": "TODO.md"}
}
```

Cada raiz é o nome de um elemento (procurado primeiro no XSD do leiaute e depois nos de tipos) ou o par `["nome C", "elemento"]`, quando dois grupos têm o mesmo nome ou o nome não serve em C.

## Geradores

O `make install` do tooldoce instala os geradores em `$(PREFIX)/share/tooldoce/ferramentas`, indicada pela variável `ferramentas` do `libnfe.pc`:

```sh
f=$(pkg-config --variable=ferramentas libnfe)
python3 "$f/gerar_esquemas.py" --config tools/documento.json   # tabelas do motor (.c e .h)
python3 "$f/gerar_padroes.py" --config tools/documento.json    # padrões, tamanhos e valores (header)
python3 "$f/gerar_diagramas.py" --config tools/documento.json --todo   # diagramas SVG e TODO.md
python3 "$f/gerar_issues.py" SAIDA --config tools/documento.json       # texto das issues por estrutura
```

`gerar_esquemas.py` e `gerar_padroes.py` aceitam `--verificar`, para o CI conferir se os arquivos gerados estão atualizados. Só usam a biblioteca padrão do Python 3.

## Uso na biblioteca do documento

O `.c` gerado entra na compilação da biblioteca; o header declara as estruturas com visibilidade oculta (`NFE_INTERNO`), para elas não fazerem parte da API nem colidirem com as de outra biblioteca. Os prefixos (`mdf_esq_`, `MDF_`) evitam colisão de nomes com os da libnfe.

```c
#include <libnfe/esquema.h>
#include <libnfe/grupo.h>

#include "esquemas.h"

nfe_grupo *rodo = nfe_grupo_new(&mdf_esq_rodo);
nfe_grupo *condutor;

nfe_grupo_set(rodo, "veicTracao/placa", "ABC1D23");
nfe_grupo_add(rodo, "veicTracao/condutor", &condutor);
nfe_grupo_set(condutor, "CPF", "12345678909");
nfe_grupo_write_xml(writer, rodo); /* E_VALOR se faltar campo obrigatório */
nfe_grupo_free(rodo);
```

O leiaute das structs de `esquema.h` faz parte da ABI da libnfe e só muda com a versão maior, junto com `NFE_ESQ_VERSAO`; o `.c` gerado não compila contra outra versão do motor. Depois de atualizar a libnfe, gere as tabelas de novo.

O teste [`tests/test_outro_documento.c`](../tests/test_outro_documento.c) usa tudo isso com um schema fictício (`tests/schemas/exemplo`, configuração em `tests/outro_documento/documento.json`), como uma biblioteca de outro documento faria.

## Limitações do gerador

O gerador falha (em vez de gerar tabelas erradas) com conteúdo misto, `simpleContent` e sequências ou escolhas repetidas (`maxOccurs` num `xs:sequence`/`xs:choice`). Elementos com `ref` (como `ds:Signature`) não são suportados dentro de uma raiz: a assinatura é feita depois, por `nfe_assinar_elemento`. Grupos com `xs:any` (como `infModal` do MDF-e, que recebe o XML do modal) são montados pela biblioteca, com o grupo do modal (`rodo`) gerado como raiz própria.
