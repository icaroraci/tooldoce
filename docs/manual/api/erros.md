# erros.h — Códigos negativos e descrição de falhas locais da biblioteca. Não são códigos cStat do autorizador; funções que retornam quantidades ou dígitos têm contratos próprios.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/erros.h>`. Códigos negativos e descrição de falhas locais da biblioteca. Não são códigos cStat do autorizador; funções que retornam quantidades ou dígitos têm contratos próprios.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
/* Códigos de erro devolvidos pelas funções da biblioteca (sempre negativos;
 * 0 indica sucesso). A biblioteca não imprime mensagens: use nfe_strerror()
 * para obter a descrição de um código. */
#define E_ISNULL  -1   /* ponteiro nulo recebido */
#define E_TAMANHO -2   /* texto fora dos limites do campo */
#define E_VALOR   -3   /* valor fora da faixa permitida */
#define E_XML     -4   /* XML malformado ou falha ao gerá-lo (libxml2) */
#define E_ARQUIVO -5   /* falha ao gravar o arquivo */
#define E_REDE    -6   /* falha na comunicação com a SEFAZ */
#define E_MALLOC  -101 /* falta de memória */

/* Descrição do código de erro (texto estático, não deve ser liberado) */
const char *nfe_strerror(int codigo);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/erros.h).
- [Implementação erros.c](../../../src/libnfe/erros.c).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
