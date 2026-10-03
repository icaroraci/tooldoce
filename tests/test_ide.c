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

#define NS     "http://www.portalfiscal.inf.br/nfe"
#define CHAVE  "35100812345678000199550010000000421123456787"
#define T0     ((time_t)1282237215) /* 2010-08-19T17:00:15Z */

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
	xmlDocPtr doc = xmlReadMemory(xml, (int)strlen(xml), "ide.xml", NULL, 0);
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
static char *gera(struct ide_s *ide)
{
	xmlBufferPtr buf = xmlBufferCreate();
	xmlTextWriterPtr w = xmlNewTextWriterMemory(buf, 0);
	const char *conteudo, *pos;
	char *xml = NULL;
	int rc;

	rc = xmlGenideNode(w, ide);
	xmlTextWriterEndDocument(w);
	xmlFreeTextWriter(w);

	conteudo = (const char *)xmlBufferContent(buf);
	pos = strstr(conteudo, "<ide>");
	if (rc == 0 && pos) {
		size_t antes = (size_t)(pos - conteudo);
		xml = malloc(strlen(conteudo) + sizeof NS + 16);
		if (xml)
			sprintf(xml, "%.*s<ide xmlns=\"" NS "\">%s",
			        (int)antes, conteudo, pos + strlen("<ide>"));
	}
	xmlBufferFree(buf);
	return xml;
}

static struct ide_s *novo(time_t dhsaient, nfe_emissao tpemis,
                          struct Cont_s *cont, nfe_tzd tzd)
{
	return ideNew(NULL, NFE_UF_SP, 12345678, "VENDA DE MERCADORIA",
	              NFE_MODELO_NFE, 1, 42, T0, dhsaient, NFE_OPERACAO_SAIDA,
	              NFE_DESTINO_INTERNO, 3550308, NFE_DANFE_NORMAL_RETRATO,
	              tpemis, 7, NFE_AMBIENTE_HOMOLOGACAO, NFE_FINALIDADE_NORMAL,
	              NFE_CONSUMIDOR_FINAL, NFE_PRESENCA_PRESENCIAL,
	              NFE_PROCESSO_APP_CONTRIBUINTE, "tooldoce 0.1", cont, tzd);
}

static void teste_emissao_normal(void)
{
	struct ide_s *ide = novo(T0 + 3600, NFE_EMISSAO_NORMAL, NULL,
	                         NFE_TZD_BRASILIA);
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
		VERIFICA(strstr(xml, "<dhEmi>2010-08-19T14:00:15-03:00</dhEmi>") != NULL);
		VERIFICA(strstr(xml, "<dhSaiEnt>2010-08-19T15:00:15-03:00</dhSaiEnt>") != NULL);
		VERIFICA(strstr(xml, "<cMunFG>3550308</cMunFG>") != NULL);
		VERIFICA(strstr(xml, "<procEmi>0</procEmi>") != NULL);
		VERIFICA(strstr(xml, "indPag") == NULL);
		VERIFICA(strstr(xml, "NFref") == NULL);
	}
	free(xml);
	ideDel(ide);
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
		struct ide_s *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, NULL,
		                         fusos[i]);
		char *xml = gera(ide);
		VERIFICA(xml != NULL);
		if (xml) {
			VERIFICA(strstr(xml, esperado[i]) != NULL);
			/* dhSaiEnt é opcional e não é gerado sem data */
			VERIFICA(strstr(xml, "dhSaiEnt") == NULL);
			VERIFICA_INT(valida(xml, 1), 0);
		}
		free(xml);
		ideDel(ide);
	}
}

static void teste_contingencia_e_referencias(void)
{
	struct Cont_s *cont = ideContNew(NULL, T0, NFE_TZD_BRASILIA,
	                                 "Falha de comunicacao com a SEFAZ");
	struct ide_s *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_CONTINGENCIA_SVC_AN,
	                         cont, NFE_TZD_BRASILIA);
	struct refNFe_s *r1 = RefNFeNew();
	struct refNF_s *r2 = RefNFNew();
	char *xml;

	VERIFICA(cont != NULL);
	VERIFICA(ide != NULL);
	VERIFICA_INT(RefNFeSetrefNFe(r1, CHAVE), 0);
	VERIFICA_INT(RefNFSetcUF(r2, NFE_UF_SP), 0);
	VERIFICA_INT(RefNFSetAAMM(r2, 10, NFE_MES_AGOSTO), 0);
	VERIFICA_INT(RefNFSetCNPJ(r2, "12345678000199"), 0);
	VERIFICA_INT(RefNFSetmod(r2, "01"), 0);
	VERIFICA_INT(RefNFSetSerie(r2, "1"), 0);
	VERIFICA_INT(RefNFSetnNF(r2, "123"), 0);
	VERIFICA_INT(ideAddRefNFe(ide, r1), 0);
	VERIFICA_INT(ideAddRefNF(ide, r2), 0);

	xml = gera(ide);
	VERIFICA(xml != NULL);
	if (xml) {
		const char *p1 = strstr(xml, "<refNFe>" CHAVE "</refNFe>");
		const char *p2 = strstr(xml, "<refNF>");
		const char *pj = strstr(xml, "<xJust>");

		VERIFICA_INT(valida(xml, 1), 0);
		VERIFICA(strstr(xml, "<dhCont>2010-08-19T14:00:15-03:00</dhCont>") != NULL);
		/* xJust antes das referências, e referências na ordem de inclusão */
		VERIFICA(pj != NULL && p1 != NULL && p2 != NULL);
		VERIFICA(pj < p1 && p1 < p2);
	}
	free(xml);
	ideDel(ide); /* libera também cont, r1 e r2 */
}

static void teste_limite_referencias(void)
{
	struct ide_s *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, NULL,
	                         NFE_TZD_BRASILIA);
	struct refNFe_s *extra;
	int i, aceitas = 0;

	for (i = 0; i < NFE_MAX_NFREF; i++)
		if (ideAddRefNFe(ide, RefNFeNew()) == 0)
			aceitas++;
	VERIFICA_INT(aceitas, NFE_MAX_NFREF);

	extra = RefNFeNew();
	VERIFICA_INT(ideAddRefNFe(ide, extra), E_VALOR);
	RefNFeDel(extra);

	VERIFICA_INT(ideAddRefNFe(NULL, NULL), E_ISNULL);
	VERIFICA_INT(ideAddRefNFe(ide, NULL), E_ISNULL);
	ideDel(ide);
}

static void teste_valores_invalidos(void)
{
	/* Fuso inválido */
	VERIFICA(novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, NULL, (nfe_tzd)7) == NULL);

	/* Justificativa com menos de 15 caracteres */
	VERIFICA(ideContNew(NULL, T0, NFE_TZD_BRASILIA, "curta") == NULL);

	/* natOp com mais de 60 caracteres */
	VERIFICA(ideNew(NULL, NFE_UF_SP, 1,
	                "1234567890123456789012345678901234567890123456789012345678901",
	                NFE_MODELO_NFE, 1, 1, T0, NFE_SEM_DATA, NFE_OPERACAO_SAIDA,
	                NFE_DESTINO_INTERNO, 3550308, NFE_DANFE_NORMAL_RETRATO,
	                NFE_EMISSAO_NORMAL, 0, NFE_AMBIENTE_HOMOLOGACAO,
	                NFE_FINALIDADE_NORMAL, NFE_CONSUMIDOR_FINAL,
	                NFE_PRESENCA_PRESENCIAL, NFE_PROCESSO_APP_CONTRIBUINTE,
	                "v1", NULL, NFE_TZD_BRASILIA) == NULL);
}

/* Garante que o validador recusa XML fora do leiaute */
static void teste_validador(void)
{
	struct ide_s *ide = novo(NFE_SEM_DATA, NFE_EMISSAO_NORMAL, NULL,
	                         NFE_TZD_BRASILIA);
	char *xml = gera(ide);
	char *p;

	VERIFICA(xml != NULL);
	if (xml) {
		p = strstr(xml, "<procEmi>0</procEmi>");
		VERIFICA(p != NULL);
		if (p) {
			/* troca procEmi por um nome inválido de mesmo tamanho */
			memcpy(p + 1, "procEmX", 7);
			memcpy(strstr(p, "</procEmi>") + 2, "procEmX", 7);
			VERIFICA(valida(xml, 0) != 0);
		}
	}
	free(xml);
	ideDel(ide);
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
		fprintf(stderr, "não foi possível carregar o schema %s\n", caminho);
		return 2;
	}

	teste_emissao_normal();
	teste_fusos();
	teste_contingencia_e_referencias();
	teste_limite_referencias();
	teste_valores_invalidos();
	teste_validador();

	xmlSchemaFree(schema);
	xmlCleanupParser();
	TESTE_FIM();
}
