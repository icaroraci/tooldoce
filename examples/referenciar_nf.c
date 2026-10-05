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

/* Exemplo do manual: refNF e repeticao/escolha em NFref.
 * Sem argumento: ide com refNF pela API especifica e refECF generico.
 * --generico: mesmo XML, preenchendo tambem refNF pelo motor de grupos.
 * --isolado: somente NFref/refNF; o chamador escreve o grupo pai.
 * A saida e um fragmento com namespace, sem assinatura ou autorizacao.
 * make exemplos && ./obj/referenciar_nf
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

#include <libnfe/erros.h>
#include <libnfe/grupo.h>
#include <libnfe/ide.h>
#include <libnfe/refNF.h>

#define NS "http://www.portalfiscal.inf.br/nfe"

/* Para interromper na primeira falha e preservar o codigo original. */
#define CONFERE(expr)                                                          \
	do {                                                                   \
		etapa = #expr;                                                 \
		rc = (expr);                                                   \
		if (rc != 0)                                                   \
			goto fim;                                              \
	} while (0)

/* Os escritores de grupos produzem fragmentos sem namespace. Para esta
 * demonstracao isolada, atribuimos o namespace a toda a arvore antes de
 * salvar. Em uma nota completa, ele vem de <NFe>. Nunca altere assim um
 * documento que ja tenha sido assinado. */
static void define_namespace(xmlNodePtr no, xmlNsPtr ns)
{
	for (; no; no = no->next) {
		if (no->type == XML_ELEMENT_NODE)
			xmlSetNs(no, ns);
		define_namespace(no->children, ns);
	}
}

int main(int argc, char **argv)
{
	nfe_ide *ide = NULL;
	struct refNF_s *ref = NULL;
	nfe_grupo *grupo = NULL; /* emprestado: pertence ao ide */
	xmlBufferPtr buf = NULL;
	xmlTextWriterPtr writer = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr raiz;
	xmlNsPtr ns;
	const char *etapa = "alocacao";
	int generico = 0, isolado = 0, rc = 0;

	if (argc == 2 && strcmp(argv[1], "--generico") == 0)
		generico = 1;
	else if (argc == 2 && strcmp(argv[1], "--isolado") == 0)
		isolado = 1;
	else if (argc != 1) {
		fprintf(stderr, "uso: %s [--generico|--isolado]\n", argv[0]);
		return 1;
	}

	if (!generico) {
		ref = RefNFNew();
		if (!ref) {
			rc = E_MALLOC;
			goto fim;
		}
		/* Dados sinteticos da nota referenciada, nao da nota atual. */
		CONFERE(RefNFSetcUF(ref, NFE_UF_SP));
		CONFERE(RefNFSetAAMM(ref, 26, NFE_MES_SETEMBRO));
		CONFERE(RefNFSetCNPJ(ref, "12345678000195"));
		CONFERE(RefNFSetmod(ref, "01"));
		CONFERE(RefNFSetSerie(ref, "0"));
		CONFERE(RefNFSetnNF(ref, "123"));
	}

	if (!isolado) {
		ide = nfe_ide_new();
		if (!ide) {
			rc = E_MALLOC;
			goto fim;
		}
		CONFERE(nfe_ide_set_cuf(ide, NFE_UF_SP));
		CONFERE(nfe_ide_set_cnf(ide, 12345678));
		CONFERE(nfe_ide_set_natop(ide, "EXEMPLO DE REFERENCIAS"));
		CONFERE(nfe_ide_set_mod(ide, NFE_MODELO_NFE));
		CONFERE(nfe_ide_set_serie(ide, 1));
		CONFERE(nfe_ide_set_nnf(ide, 1));
		/* 2026-10-05T12:00:00Z, escrito no fuso padrao de Brasilia. */
		CONFERE(nfe_ide_set_dhemi(ide, (time_t)1791201600));
		CONFERE(nfe_ide_set_cmunfg(ide, 3550308));
		CONFERE(nfe_ide_set_verproc(ide, "manual NFref"));

		if (generico) {
			CONFERE(nfe_ide_add_nfref(ide, &grupo));
			CONFERE(nfe_grupo_set(grupo, "refNF/cUF", "35"));
			CONFERE(nfe_grupo_set(grupo, "refNF/AAMM", "2609"));
			CONFERE(nfe_grupo_set(grupo, "refNF/CNPJ",
			                      "12345678000195"));
			CONFERE(nfe_grupo_set(grupo, "refNF/mod", "01"));
			CONFERE(nfe_grupo_set(grupo, "refNF/serie", "0"));
			CONFERE(nfe_grupo_set(grupo, "refNF/nNF", "123"));
		} else {
			CONFERE(nfe_ide_add_refnf(ide, ref));
			ref = NULL; /* sucesso: a posse foi transferida ao ide
			             */
		}

		/* Outro documento, em OUTRO NFref: a escolha e por ocorrencia.
		 */
		CONFERE(nfe_ide_add_nfref(ide, &grupo));
		CONFERE(nfe_grupo_set(grupo, "refECF/mod", "2D"));
		CONFERE(nfe_grupo_set(grupo, "refECF/nECF", "1"));
		CONFERE(nfe_grupo_set(grupo, "refECF/nCOO", "456"));
	}

	buf = xmlBufferCreate();
	if (!buf) {
		rc = E_MALLOC;
		goto fim;
	}
	writer = xmlNewTextWriterMemory(buf, 0);
	if (!writer) {
		rc = E_MALLOC;
		goto fim;
	}
	if (isolado) {
		if (xmlTextWriterStartElement(writer, BAD_CAST "NFref") < 0) {
			rc = E_XML;
			goto fim;
		}
		CONFERE(xmlGenRefNFNode(writer, ref));
		if (xmlTextWriterEndElement(writer) < 0) {
			rc = E_XML;
			goto fim;
		}
	} else {
		CONFERE(nfe_ide_write_xml(writer, ide));
	}
	if (xmlTextWriterEndDocument(writer) < 0) {
		rc = E_XML;
		goto fim;
	}
	xmlFreeTextWriter(writer);
	writer = NULL;

	/* Namespace e indentacao servem a leitura/validacao deste fragmento. */
	etapa = "namespace e saida do fragmento";
	doc = xmlReadMemory((const char *)xmlBufferContent(buf),
	                    xmlBufferLength(buf), "referencias.xml", NULL, 0);
	if (!doc) {
		rc = E_XML;
		goto fim;
	}
	raiz = xmlDocGetRootElement(doc);
	ns = xmlNewNs(raiz, BAD_CAST NS, NULL);
	if (!ns) {
		rc = E_MALLOC;
		goto fim;
	}
	define_namespace(raiz, ns);
	if (xmlSaveFormatFileEnc("-", doc, "UTF-8", 1) < 0)
		rc = E_ARQUIVO;

fim:
	if (rc != 0)
		fprintf(stderr, "erro em %s: %s\n", etapa, nfe_strerror(rc));
	xmlFreeDoc(doc);
	if (writer)
		xmlFreeTextWriter(writer);
	if (buf)
		xmlBufferFree(buf);
	RefNFDel(ref); /* NULL apos transferencia; proprio no modo isolado */
	nfe_ide_free(ide); /* libera as referencias especificas e genericas */
	xmlCleanupParser();
	return rc == 0 ? 0 : 1;
}
