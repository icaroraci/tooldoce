# cnpjcpf.h — Validação de documentos numéricos e CNPJ alfanumérico, com formatos e dígitos verificadores. Um documento válido matematicamente não comprova existência ou situação cadastral.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/cnpjcpf.h>`. Validação de documentos numéricos e CNPJ alfanumérico, com formatos e dígitos verificadores. Um documento válido matematicamente não comprova existência ou situação cadastral.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
/*
 * Validação de CNPJ e CPF (só os caracteres, sem pontuação).
 *
 * CNPJ: 14 posições. As 12 primeiras podem ter dígitos ou letras maiúsculas
 * (CNPJ alfanumérico, IN RFB nº 2.229/2024); as 2 últimas são os dígitos
 * verificadores. Os verificadores são calculados pelo módulo 11, com pesos
 * de 2 a 9 da direita para a esquerda, e cada caractere vale o seu código
 * ASCII menos 48 (0-9 -> 0-9, A-Z -> 17-42); resto 0 ou 1 resulta em 0.
 * Exemplo oficial: 12ABC34501DE35.
 *
 * CPF: 11 dígitos, com os dois verificadores pelo módulo 11.
 *
 * Retornam 0 (válido), E_ISNULL, E_TAMANHO (tamanho errado) ou E_VALOR
 * (caractere inválido, dígito verificador errado ou todos os dígitos
 * iguais).
 */

int nfe_cnpj_validar(const char *cnpj);
int nfe_cpf_validar(const char *cpf);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/cnpjcpf.h).
- [Implementação cnpjcpf.c](../../../src/libnfe/cnpjcpf.c).
- [Programa de testes compilável](../../../tests/test_cnpjcpf.c): `make obj/test_cnpjcpf` e `./obj/test_cnpjcpf tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
