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

/* Testes do produto (det/prod), com validação do XML contra o XSD oficial.
 *
 * Uso: test_prod <diretório tests> */

#include <libnfe/erros.h>
#include <libnfe/prod.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *prod)
{
	return nfe_prod_write_xml(writer, (const nfe_prod *)prod);
}

/* Produto com os campos obrigatórios */
static nfe_prod *novo(void)
{
	nfe_prod *prod = nfe_prod_new();
	int rc = 0;

	if (!prod)
		return NULL;
	rc |= nfe_prod_set_cprod(prod, "001");
	rc |= nfe_prod_set_xprod(prod, "CANETA ESFEROGRÁFICA AZUL");
	rc |= nfe_prod_set_ncm(prod, "96081000");
	rc |= nfe_prod_set_cfop(prod, 5102);
	rc |= nfe_prod_set_comercial(prod, "UN", "10", "1.50", "15.00");
	rc |= nfe_prod_set_tributavel(prod, "UN", "10", "1.50");
	VERIFICA_INT(rc, 0);
	return prod;
}

static void teste_minimo(void)
{
	nfe_prod *prod = novo();
	char *xml;
	int rc;

	VERIFICA(prod != NULL);
	if (!prod)
		return;
	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<prod xmlns=\"" TESTE_NS
		                "\"><cProd>001</cProd>"
		                "<cEAN>SEM GTIN</cEAN>"
		                "<xProd>CANETA ESFEROGRÁFICA AZUL</xProd>"
		                "<NCM>96081000</NCM><CFOP>5102</CFOP>"
		                "<uCom>UN</uCom><qCom>10</qCom>"
		                "<vUnCom>1.50</vUnCom><vProd>15.00</vProd>"
		                "<cEANTrib>SEM GTIN</cEANTrib><uTrib>UN</uTrib>"
		                "<qTrib>10</qTrib><vUnTrib>1.50</vUnTrib>"
		                "<indTot>1</indTot></prod>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_prod_free(prod);
}

static void teste_completo(void)
{
	nfe_prod *prod = novo();
	char *xml;
	int rc;

	VERIFICA(prod != NULL);
	if (!prod)
		return;
	VERIFICA_INT(nfe_prod_set_cean(prod, "7891234567895"), 0);
	VERIFICA_INT(nfe_prod_set_cbarra(prod, "ABC-123"), 0);
	VERIFICA_INT(nfe_prod_add_nve(prod, "AA0001"), 0);
	VERIFICA_INT(nfe_prod_add_nve(prod, "BB0002"), 0);
	VERIFICA_INT(nfe_prod_set_cest(prod, "1900100"), 0);
	VERIFICA_INT(nfe_prod_set_indescala(prod, NFE_ESCALA_NAO_RELEVANTE), 0);
	VERIFICA_INT(nfe_prod_set_cnpjfab(prod, "12345678000195"), 0);
	VERIFICA_INT(nfe_prod_set_cbenef(prod, "SP123456"), 0);
	VERIFICA_INT(nfe_prod_set_tpcredpresibszfm(
	                     prod, NFE_CRED_PRES_ZFM_CONSUMO_FINAL),
	             0);
	VERIFICA_INT(nfe_prod_set_extipi(prod, "01"), 0);
	VERIFICA_INT(nfe_prod_set_ceantrib(prod, "17891234567892"), 0);
	VERIFICA_INT(nfe_prod_set_cbarratrib(prod, "CX-12"), 0);
	VERIFICA_INT(nfe_prod_set_tributavel(prod, "CX", "1", "15"), 0);
	VERIFICA_INT(nfe_prod_set_vfrete(prod, "2.00"), 0);
	VERIFICA_INT(nfe_prod_set_vseg(prod, "0.50"), 0);
	VERIFICA_INT(nfe_prod_set_vdesc(prod, "1.00"), 0);
	VERIFICA_INT(nfe_prod_set_voutro(prod, "0.25"), 0);
	VERIFICA_INT(nfe_prod_set_indtot(prod, 0), 0);
	VERIFICA_INT(nfe_prod_set_indbemmovelusado(prod, 1), 0);
	VERIFICA_INT(nfe_prod_set_xped(prod, "PED-2026-001"), 0);
	VERIFICA_INT(nfe_prod_set_nitemped(prod, "10"), 0);
	VERIFICA_INT(
	        nfe_prod_set_nfci(prod, "B01F70AF-10BF-4B1F-848C-65FF57F616FE"),
	        0);
	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<cEAN>7891234567895</cEAN>"
		                "<cBarra>ABC-123</cBarra><xProd>") != NULL);
		VERIFICA(strstr(xml, "<NCM>96081000</NCM><NVE>AA0001</NVE>"
		                     "<NVE>BB0002</NVE><CEST>1900100</CEST>"
		                     "<indEscala>N</indEscala>"
		                     "<CNPJFab>12345678000195</CNPJFab>"
		                     "<cBenef>SP123456</cBenef>"
		                     "<tpCredPresIBSZFM>1</tpCredPresIBSZFM>"
		                     "<EXTIPI>01</EXTIPI><CFOP>5102</CFOP>") !=
		         NULL);
		VERIFICA(strstr(xml,
		                "<cEANTrib>17891234567892</cEANTrib>"
		                "<cBarraTrib>CX-12</cBarraTrib>"
		                "<uTrib>CX</uTrib><qTrib>1</qTrib>"
		                "<vUnTrib>15</vUnTrib><vFrete>2.00</vFrete>"
		                "<vSeg>0.50</vSeg><vDesc>1.00</vDesc>"
		                "<vOutro>0.25</vOutro><indTot>0</indTot>"
		                "<indBemMovelUsado>1</indBemMovelUsado>"
		                "<xPed>PED-2026-001</xPed>"
		                "<nItemPed>10</nItemPed><nFCI>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Removendo os opcionais */
	VERIFICA_INT(nfe_prod_remove_nve(prod), 0);
	VERIFICA_INT(nfe_prod_set_cest(prod, NULL), 0);
	xml = teste_gera(escreve, prod, &rc); /* indEscala sem CEST */
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_prod_set_indescala(prod, NFE_ESCALA_NAO_INFORMADA), 0);
	VERIFICA_INT(nfe_prod_set_cnpjfab(prod, NULL), 0);
	VERIFICA_INT(nfe_prod_set_cbarra(prod, NULL), 0);
	VERIFICA_INT(nfe_prod_set_cbenef(prod, NULL), 0);
	VERIFICA_INT(nfe_prod_set_tpcredpresibszfm(
	                     prod, NFE_CRED_PRES_ZFM_NAO_INFORMADO),
	             0);
	VERIFICA_INT(nfe_prod_set_vfrete(prod, NULL), 0);
	VERIFICA_INT(nfe_prod_set_indbemmovelusado(prod, 0), 0);
	VERIFICA_INT(nfe_prod_set_nfci(prod, NULL), 0);
	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "NVE") == NULL);
		VERIFICA(strstr(xml, "CEST") == NULL);
		VERIFICA(strstr(xml, "cBenef") == NULL);
		VERIFICA(strstr(xml, "<cBarra>") == NULL);
		VERIFICA(strstr(xml, "tpCredPresIBSZFM") == NULL);
		VERIFICA(strstr(xml, "vFrete") == NULL);
		VERIFICA(strstr(xml, "indBemMovelUsado") == NULL);
		VERIFICA(strstr(xml, "nFCI") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_prod_free(prod);
}

static void teste_valores_invalidos(void)
{
	nfe_prod *prod = novo();
	char *xml;
	int i, rc = 0;

	VERIFICA(prod != NULL);
	if (!prod)
		return;

	VERIFICA_INT(nfe_prod_set_cprod(prod, ""), E_TAMANHO);
	VERIFICA_INT(nfe_prod_set_cean(prod, "123456789"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cean(prod, "SEM GTIM"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cbarra(prod, "AB"), E_TAMANHO);
	VERIFICA_INT(nfe_prod_set_xprod(prod, " CANETA"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_ncm(prod, "9608100"), E_VALOR);
	VERIFICA_INT(nfe_prod_add_nve(prod, "aa0001"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cest(prod, "190010"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_indescala(prod, (nfe_escala)3), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cnpjfab(prod, "12345678000196"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cbenef(prod, "SP1234"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_tpcredpresibszfm(prod, (nfe_cred_pres_zfm)5),
	             E_VALOR);
	VERIFICA_INT(nfe_prod_set_extipi(prod, "1"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cfop(prod, 4102), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cfop(prod, 8102), E_VALOR);
	VERIFICA_INT(nfe_prod_set_cfop(prod, 510), E_VALOR);
	/* Decimais: vírgula, casas a mais, zero à esquerda */
	VERIFICA_INT(nfe_prod_set_comercial(prod, "UN", "10", "1,50", "15.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_prod_set_comercial(prod, "UN", "1.12345", "1", "1.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_prod_set_comercial(prod, "UN", "10", "1.5", "15.0"),
	             E_VALOR);
	VERIFICA_INT(nfe_prod_set_comercial(prod, "UN", "010", "1.5", "15.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_prod_set_comercial(prod, "UNIDADE", "1", "1", "1.00"),
	             E_TAMANHO);
	VERIFICA_INT(nfe_prod_set_comercial(prod, "UN", NULL, "1", "1.00"),
	             E_ISNULL);
	VERIFICA_INT(nfe_prod_set_tributavel(prod, "UN", "1", "1.12345678901"),
	             E_VALOR);
	VERIFICA_INT(nfe_prod_set_vdesc(prod, "0.00"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_vdesc(prod, "0"), E_VALOR);
	VERIFICA_INT(nfe_prod_set_indtot(prod, 2), E_VALOR);
	VERIFICA_INT(nfe_prod_set_indbemmovelusado(prod, 2), E_VALOR);
	VERIFICA_INT(nfe_prod_set_xped(prod, "1234567890123456"), E_TAMANHO);
	VERIFICA_INT(nfe_prod_set_nitemped(prod, "1234567"), E_VALOR);
	VERIFICA_INT(
	        nfe_prod_set_nfci(prod, "b01f70af-10bf-4b1f-848c-65ff57f616fe"),
	        E_VALOR);
	VERIFICA_INT(nfe_prod_set_cprod(NULL, "001"), E_ISNULL);

	/* Limite de NVE */
	for (i = 0; i < NFE_MAX_NVE; i++)
		rc |= nfe_prod_add_nve(prod, "AA0001");
	VERIFICA_INT(rc, 0);
	VERIFICA_INT(nfe_prod_add_nve(prod, "AA0001"), E_VALOR);
	VERIFICA_INT(nfe_prod_remove_nve(prod), 0);

	/* Nada mudou */
	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<cEAN>SEM GTIN</cEAN>") != NULL);
		VERIFICA(strstr(xml, "<uCom>UN</uCom><qCom>10</qCom>"
		                     "<vUnCom>1.50</vUnCom>"
		                     "<vProd>15.00</vProd>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_prod_free(prod);
}

/* Sem os campos obrigatórios, o XML não é gerado */
static void teste_obrigatorios(void)
{
	nfe_prod *prod = nfe_prod_new();
	char *xml;
	int rc;

	VERIFICA(prod != NULL);
	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_prod_set_cprod(prod, "001"), 0);
	VERIFICA_INT(nfe_prod_set_xprod(prod, "CANETA"), 0);
	VERIFICA_INT(nfe_prod_set_ncm(prod, "96081000"), 0);
	VERIFICA_INT(nfe_prod_set_cfop(prod, 5102), 0);
	VERIFICA_INT(nfe_prod_set_comercial(prod, "UN", "1", "1", "1.00"), 0);
	xml = teste_gera(escreve, prod, &rc); /* falta a unidade tributável */
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_prod_write_xml(NULL, prod), E_ISNULL);
	nfe_prod_free(prod);
	nfe_prod_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_minimo();
	teste_completo();
	teste_valores_invalidos();
	teste_obrigatorios();

	teste_libera_schema();
	TESTE_FIM();
}
