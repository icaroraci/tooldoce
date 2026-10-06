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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include <libnfe/compacta.h>
#include <libnfe/erros.h>

/* Maior entrada aceita por nfe_gzip_base64 (cabe em uInt da zlib) */
#define COMPACTA_MAX 0x40000000u

static const char alfabeto[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Base64 de n bytes de b em s (4 * ((n + 2) / 3) + 1 bytes) */
static void codifica(const unsigned char *b, size_t n, char *s)
{
	size_t i;

	for (i = 0; i + 2 < n; i += 3) {
		*s++ = alfabeto[b[i] >> 2];
		*s++ = alfabeto[(b[i] & 3) << 4 | b[i + 1] >> 4];
		*s++ = alfabeto[(b[i + 1] & 15) << 2 | b[i + 2] >> 6];
		*s++ = alfabeto[b[i + 2] & 63];
	}
	if (i < n) {
		*s++ = alfabeto[b[i] >> 2];
		if (i + 1 < n) {
			*s++ = alfabeto[(b[i] & 3) << 4 | b[i + 1] >> 4];
			*s++ = alfabeto[(b[i + 1] & 15) << 2];
		} else {
			*s++ = alfabeto[(b[i] & 3) << 4];
			*s++ = '=';
		}
		*s++ = '=';
	}
	*s = '\0';
}

/* Valor de um caractere base64; -1 se não for do alfabeto */
static int valor(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 26;
	if (c >= '0' && c <= '9')
		return c - '0' + 52;
	if (c == '+')
		return 62;
	if (c == '/')
		return 63;
	return -1;
}

/* Decodifica n caracteres base64 de s em b (ao menos 3 * (n / 4) + 3
 * bytes), ignorando espaços; *tam recebe os bytes decodificados. Retorna 0 ou
 * E_VALOR. */
static int decodifica(const char *s, size_t n, unsigned char *b, size_t *tam)
{
	unsigned long grupo = 0;
	size_t i, k = 0, lidos = 0, pad = 0;

	for (i = 0; i < n; i++) {
		int v;

		if (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')
			continue;
		if (s[i] == '=') {
			/* '=' só completa o último grupo de 4 */
			if (lidos % 4 < 2 || ++pad > 2)
				return E_VALOR;
			lidos++;
			continue;
		}
		v = valor(s[i]);
		if (v < 0 || pad)
			return E_VALOR;
		grupo = grupo << 6 | (unsigned long)v;
		if (++lidos % 4 == 0) {
			b[k++] = (unsigned char)(grupo >> 16);
			b[k++] = (unsigned char)(grupo >> 8);
			b[k++] = (unsigned char)grupo;
			grupo = 0;
		}
	}
	if (pad) {
		if (lidos % 4)
			return E_VALOR;
		/* o grupo com '=' entrou como 2 ou 3 caracteres */
		grupo <<= 6 * pad;
		b[k++] = (unsigned char)(grupo >> 16);
		if (pad == 1)
			b[k++] = (unsigned char)(grupo >> 8);
	} else if (lidos % 4 == 1) {
		return E_VALOR;
	} else if (lidos % 4) {
		/* sem '=': 2 ou 3 caracteres no último grupo */
		grupo <<= 6 * (4 - lidos % 4);
		b[k++] = (unsigned char)(grupo >> 16);
		if (lidos % 4 == 3)
			b[k++] = (unsigned char)(grupo >> 8);
	}
	*tam = k;
	return 0;
}

int nfe_gzip_base64(const char *dados, size_t tam, char **saida)
{
	z_stream z;
	unsigned char *gz;
	char *s;
	uLong cap;
	size_t n;
	int rc;

	if (!dados || !saida)
		return E_ISNULL;
	if (tam > COMPACTA_MAX)
		return E_TAMANHO;
	memset(&z, 0, sizeof z);
	/* 15 + 16: janela padrão com cabeçalho gzip */
	if (deflateInit2(&z, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8,
	                 Z_DEFAULT_STRATEGY) != Z_OK)
		return E_MALLOC;
	cap = deflateBound(&z, (uLong)tam);
	gz = (unsigned char *)malloc(cap);
	if (!gz) {
		deflateEnd(&z);
		return E_MALLOC;
	}
	z.next_in = (Bytef *)(uintptr_t)dados;
	z.avail_in = (uInt)tam;
	z.next_out = gz;
	z.avail_out = (uInt)cap;
	rc = deflate(&z, Z_FINISH);
	n = (size_t)z.total_out;
	deflateEnd(&z);
	if (rc != Z_STREAM_END) {
		free(gz);
		return E_MALLOC;
	}
	s = (char *)malloc(4 * ((n + 2) / 3) + 1);
	if (!s) {
		free(gz);
		return E_MALLOC;
	}
	codifica(gz, n, s);
	free(gz);
	*saida = s;
	return 0;
}

int nfe_base64_gunzip(const char *texto, size_t tam, size_t limite,
                      char **dados, size_t *tam_dados)
{
	z_stream z;
	unsigned char *gz;
	char *out = NULL, *novo;
	size_t n, cap, usado = 0;
	int rc;

	if (!texto || !dados || !tam_dados)
		return E_ISNULL;
	if (limite == 0)
		limite = NFE_GUNZIP_LIMITE;
	if (limite > COMPACTA_MAX)
		limite = COMPACTA_MAX;
	if (tam / 4 > COMPACTA_MAX)
		return E_TAMANHO;
	gz = (unsigned char *)malloc(3 * (tam / 4) + 3);
	if (!gz)
		return E_MALLOC;
	rc = decodifica(texto, tam, gz, &n);
	if (rc == 0 && n == 0)
		rc = E_VALOR;
	if (rc) {
		free(gz);
		return rc;
	}

	memset(&z, 0, sizeof z);
	if (inflateInit2(&z, 15 + 16) != Z_OK) {
		free(gz);
		return E_MALLOC;
	}
	z.next_in = gz;
	z.avail_in = (uInt)n;
	cap = 0;
	do {
		if (usado == cap) {
			/* dobra até o limite, com 1 byte a mais para notar que
			 * o conteúdo passa dele e outro para o terminador */
			size_t prox = cap ? cap * 2 : 4 * n + 64;

			if (cap > limite) {
				rc = E_TAMANHO;
				break;
			}
			if (prox > limite + 1)
				prox = limite + 1;
			if (prox <= cap)
				prox = cap + 1;
			novo = (char *)realloc(out, prox + 1);
			if (!novo) {
				rc = E_MALLOC;
				break;
			}
			out = novo;
			cap = prox;
		}
		z.next_out = (Bytef *)out + usado;
		z.avail_out = (uInt)(cap - usado);
		rc = inflate(&z, Z_NO_FLUSH);
		usado = cap - z.avail_out;
		if (usado > limite) {
			rc = E_TAMANHO;
			break;
		}
		if (rc == Z_STREAM_END) {
			rc = 0;
			break;
		}
		if (rc == Z_MEM_ERROR) {
			rc = E_MALLOC;
			break;
		}
		/* Z_BUF_ERROR com saída livre: o gzip acabou antes do fim */
		if (rc != Z_OK || (z.avail_in == 0 && z.avail_out > 0)) {
			rc = E_VALOR;
			break;
		}
	} while (1);
	/* bytes depois do fim do gzip também tornam o texto inválido */
	if (rc == 0 && z.avail_in > 0)
		rc = E_VALOR;
	inflateEnd(&z);
	free(gz);
	if (rc) {
		free(out);
		return rc;
	}
	out[usado] = '\0';
	*dados = out;
	*tam_dados = usado;
	return 0;
}
