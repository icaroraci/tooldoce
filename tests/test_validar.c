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
	regra(v, erros, xml, "<tpNF>1</tpNF>", "<tpNF>0</tpNF>", 706, "tpNF",
	      1);
	regra(v, erros, xml, "<idDest>1</idDest>", "<idDest>2</idDest>", 707,
	      "idDest", 1);
	regra(v, erros, xml, "</verProc>",
	      "</verProc><NFref><refNFe>35261012345678000195550010000000011"
	      "123456784</refNFe></NFref>",
	      708, "NFref", 1);
	regra(v, erros, xml, "<tpImp>4</tpImp>", "<tpImp>1</tpImp>", 709,
	      "tpImp", 1);
	regra(v, erros, xml, "<finNFe>1</finNFe>", "<finNFe>4</finNFe>", 715,
	      "finNFe", 1);
	regra(v, erros, xml, "<indFinal>1</indFinal>", "<indFinal>0</indFinal>",
	      716, "indFinal", 1);
	/* Não presencial: também sem o indicativo do intermediador */
	regra(v, erros, xml, "<indPres>1</indPres>", "<indPres>2</indPres>",
	      717, "indPres", 2);
	VERIFICA(tem_regra(erros, 434, "indIntermed"));
	regra(v, erros, xml, "<indPres>1</indPres>",
	      "<indPres>1</indPres><indIntermed>0</indIntermed>", 435,
	      "indIntermed", 1);
	/* Série reservada ao Fisco (a chave também deixa de corresponder) */
	regra(v, erros, xml, "<serie>1</serie>", "<serie>890</serie>", 244,
	      "serie", 2);
	regra(v, erros, xml, "<procEmi>0</procEmi>", "<procEmi>1</procEmi>",
	      451, "serie", 1);
	/* Contingência (tpEmis muda a chave: 502 em todos) */
	regra(v, erros, xml, "</verProc>",
	      "</verProc><dhCont>2026-10-03T05:00:00-03:00</dhCont>"
	      "<xJust>SEM CONEXAO COM A SEFAZ AUTORIZADORA</xJust>",
	      556, "dhCont", 1);
	regra(v, erros, xml, "<tpEmis>1</tpEmis>", "<tpEmis>9</tpEmis>", 557,
	      "tpEmis", 2);
	regra(v, erros, xml, "<tpEmis>1</tpEmis>", "<tpEmis>3</tpEmis>", 570,
	      "tpEmis", 2);
	regra(v, erros, xml, "<tpEmis>1</tpEmis>", "<tpEmis>5</tpEmis>", 714,
	      "tpEmis", 3);
	VERIFICA(tem_regra(erros, 557, "tpEmis"));
	regra(v, erros, xml, "<tpEmis>1</tpEmis>", "<tpEmis>7</tpEmis>", 783,
	      "tpEmis", 2);
	{
		/* Off-line com dhCont e xJust: só a chave */
		char *t = troca(xml, "</verProc>",
		                "</verProc>"
		                "<dhCont>2026-10-03T05:00:00-03:00</dhCont>"
		                "<xJust>SEM CONEXAO COM A SEFAZ AUTORIZADORA"
		                "</xJust>");

		VERIFICA(t != NULL);
		if (t)
			regra(v, erros, t, "<tpEmis>1</tpEmis>",
			      "<tpEmis>9</tpEmis>", 502, "infNFe", 1);
		free(t);
	}
	/* NF-e com o que é próprio da NFC-e */
	{
		char *t = troca(xml, "<mod>65</mod>", "<mod>55</mod>");

		VERIFICA(t != NULL);
		if (t) {
			/* mod também está na chave; tpImp 4 e indPres 1 */
			regra(v, erros, t, "<tpEmis>1</tpEmis>",
			      "<tpEmis>9</tpEmis>", 711, "tpEmis", 4);
			VERIFICA(tem_regra(erros, 710, "tpImp"));
			regra(v, erros, t, "<indPres>1</indPres>",
			      "<indPres>4</indPres><indIntermed>0</"
			      "indIntermed>",
			      794, "indPres", 3);
		}
		free(t);
	}
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

	/* Validador de um schema qualquer, sem as regras da NF-e */
	{
		static const char cons[] =
		        "<consSitNFe "
		        "xmlns=\"http://www.portalfiscal.inf.br/nfe\" "
		        "versao=\"4.00\"><tpAmb>2</tpAmb><xServ>CONSULTAR"
		        "</xServ><chNFe>35261000000000000000650010000000011000"
		        "000010</chNFe></consSitNFe>";
		char caminho[1100];
		nfe_validador *x;

		VERIFICA(nfe_validador_xsd(NULL) == NULL);
		VERIFICA(nfe_validador_xsd("/nao/existe.xsd") == NULL);
		snprintf(caminho, sizeof caminho, "%s/consSitNFe_v4.00.xsd",
		         dir);
		x = nfe_validador_xsd(caminho);
		VERIFICA(x != NULL);
		if (x) {
			VERIFICA_INT(nfe_validar_xsd(x, cons, sizeof cons - 1,
			                             0, erros),
			             0);
			/* A assinatura de mentira não cabe nesta mensagem */
			VERIFICA_INT(nfe_validar_xsd(x, cons, sizeof cons - 1,
			                             1, erros),
			             E_VALOR);
			VERIFICA_INT(nfe_validar_xsd(x, "<a", 2, 0, erros),
			             E_XML);
			VERIFICA(nfe_erros_qtd(erros) > 0);
		}
		nfe_validador_free(x);

		/* NF-e pelo schema geral: sem assinatura, só completando */
		snprintf(caminho, sizeof caminho, "%s/nfe_v4.00.xsd", dir);
		x = nfe_validador_xsd(caminho);
		VERIFICA(x != NULL);
		if (x) {
			VERIFICA_INT(nfe_validar_xsd(x, xml, tam, 1, erros), 0);
			VERIFICA_INT(nfe_validar_xsd(x, xml, tam, 0, erros),
			             E_VALOR);
			VERIFICA_INT(nfe_validar_xsd(NULL, xml, tam, 0, erros),
			             E_ISNULL);
		}
		nfe_validador_free(x);
	}

	free(xml);
	nfe_nfe_free(nfe);
	nfe_erros_free(erros);
	nfe_validador_free(v);
	nfe_validador_free(NULL);
	nfe_erros_free(NULL);
	xmlCleanupParser();
	TESTE_FIM();
}
