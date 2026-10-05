/* Diagnósticos vêm do schema efetivamente carregado, sem tabela por tag. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/xmlerror.h>
#include <libnfe/erros.h>
#include <libnfe/nfe_nfe.h>
#include <libnfe/validar.h>
#include "teste.h"
#include "nota_teste.h"

static const char XML[] = "<dados xmlns=\"urn:diagnostico\">\n"
                          "<item id=\"AB\"><codigo>AB</codigo><modo>01</modo>"
                          "<texto>ok</texto><numero>1.0</numero><a/></item>\n"
                          "<item id=\"CD\"><codigo>CD</codigo><modo>02</modo>"
                          "<texto>ok</texto><numero>2.0</numero><b/></item>\n"
                          "</dados>";

static int igual(const char *a, const char *b)
{
	return a && b && strcmp(a, b) == 0;
}

static int busca(const nfe_erros *erros, const char *faceta)
{
	int i;

	for (i = 0; i < nfe_erros_qtd(erros); i++)
		if (igual(nfe_erros_restricao(erros, i), faceta))
			return i;
	return -1;
}

static char *troca_em(const char *base, const char *de, const char *para)
{
	const char *p = strstr(base, de);
	char *xml;

	if (!p)
		return NULL;
	xml = malloc(strlen(base) + strlen(para) + 1);
	if (xml)
		sprintf(xml, "%.*s%s%s", (int)(p - base), base, para,
		        p + strlen(de));
	return xml;
}

static char *troca(const char *de, const char *para)
{
	return troca_em(XML, de, para);
}

static void refnf(const char *dir, nfe_erros *erros)
{
	static const char refs[] =
	        "<NFref><refNF><cUF>35</cUF>"
	        "<AAMM>202610</AAMM><CNPJ>12345678000195</CNPJ><mod>55</mod>"
	        "<serie>01</serie><nNF>0</nNF></refNF></NFref></ide>";
	static const char *const campos[] = { "AAMM", "mod", "serie", "nNF" };
	static const char *const valores[] = { "202610", "55", "01", "0" };
	char caminho[1024], *base = NULL, *xml;
	size_t tam = 0;
	nfe_nfe *nfe = nota(NFE_MODELO_NFCE);
	nfe_validador *v;
	int i;

	snprintf(caminho, sizeof caminho, "%s/schemas/nfe", dir);
	v = nfe_validador_new(caminho);
	VERIFICA(v != NULL);
	VERIFICA_INT(nfe_nfe_xml(nfe, &base, &tam), 0);
	if (v && base) {
		xml = troca_em(base, "</ide>", refs);
		VERIFICA(xml != NULL);
		if (xml) {
			VERIFICA_INT(
			        nfe_validar_xml(v, xml, strlen(xml), erros),
			        E_VALOR);
			VERIFICA_INT(nfe_erros_qtd(erros), 4);
			for (i = 0; i < 4; i++) {
				VERIFICA(igual(nfe_erros_campo(erros, i),
				               campos[i]));
				VERIFICA(igual(nfe_erros_valor(erros, i),
				               valores[i]));
				VERIFICA(nfe_erros_esperado(erros, i) != NULL);
				snprintf(caminho, sizeof caminho,
				         "/NFe/infNFe/ide/NFref/refNF/%s",
				         campos[i]);
				VERIFICA(igual(nfe_erros_caminho(erros, i),
				               caminho));
			}
			VERIFICA(igual(nfe_erros_restricao(erros, 1),
			               "enumeration"));
			VERIFICA(igual(nfe_erros_esperado(erros, 1),
			               "'01', '02'"));
			VERIFICA(igual(nfe_erros_esperado(erros, 0),
			               "[0-9]{2}[0]{1}[1-9]{1}|[0-9]{2}[1]{1}["
			               "0-2]{1}"));
			free(xml);
		}
	}
	free(base);
	nfe_nfe_free(nfe);
	nfe_validador_free(v);
}

static void confere(nfe_validador *v, nfe_erros *erros, const char *de,
                    const char *para, const char *faceta, const char *valor,
                    const char *esperado, const char *caminho)
{
	char *xml = troca(de, para);
	int i;

	VERIFICA(xml != NULL);
	if (!xml)
		return;
	VERIFICA_INT(nfe_validar_xsd(v, xml, strlen(xml), 0, erros), E_VALOR);
	i = busca(erros, faceta);
	VERIFICA(i >= 0);
	VERIFICA_STR(nfe_erros_valor(erros, i), valor);
	VERIFICA_STR(nfe_erros_esperado(erros, i), esperado);
	VERIFICA_STR(nfe_erros_caminho(erros, i), caminho);
	VERIFICA(nfe_erros_linha(erros, i) > 0);
	VERIFICA(nfe_erros_msg(erros, i) != NULL);
	VERIFICA_INT(nfe_erros_dominio_xml(erros, i), XML_FROM_SCHEMASV);
	VERIFICA(nfe_erros_codigo_xml(erros, i) > 0);
	VERIFICA_INT(nfe_erros_codigo(erros, i), 0);
	free(xml);
}

int main(int argc, char **argv)
{
	char caminho[1024], *xml;
	nfe_validador *v;
	nfe_erros *erros = nfe_erros_new();
	int i;

	if (argc < 2)
		return 2;
	snprintf(caminho, sizeof caminho, "%s/schemas/diagnostico/dados.xsd",
	         argv[1]);
	v = nfe_validador_xsd(caminho);
	VERIFICA(v != NULL);
	VERIFICA(erros != NULL);
	if (!v || !erros)
		TESTE_FIM();
	VERIFICA_INT(nfe_validar_xsd(v, XML, sizeof XML - 1, 0, erros), 0);
	/* Herança de tipo importado e conteúdo antes de normalizar xs:token. */
	confere(v, erros, "<codigo>CD</codigo>", "<codigo> x\n </codigo>",
	        "pattern", " x\n ", "[A-Z]{2}", "/dados/item[2]/codigo");
	confere(v, erros, "<modo>02</modo>", "<modo>55</modo>", "enumeration",
	        "55", "'01', '02'", "/dados/item[2]/modo");
	confere(v, erros, "<texto>ok</texto>", "<texto></texto>", "minLength",
	        "", "2", "/dados/item[1]/texto");
	confere(v, erros, "<texto>ok</texto>", "<texto>áéíóú</texto>",
	        "maxLength", "áéíóú", "4", "/dados/item[1]/texto");
	confere(v, erros, "id=\"CD\"", "id=\"C\"", "length", "C", "2",
	        "/dados/item[2]");
	confere(v, erros, "<numero>2.0</numero>", "<numero>0</numero>",
	        "minInclusive", "0", "1", "/dados/item[2]/numero");
	confere(v, erros, "<numero>2.0</numero>", "<numero>10</numero>",
	        "maxExclusive", "10", "10", "/dados/item[2]/numero");
	confere(v, erros, "<numero>2.0</numero>", "<numero>1234</numero>",
	        "totalDigits", "1234", "3", "/dados/item[2]/numero");
	confere(v, erros, "<numero>2.0</numero>", "<numero>2.01</numero>",
	        "fractionDigits", "2.01", "1", "/dados/item[2]/numero");
	/* Escolha/ausência: mensagem original, sem inventar valor ou faceta. */
	xml = troca("<b/>", "");
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA_INT(nfe_validar_xsd(v, xml, strlen(xml), 0, erros),
		             E_VALOR);
		VERIFICA_INT(nfe_erros_qtd(erros), 1);
		VERIFICA(igual(nfe_erros_caminho(erros, 0), "/dados/item[2]"));
		VERIFICA(nfe_erros_valor(erros, 0) == NULL);
		VERIFICA(nfe_erros_restricao(erros, 0) == NULL);
		VERIFICA(nfe_erros_esperado(erros, 0) == NULL);
		VERIFICA(strstr(nfe_erros_msg(erros, 0), "Missing child") !=
		         NULL);
		free(xml);
	}
	/* Tipo sem faceta: nunca perde o diagnóstico nativo. */
	xml = troca("<numero>2.0</numero>", "<numero>abc</numero>");
	if (xml) {
		VERIFICA_INT(nfe_validar_xsd(v, xml, strlen(xml), 0, erros),
		             E_VALOR);
		VERIFICA(igual(nfe_erros_valor(erros, 0), "abc"));
		VERIFICA(nfe_erros_restricao(erros, 0) == NULL);
		VERIFICA(nfe_erros_msg(erros, 0) != NULL);
		free(xml);
	}
	/* Dados copiados continuam válidos depois que o XML é liberado. */
	VERIFICA(igual(nfe_erros_caminho(erros, 0), "/dados/item[2]/numero"));
	VERIFICA_INT(nfe_validar_xsd(v, "<dados", 6, 0, erros), E_XML);
	VERIFICA(nfe_erros_codigo_xml(erros, 0) > 0);
	VERIFICA_INT(nfe_erros_dominio_xml(erros, 0), XML_FROM_PARSER);
	VERIFICA(nfe_erros_esperado(erros, 0) == NULL);
	VERIFICA_INT(nfe_validar_xsd(v, XML, sizeof XML - 1, 0, erros), 0);
	VERIFICA_INT(nfe_erros_qtd(erros), 0);
	for (i = -1; i <= 0; i++) {
		VERIFICA(nfe_erros_valor(erros, i) == NULL);
		VERIFICA(nfe_erros_caminho(erros, i) == NULL);
		VERIFICA(nfe_erros_restricao(erros, i) == NULL);
		VERIFICA(nfe_erros_esperado(erros, i) == NULL);
		VERIFICA_INT(nfe_erros_dominio_xml(erros, i), 0);
		VERIFICA_INT(nfe_erros_codigo_xml(erros, i), 0);
	}
	VERIFICA(nfe_erros_valor(NULL, 0) == NULL);
	VERIFICA(nfe_erros_caminho(NULL, 0) == NULL);
	VERIFICA(nfe_erros_restricao(NULL, 0) == NULL);
	VERIFICA(nfe_erros_esperado(NULL, 0) == NULL);
	VERIFICA_INT(nfe_erros_dominio_xml(NULL, 0), 0);
	VERIFICA_INT(nfe_erros_codigo_xml(NULL, 0), 0);
	/* Outro XSD, mesma tag: o diagnóstico acompanha o schema carregado. */
	nfe_validador_free(v);
	snprintf(caminho, sizeof caminho, "%s/schemas/diagnostico/alterado.xsd",
	         argv[1]);
	v = nfe_validador_xsd(caminho);
	VERIFICA(v != NULL);
	if (v) {
		static const char ruim[] =
		        "<d:codigo xmlns:d=\"urn:diagnostico\">AB</d:codigo>";
		static const char bom[] =
		        "<codigo xmlns=\"urn:diagnostico\">123</codigo>";
		VERIFICA_INT(
		        nfe_validar_xsd(v, ruim, sizeof ruim - 1, 0, erros),
		        E_VALOR);
		VERIFICA(igual(nfe_erros_esperado(erros, 0), "[0-9]{3}"));
		VERIFICA(igual(nfe_erros_caminho(erros, 0), "/d:codigo"));
		VERIFICA_INT(nfe_validar_xsd(v, bom, sizeof bom - 1, 0, erros),
		             0);
		VERIFICA_INT(nfe_erros_qtd(erros), 0);
	}
	refnf(argv[1], erros);
	nfe_erros_free(erros);
	nfe_validador_free(v);
	xmlCleanupParser();
	TESTE_FIM();
}
