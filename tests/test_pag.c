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

/* Testes do pagamento (pag), com validação do XML contra o XSD oficial.
 *
 * Uso: test_pag <diretório tests> */

#include <libnfe/erros.h>
#include <libnfe/pag.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *pag)
{
	return nfe_pag_write_xml(writer, (const nfe_pag *)pag);
}

static nfe_detpag *forma(nfe_meio_pagamento tpag, const char *vpag)
{
	nfe_detpag *dp = nfe_detpag_new();
	int rc = 0;

	if (!dp)
		return NULL;
	rc |= nfe_detpag_set_tpag(dp, tpag);
	rc |= nfe_detpag_set_vpag(dp, vpag);
	VERIFICA_INT(rc, 0);
	return dp;
}

static void teste_dinheiro_com_troco(void)
{
	nfe_pag *pag = nfe_pag_new();
	char *xml;
	int rc;

	VERIFICA(pag != NULL);
	if (!pag)
		return;
	VERIFICA_INT(nfe_pag_add_detpag(pag, forma(NFE_MEIO_DINHEIRO, "20.00")),
	             0);
	VERIFICA_INT(nfe_pag_set_vtroco(pag, "5.00"), 0);
	xml = teste_gera(escreve, pag, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<pag xmlns=\"" TESTE_NS "\"><detPag>"
		                "<tPag>01</tPag><vPag>20.00</vPag></detPag>"
		                "<vTroco>5.00</vTroco></pag>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	VERIFICA_INT(nfe_pag_set_vtroco(pag, NULL), 0);
	xml = teste_gera(escreve, pag, &rc);
	VERIFICA(xml != NULL);
	if (xml)
		VERIFICA(strstr(xml, "vTroco") == NULL);
	free(xml);
	nfe_pag_free(pag);
}

static void teste_varias_formas(void)
{
	nfe_pag *pag = nfe_pag_new();
	nfe_detpag *cartao = forma(NFE_MEIO_CARTAO_CREDITO, "50.00");
	nfe_detpag *outros = forma(NFE_MEIO_OUTROS, "10.00");
	nfe_detpag *pix = forma(NFE_MEIO_PIX_DINAMICO, "40.00");
	char *xml;
	int rc;

	VERIFICA(pag && cartao && outros && pix);
	if (!pag || !cartao || !outros || !pix) {
		nfe_pag_free(pag);
		nfe_detpag_free(cartao);
		nfe_detpag_free(outros);
		nfe_detpag_free(pix);
		return;
	}
	VERIFICA_INT(nfe_detpag_set_indpag(cartao, NFE_PAGAMENTO_PRAZO), 0);
	VERIFICA_INT(nfe_detpag_set_card(cartao, NFE_INTEGRACAO_TEF,
	                                 "12345678000195", 2, "AUT123456",
	                                 "12ABC34501DE35", "TERM-01"),
	             0);
	VERIFICA_INT(nfe_detpag_set_xpag(outros, "VALE DA EMPRESA"), 0);
	VERIFICA_INT(nfe_detpag_set_dpag(outros, "2026-10-03"), 0);
	VERIFICA_INT(nfe_detpag_set_local(pix, "12345678000195", "SP"), 0);
	VERIFICA_INT(nfe_detpag_set_card(pix, NFE_INTEGRACAO_POS, NULL, 0, NULL,
	                                 NULL, NULL),
	             0);
	VERIFICA_INT(nfe_pag_add_detpag(pag, cartao), 0);
	VERIFICA_INT(nfe_pag_add_detpag(pag, outros), 0);
	VERIFICA_INT(nfe_pag_add_detpag(pag, pix), 0);
	xml = teste_gera(escreve, pag, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<detPag><indPag>1</indPag><tPag>03</tPag>"
		                     "<vPag>50.00</vPag><card>"
		                     "<tpIntegra>1</tpIntegra>"
		                     "<CNPJ>12345678000195</CNPJ>"
		                     "<tBand>02</tBand><cAut>AUT123456</cAut>"
		                     "<CNPJReceb>12ABC34501DE35</CNPJReceb>"
		                     "<idTermPag>TERM-01</idTermPag></card>"
		                     "</detPag>") != NULL);
		VERIFICA(strstr(xml, "<detPag><tPag>99</tPag>"
		                     "<xPag>VALE DA EMPRESA</xPag>"
		                     "<vPag>10.00</vPag><dPag>2026-10-03</dPag>"
		                     "</detPag>") != NULL);
		VERIFICA(strstr(xml, "<detPag><tPag>17</tPag><vPag>40.00</vPag>"
		                     "<CNPJPag>12345678000195</CNPJPag>"
		                     "<UFPag>SP</UFPag><card>"
		                     "<tpIntegra>2</tpIntegra></card></detPag>"
		                     "</pag>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Removendo opcionais */
	VERIFICA_INT(nfe_detpag_remove_card(cartao), 0);
	VERIFICA_INT(nfe_detpag_set_indpag(cartao, NFE_PAGAMENTO_NAO_INFORMADO),
	             0);
	VERIFICA_INT(nfe_detpag_set_xpag(outros, NULL), 0);
	VERIFICA_INT(nfe_detpag_set_dpag(outros, NULL), 0);
	VERIFICA_INT(nfe_detpag_set_local(pix, NULL, NULL), 0);
	xml = teste_gera(escreve, pag, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "indPag") == NULL);
		VERIFICA(strstr(xml, "<tpIntegra>1") == NULL);
		VERIFICA(strstr(xml, "xPag") == NULL);
		VERIFICA(strstr(xml, "dPag") == NULL);
		VERIFICA(strstr(xml, "CNPJPag") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_pag_free(pag);
}

static void teste_valores_invalidos(void)
{
	nfe_pag *pag = nfe_pag_new();
	nfe_detpag *dp = nfe_detpag_new();
	char *xml;
	int i, rc = 0;

	VERIFICA(pag && dp);
	if (!pag || !dp) {
		nfe_pag_free(pag);
		nfe_detpag_free(dp);
		return;
	}
	VERIFICA_INT(nfe_detpag_set_indpag(dp, (nfe_forma_pagamento)2),
	             E_VALOR);
	VERIFICA_INT(nfe_detpag_set_tpag(dp, (nfe_meio_pagamento)100), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_tpag(dp, (nfe_meio_pagamento)-1), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_xpag(dp, "V"), E_TAMANHO);
	VERIFICA_INT(nfe_detpag_set_vpag(dp, "20,00"), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_vpag(dp, NULL), E_ISNULL);
	VERIFICA_INT(nfe_detpag_set_dpag(dp, "2026-02-30"), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_dpag(dp, "03/10/2026"), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_local(dp, "12345678000195", NULL), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_local(dp, "12345678000195", "EX"), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_local(dp, "12345678000196", "SP"), E_VALOR);
	VERIFICA_INT(nfe_detpag_set_card(dp, (nfe_integracao)3, NULL, 0, NULL,
	                                 NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_detpag_set_card(dp, NFE_INTEGRACAO_TEF, NULL, 100,
	                                 NULL, NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_detpag_set_card(dp, NFE_INTEGRACAO_TEF,
	                                 "12345678000196", 1, NULL, NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_detpag_set_card(dp, NFE_INTEGRACAO_TEF, NULL, 1, "",
	                                 NULL, NULL),
	             E_TAMANHO);
	VERIFICA_INT(nfe_pag_set_vtroco(pag, "5.0"), E_VALOR);
	VERIFICA_INT(nfe_pag_add_detpag(pag, NULL), E_ISNULL);
	VERIFICA_INT(nfe_detpag_set_tpag(NULL, NFE_MEIO_DINHEIRO), E_ISNULL);

	/* Sem forma de pagamento, ou forma incompleta */
	xml = teste_gera(escreve, pag, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_pag_add_detpag(pag, dp), 0);
	xml = teste_gera(escreve, pag, &rc); /* sem tPag e vPag */
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_detpag_set_tpag(dp, NFE_MEIO_SEM_PAGAMENTO), 0);
	xml = teste_gera(escreve, pag, &rc); /* sem vPag */
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_detpag_set_vpag(dp, "0.00"), 0);
	xml = teste_gera(escreve, pag, &rc);
	VERIFICA_INT(rc, 0);
	if (xml) {
		VERIFICA(strstr(xml, "<tPag>90</tPag><vPag>0.00</vPag>") !=
		         NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Limite de 100 formas */
	for (i = 1; i < NFE_MAX_DETPAG; i++)
		rc |= nfe_pag_add_detpag(pag, forma(NFE_MEIO_DINHEIRO, "1.00"));
	VERIFICA_INT(rc, 0);
	dp = forma(NFE_MEIO_DINHEIRO, "1.00");
	VERIFICA_INT(nfe_pag_add_detpag(pag, dp), E_VALOR);
	nfe_detpag_free(dp);
	xml = teste_gera(escreve, pag, &rc);
	VERIFICA_INT(rc, 0);
	if (xml)
		VERIFICA_INT(teste_valida(xml), 0);
	free(xml);
	VERIFICA_INT(nfe_pag_write_xml(NULL, pag), E_ISNULL);
	nfe_pag_free(pag);
	nfe_pag_free(NULL);
	nfe_detpag_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_dinheiro_com_troco();
	teste_varias_formas();
	teste_valores_invalidos();

	teste_libera_schema();
	TESTE_FIM();
}
