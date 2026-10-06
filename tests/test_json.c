/* Copyright (c) 2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
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

/* Testes da leitura de JSON (json.h).
 *
 * Uso: test_json */

#include <stdio.h>
#include <string.h>

#include <libnfe/erros.h>
#include <libnfe/json.h>

#include "teste.h"

/* Lê o texto inteiro t */
static int ler(const char *t, nfe_json **j)
{
	return nfe_json_ler(t, strlen(t), j);
}

/* Lê t e devolve o texto da raiz em buf (vazio se não houver) */
static int texto_de(const char *t, char *buf, size_t tam)
{
	nfe_json *j = NULL;
	int rc = ler(t, &j);

	buf[0] = '\0';
	if (rc == 0 && nfe_json_texto(j))
		snprintf(buf, tam, "%s", nfe_json_texto(j));
	nfe_json_free(j);
	return rc;
}

/* Lê t, que deve ser recusado com rc */
static void recusa(const char *t, int rc)
{
	nfe_json *j = NULL;

	VERIFICA_INT(ler(t, &j), rc);
	VERIFICA(j == NULL);
}

int main(void)
{
	static const char resposta[] =
	        " {\"tipoAmbiente\": 2, \"versaoAplicativo\": \"1.0\",\n"
	        "  \"dataHoraProcessamento\": \"2026-10-05T10:00:00-03:00\",\n"
	        "  \"idDps\": "
	        "\"DPS330455721234567800019500001000000000000001\",\n"
	        "  \"chaveAcesso\": "
	        "\"33045572123456780001950000100000000000000016\",\n"
	        "  \"nfseXmlGZipB64\": \"H4sI\\/AA==\", \"alertas\": null,\n"
	        "  \"erros\": [ {\"Codigo\": \"E0001\", \"Descricao\": "
	        "\"Inv\\u00e1lido\"},"
	        "  {\"Codigo\": \"E0002\", \"Complemento\": [1, -2.5e+3, true, "
	        "false, {}]} ] } ";
	char buf[64];
	const nfe_json *erros, *e, *c;
	nfe_json *j = NULL;
	int i;

	VERIFICA_INT(nfe_json_ler(resposta, sizeof resposta - 1, &j), 0);
	VERIFICA(nfe_json_tipo_de(j) == NFE_JSON_OBJETO);
	VERIFICA(nfe_json_qtd(j) == 8);
	VERIFICA_STR(nfe_json_nome(j, 0), "tipoAmbiente");
	VERIFICA(nfe_json_tipo_de(nfe_json_item(j, 0)) == NFE_JSON_NUMERO);
	VERIFICA_STR(nfe_json_texto(nfe_json_campo(j, "tipoAmbiente")), "2");
	VERIFICA_STR(nfe_json_texto(nfe_json_campo(j, "nfseXmlGZipB64")),
	             "H4sI/AA==");
	VERIFICA(nfe_json_campo(j, "alertas") != NULL);
	VERIFICA(nfe_json_tipo_de(nfe_json_campo(j, "alertas")) ==
	         NFE_JSON_NULO);
	VERIFICA(nfe_json_texto(nfe_json_campo(j, "alertas")) == NULL);
	VERIFICA(nfe_json_campo(j, "ChaveAcesso") == NULL);
	VERIFICA(nfe_json_campo(j, "faltando") == NULL);
	erros = nfe_json_campo(j, "erros");
	VERIFICA(nfe_json_tipo_de(erros) == NFE_JSON_LISTA);
	VERIFICA(nfe_json_qtd(erros) == 2);
	VERIFICA(nfe_json_nome(erros, 0) == NULL);
	e = nfe_json_item(erros, 0);
	VERIFICA_STR(nfe_json_texto(nfe_json_campo(e, "Codigo")), "E0001");
	VERIFICA_STR(nfe_json_texto(nfe_json_campo(e, "Descricao")),
	             "Inv\xC3\xA1lido");
	c = nfe_json_campo(nfe_json_item(erros, 1), "Complemento");
	VERIFICA(nfe_json_qtd(c) == 5);
	VERIFICA_STR(nfe_json_texto(nfe_json_item(c, 1)), "-2.5e+3");
	VERIFICA(nfe_json_tipo_de(nfe_json_item(c, 2)) == NFE_JSON_LOGICO);
	VERIFICA_STR(nfe_json_texto(nfe_json_item(c, 2)), "true");
	VERIFICA_STR(nfe_json_texto(nfe_json_item(c, 3)), "false");
	VERIFICA(nfe_json_tipo_de(nfe_json_item(c, 4)) == NFE_JSON_OBJETO);
	VERIFICA(nfe_json_qtd(nfe_json_item(c, 4)) == 0);
	VERIFICA(nfe_json_item(c, 5) == NULL);
	/* Encadeamento com nós ausentes */
	VERIFICA(nfe_json_campo(nfe_json_item(erros, 9), "Codigo") == NULL);
	VERIFICA(nfe_json_texto(NULL) == NULL);
	VERIFICA(nfe_json_qtd(NULL) == 0);
	VERIFICA(nfe_json_campo(erros, "Codigo") == NULL);
	VERIFICA(nfe_json_tipo_de(NULL) == NFE_JSON_NULO);
	nfe_json_free(j);
	j = NULL;

	/* O tamanho manda, não o terminador */
	VERIFICA_INT(nfe_json_ler("[1]lixo", 3, &j), 0);
	VERIFICA(nfe_json_qtd(j) == 1);
	nfe_json_free(j);
	j = NULL;

	/* Escapes */
	VERIFICA_INT(
	        texto_de("\"a\\\"b\\\\c\\/d\\b\\f\\n\\r\\t\"", buf, sizeof buf),
	        0);
	VERIFICA_STR(buf, "a\"b\\c/d\b\f\n\r\t");
	VERIFICA_INT(texto_de("\"\\u0041\\u00e9\\u20AC\\ud83d\\ude00\"", buf,
	                      sizeof buf),
	             0);
	VERIFICA_STR(buf, "A\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80");
	VERIFICA_INT(texto_de("\"ol\xC3\xA1\"", buf, sizeof buf), 0);
	VERIFICA_STR(buf, "ol\xC3\xA1");
	VERIFICA_INT(texto_de("0", buf, sizeof buf), 0);
	VERIFICA_STR(buf, "0");
	VERIFICA_INT(texto_de("-0.5E-2", buf, sizeof buf), 0);
	VERIFICA_STR(buf, "-0.5E-2");

	/* Profundidade */
	{
		char fundo[2 * (NFE_JSON_PROFUNDIDADE + 1) + 1];

		for (i = 0; i < NFE_JSON_PROFUNDIDADE; i++) {
			fundo[i] = '[';
			fundo[2 * NFE_JSON_PROFUNDIDADE - 1 - i] = ']';
		}
		fundo[2 * NFE_JSON_PROFUNDIDADE] = '\0';
		VERIFICA_INT(ler(fundo, &j), 0);
		nfe_json_free(j);
		j = NULL;
		memmove(fundo + 1, fundo, 2 * NFE_JSON_PROFUNDIDADE + 1);
		fundo[0] = '[';
		strcat(fundo, "]");
		recusa(fundo, E_VALOR);
	}

	/* JSON inválido */
	recusa("", E_VALOR);
	recusa("   ", E_VALOR);
	recusa("{", E_VALOR);
	recusa("{\"a\"}", E_VALOR);
	recusa("{\"a\":1,}", E_VALOR);
	recusa("[1,]", E_VALOR);
	recusa("[1 2]", E_VALOR);
	recusa("{a:1}", E_VALOR);
	recusa("{\"a\":1} x", E_VALOR);
	recusa("\"aberto", E_VALOR);
	recusa("\"a\\", E_VALOR);
	recusa("\"\\x\"", E_VALOR);
	recusa("\"\\u12\"", E_VALOR);
	recusa("\"\\u0000\"", E_VALOR);
	recusa("\"\\ud83d\"", E_VALOR);
	recusa("\"\\ude00\"", E_VALOR);
	recusa("\"a\nb\"", E_VALOR);
	recusa("01", E_VALOR);
	recusa("-", E_VALOR);
	recusa("1.", E_VALOR);
	recusa(".5", E_VALOR);
	recusa("1e", E_VALOR);
	recusa("+1", E_VALOR);
	recusa("tru", E_VALOR);
	recusa("nul", E_VALOR);
	recusa("True", E_VALOR);

	/* Ponteiros nulos */
	VERIFICA_INT(nfe_json_ler(NULL, 0, &j), E_ISNULL);
	VERIFICA_INT(nfe_json_ler("1", 1, NULL), E_ISNULL);
	nfe_json_free(NULL);
	TESTE_FIM();
}
