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

/* Testes do endereço (enderEmit e enderDest), com validação do XML contra
 * o XSD oficial.
 *
 * Uso: test_endereco <diretório tests>
 * O schema usado é <diretório>/schemas/nfe/tipos_v4.00.xsd. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlwriter.h>

#include <libnfe/endereco.h>
#include <libnfe/erros.h>

#include "teste.h"

#define NS "http://www.portalfiscal.inf.br/nfe"

static xmlSchemaPtr schema;

/* Valida o XML contra o schema; retorna 0 se válido */
static int valida(const char *xml)
{
	xmlDocPtr doc =
	        xmlReadMemory(xml, (int)strlen(xml), "end.xml", NULL, 0);
	xmlSchemaValidCtxtPtr ctx;
	int rc;

	if (!doc)
		return -1;
	ctx = xmlSchemaNewValidCtxt(schema);
	rc = xmlSchemaValidateDoc(ctx, doc);
	xmlSchemaFreeValidCtxt(ctx);
	xmlFreeDoc(doc);
	return rc;
}

/* Gera o XML do endereço com o namespace da NF-e no elemento raiz. Retorna
 * NULL se nfe_endereco_write_xml falhar (o código vai em *rc). O resultado
 * deve ser liberado com free(). */
static char *gera(nfe_endereco_tipo tipo, const nfe_endereco *end, int *rc)
{
	xmlBufferPtr buf = xmlBufferCreate();
	xmlTextWriterPtr w = xmlNewTextWriterMemory(buf, 0);
	const char *tag =
	        tipo == NFE_ENDERECO_EMITENTE ? "enderEmit" : "enderDest";
	const char *conteudo;
	char *xml = NULL;

	*rc = nfe_endereco_write_xml(w, tipo, end);
	xmlTextWriterEndDocument(w);
	xmlFreeTextWriter(w);

	conteudo = (const char *)xmlBufferContent(buf);
	if (*rc == 0 && conteudo[0] == '<') {
		size_t n = strlen(tag) + 1; /* "<tag" */
		xml = malloc(strlen(conteudo) + sizeof NS + 16);
		if (xml)
			sprintf(xml, "%.*s xmlns=\"" NS "\"%s", (int)n,
			        conteudo, conteudo + n);
	}
	xmlBufferFree(buf);
	return xml;
}

/* Endereço com os campos obrigatórios do emitente */
static nfe_endereco *novo(void)
{
	nfe_endereco *end = nfe_endereco_new();
	int rc = 0;

	if (!end)
		return NULL;
	rc |= nfe_endereco_set_xlgr(end, "RUA DAS FLORES");
	rc |= nfe_endereco_set_nro(end, "123");
	rc |= nfe_endereco_set_xbairro(end, "CENTRO");
	rc |= nfe_endereco_set_cmun(end, 3550308);
	rc |= nfe_endereco_set_xmun(end, "SÃO PAULO");
	rc |= nfe_endereco_set_uf(end, "SP");
	rc |= nfe_endereco_set_cep(end, "01001000");
	VERIFICA_INT(rc, 0);
	return end;
}

static void teste_emitente(void)
{
	nfe_endereco *end = novo();
	char *xml;
	int rc;

	VERIFICA(end != NULL);
	if (!end)
		return;

	/* Só os obrigatórios */
	xml = gera(NFE_ENDERECO_EMITENTE, end, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<enderEmit xmlns=\"" NS "\">"
		                "<xLgr>RUA DAS FLORES</xLgr><nro>123</nro>"
		                "<xBairro>CENTRO</xBairro>"
		                "<cMun>3550308</cMun>"
		                "<xMun>SÃO PAULO</xMun><UF>SP</UF>"
		                "<CEP>01001000</CEP></enderEmit>") != NULL);
		VERIFICA_INT(valida(xml), 0);
	}
	free(xml);

	/* Com os opcionais */
	VERIFICA_INT(nfe_endereco_set_xcpl(end, "SALA 4"), 0);
	VERIFICA_INT(nfe_endereco_set_cpais(end, NFE_CPAIS_BRASIL), 0);
	VERIFICA_INT(nfe_endereco_set_xpais(end, "BRASIL"), 0);
	VERIFICA_INT(nfe_endereco_set_fone(end, "1133334444"), 0);
	xml = gera(NFE_ENDERECO_EMITENTE, end, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<nro>123</nro><xCpl>SALA 4</xCpl>"
		                     "<xBairro>") != NULL);
		VERIFICA(strstr(xml, "<CEP>01001000</CEP><cPais>1058</cPais>"
		                     "<xPais>BRASIL</xPais>"
		                     "<fone>1133334444</fone>") != NULL);
		VERIFICA_INT(valida(xml), 0);
	}
	free(xml);

	/* Regras próprias do emitente */
	VERIFICA_INT(nfe_endereco_set_xpais(end, "Brazil"), 0);
	xml = gera(NFE_ENDERECO_EMITENTE, end, &rc);
	VERIFICA_INT(rc, E_VALOR);
	VERIFICA(xml == NULL);
	free(xml);
	VERIFICA_INT(nfe_endereco_set_xpais(end, NULL), 0);
	VERIFICA_INT(nfe_endereco_set_cpais(end, 249), 0);
	xml = gera(NFE_ENDERECO_EMITENTE, end, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_endereco_set_cpais(end, 0), 0);
	VERIFICA_INT(nfe_endereco_set_uf(end, NFE_UF_EXTERIOR), 0);
	xml = gera(NFE_ENDERECO_EMITENTE, end, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_endereco_set_uf(end, "SP"), 0);
	VERIFICA_INT(nfe_endereco_set_cep(end, NULL), 0);
	xml = gera(NFE_ENDERECO_EMITENTE, end, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);

	/* O destinatário aceita o mesmo endereço sem CEP */
	xml = gera(NFE_ENDERECO_DESTINATARIO, end, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<enderDest") != NULL);
		VERIFICA(strstr(xml, "CEP") == NULL);
		VERIFICA_INT(valida(xml), 0);
	}
	free(xml);
	nfe_endereco_free(end);
}

static void teste_exterior(void)
{
	nfe_endereco *end = novo();
	char *xml;
	int rc;

	VERIFICA(end != NULL);
	if (!end)
		return;
	VERIFICA_INT(nfe_endereco_set_cmun(end, NFE_CMUN_EXTERIOR), 0);
	VERIFICA_INT(nfe_endereco_set_xmun(end, NFE_XMUN_EXTERIOR), 0);
	VERIFICA_INT(nfe_endereco_set_uf(end, NFE_UF_EXTERIOR), 0);
	VERIFICA_INT(nfe_endereco_set_cep(end, NULL), 0);
	VERIFICA_INT(nfe_endereco_set_cpais(end, 1600), 0);
	VERIFICA_INT(nfe_endereco_set_xpais(end, "CHINA, REPUBLICA POPULAR"),
	             0);
	VERIFICA_INT(nfe_endereco_set_fone(end, "861012345678"), 0);
	xml = gera(NFE_ENDERECO_DESTINATARIO, end, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<cMun>9999999</cMun>"
		                     "<xMun>EXTERIOR</xMun><UF>EX</UF>"
		                     "<cPais>1600</cPais>") != NULL);
		VERIFICA_INT(valida(xml), 0);
	}
	free(xml);
	nfe_endereco_free(end);
}

static void teste_valores_invalidos(void)
{
	nfe_endereco *end = novo();
	char longo[62];
	char *xml;
	int rc;

	VERIFICA(end != NULL);
	if (!end)
		return;

	memset(longo, 'A', sizeof longo - 1);
	longo[sizeof longo - 1] = '\0';

	/* Tamanhos */
	VERIFICA_INT(nfe_endereco_set_xlgr(end, "R"), E_TAMANHO);
	VERIFICA_INT(nfe_endereco_set_xlgr(end, longo), E_TAMANHO);
	VERIFICA_INT(nfe_endereco_set_nro(end, ""), E_TAMANHO);
	VERIFICA_INT(nfe_endereco_set_xcpl(end, ""), E_TAMANHO);
	VERIFICA_INT(nfe_endereco_set_xbairro(end, "C"), E_TAMANHO);
	VERIFICA_INT(nfe_endereco_set_xmun(end, "S"), E_TAMANHO);
	VERIFICA_INT(nfe_endereco_set_xpais(end, longo), E_TAMANHO);

	/* Formato e domínio */
	VERIFICA_INT(nfe_endereco_set_xlgr(end, " RUA"), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_nro(end, "12 "), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_cmun(end, 999999), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_cmun(end, 10000000), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_uf(end, "XX"), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_uf(end, "sp"), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_cep(end, "0100100"), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_cep(end, "01001-000"), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_cpais(end, 10000), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_fone(end, "12345"), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_fone(end, "123456789012345"), E_VALOR);
	VERIFICA_INT(nfe_endereco_set_fone(end, "(11)33334444"), E_VALOR);

	/* NULL */
	VERIFICA_INT(nfe_endereco_set_xlgr(NULL, "RUA"), E_ISNULL);
	VERIFICA_INT(nfe_endereco_set_xlgr(end, NULL), E_ISNULL);
	VERIFICA_INT(nfe_endereco_set_uf(end, NULL), E_ISNULL);
	VERIFICA_INT(nfe_endereco_write_xml(NULL, NFE_ENDERECO_EMITENTE, end),
	             E_ISNULL);

	/* Valores recusados não alteram o endereço */
	xml = gera(NFE_ENDERECO_EMITENTE, end, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<xLgr>RUA DAS FLORES</xLgr>") != NULL);
		VERIFICA(strstr(xml, "<UF>SP</UF><CEP>01001000</CEP>") != NULL);
		VERIFICA_INT(valida(xml), 0);
	}
	free(xml);
	nfe_endereco_free(end);
}

/* Sem os campos obrigatórios, o XML não é gerado */
static void teste_obrigatorios(void)
{
	nfe_endereco *end = nfe_endereco_new();
	char *xml;
	int rc;

	VERIFICA(end != NULL);
	xml = gera(NFE_ENDERECO_DESTINATARIO, end, &rc);
	VERIFICA_INT(rc, E_VALOR);
	VERIFICA(xml == NULL);
	free(xml);
	xml = gera((nfe_endereco_tipo)9, end, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	nfe_endereco_free(end);
	nfe_endereco_free(NULL);
}

int main(int argc, char **argv)
{
	char caminho[1024];
	xmlSchemaParserCtxtPtr pctx;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	snprintf(caminho, sizeof caminho, "%s/schemas/nfe/tipos_v4.00.xsd",
	         argv[1]);
	pctx = xmlSchemaNewParserCtxt(caminho);
	schema = xmlSchemaParse(pctx);
	xmlSchemaFreeParserCtxt(pctx);
	if (!schema) {
		fprintf(stderr, "não foi possível carregar o schema %s\n",
		        caminho);
		return 2;
	}

	teste_emitente();
	teste_exterior();
	teste_valores_invalidos();
	teste_obrigatorios();

	xmlSchemaFree(schema);
	xmlCleanupParser();
	TESTE_FIM();
}
