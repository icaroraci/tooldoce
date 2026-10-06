# resptec.h — Responsável técnico e hash CSRT já calculado. O setter do hash não calcula SHA-1 nem gera o segredo CSRT.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/resptec.h>`. Responsável técnico e hash CSRT já calculado. O setter do hash não calcula SHA-1 nem gera o segredo CSRT.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

/*
 * Responsável técnico pelo sistema emissor (grupo infRespTec).
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado.
 */

typedef struct nfe_resptec nfe_resptec;

/* Cria vazio. Retorna NULL se faltar memória. */
nfe_resptec *nfe_resptec_new(void);

/* Libera; aceita NULL */
void nfe_resptec_free(nfe_resptec *rt);

/* Dados obrigatórios: CNPJ da empresa, nome do contato (2 a 60), e-mail
 * (6 a 60) e telefone (6 a 14 dígitos). Só grava se todos forem válidos. */
int nfe_resptec_set(nfe_resptec *rt, const char *cnpj, const char *xcontato,
                    const char *email, const char *fone);
/* CSRT (código de segurança do responsável técnico): identificador (0 a
 * 99) e hash já calculado (SHA-1 do CSRT concatenado com a chave de
 * acesso, em base64: 28 caracteres). hashcsrt NULL remove. */
int nfe_resptec_set_csrt(nfe_resptec *rt, unsigned idcsrt,
                         const char *hashcsrt);

/* Escreve <infRespTec>. Retorna 0, E_ISNULL, E_VALOR (dados obrigatórios
 * não informados) ou E_XML. */
int nfe_resptec_write_xml(xmlTextWriterPtr writer, const nfe_resptec *rt);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/resptec.h).
- [Implementação resptec.c](../../../src/libnfe/resptec.c).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
