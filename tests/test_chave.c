/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 * */

/* Testes da chave de acesso e do dígito verificador */

#include <stdlib.h>

#include <libxml/xmlwriter.h>

#include <libnfe/chave.h>
#include <libnfe/erros.h>
#include <libnfe/ide.h>

#include "teste.h"

/* Exemplo do Manual de Orientação do Contribuinte: dígito verificador 5 */
#define MOC_43 "5206043300991100250655012000000780026730161"

#define T0 ((time_t)1282237215) /* 2010-08-19T17:00:15Z */

static nfe_ide *novo(time_t dhemi, nfe_tzd tzd)
{
	nfe_ide *ide = nfe_ide_new();
	int rc = 0;

	rc |= nfe_ide_set_cuf(ide, NFE_UF_SP);
	rc |= nfe_ide_set_cnf(ide, 12345678);
	rc |= nfe_ide_set_serie(ide, 1);
	rc |= nfe_ide_set_nnf(ide, 42);
	rc |= nfe_ide_set_dhemi(ide, dhemi);
	rc |= nfe_ide_set_tzd(ide, tzd);
	VERIFICA_INT(rc, 0);
	return ide;
}

static void teste_dv(void)
{
	VERIFICA_INT(nfe_chave_dv(MOC_43), 5);
	VERIFICA_INT(nfe_chave_validar(MOC_43 "5"), 0);
	VERIFICA_INT(nfe_chave_validar(MOC_43 "4"), E_VALOR);
	VERIFICA_INT(nfe_chave_validar(MOC_43), E_TAMANHO);
	VERIFICA_INT(nfe_chave_validar(MOC_43 "55"), E_TAMANHO);
	VERIFICA_INT(nfe_chave_dv("520604330099110025065501200000078002673016"),
	             E_TAMANHO);
	VERIFICA_INT(
	        nfe_chave_dv("52060433009911002506550120000007800267301X1"),
	        E_VALOR);
	VERIFICA_INT(nfe_chave_dv(NULL), E_ISNULL);
	VERIFICA_INT(nfe_chave_validar(NULL), E_ISNULL);
}

static void teste_gerar(void)
{
	nfe_ide *ide = novo(T0, NFE_TZD_BRASILIA);
	char chave[45];

	/* CNPJ: 35 1008 12345678000199 55 001 000000042 1 12345678 + DV 9 */
	VERIFICA_INT(
	        nfe_ide_gerar_chave(ide, "12345678000199", chave, sizeof chave),
	        0);
	VERIFICA_STR(chave, "35100812345678000199550010000000421123456789");
	VERIFICA_INT(nfe_chave_validar(chave), 0);

	/* CPF: completado com zeros à esquerda */
	VERIFICA_INT(
	        nfe_ide_gerar_chave(ide, "12345678909", chave, sizeof chave),
	        0);
	VERIFICA(strncmp(chave + 6, "00012345678909", 14) == 0);
	VERIFICA_INT(nfe_chave_validar(chave), 0);

	/* Erros */
	VERIFICA_INT(nfe_ide_gerar_chave(ide, "123", chave, sizeof chave),
	             E_TAMANHO);
	VERIFICA_INT(
	        nfe_ide_gerar_chave(ide, "1234567800019A", chave, sizeof chave),
	        E_VALOR);
	VERIFICA_INT(nfe_ide_gerar_chave(ide, "12345678000199", chave, 44),
	             E_TAMANHO);
	VERIFICA_INT(nfe_ide_gerar_chave(NULL, "12345678000199", chave,
	                                 sizeof chave),
	             E_ISNULL);
	nfe_ide_free(ide);

	/* Sem os campos necessários */
	ide = nfe_ide_new();
	VERIFICA_INT(
	        nfe_ide_gerar_chave(ide, "12345678000199", chave, sizeof chave),
	        E_VALOR);
	nfe_ide_free(ide);
}

/* AAMM usa o mês no fuso da nota: 2011-01-01T02:00:00Z ainda é dezembro
 * de 2010 em Brasília, mas já é janeiro de 2011 em Fernando de Noronha */
static void teste_virada_de_ano(void)
{
	const time_t t = (time_t)1293847200; /* 2011-01-01T02:00:00Z */
	nfe_ide *ide = novo(t, NFE_TZD_BRASILIA);
	char chave[45];

	VERIFICA_INT(
	        nfe_ide_gerar_chave(ide, "12345678000199", chave, sizeof chave),
	        0);
	VERIFICA(strncmp(chave + 2, "1012", 4) == 0);
	nfe_ide_free(ide);

	ide = novo(t, NFE_TZD_FERNANDO_NORONHA);
	VERIFICA_INT(
	        nfe_ide_gerar_chave(ide, "12345678000199", chave, sizeof chave),
	        0);
	VERIFICA(strncmp(chave + 2, "1101", 4) == 0);
	nfe_ide_free(ide);
}

/* O cDV calculado vai para o XML */
static void teste_cdv_no_xml(void)
{
	nfe_ide *ide = novo(T0, NFE_TZD_BRASILIA);
	xmlBufferPtr buf = xmlBufferCreate();
	xmlTextWriterPtr w = xmlNewTextWriterMemory(buf, 0);
	char chave[45];
	int rc = 0;

	rc |= nfe_ide_set_natop(ide, "VENDA");
	rc |= nfe_ide_set_cmunfg(ide, 3550308);
	rc |= nfe_ide_set_verproc(ide, "teste");
	rc |= nfe_ide_gerar_chave(ide, "12345678000199", chave, sizeof chave);
	VERIFICA_INT(rc, 0);
	VERIFICA_INT(nfe_ide_write_xml(w, ide), 0);
	xmlTextWriterEndDocument(w);
	VERIFICA(strstr((const char *)xmlBufferContent(buf), "<cDV>9</cDV>") !=
	         NULL);

	xmlFreeTextWriter(w);
	xmlBufferFree(buf);
	nfe_ide_free(ide);
}

int main(void)
{
	teste_dv();
	teste_gerar();
	teste_virada_de_ano();
	teste_cdv_no_xml();
	TESTE_FIM();
}
