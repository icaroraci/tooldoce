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

/* Apoio aos testes que geram XML: carrega o schema de testes
 * (schemas/nfe/tipos_v4.00.xsd), gera o XML de um grupo com o namespace da
 * NF-e e o valida. */

#ifndef TOOLDOCE_TESTE_XML_H
#define TOOLDOCE_TESTE_XML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlwriter.h>

#define TESTE_NS "http://www.portalfiscal.inf.br/nfe"

static xmlSchemaPtr teste_schema;

/* Carrega <dir>/schemas/nfe/tipos_v4.00.xsd; retorna 0 ou 2 (erro) */
static inline int teste_carrega_schema(const char *dir)
{
	char caminho[1024];
	xmlSchemaParserCtxtPtr pctx;

	snprintf(caminho, sizeof caminho, "%s/schemas/nfe/tipos_v4.00.xsd",
	         dir);
	pctx = xmlSchemaNewParserCtxt(caminho);
	teste_schema = xmlSchemaParse(pctx);
	xmlSchemaFreeParserCtxt(pctx);
	if (!teste_schema) {
		fprintf(stderr, "não foi possível carregar o schema %s\n",
		        caminho);
		return 2;
	}
	return 0;
}

static inline void teste_libera_schema(void)
{
	xmlSchemaFree(teste_schema);
	xmlCleanupParser();
}

/* Valida o XML contra o schema; retorna 0 se válido */
static inline int teste_valida(const char *xml)
{
	xmlDocPtr doc =
	        xmlReadMemory(xml, (int)strlen(xml), "teste.xml", NULL, 0);
	xmlSchemaValidCtxtPtr ctx;
	int rc;

	if (!doc)
		return -1;
	ctx = xmlSchemaNewValidCtxt(teste_schema);
	rc = xmlSchemaValidateDoc(ctx, doc);
	xmlSchemaFreeValidCtxt(ctx);
	xmlFreeDoc(doc);
	return rc;
}

/* Valida o XML (len bytes) contra o schema <dir>/schemas/<xsd>, carregado
 * na hora; retorna 0 se válido. Os erros são impressos, para ajudar a
 * corrigir o teste. */
static inline int teste_valida_xsd(const char *dir, const char *xsd,
                                   const char *xml, size_t len)
{
	char caminho[1024];
	xmlSchemaParserCtxtPtr pctx;
	xmlSchemaValidCtxtPtr ctx;
	xmlSchemaPtr schema;
	xmlDocPtr doc;
	int rc = -1;

	snprintf(caminho, sizeof caminho, "%s/schemas/%s", dir, xsd);
	pctx = xmlSchemaNewParserCtxt(caminho);
	schema = xmlSchemaParse(pctx);
	xmlSchemaFreeParserCtxt(pctx);
	doc = xmlReadMemory(xml, (int)len, "teste.xml", NULL, 0);
	if (schema && doc) {
		ctx = xmlSchemaNewValidCtxt(schema);
		rc = xmlSchemaValidateDoc(ctx, doc);
		xmlSchemaFreeValidCtxt(ctx);
	}
	xmlFreeDoc(doc);
	xmlSchemaFree(schema);
	return rc;
}

/* Função que escreve um grupo no writer (ex.: nfe_emit_write_xml) */
typedef int (*teste_escreve_fn)(xmlTextWriterPtr writer, const void *obj);

/* Gera o XML com escreve, acrescentando o namespace da NF-e ao elemento
 * raiz. Retorna NULL se escreve falhar (o código vai em *rc). O resultado
 * deve ser liberado com free(). */
static inline char *teste_gera(teste_escreve_fn escreve, const void *obj,
                               int *rc)
{
	xmlBufferPtr buf = xmlBufferCreate();
	xmlTextWriterPtr w = xmlNewTextWriterMemory(buf, 0);
	const char *conteudo;
	char *xml = NULL;

	*rc = escreve(w, obj);
	xmlTextWriterEndDocument(w);
	xmlFreeTextWriter(w);

	conteudo = (const char *)xmlBufferContent(buf);
	if (*rc == 0 && conteudo[0] == '<') {
		size_t n = strcspn(conteudo, " />"); /* "<tag" */
		xml = malloc(strlen(conteudo) + sizeof TESTE_NS + 16);
		if (xml)
			sprintf(xml, "%.*s xmlns=\"" TESTE_NS "\"%s", (int)n,
			        conteudo, conteudo + n);
	}
	xmlBufferFree(buf);
	return xml;
}

#endif
