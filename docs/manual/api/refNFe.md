# refNFe.h — Referência de NF-e por chave eletrônica. A API específica confere a chave/DV; não fornece automaticamente o fluxo de referência com sigilo.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/refNFe.h>`. Referência de NF-e por chave eletrônica. A API específica confere a chave/DV; não fornece automaticamente o fluxo de referência com sigilo.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
/*
 * Elemento refNFe (dentro de NFref): NF-e ou NFC-e referenciada pela chave
 * de acesso.
 */



#include <libxml/xmlwriter.h>

struct refNFe_s;

/* Cria um refNFe vazio; NULL se faltar memória */
struct refNFe_s *RefNFeNew(void);
/* Libera o refNFe (aceita NULL) */
void RefNFeDel(struct refNFe_s *nf);

/* Chave de acesso: 44 caracteres com dígito verificador válido.
 * Retorna 0, E_ISNULL, E_TAMANHO ou E_VALOR; se recusada, a chave anterior
 * é mantida. */
int RefNFeSetrefNFe(struct refNFe_s *nf, const char *ref);
/* "" enquanto a chave não for informada; NULL se nf for NULL */
const char *RefNFeGetrefNFe(const struct refNFe_s *nf);

/* Escreve <refNFe>; quem chama abre e fecha <NFref> */
int xmlGenRefNFeNode(xmlTextWriterPtr writer, const struct refNFe_s *nf);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/refNFe.h).
- [Implementação refNFe.c](../../../src/libnfe/refNFe.c).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
