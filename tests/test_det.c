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

/* Testes do item da nota (det), com validação do XML contra o XSD oficial.
 *
 * Uso: test_det <diretório tests> */

#include <libnfe/det.h>
#include <libnfe/erros.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *det)
{
	return nfe_det_write_xml(writer, (const nfe_det *)det);
}

static nfe_prod *produto(void)
{
	nfe_prod *prod = nfe_prod_new();
	int rc = 0;

	if (!prod)
		return NULL;
	rc |= nfe_prod_set_cprod(prod, "001");
	rc |= nfe_prod_set_xprod(prod, "CANETA AZUL");
	rc |= nfe_prod_set_ncm(prod, "96081000");
	rc |= nfe_prod_set_cfop(prod, 5102);
	rc |= nfe_prod_set_comercial(prod, "UN", "10", "1.50", "15.00");
	rc |= nfe_prod_set_tributavel(prod, "UN", "10", "1.50");
	VERIFICA_INT(rc, 0);
	return prod;
}

static nfe_imposto *imposto(void)
{
	nfe_imposto *imp = nfe_imposto_new();
	int rc = 0;

	if (!imp)
		return NULL;
	rc |= nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NACIONAL,
	                                NFE_CSOSN_102);
	rc |= nfe_imposto_set_pisnt(imp, NFE_CST_PC_SEM_INCIDENCIA);
	rc |= nfe_imposto_set_cofinsnt(imp, NFE_CST_PC_SEM_INCIDENCIA);
	VERIFICA_INT(rc, 0);
	return imp;
}

static void teste_item(void)
{
	nfe_det *det = nfe_det_new();
	char *xml;
	int rc;

	VERIFICA(det != NULL);
	if (!det)
		return;
	VERIFICA_INT(nfe_det_set_nitem(det, 1), 0);
	VERIFICA_INT(nfe_det_set_prod(det, produto()), 0);
	VERIFICA_INT(nfe_det_set_imposto(det, imposto()), 0);
	xml = teste_gera(escreve, det, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<det xmlns=\"" TESTE_NS "\" nItem=\"1\">"
		                     "<prod><cProd>001</cProd>") != NULL);
		VERIFICA(strstr(xml, "</prod><imposto><ICMS>") != NULL);
		VERIFICA(strstr(xml, "</COFINS></imposto></det>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Opcionais e o último número de item */
	VERIFICA_INT(nfe_det_set_nitem(det, NFE_MAX_ITENS), 0);
	VERIFICA_INT(nfe_det_set_infadprod(det, "Cor: azul; ponta fina"), 0);
	VERIFICA_INT(nfe_det_set_vitem(det, "15.00"), 0);
	xml = teste_gera(escreve, det, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "nItem=\"990\"") != NULL);
		VERIFICA(strstr(xml,
		                "</imposto>"
		                "<infAdProd>Cor: azul; ponta fina</infAdProd>"
		                "<vItem>15.00</vItem></det>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Removendo os opcionais */
	VERIFICA_INT(nfe_det_set_infadprod(det, NULL), 0);
	VERIFICA_INT(nfe_det_set_vitem(det, NULL), 0);
	xml = teste_gera(escreve, det, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "infAdProd") == NULL);
		VERIFICA(strstr(xml, "vItem") == NULL);
	}
	free(xml);
	nfe_det_free(det);
}

static void teste_valores_invalidos(void)
{
	nfe_det *det = nfe_det_new();
	nfe_prod *prod;
	char longo[502];
	char *xml;
	int rc;

	VERIFICA(det != NULL);
	if (!det)
		return;
	memset(longo, 'A', sizeof longo - 1);
	longo[sizeof longo - 1] = '\0';

	VERIFICA_INT(nfe_det_set_nitem(det, 0), E_VALOR);
	VERIFICA_INT(nfe_det_set_nitem(det, NFE_MAX_ITENS + 1), E_VALOR);
	VERIFICA_INT(nfe_det_set_prod(det, NULL), E_ISNULL);
	VERIFICA_INT(nfe_det_set_imposto(det, NULL), E_ISNULL);
	VERIFICA_INT(nfe_det_set_infadprod(det, ""), E_TAMANHO);
	VERIFICA_INT(nfe_det_set_infadprod(det, longo), E_TAMANHO);
	VERIFICA_INT(nfe_det_set_infadprod(det, "texto "), E_VALOR);
	VERIFICA_INT(nfe_det_set_vitem(det, "15.0"), E_VALOR);
	VERIFICA_INT(nfe_det_set_nitem(NULL, 1), E_ISNULL);

	/* Faltam nItem, prod e imposto */
	xml = teste_gera(escreve, det, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_det_set_nitem(det, 2), 0);
	VERIFICA_INT(nfe_det_set_imposto(det, imposto()), 0);
	xml = teste_gera(escreve, det, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);

	/* Produto incompleto: o erro do prod é repassado */
	prod = nfe_prod_new();
	VERIFICA_INT(nfe_det_set_prod(det, prod), 0);
	VERIFICA_INT(nfe_det_set_prod(det, prod), 0); /* o mesmo */
	xml = teste_gera(escreve, det, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);

	/* Trocar o produto libera o anterior */
	VERIFICA_INT(nfe_det_set_prod(det, produto()), 0);
	xml = teste_gera(escreve, det, &rc);
	VERIFICA_INT(rc, 0);
	if (xml) {
		VERIFICA(strstr(xml, "nItem=\"2\"") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	VERIFICA_INT(nfe_det_write_xml(NULL, det), E_ISNULL);
	nfe_det_free(det);
	nfe_det_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_item();
	teste_valores_invalidos();

	teste_libera_schema();
	TESTE_FIM();
}
