/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
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
 * */

/* Testes das funções de validação (valida.h) e dos padrões gerados do XSD */

#include <libnfe/erros.h>
#include <libnfe/valida.h>

#include "teste.h"

#define OK(v, p)  VERIFICA_INT(nfe_valida_padrao(v, p), 0)
#define NAO(v, p) VERIFICA_INT(nfe_valida_padrao(v, p), E_VALOR)

/* Todo padrão gerado de padroes.h precisa ser aceito pela libxml2 */
static void teste_padroes_compilam(void)
{
#define COMPILA(tipo)                                                          \
	VERIFICA(nfe_valida_padrao("", NFE_PADRAO_##tipo) != E_MALLOC);
	NFE_TIPOS_COM_PADRAO(COMPILA)
#undef COMPILA
}

static void teste_decimais(void)
{
	OK("0", NFE_PADRAO_TDec_1302);
	OK("0.00", NFE_PADRAO_TDec_1302);
	OK("10.50", NFE_PADRAO_TDec_1302);
	OK("1234567890123.45", NFE_PADRAO_TDec_1302);
	NAO("1.5", NFE_PADRAO_TDec_1302);
	NAO("01", NFE_PADRAO_TDec_1302);
	NAO("12345678901234", NFE_PADRAO_TDec_1302);
	NAO("-1", NFE_PADRAO_TDec_1302);
	NAO("1,50", NFE_PADRAO_TDec_1302);
	NAO("", NFE_PADRAO_TDec_1302);

	/* Opc: zero não é aceito */
	NAO("0", NFE_PADRAO_TDec_1302Opc);
	NAO("0.00", NFE_PADRAO_TDec_1302Opc);
	OK("0.01", NFE_PADRAO_TDec_1302Opc);

	OK("1.5", NFE_PADRAO_TDec_1104v);
	OK("1.1234", NFE_PADRAO_TDec_1104v);
	NAO("1.12345", NFE_PADRAO_TDec_1104v);
}

static void teste_ancorado(void)
{
	/* O padrão casa com o valor inteiro, não com um trecho */
	OK("4106902", NFE_PADRAO_TCodMunIBGE);
	NAO("41069021", NFE_PADRAO_TCodMunIBGE);
	NAO("x4106902", NFE_PADRAO_TCodMunIBGE);
}

static void teste_datas(void)
{
	OK("2024-02-29", NFE_PADRAO_TData);
	NAO("2023-02-29", NFE_PADRAO_TData);
	NAO("2026-13-01", NFE_PADRAO_TData);
	OK("2026-10-03T10:00:00-03:00", NFE_PADRAO_TDateTimeUTC);
	NAO("2026-10-03 10:00:00", NFE_PADRAO_TDateTimeUTC);
	OK("2026-10", NFE_PADRAO_TCompetApur);
	NAO("2026-13", NFE_PADRAO_TCompetApur);
}

static void teste_documentos(void)
{
	OK("12ABC34501DE35", NFE_PADRAO_TCnpj);
	NAO("12abc34501de35", NFE_PADRAO_TCnpj);
	OK("", NFE_PADRAO_TCnpjOpc);
	OK("35260912ABC34501DE3555001000000123112345678"
	   "4",
	   NFE_PADRAO_TChNFe);
}

static void teste_texto(void)
{
	VERIFICA_INT(nfe_valida_texto("Venda", 1, 60), 0);
	VERIFICA_INT(nfe_valida_texto("Ação à vista", 1, 60), 0);
	VERIFICA_INT(nfe_valida_texto("a", 1, 60), 0);
	VERIFICA_INT(nfe_valida_texto(" Venda", 1, 60), E_VALOR);
	VERIFICA_INT(nfe_valida_texto("Venda ", 1, 60), E_VALOR);
	VERIFICA_INT(nfe_valida_texto("a\tb", 1, 60), E_VALOR);
	/* U+20AC (euro) está fora da faixa U+0020..U+00FF */
	VERIFICA_INT(nfe_valida_texto("10 \xE2\x82\xAC", 1, 60), E_VALOR);
	/* UTF-8 inválido */
	VERIFICA_INT(nfe_valida_texto("a\xC3", 1, 60), E_VALOR);
	VERIFICA_INT(nfe_valida_texto("", 1, 60), E_TAMANHO);
	VERIFICA_INT(nfe_valida_texto("ab", 3, 60), E_TAMANHO);
	/* Tamanho em caracteres, não em bytes: "ação" tem 4 */
	VERIFICA_INT(nfe_valida_texto("ação", 1, 4), 0);
	VERIFICA_INT(nfe_valida_texto("ação", 1, 3), E_TAMANHO);
	VERIFICA_INT(nfe_valida_texto(NULL, 1, 60), E_ISNULL);
}

static void teste_lista(void)
{
	static const char *const uf[] = { NFE_VALORES_TUf, NULL };
	static const char *const uf_emi[] = { NFE_VALORES_TUfEmi, NULL };

	VERIFICA_INT(nfe_valida_lista("SP", uf), 0);
	VERIFICA_INT(nfe_valida_lista("EX", uf), 0);
	VERIFICA_INT(nfe_valida_lista("EX", uf_emi), E_VALOR);
	VERIFICA_INT(nfe_valida_lista("sp", uf), E_VALOR);
	VERIFICA_INT(nfe_valida_lista(NULL, uf), E_ISNULL);
	VERIFICA_INT(nfe_valida_lista("SP", NULL), E_ISNULL);
}

static void teste_copia(void)
{
	char buf[8] = "antigo";

	VERIFICA_INT(
	        nfe_copia_padrao(buf, sizeof buf, "1.5", NFE_PADRAO_TDec_1302),
	        E_VALOR);
	VERIFICA_STR(buf, "antigo");
	VERIFICA_INT(
	        nfe_copia_padrao(buf, sizeof buf, "1.50", NFE_PADRAO_TDec_1302),
	        0);
	VERIFICA_STR(buf, "1.50");
	VERIFICA_INT(nfe_copia_padrao(buf, sizeof buf, "12345678.00",
	                              NFE_PADRAO_TDec_1302),
	             E_TAMANHO);
	VERIFICA_STR(buf, "1.50");
	VERIFICA_INT(nfe_copia_padrao(NULL, 8, "1", NFE_PADRAO_TDec_1302),
	             E_ISNULL);
	VERIFICA_INT(nfe_valida_padrao("1", NULL), E_ISNULL);

	VERIFICA_INT(nfe_copia_texto_validado(buf, sizeof buf, " x", 1, 7),
	             E_VALOR);
	VERIFICA_STR(buf, "1.50");
	VERIFICA_INT(nfe_copia_texto_validado(buf, sizeof buf, "Brasil", 1, 7),
	             0);
	VERIFICA_STR(buf, "Brasil");
}

int main(void)
{
	teste_padroes_compilam();
	teste_decimais();
	teste_ancorado();
	teste_datas();
	teste_documentos();
	teste_texto();
	teste_lista();
	teste_copia();
	TESTE_FIM();
}
