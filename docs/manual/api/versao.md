# versao.h — Versão compilada e versão em execução da biblioteca. Registre também o pacote XSD e as normas aplicáveis, pois não são versões intercambiáveis.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/versao.h>`. Versão compilada e versão em execução da biblioteca. Registre também o pacote XSD e as normas aplicáveis, pois não são versões intercambiáveis.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
/* Versão da biblioteca, no formato MAIOR.MENOR.REVISÃO[-PRÉ] (versionamento
 * semântico): a versão maior muda quando a API ou a ABI deixam de ser
 * compatíveis. NFE_VERSAO_PRE marca uma pré-versão (ex.: "rc1") e fica
 * vazio numa versão final. O Makefile lê estas macros para nomear
 * libnfe.so. */
#define NFE_VERSAO_MAIOR   1
#define NFE_VERSAO_MENOR   0
#define NFE_VERSAO_REVISAO 0
#define NFE_VERSAO_PRE     "rc5"
#define NFE_VERSAO         "1.0.0-rc5"

/* Versão da biblioteca carregada em tempo de execução (ex.: "1.0.0-rc1"), que
 * pode diferir de NFE_VERSAO, a dos headers usados na compilação. */
const char *nfe_versao(void);

#endif /* LIBNFE_VERSAO_H */
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/versao.h).
- [Implementação versao.c](../../../src/libnfe/versao.c).
- [Programa de testes compilável](../../../tests/test_versao.c): `make obj/test_versao` e `./obj/test_versao tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
