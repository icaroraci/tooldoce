# chave.h — Composição lexical da chave e cálculo/conferência do DV. As posições do CNPJ admitem letras maiúsculas no formato atualizado; preserve tamanho, posições e zeros.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/chave.h>`. Composição lexical da chave e cálculo/conferência do DV. As posições do CNPJ admitem letras maiúsculas no formato atualizado; preserve tamanho, posições e zeros.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
/*
 * Chave de acesso da NF-e/NFC-e: 44 dígitos.
 *
 *   cUF(2) AAMM(4) CNPJ/CPF(14) mod(2) serie(3) nNF(9) tpEmis(1) cNF(8) cDV(1)
 *
 * Com CNPJ alfanumérico, as 12 primeiras posições do CNPJ (posições 7 a 18
 * da chave, contando de 1) podem ter letras maiúsculas; as demais posições
 * são sempre dígitos.
 *
 * O dígito verificador (cDV) é calculado pelo módulo 11 sobre os 43
 * primeiros caracteres, com pesos de 2 a 9 da direita para a esquerda, e
 * cada caractere vale o seu código ASCII menos 48 (para dígitos, o próprio
 * valor); resto 0 ou 1 resulta em 0. Para gerar a chave de uma nota, use
 * nfe_ide_gerar_chave() (ide.h).
 */

/* Calcula o dígito verificador dos 43 primeiros caracteres de chave.
 * Retorna o dígito (0 a 9), E_ISNULL, E_TAMANHO (menos de 43 caracteres) ou
 * E_VALOR (caractere inválido para a posição). */
int nfe_chave_dv(const char *chave);

/* Confere uma chave completa: 44 caracteres com o dígito verificador correto.
 * Retorna 0, E_ISNULL, E_TAMANHO ou E_VALOR. */
int nfe_chave_validar(const char *chave);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/chave.h).
- [Implementação chave.c](../../../src/libnfe/chave.c).
- [Programa de testes compilável](../../../tests/test_chave.c): `make obj/test_chave` e `./obj/test_chave tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
