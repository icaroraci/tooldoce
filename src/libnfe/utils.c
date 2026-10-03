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
#include <string.h>

#include <libnfe/utils.h>

int nfe_ptrnull(const void *ptr)
{
	if (ptr == NULL) {
		return E_ISNULL;
	}
	return 0;
}

int nfe_copia_texto(char *dst, size_t tam, const char *src, size_t min,
                    size_t max)
{
	size_t bytes = 0, caracteres = 0;

	if (dst == NULL || src == NULL) {
		return E_ISNULL;
	}
	for (; src[bytes] != '\0'; bytes++) {
		/* Em UTF-8, bytes de continuação têm a forma 10xxxxxx */
		if (((unsigned char)src[bytes] & 0xC0) != 0x80) {
			caracteres++;
		}
	}
	if (bytes >= tam || caracteres < min || (max > 0 && caracteres > max)) {
		return E_TAMANHO;
	}
	memcpy(dst, src, bytes + 1);
	return 0;
}
