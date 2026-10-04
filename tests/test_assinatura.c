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

/* Testes da assinatura digital (assinatura.h) com o certificado de teste
 * tests/certificados/teste.pfx (autoassinado, senha "teste").
 *
 * Uso: test_assinatura <diretório tests> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/assinatura.h>
#include <libnfe/erros.h>
#include <libnfe/nfe_nfe.h>
#include <libnfe/validar.h>

#include "teste.h"
#include "nota_teste.h"

/* Troca a primeira ocorrência de de por para em xml (alocado) */
static char *troca(const char *xml, const char *de, const char *para)
{
	const char *p = strstr(xml, de);
	char *novo;

	if (!p)
		return NULL;
	novo = malloc(strlen(xml) - strlen(de) + strlen(para) + 1);
	if (novo)
		sprintf(novo, "%.*s%s%s", (int)(p - xml), xml, para,
		        p + strlen(de));
	return novo;
}

int main(int argc, char **argv)
{
	char caminho[1024], *xml = NULL, *assinado = NULL, *outro = NULL;
	nfe_certificado *cert;
	nfe_validador *v;
	nfe_erros *erros;
	nfe_nfe *nfe;
	size_t tam = 0, tam_ass = 0;
	int rc;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	snprintf(caminho, sizeof caminho, "%s/certificados/teste.pfx", argv[1]);

	/* Carga do certificado */
	VERIFICA(nfe_certificado_pfx(caminho, "errada", &rc) == NULL);
	VERIFICA_INT(rc, E_VALOR);
	VERIFICA(nfe_certificado_pfx("/nao/existe.pfx", "teste", &rc) == NULL);
	VERIFICA_INT(rc, E_ARQUIVO);
	VERIFICA(nfe_certificado_pfx(NULL, "teste", &rc) == NULL);
	VERIFICA_INT(rc, E_ISNULL);
	VERIFICA(nfe_certificado_pfx_memoria("abc", 3, "teste", &rc) == NULL);
	VERIFICA_INT(rc, E_VALOR);
	cert = nfe_certificado_pfx(caminho, "teste", &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(cert != NULL);
	if (!cert)
		TESTE_FIM();
	VERIFICA_STR(nfe_certificado_titular(cert),
	             "EMPRESA DE TESTE LTDA:12345678000195");
	/* Válido até 2056: depois de 2050-01-01 */
	VERIFICA(nfe_certificado_validade(cert) > (time_t)2524608000);

	/* Assina a nota gerada pela biblioteca */
	nfe = nota(NFE_MODELO_NFE);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, &tam), 0);
	VERIFICA_INT(nfe_assinar_xml(cert, xml, tam, &assinado, &tam_ass), 0);
	VERIFICA(assinado != NULL);
	if (assinado) {
		VERIFICA_INT(tam_ass, strlen(assinado));
		VERIFICA(strstr(assinado,
		                "</infNFe><Signature xmlns=\"http://www.w3.org/"
		                "2000/09/xmldsig#\"><SignedInfo>") != NULL);
		VERIFICA(strstr(assinado, "<Reference URI=\"#NFe") != NULL);
		VERIFICA(strstr(assinado, "rsa-sha1") != NULL);
		VERIFICA(strstr(assinado, "<X509Certificate>") != NULL);
		VERIFICA(strstr(assinado, "</Signature></NFe>") != NULL);
		/* Sem quebras de linha, nem no base64 */
		VERIFICA(strchr(assinado, '\n') == NULL);
		VERIFICA_INT(nfe_verificar_assinatura(assinado, tam_ass), 0);

		/* O documento assinado é válido contra o schema completo */
		snprintf(caminho, sizeof caminho, "%s/schemas/nfe", argv[1]);
		v = nfe_validador_new(caminho);
		erros = nfe_erros_new();
		VERIFICA(v != NULL);
		if (v) {
			rc = nfe_validar_xml(v, assinado, tam_ass, erros);
			VERIFICA_INT(rc, 0);
			if (rc != 0 && nfe_erros_qtd(erros) > 0)
				fprintf(stderr, "%s\n",
				        nfe_erros_msg(erros, 0));
		}
		nfe_erros_free(erros);
		nfe_validador_free(v);

		/* Documento alterado depois de assinado */
		outro = troca(assinado, "<natOp>VENDA</natOp>",
		              "<natOp>VENDA2</natOp>");
		VERIFICA(outro != NULL);
		if (outro)
			VERIFICA_INT(
			        nfe_verificar_assinatura(outro, strlen(outro)),
			        E_VALOR);
		free(outro);

		/* Referência a outro Id */
		outro = troca(assinado, "<Reference URI=\"#NFe",
		              "<Reference URI=\"#XFe");
		if (outro)
			VERIFICA_INT(
			        nfe_verificar_assinatura(outro, strlen(outro)),
			        E_VALOR);
		free(outro);

		/* Já assinado */
		outro = NULL;
		VERIFICA_INT(
		        nfe_assinar_xml(cert, assinado, tam_ass, &outro, NULL),
		        E_XML);
	}

	/* Sem assinatura, malformado, não é NF-e */
	VERIFICA_INT(nfe_verificar_assinatura(xml, tam), E_XML);
	VERIFICA_INT(nfe_verificar_assinatura("<NFe", 4), E_XML);
	VERIFICA_INT(nfe_assinar_xml(cert, "<a/>", 4, &outro, NULL), E_XML);
	VERIFICA_INT(nfe_assinar_xml(NULL, xml, tam, &outro, NULL), E_ISNULL);
	VERIFICA_INT(nfe_verificar_assinatura(NULL, 0), E_ISNULL);

	/* Assinatura de dados avulsos (RSA-SHA1 em base64). Vetor calculado à
	 * parte com a chave de teste.pfx, sendo P os parâmetros de um QR Code
	 * versão 3 offline:
	 *   printf '%s' "$P" | openssl dgst -sha1 -sign chave.pem | base64 */
	{
		static const char p[] =
		        "35261012345678000195650010000000019000000017|3|2|03|"
		        "1.00|1|12345678000195";
		static const char esperado[] =
		        "a0MEks0uud8ujnOTnv/dSWzGKVxFn2bZsOji1RZd0G5C"
		        "+fS1NNcdSVcRfx8mOdaoZepiGkHsnzR/lj3FihjYRPTb"
		        "HNf9Ow7MAzAScH+hJNho4EeD7OrHGqfsPBZ9xCtA8Jn7"
		        "zh+jptikbr38q/dSqLbBqTX4XdYk0kTiWRzLh3/v5fDn"
		        "xbdNgEafLGmXG+bk8raT02mdS3R7qYYlRl9rA5ULOyPj"
		        "KGoKkAZfDT/xgBpE4MkVRr/2l3JOAKuLphf1fW6DWMeJ"
		        "4gHIEPh2itRZ7iQm3c9Fs1MrbOR7thDgl6xzHnZX3EHx"
		        "DvNfLgBSY1jmW5wdCdfPeJcGBgLz+EOq9Q==";
		char *b64 = NULL;
		size_t tam_b64 = 0;

		VERIFICA_INT(
		        nfe_assinar_dados(cert, p, strlen(p), &b64, &tam_b64),
		        0);
		VERIFICA(b64 != NULL);
		if (b64) {
			VERIFICA_STR(b64, esperado);
			VERIFICA_INT(tam_b64, strlen(esperado));
		}
		free(b64);
		b64 = NULL;
		/* Chave de 2048 bits: 256 bytes, 344 caracteres em base64 */
		VERIFICA_INT(nfe_assinar_dados(cert, "", 0, &b64, NULL), 0);
		VERIFICA(b64 != NULL && strlen(b64) == 344);
		free(b64);
		VERIFICA_INT(nfe_assinar_dados(NULL, p, 1, &b64, NULL),
		             E_ISNULL);
		VERIFICA_INT(nfe_assinar_dados(cert, NULL, 1, &b64, NULL),
		             E_ISNULL);
		VERIFICA_INT(nfe_assinar_dados(cert, p, 1, NULL, NULL),
		             E_ISNULL);
	}

	free(assinado);
	free(xml);
	nfe_nfe_free(nfe);
	nfe_certificado_free(cert);
	nfe_certificado_free(NULL);
	TESTE_FIM();
}
