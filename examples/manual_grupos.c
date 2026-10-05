/* Copyright (c) 2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with tooldoce.  If not, see <https://www.gnu.org/licenses/>.
 */

/* Exercita os grupos do manual pela API publica, com casos independentes.
 * Cada caminho seleciona um exemplo estrutural sintetico, sem assinatura,
 * transmissao ou afirmacao de enquadramento fiscal. A raiz e obtida do seu
 * objeto real (produto, imposto, totais, transporte, nota ou item).
 * make exemplos && ./obj/manual_grupos NFe/infNFe/det/imposto/ICMS/ICMS02
 */
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libnfe/erros.h>
#include <libnfe/esquema.h>
#include <libnfe/nfe_nfe.h>

#define CONFERE(expr)                                                          \
	do {                                                                   \
		rc = (expr);                                                   \
		if (rc != 0) {                                                 \
			fprintf(stderr, "falha em %s: %d\n", #expr, rc);       \
			goto fim;                                              \
		}                                                              \
	} while (0)
struct caso {
	const char *caminho;
	const char *raiz;
	int (*preenche)(nfe_grupo *);
};
#include "manual/casos.inc"

static void namespace_no(xmlNodePtr no, xmlNsPtr ns)
{
	for (; no; no = no->next) {
		if (no->type == XML_ELEMENT_NODE)
			xmlSetNs(no, ns);
		namespace_no(no->children, ns);
	}
}
int main(int argc, char **argv)
{
	nfe_prod *prod = NULL;
	nfe_imposto *imp = NULL;
	nfe_total *total = NULL;
	nfe_transp *transp = NULL;
	nfe_nfe *nota = NULL;
	nfe_det *det = NULL;
	nfe_grupo *g = NULL;
	const struct caso *c = NULL;
	xmlBufferPtr buf = NULL;
	xmlTextWriterPtr w = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr no;
	xmlNsPtr ns;
	int rc = 0;
	if (argc != 2) {
		fprintf(stderr, "uso: %s caminho-XML\n", argv[0]);
		return 1;
	}
	for (size_t i = 0; i < sizeof casos / sizeof casos[0]; i++)
		if (strcmp(argv[1], casos[i].caminho) == 0) {
			c = &casos[i];
			break;
		}
	if (!c) {
		fprintf(stderr, "caminho sem caso generico: %s\n", argv[1]);
		return 1;
	}
	if (strcmp(c->raiz, "prod") == 0) {
		prod = nfe_prod_new();
		g = nfe_prod_grupo(prod);
	} else if (strcmp(c->raiz, "imposto") == 0) {
		imp = nfe_imposto_new();
		g = nfe_imposto_grupo(imp);
	} else if (strcmp(c->raiz, "total") == 0) {
		total = nfe_total_new();
		g = nfe_total_grupo(total);
	} else if (strcmp(c->raiz, "transp") == 0) {
		transp = nfe_transp_new();
		g = nfe_transp_grupo(transp);
	} else if (strcmp(c->raiz, "obsItem") == 0) {
		det = nfe_det_new();
		g = nfe_det_grupo(det, c->raiz);
	} else {
		nota = nfe_nfe_new();
		g = nfe_nfe_grupo(nota, c->raiz);
	}
	if (!g) {
		rc = E_MALLOC;
		goto fim;
	}
	CONFERE(c->preenche(g));
	buf = xmlBufferCreate();
	if (!buf) {
		rc = E_MALLOC;
		goto fim;
	}
	w = xmlNewTextWriterMemory(buf, 0);
	if (!w) {
		rc = E_MALLOC;
		goto fim;
	}
	CONFERE(nfe_grupo_write_xml(w, g));
	if (xmlTextWriterEndDocument(w) < 0) {
		rc = E_XML;
		goto fim;
	}
	xmlFreeTextWriter(w);
	w = NULL;
	doc = xmlReadMemory((const char *)xmlBufferContent(buf),
	                    xmlBufferLength(buf), "manual.xml", NULL, 0);
	if (!doc) {
		rc = E_XML;
		goto fim;
	}
	no = xmlDocGetRootElement(doc);
	ns = xmlNewNs(no, BAD_CAST "http://www.portalfiscal.inf.br/nfe", NULL);
	if (!ns) {
		rc = E_MALLOC;
		goto fim;
	}
	namespace_no(no, ns);
	if (xmlSaveFormatFileEnc("-", doc, "UTF-8", 1) < 0)
		rc = E_ARQUIVO;
fim:
	if (rc)
		fprintf(stderr, "erro: %s (%d)\n", nfe_strerror(rc), rc);
	xmlFreeDoc(doc);
	if (w)
		xmlFreeTextWriter(w);
	if (buf)
		xmlBufferFree(buf);
	/* g e emprestado; somente o objeto proprietario o libera. */
	nfe_prod_free(prod);
	nfe_imposto_free(imp);
	nfe_total_free(total);
	nfe_transp_free(transp);
	nfe_nfe_free(nota);
	nfe_det_free(det);
	xmlCleanupParser();
	return rc != 0;
}
