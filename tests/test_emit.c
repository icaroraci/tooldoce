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

/* Testes do emitente (emit), com validação do XML contra o XSD oficial.
 *
 * Uso: test_emit <diretório tests> */

#include <libnfe/emit.h>
#include <libnfe/erros.h>

#include "teste.h"
#include "teste_xml.h"

static int escreve(xmlTextWriterPtr writer, const void *emit)
{
	return nfe_emit_write_xml(writer, (const nfe_emit *)emit);
}

static nfe_endereco *endereco(void)
{
	nfe_endereco *end = nfe_endereco_new();
	int rc = 0;

	if (!end)
		return NULL;
	rc |= nfe_endereco_set_xlgr(end, "RUA DAS FLORES");
	rc |= nfe_endereco_set_nro(end, "123");
	rc |= nfe_endereco_set_xbairro(end, "CENTRO");
	rc |= nfe_endereco_set_cmun(end, 3550308);
	rc |= nfe_endereco_set_xmun(end, "SAO PAULO");
	rc |= nfe_endereco_set_uf(end, "SP");
	rc |= nfe_endereco_set_cep(end, "01001000");
	VERIFICA_INT(rc, 0);
	return end;
}

/* Emitente com os campos obrigatórios */
static nfe_emit *novo(void)
{
	nfe_emit *emit = nfe_emit_new();
	int rc = 0;

	if (!emit)
		return NULL;
	rc |= nfe_emit_set_cnpj(emit, "12345678000195");
	rc |= nfe_emit_set_xnome(emit, "EMPRESA EXEMPLO LTDA");
	rc |= nfe_emit_set_endereco(emit, endereco());
	rc |= nfe_emit_set_crt(emit, NFE_CRT_REGIME_NORMAL);
	VERIFICA_INT(rc, 0);
	return emit;
}

static void teste_completo(void)
{
	nfe_emit *emit = novo();
	char *xml;
	int rc;

	VERIFICA(emit != NULL);
	if (!emit)
		return;

	xml = teste_gera(escreve, emit, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<emit xmlns=\"" TESTE_NS "\">"
		                     "<CNPJ>12345678000195</CNPJ>"
		                     "<xNome>EMPRESA EXEMPLO LTDA</xNome>"
		                     "<enderEmit>") != NULL);
		VERIFICA(strstr(xml, "</enderEmit><CRT>3</CRT></emit>") !=
		         NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Todos os opcionais, na ordem do leiaute */
	VERIFICA_INT(nfe_emit_set_xfant(emit, "EXEMPLO"), 0);
	VERIFICA_INT(nfe_emit_set_ie(emit, "123456789012"), 0);
	VERIFICA_INT(nfe_emit_set_iest(emit, "98765432"), 0);
	VERIFICA_INT(nfe_emit_set_im(emit, "IM-12345"), 0);
	VERIFICA_INT(nfe_emit_set_cnae(emit, "4751201"), 0);
	VERIFICA_INT(nfe_emit_set_isufemit(emit, "123456789"), 0);
	VERIFICA_INT(nfe_emit_set_crt(emit, NFE_CRT_MEI), 0);
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "</xNome><xFant>EXEMPLO</xFant>"
		                     "<enderEmit>") != NULL);
		VERIFICA(strstr(xml, "</enderEmit><IE>123456789012</IE>"
		                     "<IEST>98765432</IEST><IM>IM-12345</IM>"
		                     "<CNAE>4751201</CNAE><CRT>4</CRT>"
		                     "<ISUFEmit>123456789</ISUFEmit></emit>") !=
		         NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* CPF e CNPJ alfanumérico; IE ISENTO */
	VERIFICA_INT(nfe_emit_set_cpf(emit, "12345678909"), 0);
	VERIFICA_INT(nfe_emit_set_ie(emit, "ISENTO"), 0);
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<CPF>12345678909</CPF>") != NULL);
		VERIFICA(strstr(xml, "CNPJ") == NULL);
		VERIFICA(strstr(xml, "<IE>ISENTO</IE>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	VERIFICA_INT(nfe_emit_set_cnpj(emit, "12ABC34501DE35"), 0);
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<CNPJ>12ABC34501DE35</CNPJ>") != NULL);
		VERIFICA(strstr(xml, "CPF") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* CNAE só com IM */
	VERIFICA_INT(nfe_emit_set_im(emit, NULL), 0);
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_emit_set_cnae(emit, NULL), 0);
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA_INT(rc, 0);
	if (xml)
		VERIFICA_INT(teste_valida(xml), 0);
	free(xml);
	nfe_emit_free(emit);
}

static void teste_valores_invalidos(void)
{
	nfe_emit *emit = novo();
	nfe_endereco *end;
	char *xml;
	int rc;

	VERIFICA(emit != NULL);
	if (!emit)
		return;

	VERIFICA_INT(nfe_emit_set_cnpj(emit, "12345678000196"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_cnpj(emit, "1234567800019"), E_TAMANHO);
	VERIFICA_INT(nfe_emit_set_cpf(emit, "12345678900"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_cpf(emit, NULL), E_ISNULL);
	VERIFICA_INT(nfe_emit_set_xnome(emit, "E"), E_TAMANHO);
	VERIFICA_INT(nfe_emit_set_xnome(emit, "EMPRESA "), E_VALOR);
	VERIFICA_INT(nfe_emit_set_xfant(emit, ""), E_TAMANHO);
	VERIFICA_INT(nfe_emit_set_ie(emit, "1"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_ie(emit, "123.456"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_ie(emit, "isento"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_iest(emit, "ISENTO"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_im(emit, "1234567890123456"), E_TAMANHO);
	VERIFICA_INT(nfe_emit_set_cnae(emit, "475120"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_crt(emit, NFE_CRT_NAO_INFORMADO), E_VALOR);
	VERIFICA_INT(nfe_emit_set_crt(emit, (nfe_crt)5), E_VALOR);
	VERIFICA_INT(nfe_emit_set_isufemit(emit, "1234567"), E_VALOR);
	VERIFICA_INT(nfe_emit_set_endereco(emit, NULL), E_ISNULL);
	VERIFICA_INT(nfe_emit_set_xnome(NULL, "EMPRESA"), E_ISNULL);

	/* Nada mudou */
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<CNPJ>12345678000195</CNPJ>"
		                     "<xNome>EMPRESA EXEMPLO LTDA</xNome>"
		                     "<enderEmit>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Endereço inválido para o emitente (sem CEP) */
	end = endereco();
	VERIFICA_INT(nfe_endereco_set_cep(end, NULL), 0);
	VERIFICA_INT(nfe_emit_set_endereco(emit, end), 0);
	VERIFICA_INT(nfe_emit_set_endereco(emit, end), 0); /* o mesmo */
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	nfe_emit_free(emit);
}

/* Sem os campos obrigatórios, o XML não é gerado */
static void teste_obrigatorios(void)
{
	nfe_emit *emit = nfe_emit_new();
	char *xml;
	int rc;

	VERIFICA(emit != NULL);
	xml = teste_gera(escreve, emit, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_emit_set_cnpj(emit, "12345678000195"), 0);
	VERIFICA_INT(nfe_emit_set_xnome(emit, "EMPRESA"), 0);
	VERIFICA_INT(nfe_emit_set_endereco(emit, endereco()), 0);
	xml = teste_gera(escreve, emit, &rc); /* falta o CRT */
	VERIFICA_INT(rc, E_VALOR);
	free(xml);
	VERIFICA_INT(nfe_emit_write_xml(NULL, emit), E_ISNULL);
	nfe_emit_free(emit);
	nfe_emit_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_completo();
	teste_valores_invalidos();
	teste_obrigatorios();

	teste_libera_schema();
	TESTE_FIM();
}
