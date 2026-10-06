/* Copyright (c) 2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

/* Apoio da documentação: observa XMLs que o teste já validou com sucesso.
 * O resultado da validação não é alterado. Carregar apenas no processo do
 * teste, mantendo o runtime do sanitizador antes deste interceptor. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

static void salvar(xmlDocPtr doc)
{
	static unsigned numero;
	const char *pasta = getenv("MANUAL_CAPTURA");
	char arquivo[4096];
	if (!pasta)
		return;
	int tam = snprintf(arquivo, sizeof arquivo, "%s/%ld-%04u.xml", pasta,
	                   (long)getpid(), ++numero);
	if (tam < 0 || (size_t)tam >= sizeof arquivo ||
	    xmlSaveFormatFileEnc(arquivo, doc, "UTF-8", 1) < 0) {
		fprintf(stderr, "falha ao capturar XML do teste\n");
		abort();
	}
}

int xmlSchemaValidateDoc(xmlSchemaValidCtxtPtr ctx, xmlDocPtr doc)
{
	static int (*original)(xmlSchemaValidCtxtPtr, xmlDocPtr);
	if (!original)
		original = dlsym(RTLD_NEXT, "xmlSchemaValidateDoc");
	if (!original)
		abort();
	int rc = original(ctx, doc);
	if (rc == 0)
		salvar(doc);
	return rc;
}

int xmlSchemaValidateOneElement(xmlSchemaValidCtxtPtr ctx, xmlNodePtr no)
{
	static int (*original)(xmlSchemaValidCtxtPtr, xmlNodePtr);
	if (!original)
		original = dlsym(RTLD_NEXT, "xmlSchemaValidateOneElement");
	if (!original)
		abort();
	int rc = original(ctx, no);
	if (rc == 0) {
		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
		if (!doc)
			abort();
		xmlNodePtr copia = xmlDocCopyNode(no, doc, 1);
		if (!copia) {
			xmlFreeDoc(doc);
			abort();
		}
		xmlDocSetRootElement(doc, copia);
		salvar(doc);
		xmlFreeDoc(doc);
	}
	return rc;
}
