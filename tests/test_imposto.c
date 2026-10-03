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

/* Testes dos tributos do item (det/imposto), com validação do XML contra o
 * XSD oficial.
 *
 * Uso: test_imposto <diretório tests> */

#include <libnfe/erros.h>
#include <libnfe/imposto.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *imp)
{
	return nfe_imposto_write_xml(writer, (const nfe_imposto *)imp);
}

static void teste_regime_normal(void)
{
	nfe_imposto *imp = nfe_imposto_new();
	char *xml;
	int rc;

	VERIFICA(imp != NULL);
	if (!imp)
		return;
	VERIFICA_INT(nfe_imposto_set_vtottrib(imp, "31.45"), 0);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", NULL, NULL),
	             0);
	VERIFICA_INT(nfe_imposto_set_pisaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA,
	                                     "100.00", "1.65", "1.65"),
	             0);
	VERIFICA_INT(nfe_imposto_set_cofinsaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA,
	                                        "100.00", "7.60", "7.60"),
	             0);
	xml = teste_gera(escreve, imp, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<imposto xmlns=\"" TESTE_NS "\">"
		                "<vTotTrib>31.45</vTotTrib>"
		                "<ICMS><ICMS00><orig>0</orig><CST>00</CST>"
		                "<modBC>3</modBC><vBC>100.00</vBC>"
		                "<pICMS>18.00</pICMS><vICMS>18.00</vICMS>"
		                "</ICMS00></ICMS>"
		                "<PIS><PISAliq><CST>01</CST><vBC>100.00</vBC>"
		                "<pPIS>1.65</pPIS><vPIS>1.65</vPIS></PISAliq>"
		                "</PIS>"
		                "<COFINS><COFINSAliq><CST>01</CST>"
		                "<vBC>100.00</vBC><pCOFINS>7.60</pCOFINS>"
		                "<vCOFINS>7.60</vCOFINS></COFINSAliq></COFINS>"
		                "</imposto>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Com FCP e alíquota diferenciada */
	VERIFICA_INT(nfe_imposto_set_icms00(imp,
	                                    NFE_ORIGEM_ESTRANGEIRA_IMPORTACAO,
	                                    NFE_MOD_BC_PAUTA, "100.00",
	                                    "20.0000", "20.00", "2.00", "2.00"),
	             0);
	VERIFICA_INT(nfe_imposto_set_pisaliq(imp,
	                                     NFE_CST_PC_ALIQUOTA_DIFERENCIADA,
	                                     "100.00", "2.10", "2.10"),
	             0);
	xml = teste_gera(escreve, imp, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<orig>1</orig><CST>00</CST><modBC>1</modBC>"
		                "<vBC>100.00</vBC><pICMS>20.0000</pICMS>"
		                "<vICMS>20.00</vICMS><pFCP>2.00</pFCP>"
		                "<vFCP>2.00</vFCP></ICMS00>") != NULL);
		VERIFICA(strstr(xml, "<PISAliq><CST>02</CST>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_imposto_free(imp);
}

static void teste_simples_nacional(void)
{
	nfe_imposto *imp = nfe_imposto_new();
	char *xml;
	int rc;

	VERIFICA(imp != NULL);
	if (!imp)
		return;
	VERIFICA_INT(nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NACIONAL,
	                                       NFE_CSOSN_102),
	             0);
	VERIFICA_INT(nfe_imposto_set_pisnt(imp, NFE_CST_PC_SEM_INCIDENCIA), 0);
	VERIFICA_INT(nfe_imposto_set_cofinsnt(imp, NFE_CST_PC_ISENTA), 0);
	xml = teste_gera(escreve, imp, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<ICMS><ICMSSN102><orig>0</orig>"
		                "<CSOSN>102</CSOSN></ICMSSN102></ICMS>"
		                "<PIS><PISNT><CST>08</CST></PISNT></PIS>"
		                "<COFINS><COFINSNT><CST>07</CST></COFINSNT>"
		                "</COFINS></imposto>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* orig é opcional no ICMSSN102 */
	VERIFICA_INT(nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NAO_INFORMADA,
	                                       NFE_CSOSN_400),
	             0);
	xml = teste_gera(escreve, imp, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<ICMSSN102><CSOSN>400</CSOSN>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Removendo os grupos */
	VERIFICA_INT(nfe_imposto_remove_icms(imp), 0);
	VERIFICA_INT(nfe_imposto_remove_pis(imp), 0);
	VERIFICA_INT(nfe_imposto_remove_cofins(imp), 0);
	xml = teste_gera(escreve, imp, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "ICMS") == NULL);
		VERIFICA(strstr(xml, "PIS") == NULL);
		VERIFICA(strstr(xml, "COFINS") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_imposto_free(imp);
}

static void teste_valores_invalidos(void)
{
	nfe_imposto *imp = nfe_imposto_new();
	char *xml;
	int rc;

	VERIFICA(imp != NULL);
	if (!imp)
		return;
	VERIFICA_INT(nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NACIONAL,
	                                       NFE_CSOSN_103),
	             0);
	VERIFICA_INT(nfe_imposto_set_pisnt(imp, NFE_CST_PC_ALIQUOTA_ZERO), 0);

	/* ICMS00 */
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NAO_INFORMADA,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, (nfe_origem)9,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    (nfe_mod_bc)4, "100.00", "18.00",
	                                    "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.0",
	                                    "18.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.0", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "1000.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", "2.00", NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", "0", "0.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, NULL,
	                                    "18.00", "18.00", NULL, NULL),
	             E_ISNULL);

	/* ICMSSN102 */
	VERIFICA_INT(nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NACIONAL,
	                                       (nfe_csosn_102)101),
	             E_VALOR);
	VERIFICA_INT(
	        nfe_imposto_set_icmssn102(imp, (nfe_origem)-2, NFE_CSOSN_102),
	        E_VALOR);

	/* PIS / COFINS */
	VERIFICA_INT(nfe_imposto_set_pisaliq(imp, NFE_CST_PC_ISENTA, "1.00",
	                                     "1.65", "0.02"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_pisaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA,
	                                     "1.00", "1.6", "0.02"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_cofinsaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA,
	                                        "1,00", "7.60", "0.08"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_pisnt(imp, NFE_CST_PC_ALIQUOTA_BASICA),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_cofinsnt(imp, (nfe_cst_pis_cofins)9),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_vtottrib(imp, "1.5"), E_VALOR);
	VERIFICA_INT(nfe_imposto_set_vtottrib(NULL, "1.50"), E_ISNULL);

	/* Nada mudou */
	xml = teste_gera(escreve, imp, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<CSOSN>103</CSOSN>") != NULL);
		VERIFICA(strstr(xml, "<PISNT><CST>06</CST></PISNT>") != NULL);
		VERIFICA(strstr(xml, "vTotTrib") == NULL);
		VERIFICA(strstr(xml, "COFINS") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	VERIFICA_INT(nfe_imposto_write_xml(NULL, imp), E_ISNULL);
	nfe_imposto_free(imp);
	nfe_imposto_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_regime_normal();
	teste_simples_nacional();
	teste_valores_invalidos();

	teste_libera_schema();
	TESTE_FIM();
}
