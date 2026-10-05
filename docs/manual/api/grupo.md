# grupo.h — Preenchimento por caminhos, escolhas, remoção e listas. Caminhos completos evitam ambiguidade; itens de lista pertencem ao grupo; getters retornam dados emprestados.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/grupo.h>`. Preenchimento por caminhos, escolhas, remoção e listas. Caminhos completos evitam ambiguidade; itens de lista pertencem ao grupo; getters retornam dados emprestados.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
/*
 * Grupo genérico do leiaute.
 *
 * Grupos com muitos campos (os tributos do item, os subgrupos do produto,
 * cana, exportação...) são preenchidos campo a campo pelo caminho: os nomes
 * dos elementos do leiaute, separados por "/", a partir do grupo. O caminho
 * completo identifica sempre um único campo: no grupo de um reboque do
 * MDF-e, "UF" é a UF do reboque e "prop/UF" a do proprietário. O caminho
 * pode ser abreviado, com os nomes na ordem, quando só um campo casa com
 * ele:
 *   nfe_grupo_set(g, "ICMS10/vBC", "100.00");   (ICMS/ICMS10/vBC)
 * Um caminho abreviado que casa com mais de um campo é recusado (E_VALOR),
 * sem escolher um deles: em <imposto>, "vBC" casa com o vBC de cada grupo
 * do ICMS, do IPI, do PIS...
 * Atributos de um elemento são gravados como campos do elemento.
 *
 * Ao gravar um campo de um ramo de uma escolha do leiaute, os campos dos
 * outros ramos são apagados. Campos com um único valor possível são
 * preenchidos sozinhos. Os campos obrigatórios são conferidos ao gerar o
 * XML.
 *
 * Elementos que se repetem (listas, como DI ou rastro no produto) não são
 * gravados pelo caminho: nfe_grupo_add acrescenta um item e devolve o grupo
 * do item, cujos campos são gravados a partir dele (inclusive os atributos
 * do item, como "dia" em forDia, e o próprio valor quando o item é um campo
 * simples, como NVE: nfe_grupo_set(item, "NVE", "AA0001")).
 *
 * Os grupos pertencem ao objeto que os contém (nfe_imposto, nfe_prod...),
 * que os libera.
 *
 * Retornos: 0, E_ISNULL, E_TAMANHO, E_VALOR (campo inexistente ou caminho
 * ambíguo, valor fora do domínio ou do formato, ou lista cheia) ou
 * E_MALLOC. Em caso de erro o grupo não é alterado.
 */

typedef struct nfe_grupo nfe_grupo;

/* Grava o campo; NULL apaga */
int nfe_grupo_set(nfe_grupo *g, const char *caminho, const char *valor);
/* Valor do campo, ou NULL (também com caminho ambíguo; o texto pertence
 * ao grupo) */
const char *nfe_grupo_get(const nfe_grupo *g, const char *caminho);
/* Apaga todos os campos e itens de listas dentro dos elementos do
 * caminho */
int nfe_grupo_remove(nfe_grupo *g, const char *caminho);
/* Acrescenta um item à lista do caminho e devolve o seu grupo em *item */
int nfe_grupo_add(nfe_grupo *g, const char *caminho, nfe_grupo **item);
/* Quantidade de itens da lista do caminho, ou código de erro (< 0) */
int nfe_grupo_quantidade(const nfe_grupo *g, const char *caminho);
/* Item i (a partir de 0) da lista do caminho, ou NULL */
nfe_grupo *nfe_grupo_item(const nfe_grupo *g, const char *caminho, int i);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/grupo.h).
- [Implementação grupo.c](../../../src/libnfe/grupo.c).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
