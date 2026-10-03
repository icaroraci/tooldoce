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

#include <stdio.h>

#include <libnfe/decimal.h>
#include <libnfe/erros.h>

/* Limite: 13 dígitos inteiros (TDec_1302), em centavos */
#define MAXIMO 999999999999999LL

int nfe_dec2_ler(const char *valor, long long *centavos)
{
	long long v = 0;
	int casas = -1; /* -1: antes do ponto */
	const char *p;

	if (!valor || !centavos)
		return E_ISNULL;
	for (p = valor; *p; p++) {
		if (*p == '.') {
			if (casas >= 0 || p == valor)
				return E_VALOR;
			casas = 0;
			continue;
		}
		if (*p < '0' || *p > '9' || casas == 2)
			return E_VALOR;
		v = v * 10 + (*p - '0');
		if (v > MAXIMO)
			return E_VALOR;
		if (casas >= 0)
			casas++;
	}
	if (casas == 0) /* "15." */
		return E_VALOR;
	for (; casas < 2; casas = casas < 0 ? 1 : casas + 1)
		v *= 10;
	if (v > MAXIMO)
		return E_VALOR;
	*centavos = v;
	return 0;
}

int nfe_dec2_escreve(long long centavos, char *dst, size_t tam)
{
	int n;

	if (!dst)
		return E_ISNULL;
	if (centavos < 0 || centavos > MAXIMO)
		return E_VALOR;
	n = snprintf(dst, tam, "%lld.%02lld", centavos / 100, centavos % 100);
	if (n < 0 || (size_t)n >= tam)
		return E_TAMANHO;
	return 0;
}
