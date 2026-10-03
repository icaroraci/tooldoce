/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 * */

/* Testes do grupo ide, com validação do XML contra o XSD oficial.
 *
 * Uso: test_ide <diretório tests>
 * O schema usado é <diretório>/schemas/PL_009_V4/ide_v4.00.xsd. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlwriter.h>

#include <libnfe/erros.h>
#include <libnfe/ide.h>
#include <libnfe/refNF.h>
#include <libnfe/refNFe.h>

#include "teste.h"

#define NS    "http://www.portalfiscal.inf.br/nfe"
#define CHAVE "35100812345678000199550010000000421123456787"
#define T0    ((time_t)1282237215) /* 2010-08-19T17:00:15Z */

static xmlSchemaPtr schema;

/* A assinatura do tratador de erros passou a usar const no libxml2 2.12 */
#if LIBXML_VERSION >= 21200
static void ignora_erro(void *ctx, const xmlError *erro)
#else
static void ignora_erro(void *ctx, xmlErrorPtr erro)
#endif
{
	(void)ctx;
	(void)erro;
}

/* Valida o XML contra o schema; retorna 0 se válido. Com mostrar == 0,
 * os erros do libxml2 não são impressos (testes negativos). */
static int valida(const char *xml, int mostrar)
{
	xmlDocPtr doc =
	        xmlReadMemory(xml, (int)strlen(xml), "ide.xml", NULL, 0);
	xmlSchemaValidCtxtPtr ctx;
	int rc;

	if (!doc)
		return -1;
	ctx = xmlSchemaNewValidCtxt(schema);
	if (!mostrar)
		xmlSchemaSetValidStructuredErrors(ctx, ignora_erro, NULL);
	rc = xmlSchemaValidateDoc(ctx, doc);
	xmlSchemaFreeValidCtxt(ctx);
	xmlFreeDoc(doc);
	return rc;
}

/* Gera o XML do ide com o namespace da NF-e no elemento raiz (que a
 * biblioteca ainda não escreve, pois o grupo <NFe> não existe).
 * O resultado deve ser liberado com free(). */
static char *gera(const nfe_ide *ide)
{
	xmlBufferPtr buf = xmlBufferCreate();
	xmlTextWriterPtr w = xmlNewTextWriterMemory(buf, 0);
	const char *conteudo, *pos;
	char *xml = NULL;
	int rc;

	rc = nfe_ide_write_xml(w, ide);
	xmlTextWriterEndDocument(w);
	xmlFreeTextWriter(w);

	conteudo = (const char *)xmlBufferContent(buf);
	pos = strstr(conteudo, "<ide>");
	if (rc == 0 && pos) {
		size_t antes = (size_t)(pos - conteudo);
		xml = malloc(strlen(conteudo) + sizeof NS + 16);
		if (xml)
			sprintf(xml, "%.*s<ide xmlns=\"" NS "\">%s", (int)antes,
			        conteudo, pos + strlen("<ide>"));
	}
	xmlBufferFree(buf);
	return xml;
}

/* Monta um ide completo; aborta o teste se algum setter falhar */
static nfe_ide *novo(time_t dhsaient, nfe_emissao tpemis, nfe_tzd tzd)
{
	nfe_ide *ide = nfe_ide_new();
	int rc = 0;

	if (!ide)
		return NULL;
	rc |= nfe_ide_set_cuf(ide, NFE_UF_SP);
	rc |= nfe_ide_set_cnf(ide, 12345678);
	rc |= nfe_ide_set_natop(ide, "VENDA DE MERCADORIA");
	rc |= nfe_ide_set_serie(ide, 1);
	rc |= nfe_ide_set_nnf(ide, 42);
	rc |= nfe_ide_set_dhemi(ide, T0);
	rc |= nfe_ide_set_dhsaient(ide, dhsaient);
	rc |= nfe_ide_set_cmunfg(ide, 3550308);
	rc |= nfe_ide_set_tpemis(ide, tpemis);
	rc |= nfe_ide_set_cdv(ide, 7);
	rc |= nfe_ide_set_indfinal(ide, NFE_CONSUMIDOR_FINAL);
	rc |= nfe_ide_set_indpres(ide, NFE_PRESENCA_PRESENCIAL);
	rc |= nfe_ide_set_verproc(ide, "tooldoce 0.1");
	rc |= nfe_ide_set_tzd(ide, tzd);
	VERIFICA_INT(rc, 0);
	return ide;
}

static void teste_emissao_normal(void)
{
	nfe_ide *ide = novo(T0 + 3600, NFE_EMISSAO_NORMAL, NFE_TZD_BRASILIA);
	char *xml;

	VERIFICA(ide != NULL);
	xml = gera(ide);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA_INT(valida(xml, 1), 0);
		VERIFICA(strstr(xml, "<cUF>35</cUF>") != NULL);
		VERIFICA(strstr(xml, "<cNF>12345678</cNF>") != NULL);
		VERIFICA(strstr(xml, "<serie>1</serie>") != NULL);
		VERIFICA(strstr(xml, "<nNF>42</nNF>") != NULL);
		VERIFICA(strstr(xml,
		                "<dhEmi>2010-08-19T14:00:15-03:00</dhEmi>") !=
		         NULL);
		VERIFICA(strstr(xml, "<dhSaiEnt>2010-08-19T15:00:15-03:00</"
		                     "dhSaiEnt>") != NULL);
		VERIFICA(strstr(xml, "<cMunFG>3550308</cMunFG>") != NULL);
		VERIFICA(strstr(xml, "<procEmi>0</procEmi>") != NULL);
		VERIFICA(strstr(xml, "indPag") == NULL);
		VERIFICA(strstr(xml, "NFref") == NULL);
	}
	free(xml);
	nfe_ide_free(ide);
}

static void teste_fusos(void)
{
	const nfe_tzd fusos[] = { NFE_TZD_FERNANDO_NORONHA, NFE_TZD_BRASILIA,
		                  NFE_TZD_MANAUS, NFE_TZD_ACRE };
	const char *esperado[] = {
		"<dhEmi>2010-08-19T15:00:15-02:00</dhEmi>",
		"<dhEmi>2010-08-19T14:00:15-03:00</dhEmi>",
		"<dhEmi>2010-08-19T13:00:15-04:00</dhEmi>",
		"<dhEmi>2010-08-19T12:00:15-05:00</dhEmi>",
	};
	size_t i;

	for (i = 0; i < sizeof fusos / sizeof fusos[0]; i++) {
		nfe_ide *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, fusos[i]);
		char *xml = gera(ide);
		VERIFICA(xml != NULL);
		if (xml) {
			VERIFICA(strstr(xml, esperado[i]) != NULL);
			/* dhSaiEnt é opcional e não é gerado sem data */
			VERIFICA(strstr(xml, "dhSaiEnt") == NULL);
			VERIFICA_INT(valida(xml, 1), 0);
		}
		free(xml);
		nfe_ide_free(ide);
	}
}

static void teste_contingencia_e_referencias(void)
{
	nfe_ide *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_CONTINGENCIA_SVC_AN,
	                    NFE_TZD_BRASILIA);
	struct refNFe_s *r1 = RefNFeNew();
	struct refNF_s *r2 = RefNFNew();
	char *xml;

	VERIFICA(ide != NULL);
	VERIFICA_INT(nfe_ide_set_contingencia(
	                     ide, T0, "Falha de comunicacao com a SEFAZ"),
	             0);
	VERIFICA_INT(RefNFeSetrefNFe(r1, CHAVE), 0);
	VERIFICA_INT(RefNFSetcUF(r2, NFE_UF_SP), 0);
	VERIFICA_INT(RefNFSetAAMM(r2, 10, NFE_MES_AGOSTO), 0);
	VERIFICA_INT(RefNFSetCNPJ(r2, "12345678000199"), 0);
	VERIFICA_INT(RefNFSetmod(r2, "01"), 0);
	VERIFICA_INT(RefNFSetSerie(r2, "1"), 0);
	VERIFICA_INT(RefNFSetnNF(r2, "123"), 0);
	VERIFICA_INT(nfe_ide_add_refnfe(ide, r1), 0);
	VERIFICA_INT(nfe_ide_add_refnf(ide, r2), 0);

	xml = gera(ide);
	VERIFICA(xml != NULL);
	if (xml) {
		const char *p1 = strstr(xml, "<refNFe>" CHAVE "</refNFe>");
		const char *p2 = strstr(xml, "<refNF>");
		const char *pj = strstr(xml, "<xJust>");

		VERIFICA_INT(valida(xml, 1), 0);
		VERIFICA(strstr(xml,
		                "<dhCont>2010-08-19T14:00:15-03:00</dhCont>") !=
		         NULL);
		/* xJust antes das referências, e referências na ordem de
		 * inclusão */
		VERIFICA(pj != NULL && p1 != NULL && p2 != NULL);
		VERIFICA(pj < p1 && p1 < p2);
	}
	free(xml);
	nfe_ide_free(ide); /* libera também r1 e r2 */
}

static void teste_limite_referencias(void)
{
	nfe_ide *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, NFE_TZD_BRASILIA);
	struct refNFe_s *extra;
	int i, aceitas = 0;

	for (i = 0; i < NFE_MAX_NFREF; i++)
		if (nfe_ide_add_refnfe(ide, RefNFeNew()) == 0)
			aceitas++;
	VERIFICA_INT(aceitas, NFE_MAX_NFREF);

	extra = RefNFeNew();
	VERIFICA_INT(nfe_ide_add_refnfe(ide, extra), E_VALOR);
	RefNFeDel(extra);

	VERIFICA_INT(nfe_ide_add_refnfe(NULL, NULL), E_ISNULL);
	VERIFICA_INT(nfe_ide_add_refnfe(ide, NULL), E_ISNULL);
	nfe_ide_free(ide);
}

static void teste_valores_invalidos(void)
{
	nfe_ide *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, NFE_TZD_BRASILIA);
	char *xml;

	/* Valores fora do domínio do leiaute são recusados (E_VALOR) */
	VERIFICA_INT(nfe_ide_set_cuf(ide, (nfe_uf)18), E_VALOR);
	VERIFICA_INT(nfe_ide_set_cnf(ide, 100000000u), E_VALOR);
	VERIFICA_INT(nfe_ide_set_mod(ide, (nfe_modelo)56), E_VALOR);
	VERIFICA_INT(nfe_ide_set_serie(ide, 1000), E_VALOR);
	VERIFICA_INT(nfe_ide_set_nnf(ide, 0), E_VALOR);
	VERIFICA_INT(nfe_ide_set_nnf(ide, 1000000000u), E_VALOR);
	VERIFICA_INT(nfe_ide_set_dhemi(ide, NFE_SEM_DATA), E_VALOR);
	VERIFICA_INT(nfe_ide_set_cmunfg(ide, 355030), E_VALOR);
	VERIFICA_INT(nfe_ide_set_tpemis(ide, (nfe_emissao)8), E_VALOR);
	VERIFICA_INT(nfe_ide_set_cdv(ide, 10), E_VALOR);
	VERIFICA_INT(nfe_ide_set_indpres(ide, (nfe_presenca)6), E_VALOR);
	VERIFICA_INT(nfe_ide_set_indpres(ide, NFE_PRESENCA_PRESENCIAL_FORA), 0);
	VERIFICA_INT(nfe_ide_set_procemi(ide, (nfe_processo_emissao)4),
	             E_VALOR);
	VERIFICA_INT(nfe_ide_set_tzd(ide, (nfe_tzd)7), E_VALOR);
	VERIFICA_INT(nfe_ide_set_cuf(NULL, NFE_UF_SP), E_ISNULL);

	/* Textos fora dos limites (E_TAMANHO) */
	VERIFICA_INT(nfe_ide_set_natop(ide, "1234567890123456789012345678901234"
	                                    "567890123456789012345678901"),
	             E_TAMANHO);
	VERIFICA_INT(nfe_ide_set_verproc(ide, ""), E_TAMANHO);
	VERIFICA_INT(nfe_ide_set_contingencia(ide, T0, "curta"), E_TAMANHO);

	/* Valores recusados não alteram o XML, que continua válido */
	xml = gera(ide);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<cUF>35</cUF>") != NULL);
		VERIFICA(strstr(xml, "<nNF>42</nNF>") != NULL);
		VERIFICA(strstr(xml, "<indPres>5</indPres>") != NULL);
		VERIFICA(strstr(xml, "dhCont") == NULL);
		VERIFICA_INT(valida(xml, 1), 0);
	}
	free(xml);
	nfe_ide_free(ide);
}

/* Sem os campos obrigatórios, o XML não é gerado */
static void teste_obrigatorios(void)
{
	nfe_ide *ide = nfe_ide_new();
	xmlBufferPtr buf = xmlBufferCreate();
	xmlTextWriterPtr w = xmlNewTextWriterMemory(buf, 0);

	VERIFICA(ide != NULL);
	VERIFICA_INT(nfe_ide_write_xml(w, ide), E_VALOR);
	VERIFICA_INT(nfe_ide_write_xml(w, NULL), E_ISNULL);
	VERIFICA_INT(nfe_ide_write_xml(NULL, ide), E_ISNULL);

	xmlFreeTextWriter(w);
	xmlBufferFree(buf);
	nfe_ide_free(ide);
	nfe_ide_free(NULL);
}

/* Garante que o validador recusa XML fora do leiaute */
static void teste_validador(void)
{
	nfe_ide *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, NFE_TZD_BRASILIA);
	char *xml = gera(ide);
	char *p;

	VERIFICA(xml != NULL);
	if (xml) {
		p = strstr(xml, "<procEmi>0</procEmi>");
		VERIFICA(p != NULL);
		if (p) {
			/* troca procEmi por um nome inválido de mesmo tamanho
			 */
			memcpy(p + 1, "procEmX", 7);
			memcpy(strstr(p, "</procEmi>") + 2, "procEmX", 7);
			VERIFICA(valida(xml, 0) != 0);
		}
	}
	free(xml);
	nfe_ide_free(ide);
}

int main(int argc, char **argv)
{
	char caminho[1024];
	xmlSchemaParserCtxtPtr pctx;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	snprintf(caminho, sizeof caminho, "%s/schemas/PL_009_V4/ide_v4.00.xsd",
	         argv[1]);
	pctx = xmlSchemaNewParserCtxt(caminho);
	schema = xmlSchemaParse(pctx);
	xmlSchemaFreeParserCtxt(pctx);
	if (!schema) {
		fprintf(stderr, "não foi possível carregar o schema %s\n",
		        caminho);
		return 2;
	}

	teste_emissao_normal();
	teste_fusos();
	teste_contingencia_e_referencias();
	teste_limite_referencias();
	teste_valores_invalidos();
	teste_obrigatorios();
	teste_validador();

	xmlSchemaFree(schema);
	xmlCleanupParser();
	TESTE_FIM();
}
