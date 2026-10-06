# compacta.h — Compactação gzip com base64

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/compacta.h>`. Compactação gzip com base64 do XML que trafega compactado (MDF-e, NFS-e nacional). A descompactação tem limite de tamanho, porque o texto vem de fora.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <stddef.h>

#include <libnfe/erros.h>

/* Compactação gzip com base64, usada por documentos que trafegam o XML
 * compactado: a autorização síncrona do MDF-e e a NFS-e nacional
 * (dpsXmlGZipB64 no envio, nfseXmlGZipB64 no retorno). */

/* Limite padrão, em bytes, do conteúdo descompactado por nfe_base64_gunzip
 * quando limite é 0 */
#define NFE_GUNZIP_LIMITE (16u * 1024u * 1024u)

/* Compacta tam bytes de dados em gzip e codifica o resultado em base64 (RFC
 * 4648, alfabeto padrão, com '=' e sem quebras de linha). *saida recebe o
 * texto terminado em '\0', a liberar com free(). Em caso de erro *saida não
 * é alterado.
 * Retorna 0, E_ISNULL (dados ou saida NULL), E_TAMANHO (tam acima de 1 GiB)
 * ou E_MALLOC. */
int nfe_gzip_base64(const char *dados, size_t tam, char **saida);

/* Decodifica tam bytes de texto em base64 e descompacta o gzip resultante.
 * Espaços e quebras de linha no texto são ignorados. Como o texto costuma
 * vir de fora (a resposta do autorizador), o conteúdo descompactado não pode
 * passar de limite bytes (0: NFE_GUNZIP_LIMITE). *dados recebe o conteúdo
 * terminado em '\0' (o terminador não conta em *tam_dados), a liberar com
 * free(). Em caso de erro *dados e *tam_dados não são alterados.
 * Retorna 0, E_ISNULL (texto, dados ou tam_dados NULL), E_VALOR (base64 ou
 * gzip inválido ou incompleto), E_TAMANHO (conteúdo acima do limite) ou
 * E_MALLOC. */
int nfe_base64_gunzip(const char *texto, size_t tam, size_t limite,
                      char **dados, size_t *tam_dados);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/compacta.h).
- [Implementação compacta.c](../../../src/libnfe/compacta.c).
- [Programa de testes compilável](../../../tests/test_compacta.c): `make obj/test_compacta` e `./obj/test_compacta`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
