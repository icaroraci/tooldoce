# cobr.h — Fatura e duplicatas da cobrança, com valores e vencimentos. Estes dados não criam automaticamente formas de pagamento.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/cobr.h>`. Fatura e duplicatas da cobrança, com valores e vencimentos. Estes dados não criam automaticamente formas de pagamento.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

/*
 * Cobrança (grupo cobr): fatura (fat) e duplicatas (dup).
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado. Valores são texto no formato do XML ("100.00");
 * NULL omite os opcionais.
 */

typedef struct nfe_cobr nfe_cobr;

/* Número máximo de duplicatas */
#define NFE_MAX_DUP 120

/* Cria a cobrança vazia. Retorna NULL se faltar memória. */
nfe_cobr *nfe_cobr_new(void);

/* Libera; aceita NULL */
void nfe_cobr_free(nfe_cobr *cobr);

/* fat: número (1 a 60), valor original, desconto e valor líquido, todos
 * opcionais. Com todos NULL, remove a fatura. */
int nfe_cobr_set_fat(nfe_cobr *cobr, const char *nfat, const char *vorig,
                     const char *vdesc, const char *vliq);
/* dup: número (1 a 60) e vencimento ("AAAA-MM-DD") opcionais; valor
 * obrigatório, maior que zero. Até NFE_MAX_DUP. */
int nfe_cobr_add_dup(nfe_cobr *cobr, const char *ndup, const char *dvenc,
                     const char *vdup);
/* Apaga todas as duplicatas */
int nfe_cobr_remove_dup(nfe_cobr *cobr);

/* Escreve o elemento <cobr>. Retorna 0, E_ISNULL ou E_XML. */
int nfe_cobr_write_xml(xmlTextWriterPtr writer, const nfe_cobr *cobr);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/cobr.h).
- [Implementação cobr.c](../../../src/libnfe/cobr.c).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
