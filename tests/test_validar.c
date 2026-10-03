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

/* Testes da validação do XML (validar.h) contra os schemas oficiais.
 *
 * Uso: test_validar <diretório tests> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/erros.h>
#include <libnfe/nfe_nfe.h>
#include <libnfe/validar.h>

#include "teste.h"
#include "nota_teste.h"

/* Algum erro menciona o campo e tem linha */
static int tem_erro_no_campo(const nfe_erros *erros, const char *campo)
{
	int i;

	for (i = 0; i < nfe_erros_qtd(erros); i++)
		if (nfe_erros_campo(erros, i) &&
		    strcmp(nfe_erros_campo(erros, i), campo) == 0 &&
		    nfe_erros_msg(erros, i) && nfe_erros_linha(erros, i) > 0)
			return 1;
	return 0;
}

/* Algum erro tem o código (de rejeição da SEFAZ) e o campo */
static int tem_regra(const nfe_erros *erros, int codigo, const char *campo)
{
	int i;

	for (i = 0; i < nfe_erros_qtd(erros); i++)
		if (nfe_erros_codigo(erros, i) == codigo &&
		    strcmp(nfe_erros_campo(erros, i), campo) == 0 &&
		    nfe_erros_linha(erros, i) > 0)
			return 1;
	return 0;
}

/* Troca a primeira ocorrência de de por para em xml (alocado) */
static char *troca(const char *xml, const char *de, const char *para)
{
	const char *p = strstr(xml, de);
	char *novo;
	size_t n;

	if (!p)
		return NULL;
	n = strlen(xml) - strlen(de) + strlen(para);
	novo = malloc(n + 1);
	if (!novo)
		return NULL;
	sprintf(novo, "%.*s%s%s", (int)(p - xml), xml, para, p + strlen(de));
	return novo;
}

/* Troca de por para em xml e confere que a regra (código e campo) falha,
 * com qtd problemas no total */
static void regra(nfe_validador *v, nfe_erros *erros, const char *xml,
                  const char *de, const char *para, int codigo,
                  const char *campo, int qtd)
{
	char *ruim = troca(xml, de, para);

	VERIFICA(ruim != NULL);
	if (!ruim)
		return;
	VERIFICA_INT(nfe_validar_xml(v, ruim, strlen(ruim), erros), E_VALOR);
	VERIFICA_INT(nfe_erros_qtd(erros), qtd);
	VERIFICA(tem_regra(erros, codigo, campo));
	free(ruim);
}

int main(int argc, char **argv)
{
	char dir[1024], *xml = NULL, *ruim;
	nfe_validador *v;
	nfe_erros *erros = nfe_erros_new();
	nfe_nfe *nfe;
	size_t tam = 0;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	snprintf(dir, sizeof dir, "%s/schemas/nfe", argv[1]);
	v = nfe_validador_new(dir);
	VERIFICA(v != NULL);
	VERIFICA(erros != NULL);
	if (!v || !erros)
		TESTE_FIM();

	/* Diretório sem schemas */
	VERIFICA(nfe_validador_new("/nao/existe") == NULL);
	VERIFICA(nfe_dir_schemas() != NULL);

	/* Nota gerada pela biblioteca: válida (sem assinatura ainda) */
	nfe = nota(NFE_MODELO_NFCE);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, &tam), 0);
	VERIFICA_INT(nfe_validar_xml(v, xml, tam, erros), 0);
	VERIFICA_INT(nfe_erros_qtd(erros), 0);
	/* Reaproveitando o validador, e sem lista de erros */
	VERIFICA_INT(nfe_validar_xml(v, xml, tam, NULL), 0);

	/* Campo fora da ordem do leiaute */
	ruim = troca(xml, "<natOp>VENDA</natOp><mod>65</mod>",
	             "<mod>65</mod><natOp>VENDA</natOp>");
	VERIFICA(ruim != NULL);
	if (ruim) {
		VERIFICA_INT(nfe_validar_xml(v, ruim, strlen(ruim), erros),
		             E_VALOR);
		VERIFICA(nfe_erros_qtd(erros) > 0);
		VERIFICA(tem_erro_no_campo(erros, "mod"));
	}
	free(ruim);

	/* Valor fora do padrão */
	ruim = troca(xml, "<cMunFG>3550308</cMunFG>", "<cMunFG>355</cMunFG>");
	VERIFICA(ruim != NULL);
	if (ruim) {
		VERIFICA_INT(nfe_validar_xml(v, ruim, strlen(ruim), erros),
		             E_VALOR);
		VERIFICA(tem_erro_no_campo(erros, "cMunFG"));
	}
	free(ruim);

	/* Regras da SEFAZ além do schema */
	VERIFICA_INT(nfe_erros_codigo(erros, 0), 0);
	/* Chave: dígito verificador errado (e cDV diferente) */
	regra(v, erros, xml, "123456784\"", "123456785\"", 236, "infNFe", 2);
	VERIFICA(tem_regra(erros, 502, "infNFe"));
	/* Chave não corresponde ao número da nota */
	regra(v, erros, xml, "<nNF>1</nNF>", "<nNF>2</nNF>", 502, "infNFe", 1);
	/* Totais diferentes da soma dos itens */
	{
		char *t = troca(xml, "<vProd>30.00</vProd>",
		                "<vProd>31.00</vProd>");

		VERIFICA(t != NULL);
		if (t)
			regra(v, erros, t, "<vNF>30.00</vNF>",
			      "<vNF>31.00</vNF>", 564, "vProd", 1);
		free(t);
	}
	regra(v, erros, xml, "<vNF>30.00</vNF>", "<vNF>29.00</vNF>", 610, "vNF",
	      1);
	regra(v, erros, xml, "<vBC>0.00</vBC>", "<vBC>1.00</vBC>", 531, "vBC",
	      1);
	regra(v, erros, xml, "<vST>0.00</vST>", "<vST>1.00</vST>", 534, "vST",
	      2); /* vST também entra em vNF */
	VERIFICA(tem_regra(erros, 610, "vNF"));
	/* UF e municípios fora de cUF */
	regra(v, erros, xml, "<UF>SP</UF>", "<UF>RJ</UF>", 0, "UF", 1);
	regra(v, erros, xml, "<cMunFG>3550308</cMunFG>",
	      "<cMunFG>3304557</cMunFG>", 0, "cMunFG", 1);
	/* NFC-e */
	regra(v, erros, xml, "<tpImp>4</tpImp>", "<tpImp>1</tpImp>", 0, "tpImp",
	      1);
	regra(v, erros, xml, "<indFinal>1</indFinal>", "<indFinal>0</indFinal>",
	      0, "indFinal", 1);
	regra(v, erros, xml, "<idDest>1</idDest>", "<idDest>2</idDest>", 0,
	      "idDest", 1);
	VERIFICA_INT(nfe_erros_codigo(NULL, 0), 0);

	/* XML malformado e documento que não é NF-e */
	VERIFICA_INT(nfe_validar_xml(v, "<NFe>", 5, erros), E_XML);
	VERIFICA(nfe_erros_qtd(erros) > 0);
	VERIFICA_INT(nfe_validar_xml(v, "<a/>", 4, erros), E_XML);
	VERIFICA_INT(nfe_erros_qtd(erros), 1);
	VERIFICA_INT(nfe_validar_xml(NULL, xml, tam, erros), E_ISNULL);
	VERIFICA(nfe_erros_msg(erros, 99) == NULL);
	VERIFICA(nfe_erros_campo(NULL, 0) == NULL);
	VERIFICA_INT(nfe_erros_linha(erros, -1), 0);

	free(xml);
	nfe_nfe_free(nfe);
	nfe_erros_free(erros);
	nfe_validador_free(v);
	nfe_validador_free(NULL);
	nfe_erros_free(NULL);
	xmlCleanupParser();
	TESTE_FIM();
}
