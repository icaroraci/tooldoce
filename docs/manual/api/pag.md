# pag.h — Formas de pagamento, dados eletrônicos e troco. Cada detPag transferido pertence a pag; o valor do pagamento não é recalculado pelo valor da fatura.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/pag.h>`. Formas de pagamento, dados eletrônicos e troco. Cada detPag transferido pertence a pag; o valor do pagamento não é recalculado pelo valor da fatura.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/nfe.h>

/*
 * Pagamento (grupo pag): até 100 formas de pagamento (detPag) e o troco.
 *
 * Uso típico:
 *   nfe_detpag *dp = nfe_detpag_new();
 *   nfe_detpag_set_tpag(dp, NFE_MEIO_DINHEIRO);
 *   nfe_detpag_set_vpag(dp, "20.00");
 *   nfe_pag *pag = nfe_pag_new();
 *   nfe_pag_add_detpag(pag, dp);    (o pag passa a ser dono de dp)
 *   nfe_pag_set_vtroco(pag, "5.00");
 *   nfe_pag_write_xml(writer, pag);
 *   nfe_pag_free(pag);
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado. Valores são texto no formato do XML ("20.00");
 * campos opcionais são removidos com NULL.
 */

typedef struct nfe_detpag nfe_detpag;
typedef struct nfe_pag nfe_pag;

/* Número máximo de formas de pagamento (detPag) */
#define NFE_MAX_DETPAG 100

/* Cria uma forma de pagamento vazia. Retorna NULL se faltar memória. */
nfe_detpag *nfe_detpag_new(void);

/* Libera a forma de pagamento; aceita NULL */
void nfe_detpag_free(nfe_detpag *dp);

/* indPag: à vista ou a prazo; NFE_PAGAMENTO_NAO_INFORMADO remove */
int nfe_detpag_set_indpag(nfe_detpag *dp, nfe_forma_pagamento indpag);
/* tPag: meio de pagamento, código de 0 a 99 (ver nfe_meio_pagamento) */
int nfe_detpag_set_tpag(nfe_detpag *dp, nfe_meio_pagamento tpag);
/* xPag: descrição do meio de pagamento (2 a 60 caracteres), usada com
 * NFE_MEIO_OUTROS */
int nfe_detpag_set_xpag(nfe_detpag *dp, const char *xpag);
int nfe_detpag_set_vpag(nfe_detpag *dp, const char *vpag);
/* dPag: data do pagamento, "AAAA-MM-DD" */
int nfe_detpag_set_dpag(nfe_detpag *dp, const char *dpag);
/* CNPJPag e UFPag: CNPJ e UF do estabelecimento onde o pagamento foi
 * processado, quando diferente do emitente. Vão juntos: ambos NULL
 * removem. */
int nfe_detpag_set_local(nfe_detpag *dp, const char *cnpjpag,
                         const char *ufpag);
/* card: dados do cartão. cnpj (credenciadora), tband (bandeira, 1 a 99:
 * 1 Visa, 2 Mastercard, 3 American Express, 6 Elo, 7 Hipercard, 99 outros;
 * 0 omite), caut (autorização, 1 a 128), cnpjreceb (beneficiário do
 * pagamento) e idtermpag (terminal, 1 a 40) são opcionais (NULL omite). */
int nfe_detpag_set_card(nfe_detpag *dp, nfe_integracao tpintegra,
                        const char *cnpj, unsigned tband, const char *caut,
                        const char *cnpjreceb, const char *idtermpag);
int nfe_detpag_remove_card(nfe_detpag *dp);

/* Cria um pagamento vazio. Retorna NULL se faltar memória. */
nfe_pag *nfe_pag_new(void);

/* Libera o pagamento e as formas de pagamento que ele possui; aceita
 * NULL */
void nfe_pag_free(nfe_pag *pag);

/* Acrescenta uma forma de pagamento (até NFE_MAX_DETPAG). Em caso de
 * sucesso o pagamento passa a ser dono de dp. Retorna 0, E_ISNULL, E_VALOR
 * (limite atingido) ou E_MALLOC. */
int nfe_pag_add_detpag(nfe_pag *pag, nfe_detpag *dp);
/* vTroco: valor do troco; NULL remove */
int nfe_pag_set_vtroco(nfe_pag *pag, const char *vtroco);

/* Escreve o elemento <pag>. Retorna 0, E_ISNULL, E_VALOR (nenhuma forma de
 * pagamento, ou uma sem tPag ou vPag) ou E_XML. */
int nfe_pag_write_xml(xmlTextWriterPtr writer, const nfe_pag *pag);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/pag.h).
- [Implementação pag.c](../../../src/libnfe/pag.c).
- [Programa de testes compilável](../../../tests/test_pag.c): `make obj/test_pag` e `./obj/test_pag tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
