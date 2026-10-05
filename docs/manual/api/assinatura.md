# assinatura.h — Carga de certificado A1, assinatura XML ou binária e verificação criptográfica. O certificado deve continuar existindo durante as operações que o usam; a conferência matemática não valida a cadeia ICP-Brasil nem a revogação.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/assinatura.h>`. Carga de certificado A1, assinatura XML ou binária e verificação criptográfica. O certificado deve continuar existindo durante as operações que o usam; a conferência matemática não valida a cadeia ICP-Brasil nem a revogação.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <stddef.h>
#include <time.h>

#include <libnfe/utils.h>

/*
 * Assinatura digital da NF-e (XMLDSig enveloped, RSA-SHA1, canonicalização
 * C14N, referência ao Id de infNFe), como exige o leiaute. Os eventos
 * (<evento>, Id de infEvento; ver evento.h) e a inutilização (<inutNFe>,
 * Id de infInut) são assinados da mesma forma.
 *
 * Por enquanto o certificado é o A1 (arquivo .pfx/.p12). O certificado é um
 * objeto à parte, para que o A3 (token/cartão) possa ser acrescentado depois
 * com outra forma de carga, sem mudar as funções de assinatura.
 *
 * Uso típico:
 *   int rc;
 *   nfe_certificado *cert = nfe_certificado_pfx("empresa.pfx", "senha", &rc);
 *   char *assinado; size_t tam;
 *   rc = nfe_assinar_xml(cert, xml, strlen(xml), &assinado, &tam);
 *   ...
 *   free(assinado);
 *   nfe_certificado_free(cert);
 *
 * A biblioteca não guarda a senha nem grava a chave em lugar algum.
 */

typedef struct nfe_certificado nfe_certificado;

/* Carrega um certificado A1 de um arquivo .pfx/.p12 ou da memória. Em caso
 * de erro retorna NULL e põe em *rc (se rc não for NULL) E_ISNULL,
 * E_ARQUIVO (arquivo inexistente), E_VALOR (senha errada ou arquivo
 * inválido) ou E_MALLOC. */
nfe_certificado *nfe_certificado_pfx(const char *caminho, const char *senha,
                                     int *rc);
nfe_certificado *nfe_certificado_pfx_memoria(const void *dados, size_t tam,
                                             const char *senha, int *rc);
void nfe_certificado_free(nfe_certificado *cert);

/* Nome do titular (CN do certificado; nos certificados ICP-Brasil de
 * pessoa jurídica, "RAZÃO SOCIAL:CNPJ"), ou NULL. Texto pertencente ao
 * certificado. */
const char *nfe_certificado_titular(const nfe_certificado *cert);
/* Fim da validade do certificado, ou (time_t)-1 se desconhecido */
time_t nfe_certificado_validade(const nfe_certificado *cert);

/* Assina o documento <NFe>, <evento> ou <inutNFe> (tam bytes) e devolve em
 * *assinado um novo documento, alocado e terminado em '\0' (libere com
 * free()), com a assinatura (<Signature>) ao fim da raiz; o tamanho vai em
 * *tam_assinado, se não for NULL. Retorna 0, E_ISNULL, E_XML (documento
 * malformado, de outro tipo ou já assinado), E_VALOR (falha ao assinar) ou
 * E_MALLOC. */
int nfe_assinar_xml(const nfe_certificado *cert, const char *xml, size_t tam,
                    char **assinado, size_t *tam_assinado);

/* Como nfe_assinar_xml, para documentos de outros leiautes que seguem o
 * mesmo padrão de assinatura (RSA-SHA1, C14N, <Signature> ao fim da raiz),
 * como os do MDF-e e do CT-e: assina o elemento filho da raiz chamado
 * elemento (ex.: "infMDFe" de <MDFe>, "infEvento" de <eventoMDFe>), que
 * tem de ter o atributo Id. Não confere o namespace nem o nome da raiz.
 * Retorna 0, E_ISNULL, E_XML (documento malformado, sem o elemento ou sem
 * Id, ou já assinado), E_VALOR (falha ao assinar) ou E_MALLOC. */
int nfe_assinar_elemento(const nfe_certificado *cert, const char *xml,
                         size_t tam, const char *elemento, char **assinado,
                         size_t *tam_assinado);

/* Assina tam bytes de dados com a chave privada do certificado (RSA
 * PKCS#1 v1.5 com SHA-1, o algoritmo do leiaute) e devolve a assinatura
 * binária em *assinatura, alocada (libere com free()), com o tamanho em
 * *tam_assinatura. Serve para o que o leiaute manda assinar fora do XML,
 * como o QR Code versão 3 da NFC-e em contingência offline
 * (NT 2025.001). A chave não sai do certificado. Retorna 0, E_ISNULL,
 * E_VALOR (falha ao assinar) ou E_MALLOC. */
int nfe_certificado_assinar(const nfe_certificado *cert, const void *dados,
                            size_t tam, unsigned char **assinatura,
                            size_t *tam_assinatura);

/* Confere a assinatura de um documento <NFe>, <evento> ou <inutNFe>
 * assinado, com o certificado
 * que vem dentro dele (sem conferir a cadeia ICP-Brasil nem se o
 * certificado foi revogado). Retorna 0 (assinatura correta), E_VALOR
 * (assinatura inválida ou documento alterado), E_XML (documento malformado
 * ou sem assinatura), E_ISNULL ou E_MALLOC. */
int nfe_verificar_assinatura(const char *xml, size_t tam);

/* Como nfe_verificar_assinatura, para documentos assinados com
 * nfe_assinar_elemento: a assinatura tem de cobrir o filho da raiz chamado
 * elemento. Mesmos retornos. */
int nfe_verificar_assinatura_elemento(const char *xml, size_t tam,
                                      const char *elemento);

/* Uso interno (sefaz.c): põe o certificado, a chave e a cadeia no SSL_CTX
 * da OpenSSL (ssl_ctx). Retorna 0 ou E_VALOR. */
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/assinatura.h).
- [Implementação assinatura.c](../../../src/libnfe/assinatura.c).
- [Programa de testes compilável](../../../tests/test_assinatura.c): `make obj/test_assinatura` e `./obj/test_assinatura tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
