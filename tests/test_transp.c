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

/* Testes do transporte (transp), com validação do XML contra o XSD
 * oficial.
 *
 * Uso: test_transp <diretório tests> */

#include <libnfe/erros.h>
#include <libnfe/transp.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *tr)
{
	return nfe_transp_write_xml(writer, (const nfe_transp *)tr);
}

static void teste_sem_transporte(void)
{
	nfe_transp *tr = nfe_transp_new();
	char *xml;
	int rc;

	VERIFICA(tr != NULL);
	if (!tr)
		return;
	xml = teste_gera(escreve, tr, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<transp xmlns=\"" TESTE_NS "\">"
		                "<modFrete>9</modFrete></transp>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_transp_free(tr);
}

static void teste_transportador_e_volumes(void)
{
	nfe_transp *tr = nfe_transp_new();
	char *xml;
	int rc;

	VERIFICA(tr != NULL);
	if (!tr)
		return;
	VERIFICA_INT(nfe_transp_set_modfrete(tr, NFE_FRETE_DESTINATARIO), 0);
	VERIFICA_INT(nfe_transp_set_transporta_cnpj(tr, "12345678000195"), 0);
	VERIFICA_INT(nfe_transp_set_transporta_xnome(tr, "TRANSPORTES LTDA"),
	             0);
	VERIFICA_INT(nfe_transp_set_transporta_ie(tr, "ISENTO"), 0);
	VERIFICA_INT(nfe_transp_set_transporta_xender(tr, "RUA A, 1, CENTRO"),
	             0);
	VERIFICA_INT(nfe_transp_set_transporta_xmun(tr, "CAMPINAS"), 0);
	VERIFICA_INT(nfe_transp_set_transporta_uf(tr, "SP"), 0);
	VERIFICA_INT(nfe_transp_add_vol(tr, "2", "CAIXA", "ACME", "1-2",
	                                "10.500", "11.000"),
	             0);
	VERIFICA_INT(
	        nfe_transp_add_vol(tr, NULL, "PALETE", NULL, NULL, NULL, "250"),
	        0);
	xml = teste_gera(escreve, tr, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<modFrete>1</modFrete><transporta>"
		                     "<CNPJ>12345678000195</CNPJ>"
		                     "<xNome>TRANSPORTES LTDA</xNome>"
		                     "<IE>ISENTO</IE>"
		                     "<xEnder>RUA A, 1, CENTRO</xEnder>"
		                     "<xMun>CAMPINAS</xMun><UF>SP</UF>"
		                     "</transporta>") != NULL);
		VERIFICA(strstr(xml,
		                "<vol><qVol>2</qVol><esp>CAIXA</esp>"
		                "<marca>ACME</marca><nVol>1-2</nVol>"
		                "<pesoL>10.500</pesoL><pesoB>11.000</pesoB>"
		                "</vol><vol><esp>PALETE</esp>"
		                "<pesoB>250</pesoB></vol></transp>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* CPF no lugar do CNPJ; só parte dos campos */
	VERIFICA_INT(nfe_transp_set_transporta_cpf(tr, "12345678909"), 0);
	VERIFICA_INT(nfe_transp_set_transporta_ie(tr, NULL), 0);
	VERIFICA_INT(nfe_transp_set_transporta_xender(tr, NULL), 0);
	VERIFICA_INT(nfe_transp_remove_vol(tr), 0);
	xml = teste_gera(escreve, tr, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<transporta><CPF>12345678909</CPF>"
		                     "<xNome>") != NULL);
		VERIFICA(strstr(xml, "<IE>") == NULL);
		VERIFICA(strstr(xml, "xEnder") == NULL);
		VERIFICA(strstr(xml, "<vol>") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Sem nenhum campo do transportador, o grupo some */
	VERIFICA_INT(nfe_transp_set_transporta_cpf(tr, NULL), 0);
	VERIFICA_INT(nfe_transp_set_transporta_xnome(tr, NULL), 0);
	VERIFICA_INT(nfe_transp_set_transporta_xmun(tr, NULL), 0);
	VERIFICA_INT(nfe_transp_set_transporta_uf(tr, NULL), 0);
	xml = teste_gera(escreve, tr, &rc);
	VERIFICA(xml != NULL);
	if (xml)
		VERIFICA(strstr(xml, "transporta") == NULL);
	free(xml);
	nfe_transp_free(tr);
}

static void teste_valores_invalidos(void)
{
	nfe_transp *tr = nfe_transp_new();
	char *xml;
	int i, rc = 0;

	VERIFICA(tr != NULL);
	if (!tr)
		return;
	VERIFICA_INT(nfe_transp_set_modfrete(tr, (nfe_mod_frete)5), E_VALOR);
	VERIFICA_INT(nfe_transp_set_modfrete(tr, (nfe_mod_frete)-1), E_VALOR);
	VERIFICA_INT(nfe_transp_set_transporta_cnpj(tr, "12345678000196"),
	             E_VALOR);
	VERIFICA_INT(nfe_transp_set_transporta_cpf(tr, "123"), E_TAMANHO);
	VERIFICA_INT(nfe_transp_set_transporta_xnome(tr, "T"), E_TAMANHO);
	VERIFICA_INT(nfe_transp_set_transporta_ie(tr, "1"), E_VALOR);
	VERIFICA_INT(nfe_transp_set_transporta_xender(tr, ""), E_TAMANHO);
	VERIFICA_INT(nfe_transp_set_transporta_uf(tr, "XX"), E_VALOR);
	VERIFICA_INT(
	        nfe_transp_add_vol(tr, "1.5", NULL, NULL, NULL, NULL, NULL),
	        E_VALOR);
	VERIFICA_INT(nfe_transp_add_vol(tr, NULL, "", NULL, NULL, NULL, NULL),
	             E_TAMANHO);
	VERIFICA_INT(
	        nfe_transp_add_vol(tr, NULL, NULL, NULL, NULL, "10.5", NULL),
	        E_VALOR);
	VERIFICA_INT(nfe_transp_set_modfrete(NULL, NFE_FRETE_TERCEIROS),
	             E_ISNULL);

	/* Nada mudou */
	xml = teste_gera(escreve, tr, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml)
		VERIFICA(strstr(xml, "<modFrete>9</modFrete></transp>") !=
		         NULL);
	free(xml);

	/* Limite de volumes (e crescimento do vetor) */
	for (i = 0; i < NFE_MAX_VOL; i++)
		rc |= nfe_transp_add_vol(tr, "1", NULL, NULL, NULL, NULL, NULL);
	VERIFICA_INT(rc, 0);
	VERIFICA_INT(nfe_transp_add_vol(tr, "1", NULL, NULL, NULL, NULL, NULL),
	             E_VALOR);
	xml = teste_gera(escreve, tr, &rc);
	VERIFICA_INT(rc, 0);
	if (xml)
		VERIFICA_INT(teste_valida(xml), 0);
	free(xml);
	VERIFICA_INT(nfe_transp_write_xml(NULL, tr), E_ISNULL);
	nfe_transp_free(tr);
	nfe_transp_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_sem_transporte();
	teste_transportador_e_volumes();
	teste_valores_invalidos();

	teste_libera_schema();
	TESTE_FIM();
}
