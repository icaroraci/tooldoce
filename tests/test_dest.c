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

/* Testes do destinatário (dest), com validação do XML contra o XSD
 * oficial.
 *
 * Uso: test_dest <diretório tests> */

#include <libnfe/dest.h>
#include <libnfe/erros.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *dest)
{
	return nfe_dest_write_xml(writer, (const nfe_dest *)dest);
}

static nfe_endereco *endereco(void)
{
	nfe_endereco *end = nfe_endereco_new();
	int rc = 0;

	if (!end)
		return NULL;
	rc |= nfe_endereco_set_xlgr(end, "AV. BRASIL");
	rc |= nfe_endereco_set_nro(end, "S/N");
	rc |= nfe_endereco_set_xbairro(end, "JARDIM");
	rc |= nfe_endereco_set_cmun(end, 3304557);
	rc |= nfe_endereco_set_xmun(end, "RIO DE JANEIRO");
	rc |= nfe_endereco_set_uf(end, "RJ");
	VERIFICA_INT(rc, 0);
	return end;
}

static void teste_consumidor(void)
{
	nfe_dest *dest = nfe_dest_new();
	char *xml;
	int rc;

	VERIFICA(dest != NULL);
	if (!dest)
		return;

	/* Mínimo: só a identificação e o indIEDest */
	VERIFICA_INT(nfe_dest_set_cpf(dest, "12345678909"), 0);
	VERIFICA_INT(nfe_dest_set_indiedest(dest, NFE_IE_DEST_NAO_CONTRIBUINTE),
	             0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<dest xmlns=\"" TESTE_NS "\">"
		                "<CPF>12345678909</CPF>"
		                "<indIEDest>9</indIEDest></dest>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Com nome, endereço e e-mail */
	VERIFICA_INT(nfe_dest_set_xnome(dest, "FULANO DE TAL"), 0);
	VERIFICA_INT(nfe_dest_set_endereco(dest, endereco()), 0);
	VERIFICA_INT(nfe_dest_set_email(dest, "fulano@exemplo.com.br"), 0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<CPF>12345678909</CPF>"
		                "<xNome>FULANO DE TAL</xNome>"
		                "<enderDest><xLgr>AV. BRASIL</xLgr>") != NULL);
		VERIFICA(strstr(xml, "</enderDest><indIEDest>9</indIEDest>"
		                     "<email>fulano@exemplo.com.br</email>"
		                     "</dest>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* NULL remove o endereço e os opcionais */
	VERIFICA_INT(nfe_dest_set_endereco(dest, NULL), 0);
	VERIFICA_INT(nfe_dest_set_xnome(dest, NULL), 0);
	VERIFICA_INT(nfe_dest_set_email(dest, NULL), 0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "enderDest") == NULL);
		VERIFICA(strstr(xml, "xNome") == NULL);
		VERIFICA(strstr(xml, "email") == NULL);
	}
	free(xml);
	nfe_dest_free(dest);
}

static void teste_empresa(void)
{
	nfe_dest *dest = nfe_dest_new();
	char *xml;
	int rc;

	VERIFICA(dest != NULL);
	if (!dest)
		return;
	VERIFICA_INT(nfe_dest_set_cnpj(dest, "12ABC34501DE35"), 0);
	VERIFICA_INT(nfe_dest_set_xnome(dest, "CLIENTE S.A."), 0);
	VERIFICA_INT(nfe_dest_set_endereco(dest, endereco()), 0);
	VERIFICA_INT(nfe_dest_set_indiedest(dest, NFE_IE_DEST_CONTRIBUINTE), 0);
	VERIFICA_INT(nfe_dest_set_ie(dest, "12345678"), 0);
	VERIFICA_INT(nfe_dest_set_isuf(dest, "12345678"), 0);
	VERIFICA_INT(nfe_dest_set_im(dest, "998877"), 0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<CNPJ>12ABC34501DE35</CNPJ>") != NULL);
		VERIFICA(strstr(xml, "<indIEDest>1</indIEDest><IE>12345678</IE>"
		                     "<ISUF>12345678</ISUF><IM>998877</IM>"
		                     "</dest>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_dest_free(dest);
}

static void teste_estrangeiro(void)
{
	nfe_dest *dest = nfe_dest_new();
	nfe_endereco *end = nfe_endereco_new();
	char *xml;
	int rc = 0;

	VERIFICA(dest != NULL && end != NULL);
	if (!dest || !end) {
		nfe_dest_free(dest);
		nfe_endereco_free(end);
		return;
	}
	rc |= nfe_endereco_set_xlgr(end, "5TH AVENUE");
	rc |= nfe_endereco_set_nro(end, "100");
	rc |= nfe_endereco_set_xbairro(end, "MANHATTAN");
	rc |= nfe_endereco_set_cmun(end, NFE_CMUN_EXTERIOR);
	rc |= nfe_endereco_set_xmun(end, NFE_XMUN_EXTERIOR);
	rc |= nfe_endereco_set_uf(end, NFE_UF_EXTERIOR);
	rc |= nfe_endereco_set_cpais(end, 2496);
	rc |= nfe_endereco_set_xpais(end, "ESTADOS UNIDOS");
	VERIFICA_INT(rc, 0);

	VERIFICA_INT(nfe_dest_set_idestrangeiro(dest, "P1234567"), 0);
	VERIFICA_INT(nfe_dest_set_xnome(dest, "JOHN DOE"), 0);
	VERIFICA_INT(nfe_dest_set_endereco(dest, end), 0);
	VERIFICA_INT(nfe_dest_set_indiedest(dest, NFE_IE_DEST_NAO_CONTRIBUINTE),
	             0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<idEstrangeiro>P1234567</idEstrangeiro>"
		                     "<xNome>JOHN DOE</xNome>") != NULL);
		VERIFICA(strstr(xml, "<UF>EX</UF><cPais>2496</cPais>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Estrangeiro sem documento: idEstrangeiro vazio */
	VERIFICA_INT(nfe_dest_set_idestrangeiro(dest, ""), 0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<idEstrangeiro></idEstrangeiro>") !=
		         NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	nfe_dest_free(dest);
}

static void teste_valores_invalidos(void)
{
	nfe_dest *dest = nfe_dest_new();
	nfe_endereco *end;
	char *xml;
	int rc;

	VERIFICA(dest != NULL);
	if (!dest)
		return;
	VERIFICA_INT(nfe_dest_set_cpf(dest, "12345678909"), 0);
	VERIFICA_INT(nfe_dest_set_indiedest(dest, NFE_IE_DEST_ISENTO), 0);

	VERIFICA_INT(nfe_dest_set_cnpj(dest, "12345678000196"), E_VALOR);
	VERIFICA_INT(nfe_dest_set_cpf(dest, "1234567890"), E_TAMANHO);
	VERIFICA_INT(nfe_dest_set_idestrangeiro(dest, "P123"), E_VALOR);
	VERIFICA_INT(nfe_dest_set_idestrangeiro(dest, "P 12345"), E_VALOR);
	VERIFICA_INT(nfe_dest_set_idestrangeiro(dest, "123456789012345678901"),
	             E_VALOR);
	VERIFICA_INT(nfe_dest_set_idestrangeiro(dest, NULL), E_ISNULL);
	VERIFICA_INT(nfe_dest_set_xnome(dest, "F"), E_TAMANHO);
	VERIFICA_INT(nfe_dest_set_indiedest(dest, NFE_IE_DEST_NAO_INFORMADO),
	             E_VALOR);
	VERIFICA_INT(nfe_dest_set_indiedest(dest, (nfe_ind_ie_dest)3), E_VALOR);
	VERIFICA_INT(nfe_dest_set_ie(dest, "ISENTO"), E_VALOR);
	VERIFICA_INT(nfe_dest_set_ie(dest, "1"), E_VALOR);
	VERIFICA_INT(nfe_dest_set_isuf(dest, "1234567890"), E_VALOR);
	VERIFICA_INT(nfe_dest_set_im(dest, ""), E_TAMANHO);
	VERIFICA_INT(nfe_dest_set_email(dest, " fulano@exemplo.com"), E_VALOR);
	VERIFICA_INT(nfe_dest_set_cpf(NULL, "12345678909"), E_ISNULL);

	/* Nada mudou */
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<CPF>12345678909</CPF>"
		                "<indIEDest>2</indIEDest></dest>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Endereço incompleto */
	end = nfe_endereco_new();
	VERIFICA_INT(nfe_dest_set_endereco(dest, end), 0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	nfe_dest_free(dest);
}

/* Sem identificação ou indIEDest, o XML não é gerado */
static void teste_obrigatorios(void)
{
	nfe_dest *dest = nfe_dest_new();
	char *xml;
	int rc;

	VERIFICA(dest != NULL);
	VERIFICA_INT(nfe_dest_set_indiedest(dest, NFE_IE_DEST_NAO_CONTRIBUINTE),
	             0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	nfe_dest_free(dest);

	dest = nfe_dest_new();
	VERIFICA_INT(nfe_dest_set_cpf(dest, "12345678909"), 0);
	xml = teste_gera(escreve, dest, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_dest_write_xml(NULL, dest), E_ISNULL);
	nfe_dest_free(dest);
	nfe_dest_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_consumidor();
	teste_empresa();
	teste_estrangeiro();
	teste_valores_invalidos();
	teste_obrigatorios();

	teste_libera_schema();
	TESTE_FIM();
}
