# utils.h — Macros e apoio interno

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/utils.h>`. Tipos e rotinas auxiliares da biblioteca. O contrato público exclui funções marcadas NFE_INTERNO; não use os símbolos ocultos como API de integração.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <stddef.h>

#include <libnfe/erros.h>

/* Funções de uso interno da biblioteca: não são exportadas na libnfe.so */
#if defined(__GNUC__) && __GNUC__ >= 4
#define

/* Copia o texto src para dst (tam bytes, incluindo o terminador).
 * min e max são os limites do campo em caracteres UTF-8 (max == 0: sem
 * limite além do buffer). Em caso de erro dst não é alterado.
 * Retorna 0, E_ISNULL (dst ou src NULL) ou E_TAMANHO. */
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/utils.h).
- [Implementação utils.c](../../../src/libnfe/utils.c).
- [Programa de testes compilável](../../../tests/test_utils.c): `make obj/test_utils` e `./obj/test_utils tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
