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

/* Testes do gzip com base64 (compacta.h).
 *
 * Uso: test_compacta */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/compacta.h>
#include <libnfe/erros.h>

#include "teste.h"

/* <DPS>ola</DPS> em gzip (Python, mtime 0), base64 */
#define DPS_GZ "H4sIAAAAAAACA7NxCQi2y89JtNEHMQD0Nfq3DgAAAA=="

/* Raiz vazia da DPS com o namespace */
#define DPS_NS "<DPS xmlns=\"http://www.sped.fazenda.gov.br/nfse\"/>"

/* Decodifica t (inteiro) com o limite e confere o retorno */
static int gunzip(const char *t, size_t limite, char **d, size_t *n)
{
	return nfe_base64_gunzip(t, strlen(t), limite, d, n);
}

/* Compacta e descompacta n bytes de s; confere que voltam iguais */
static void ida_e_volta(const char *s, size_t n)
{
	char *b64 = NULL, *d = NULL;
	size_t tam = 99;

	VERIFICA_INT(nfe_gzip_base64(s, n, &b64), 0);
	if (!b64)
		return;
	VERIFICA(strlen(b64) % 4 == 0);
	VERIFICA_INT(gunzip(b64, 0, &d, &tam), 0);
	VERIFICA(tam == n);
	VERIFICA(d && memcmp(d, s, n) == 0 && d[n] == '\0');
	free(d);
	free(b64);
}

int main(void)
{
	char *d = NULL, *b64 = NULL, *grande;
	size_t tam = 0, i;

	/* Resposta pronta (gerada fora da libnfe) */
	VERIFICA_INT(gunzip(DPS_GZ, 0, &d, &tam), 0);
	VERIFICA_STR(d, "<DPS>ola</DPS>");
	VERIFICA(tam == 14);
	free(d);
	d = NULL;

	/* Quebras de linha e espaços no meio do base64 são ignorados */
	VERIFICA_INT(
	        gunzip("H4sIAAAAAAACA7Nx\r\nCQi2y89JtNEHMQD0\n Nfq3DgAAAA==", 0,
	               &d, &tam),
	        0);
	VERIFICA_STR(d, "<DPS>ola</DPS>");
	free(d);
	d = NULL;

	/* Ida e volta com tamanhos que cobrem os três finais do base64 */
	ida_e_volta("", 0);
	ida_e_volta("a", 1);
	ida_e_volta("ab", 2);
	ida_e_volta("abc", 3);
	ida_e_volta(DPS_NS, sizeof DPS_NS - 1);
	ida_e_volta("\0\1\2\377", 4);

	/* Limite: 1000 bytes passam com limite 1000 e não com 999 */
	grande = (char *)malloc(1000);
	VERIFICA(grande != NULL);
	if (grande) {
		for (i = 0; i < 1000; i++)
			grande[i] = (char)('a' + i % 7);
		VERIFICA_INT(nfe_gzip_base64(grande, 1000, &b64), 0);
		VERIFICA_INT(gunzip(b64, 1000, &d, &tam), 0);
		VERIFICA(tam == 1000 && memcmp(d, grande, 1000) == 0);
		free(d);
		d = NULL;
		tam = 7;
		VERIFICA_INT(gunzip(b64, 999, &d, &tam), E_TAMANHO);
		VERIFICA(d == NULL && tam == 7);
		VERIFICA_INT(gunzip(b64, 1, &d, &tam), E_TAMANHO);
		free(b64);
		b64 = NULL;
		free(grande);
	}

	/* Base64 inválido */
	VERIFICA_INT(gunzip("", 0, &d, &tam), E_VALOR);
	VERIFICA_INT(gunzip("@@@@", 0, &d, &tam), E_VALOR);
	VERIFICA_INT(gunzip("H4sIA", 0, &d, &tam), E_VALOR);
	VERIFICA_INT(gunzip("H4s=IAAA", 0, &d, &tam), E_VALOR);
	VERIFICA_INT(gunzip("H4sIAAA===", 0, &d, &tam), E_VALOR);
	/* Base64 válido mas não é gzip */
	VERIFICA_INT(gunzip("eHl6eHl6eHl6eHl6", 0, &d, &tam), E_VALOR);
	/* gzip truncado (sem o tamanho final) e com bytes depois do fim */
	VERIFICA_INT(
	        gunzip("H4sIAAAAAAACA7NxCQi2y89JtNEHMQD0Nfq3", 0, &d, &tam),
	        E_VALOR);
	VERIFICA_INT(gunzip("H4sIAAAAAAACA7NxCQi2y89JtNEHMQD0Nfq3DgAAAHg=", 0,
	                    &d, &tam),
	             E_VALOR);
	VERIFICA(d == NULL);

	/* Ponteiros nulos */
	VERIFICA_INT(nfe_gzip_base64(NULL, 0, &b64), E_ISNULL);
	VERIFICA_INT(nfe_gzip_base64("a", 1, NULL), E_ISNULL);
	VERIFICA_INT(nfe_base64_gunzip(NULL, 0, 0, &d, &tam), E_ISNULL);
	VERIFICA_INT(nfe_base64_gunzip(DPS_GZ, 4, 0, NULL, &tam), E_ISNULL);
	VERIFICA_INT(nfe_base64_gunzip(DPS_GZ, 4, 0, &d, NULL), E_ISNULL);
	TESTE_FIM();
}
