/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 **
 ** This file is part of tooldoce.
 **
 ** tooldoce is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** tooldoce is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 ** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

#include <libnfe/erros.h>
#include <libnfe/validar.h>

#ifndef NFE_DIR_SCHEMAS
#define NFE_DIR_SCHEMAS "/usr/local/share/tooldoce/schemas"
#endif

#define NS_NFE "http://www.portalfiscal.inf.br/nfe"
#define NS_DS  "http://www.w3.org/2000/09/xmldsig#"

/* Assinatura de mentira, com a estrutura exigida pelo schema, usada só na
 * cópia do documento que é validada quando ele ainda não está assinado */
static const char ASSINATURA[] =
        "<Signature xmlns=\"" NS_DS "\"><SignedInfo>"
        "<CanonicalizationMethod Algorithm=\"http://www.w3.org/TR/2001/"
        "REC-xml-c14n-20010315\"/><SignatureMethod Algorithm=\"" NS_DS
        "rsa-sha1\"/><Reference URI=\"#NFe\"><Transforms><Transform "
        "Algorithm=\"" NS_DS "enveloped-signature\"/><Transform Algorithm="
        "\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315\"/></Transforms>"
        "<DigestMethod Algorithm=\"" NS_DS "sha1\"/><DigestValue>AAAA"
        "</DigestValue></Reference></SignedInfo><SignatureValue>AAAA"
        "</SignatureValue><KeyInfo><X509Data><X509Certificate>AAAA"
        "</X509Certificate></X509Data></KeyInfo></Signature>";

struct erro_s {
	char *msg;
	char *campo;
	int linha;
};

struct nfe_erros {
	struct erro_s *e;
	int n, cap;
};

struct nfe_validador {
	xmlSchemaPtr schema;
};

nfe_erros *nfe_erros_new(void)
{
	return (nfe_erros *)calloc(1, sizeof(nfe_erros));
}

void nfe_erros_limpa(nfe_erros *erros)
{
	int i;

	if (!erros)
		return;
	for (i = 0; i < erros->n; i++) {
		free(erros->e[i].msg);
		free(erros->e[i].campo);
	}
	erros->n = 0;
}

void nfe_erros_free(nfe_erros *erros)
{
	if (!erros)
		return;
	nfe_erros_limpa(erros);
	free(erros->e);
	free(erros);
}

int nfe_erros_qtd(const nfe_erros *erros)
{
	return erros ? erros->n : 0;
}

const char *nfe_erros_msg(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].msg : NULL;
}

const char *nfe_erros_campo(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].campo : NULL;
}

int nfe_erros_linha(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].linha : 0;
}

/* Cópia de s (NULL vira NULL); tira a quebra de linha do fim */
static char *copia(const char *s)
{
	char *c;
	size_t n;

	if (!s)
		return NULL;
	n = strlen(s);
	while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
		n--;
	c = (char *)malloc(n + 1);
	if (c) {
		memcpy(c, s, n);
		c[n] = '\0';
	}
	return c;
}

static void acrescenta(nfe_erros *erros, const char *msg, const char *campo,
                       int linha)
{
	struct erro_s *e;

	if (!erros)
		return;
	if (erros->n == erros->cap) {
		int cap = erros->cap ? erros->cap * 2 : 8;
		e = (struct erro_s *)realloc(
		        erros->e, (size_t)cap * sizeof(struct erro_s));
		if (!e)
			return;
		erros->e = e;
		erros->cap = cap;
	}
	e = &erros->e[erros->n];
	e->msg = copia(msg ? msg : "erro");
	e->campo = copia(campo);
	e->linha = linha;
	if (e->msg)
		erros->n++;
	else
		free(e->campo);
}

/* Tratador de erros da libxml2: guarda o erro na lista */
#if LIBXML_VERSION >= 21200
static void guarda_erro(void *ctx, const xmlError *erro)
#else
static void guarda_erro(void *ctx, xmlErrorPtr erro)
#endif
{
	const char *campo = NULL;

	if (!erro)
		return;
	if (erro->node && ((xmlNodePtr)erro->node)->type == XML_ELEMENT_NODE)
		campo = (const char *)((xmlNodePtr)erro->node)->name;
	acrescenta((nfe_erros *)ctx, erro->message, campo, erro->line);
}

const char *nfe_dir_schemas(void)
{
	return NFE_DIR_SCHEMAS;
}

nfe_validador *nfe_validador_new(const char *dir_schemas)
{
	char caminho[4096];
	xmlSchemaParserCtxtPtr pctx;
	nfe_validador *v;
	int n;

	if (!dir_schemas)
		dir_schemas = nfe_dir_schemas();
	n = snprintf(caminho, sizeof caminho, "%s/nfe_v4.00.xsd", dir_schemas);
	if (n < 0 || (size_t)n >= sizeof caminho)
		return NULL;
	/* Sem o arquivo, a libxml2 imprimiria um aviso: confere antes */
	{
		FILE *f = fopen(caminho, "rb");

		if (!f)
			return NULL;
		fclose(f);
	}
	v = (nfe_validador *)calloc(1, sizeof(nfe_validador));
	if (!v)
		return NULL;
	pctx = xmlSchemaNewParserCtxt(caminho);
	if (pctx) {
		/* Erros de carga não são impressos */
		xmlSchemaSetParserStructuredErrors(pctx, guarda_erro, NULL);
		v->schema = xmlSchemaParse(pctx);
		xmlSchemaFreeParserCtxt(pctx);
	}
	if (!v->schema) {
		free(v);
		return NULL;
	}
	return v;
}

void nfe_validador_free(nfe_validador *v)
{
	if (!v)
		return;
	xmlSchemaFree(v->schema);
	free(v);
}

/* Acrescenta a assinatura de mentira ao fim de <NFe>, se não houver
 * assinatura. Retorna 0, E_XML ou E_MALLOC. */
static int assina_de_mentira(xmlDocPtr doc)
{
	xmlNodePtr raiz = xmlDocGetRootElement(doc), filho, assinatura = NULL;
	xmlDocPtr frag;
	xmlNodePtr copia_no;

	if (!raiz || !xmlStrEqual(raiz->name, BAD_CAST "NFe") || !raiz->ns ||
	    !xmlStrEqual(raiz->ns->href, BAD_CAST NS_NFE))
		return E_XML;
	for (filho = raiz->children; filho; filho = filho->next)
		if (filho->type == XML_ELEMENT_NODE &&
		    xmlStrEqual(filho->name, BAD_CAST "Signature"))
			return 0;
	frag = xmlReadMemory(ASSINATURA, (int)sizeof ASSINATURA - 1,
	                     "assinatura.xml", NULL,
	                     XML_PARSE_NONET | XML_PARSE_NOERROR);
	if (!frag)
		return E_MALLOC;
	assinatura = xmlDocGetRootElement(frag);
	copia_no = xmlDocCopyNode(assinatura, doc, 1);
	xmlFreeDoc(frag);
	if (!copia_no)
		return E_MALLOC;
	if (!xmlAddChild(raiz, copia_no)) {
		xmlFreeNode(copia_no);
		return E_MALLOC;
	}
	return 0;
}

int nfe_validar_xml(nfe_validador *v, const char *xml, size_t tam,
                    nfe_erros *erros)
{
	xmlSchemaValidCtxtPtr ctx;
	xmlParserCtxtPtr pctx;
	xmlDocPtr doc;
	int rc;

	if (!v || !xml)
		return E_ISNULL;
	nfe_erros_limpa(erros);
	if (tam > 0x7fffffff)
		return E_XML;
	pctx = xmlNewParserCtxt();
	if (!pctx)
		return E_MALLOC;
	/* Sem impressão: o erro de leitura é pego do contexto */
	doc = xmlCtxtReadMemory(pctx, xml, (int)tam, "nfe.xml", NULL,
	                        XML_PARSE_NONET | XML_PARSE_NOERROR |
	                                XML_PARSE_NOWARNING);
	if (!doc) {
		const xmlError *e = xmlCtxtGetLastError(pctx);

		acrescenta(erros, e ? e->message : "XML malformado", NULL,
		           e ? e->line : 0);
		xmlFreeParserCtxt(pctx);
		return E_XML;
	}
	xmlFreeParserCtxt(pctx);
	rc = assina_de_mentira(doc);
	if (rc == E_XML)
		acrescenta(erros, "o elemento raiz não é <NFe> da NF-e", NULL,
		           0);
	if (rc != 0) {
		xmlFreeDoc(doc);
		return rc;
	}
	ctx = xmlSchemaNewValidCtxt(v->schema);
	if (!ctx) {
		xmlFreeDoc(doc);
		return E_MALLOC;
	}
	xmlSchemaSetValidStructuredErrors(ctx, guarda_erro, erros);
	rc = xmlSchemaValidateDoc(ctx, doc);
	xmlSchemaFreeValidCtxt(ctx);
	xmlFreeDoc(doc);
	if (rc < 0)
		return E_MALLOC;
	return rc == 0 ? 0 : E_VALOR;
}
