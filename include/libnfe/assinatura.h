/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 **
 ** This file is part of tooldoce.
 **
 ** tooldoce is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published
 ** by the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** tooldoce is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with tooldoce.  If not, see <https://www.gnu.org/licenses/>.
 ** */

#ifndef LIBNFE_ASSINATURA_H
#define LIBNFE_ASSINATURA_H

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

/* Confere a assinatura de um documento <NFe>, <evento> ou <inutNFe>
 * assinado, com o certificado
 * que vem dentro dele (sem conferir a cadeia ICP-Brasil nem se o
 * certificado foi revogado). Retorna 0 (assinatura correta), E_VALOR
 * (assinatura inválida ou documento alterado), E_XML (documento malformado
 * ou sem assinatura), E_ISNULL ou E_MALLOC. */
int nfe_verificar_assinatura(const char *xml, size_t tam);

/* Uso interno (sefaz.c): põe o certificado, a chave e a cadeia no SSL_CTX
 * da OpenSSL (ssl_ctx). Retorna 0 ou E_VALOR. */
NFE_INTERNO int nfe_certificado_ssl_ctx(const nfe_certificado *cert,
                                        void *ssl_ctx);

#endif
