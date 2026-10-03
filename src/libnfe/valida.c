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

#include <string.h>

#include <libxml/xmlregexp.h>
#include <libxml/xmlstring.h>

#include <libnfe/valida.h>

int nfe_valida_padrao(const char *valor, const char *padrao)
{
	xmlRegexpPtr re;
	int r;

	if (valor == NULL || padrao == NULL) {
		return E_ISNULL;
	}
	if (!xmlCheckUTF8((const xmlChar *)valor)) {
		return E_VALOR;
	}
	/* Compilado a cada chamada: sem estado global, a função pode ser usada
	 * de várias threads ao mesmo tempo */
	re = xmlRegexpCompile((const xmlChar *)padrao);
	if (re == NULL) {
		return E_MALLOC;
	}
	r = xmlRegexpExec(re, (const xmlChar *)valor);
	xmlRegFreeRegexp(re);
	if (r < 0) {
		return E_MALLOC;
	}
	return r == 1 ? 0 : E_VALOR;
}

int nfe_valida_lista(const char *valor, const char *const *lista)
{
	if (valor == NULL || lista == NULL) {
		return E_ISNULL;
	}
	for (; *lista != NULL; lista++) {
		if (strcmp(valor, *lista) == 0) {
			return 0;
		}
	}
	return E_VALOR;
}

int nfe_valida_texto(const char *valor, size_t min, size_t max)
{
	size_t caracteres = 0;
	const char *p;

	if (valor == NULL) {
		return E_ISNULL;
	}
	for (p = valor; *p != '\0'; p++) {
		/* Em UTF-8, bytes de continuação têm a forma 10xxxxxx */
		if (((unsigned char)*p & 0xC0) != 0x80) {
			caracteres++;
		}
	}
	if (caracteres < min || (max > 0 && caracteres > max)) {
		return E_TAMANHO;
	}
	return nfe_valida_padrao(valor, NFE_PADRAO_TString);
}

static int copia(char *dst, size_t tam, const char *valor)
{
	size_t n = strlen(valor);

	if (n >= tam) {
		return E_TAMANHO;
	}
	memcpy(dst, valor, n + 1);
	return 0;
}

int nfe_copia_padrao(char *dst, size_t tam, const char *valor,
                     const char *padrao)
{
	int r;

	if (dst == NULL) {
		return E_ISNULL;
	}
	r = nfe_valida_padrao(valor, padrao);
	if (r != 0) {
		return r;
	}
	return copia(dst, tam, valor);
}

int nfe_copia_texto_validado(char *dst, size_t tam, const char *valor,
                             size_t min, size_t max)
{
	int r;

	if (dst == NULL) {
		return E_ISNULL;
	}
	r = nfe_valida_texto(valor, min, max);
	if (r != 0) {
		return r;
	}
	return copia(dst, tam, valor);
}

int nfe_uf_valida(int cuf)
{
	static const int codigos[] = { 11, 12, 13, 14, 15, 16, 17, 21, 22,
		                       23, 24, 25, 26, 27, 28, 29, 31, 32,
		                       33, 35, 41, 42, 43, 50, 51, 52, 53 };
	size_t i;

	for (i = 0; i < sizeof codigos / sizeof codigos[0]; i++)
		if (cuf == codigos[i])
			return 0;
	return E_VALOR;
}
