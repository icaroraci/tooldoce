# local.h — Locais de retirada e entrega. O grupo recebe um endereço e passa a possuí-lo após sucesso; identificação e endereço não são substitutos do destinatário da nota.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/local.h>`. Locais de retirada e entrega. O grupo recebe um endereço e passa a possuí-lo após sucesso; identificação e endereço não são substitutos do destinatário da nota.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/endereco.h>

/*
 * Local de retirada ou de entrega da mercadoria (grupos retirada e entrega,
 * tipo TLocal), quando diferente do endereço do emitente ou do
 * destinatário.
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado. NULL remove os opcionais.
 */

typedef struct nfe_local nfe_local;

/* Qual grupo nfe_local_write_xml escreve */
typedef enum nfe_local_tipo {
	NFE_LOCAL_RETIRADA, /* <retirada> */
	NFE_LOCAL_ENTREGA   /* <entrega> */
} nfe_local_tipo;

/* Cria um local vazio. Retorna NULL se faltar memória. */
nfe_local *nfe_local_new(void);

/* Libera o local e o endereço que ele possui; aceita NULL */
void nfe_local_free(nfe_local *local);

/* CNPJ ou CPF de quem entrega/recebe (obrigatório; um substitui o outro) */
int nfe_local_set_cnpj(nfe_local *local, const char *cnpj);
int nfe_local_set_cpf(nfe_local *local, const char *cpf);
int nfe_local_set_xnome(nfe_local *local, const char *xnome); /* 2 a 60 */
/* Endereço (obrigatório). Em caso de sucesso o local passa a ser dono de
 * end. */
int nfe_local_set_endereco(nfe_local *local, nfe_endereco *end);
int nfe_local_set_email(nfe_local *local, const char *email); /* 1 a 60 */
/* IE: 2 a 14 dígitos ou "ISENTO" */
int nfe_local_set_ie(nfe_local *local, const char *ie);

/* Escreve <retirada> ou <entrega>. Retorna 0, E_ISNULL, E_VALOR (falta
 * CNPJ/CPF ou endereço, ou endereço incompleto) ou E_XML. */
int nfe_local_write_xml(xmlTextWriterPtr writer, nfe_local_tipo tipo,
                        const nfe_local *local);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/local.h).
- [Implementação local.c](../../../src/libnfe/local.c).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
