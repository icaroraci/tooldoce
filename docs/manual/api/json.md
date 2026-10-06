# json.h — Leitura mínima de JSON

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/json.h>`. Leitura mínima de JSON para respostas de serviços REST, como a NFS-e nacional. A árvore é somente de leitura e pertence à raiz; os textos devolvidos valem até nfe_json_free.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <stddef.h>

#include <libnfe/erros.h>

/* Leitura mínima de JSON (RFC 8259), para as respostas de serviços REST
 * como a NFS-e nacional. O texto inteiro vira uma árvore somente de
 * leitura; os nós pertencem à raiz e valem até nfe_json_free:
 *   nfe_json *j;
 *   if (nfe_json_ler(resposta, tam, &j) == 0) {
 *       const char *chave = nfe_json_texto(nfe_json_campo(j, "chaveAcesso"));
 *       const nfe_json *erros = nfe_json_campo(j, "erros");
 *       size_t i;
 *       for (i = 0; i < nfe_json_qtd(erros); i++)
 *           ... nfe_json_texto(nfe_json_campo(nfe_json_item(erros, i),
 *                                             "Codigo")) ...
 *       nfe_json_free(j);
 *   }
 * As funções de consulta aceitam NULL (devolvem NULL ou 0), para encadear
 * campos que podem faltar. */

typedef struct nfe_json nfe_json;

typedef enum nfe_json_tipo {
	NFE_JSON_NULO,
	NFE_JSON_LOGICO, /* true ou false */
	NFE_JSON_NUMERO,
	NFE_JSON_TEXTO,
	NFE_JSON_LISTA,
	NFE_JSON_OBJETO
} nfe_json_tipo;

/* Maior profundidade de listas e objetos aceita por nfe_json_ler */
#define NFE_JSON_PROFUNDIDADE 64

/* Lê tam bytes de texto JSON (UTF-8; espaços em volta são aceitos) em
 * *raiz, a liberar com nfe_json_free. Textos com \u0000 são recusados,
 * para que nfe_json_texto devolva textos C completos. Em caso de erro
 * *raiz não é alterado.
 * Retorna 0, E_ISNULL (texto ou raiz NULL), E_VALOR (JSON inválido, ou
 * aninhado além de NFE_JSON_PROFUNDIDADE) ou E_MALLOC. */
int nfe_json_ler(const char *texto, size_t tam, nfe_json **raiz);
void nfe_json_free(nfe_json *raiz);

/* Tipo do nó; NFE_JSON_NULO também para j NULL */
nfe_json_tipo nfe_json_tipo_de(const nfe_json *j);

/* Valor do nó como texto: o texto já sem escapes (UTF-8), o número como
 * escrito no JSON, "true" ou "false"; NULL para null, lista, objeto ou j
 * NULL. O texto pertence à árvore. */
const char *nfe_json_texto(const nfe_json *j);

/* Quantidade de itens de uma lista ou de campos de um objeto; 0 para
 * outros nós ou j NULL */
size_t nfe_json_qtd(const nfe_json *j);

/* Item i (a partir de 0) de uma lista ou valor do campo i de um objeto;
 * NULL se i estiver fora da faixa ou j não for lista nem objeto */
const nfe_json *nfe_json_item(const nfe_json *j, size_t i);

/* Nome do campo i de um objeto; NULL se j não for objeto ou i estiver fora
 * da faixa */
const char *nfe_json_nome(const nfe_json *j, size_t i);

/* Valor do campo nome de um objeto (o primeiro, se repetido), com
 * diferença entre maiúsculas e minúsculas; NULL se faltar ou se j não for
 * objeto */
const nfe_json *nfe_json_campo(const nfe_json *j, const char *nome);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/json.h).
- [Implementação json.c](../../../src/libnfe/json.c).
- [Programa de testes compilável](../../../tests/test_json.c): `make obj/test_json` e `./obj/test_json`.
- Uso com uma API REST: `testa_rest` em [test_sefaz.c](../../../tests/test_sefaz.c), contra o servidor falso.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
