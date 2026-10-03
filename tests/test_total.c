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

/* Testes dos totais da nota (total), com validação do XML contra o XSD
 * oficial.
 *
 * Uso: test_total <diretório tests> */

#include <libnfe/erros.h>
#include <libnfe/total.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *tot)
{
	return nfe_total_write_xml(writer, (const nfe_total *)tot);
}

static void teste_padrao(void)
{
	nfe_total *tot = nfe_total_new();
	char *xml;
	int rc;

	VERIFICA(tot != NULL);
	if (!tot)
		return;

	/* Recém-criado: todos os obrigatórios em 0.00, já válido */
	xml = teste_gera(escreve, tot, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<total xmlns=\"" TESTE_NS "\"><ICMSTot>"
		                "<vBC>0.00</vBC><vICMS>0.00</vICMS>"
		                "<vICMSDeson>0.00</vICMSDeson><vFCP>0.00</vFCP>"
		                "<vBCST>0.00</vBCST><vST>0.00</vST>"
		                "<vFCPST>0.00</vFCPST>"
		                "<vFCPSTRet>0.00</vFCPSTRet>"
		                "<vProd>0.00</vProd><vFrete>0.00</vFrete>"
		                "<vSeg>0.00</vSeg><vDesc>0.00</vDesc>"
		                "<vII>0.00</vII><vIPI>0.00</vIPI>"
		                "<vIPIDevol>0.00</vIPIDevol><vPIS>0.00</vPIS>"
		                "<vCOFINS>0.00</vCOFINS><vOutro>0.00</vOutro>"
		                "<vNF>0.00</vNF></ICMSTot></total>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_total_free(tot);
}

static void teste_valores(void)
{
	nfe_total *tot = nfe_total_new();
	char *xml;
	int rc;

	VERIFICA(tot != NULL);
	if (!tot)
		return;
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VBC, "100.00"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VICMS, "18.00"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VPROD, "100.00"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VPIS, "1.65"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VCOFINS, "7.60"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VNF, "100"), 0);
	/* Opcionais */
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VFCPUFDEST, "1.00"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VICMSUFDEST, "2.00"),
	             0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VICMSUFREMET, "0.00"),
	             0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_QBCMONO, "10.00"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VICMSMONO, "5.00"), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VTOTTRIB, "31.45"), 0);
	VERIFICA_INT(nfe_total_set_vnftot(tot, "100.00"), 0);
	xml = teste_gera(escreve, tot, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<vBC>100.00</vBC><vICMS>18.00</vICMS>"
		                     "<vICMSDeson>0.00</vICMSDeson>"
		                     "<vFCPUFDest>1.00</vFCPUFDest>"
		                     "<vICMSUFDest>2.00</vICMSUFDest>"
		                     "<vICMSUFRemet>0.00</vICMSUFRemet>"
		                     "<vFCP>0.00</vFCP>") != NULL);
		VERIFICA(strstr(xml, "<vFCPSTRet>0.00</vFCPSTRet>"
		                     "<qBCMono>10.00</qBCMono>"
		                     "<vICMSMono>5.00</vICMSMono>"
		                     "<vProd>100.00</vProd>") != NULL);
		VERIFICA(strstr(xml,
		                "<vPIS>1.65</vPIS><vCOFINS>7.60</vCOFINS>"
		                "<vOutro>0.00</vOutro><vNF>100</vNF>"
		                "<vTotTrib>31.45</vTotTrib></ICMSTot>"
		                "<vNFTot>100.00</vNFTot></total>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* NULL volta ao valor inicial */
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VBC, NULL), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VTOTTRIB, NULL), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_QBCMONO, NULL), 0);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VICMSMONO, NULL), 0);
	VERIFICA_INT(nfe_total_set_vnftot(tot, NULL), 0);
	xml = teste_gera(escreve, tot, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<vBC>0.00</vBC>") != NULL);
		VERIFICA(strstr(xml, "vTotTrib") == NULL);
		VERIFICA(strstr(xml, "Mono") == NULL);
		VERIFICA(strstr(xml, "vNFTot") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_total_free(tot);
}

/* Totais da Reforma Tributária pelo grupo genérico */
static void teste_ibscbs(void)
{
	nfe_total *tot = nfe_total_new();
	nfe_grupo *g = nfe_total_grupo(tot);
	static const char *const campos[] = {
		"ISTot/vIS",
		"1.00",
		"IBSCBSTot/vBCIBSCBS",
		"100.00",
		"gIBSUF/vDif",
		"0",
		"gIBSUF/vDevTrib",
		"0",
		"gIBSUF/vIBSUF",
		"0.10",
		"gIBSMun/vDif",
		"0",
		"gIBSMun/vDevTrib",
		"0",
		"gIBSMun/vIBSMun",
		"0",
		"gIBS/vIBS",
		"0.10",
		"gIBS/vCredPres",
		"0",
		"gIBS/vCredPresCondSus",
		"0",
		"gCBS/vDif",
		"0",
		"gCBS/vDevTrib",
		"0",
		"gCBS/vCBS",
		"0.90",
		"gCBS/vCredPres",
		"0",
		"gCBS/vCredPresCondSus",
		"0",
		NULL,
	};
	const char *const *c;
	char *xml;
	int rc = 0;

	for (c = campos; c[0]; c += 2)
		rc |= nfe_grupo_set(g, c[0], c[1]);
	rc |= nfe_total_set_vnftot(tot, "101.00");
	VERIFICA_INT(rc, 0);
	xml = teste_gera(escreve, tot, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "</ICMSTot><ISTot><vIS>1.00</vIS></ISTot>"
		                "<IBSCBSTot><vBCIBSCBS>100.00</vBCIBSCBS>") !=
		         NULL);
		VERIFICA(strstr(xml, "</IBSCBSTot><vNFTot>101.00</vNFTot>"
		                     "</total>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* IBSCBSTot incompleto */
	VERIFICA_INT(nfe_grupo_set(g, "gCBS/vCBS", NULL), 0);
	xml = teste_gera(escreve, tot, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	nfe_total_free(tot);
}

static void teste_valores_invalidos(void)
{
	nfe_total *tot = nfe_total_new();
	char *xml;
	int rc;

	VERIFICA(tot != NULL);
	if (!tot)
		return;
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VNF, "15,00"), E_VALOR);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VNF, "15.0"), E_VALOR);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_VNF, "-1.00"), E_VALOR);
	VERIFICA_INT(nfe_total_set_icmstot(tot, NFE_TOT_QUANTIDADE, "1.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_total_set_icmstot(tot, (nfe_campo_icmstot)-1, "1.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_total_set_vnftot(tot, "abc"), E_VALOR);
	VERIFICA_INT(nfe_total_set_icmstot(NULL, NFE_TOT_VNF, "1.00"),
	             E_ISNULL);
	VERIFICA_INT(nfe_total_set_vnftot(NULL, "1.00"), E_ISNULL);

	xml = teste_gera(escreve, tot, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<vNF>0.00</vNF>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	VERIFICA_INT(nfe_total_write_xml(NULL, tot), E_ISNULL);
	nfe_total_free(tot);
	nfe_total_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_padrao();
	teste_valores();
	teste_ibscbs();
	teste_valores_invalidos();

	teste_libera_schema();
	TESTE_FIM();
}
