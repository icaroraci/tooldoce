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

#include <libnfe/chave.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>

/* Posição (a partir de 0) do CNPJ/CPF na chave: após cUF(2) e AAMM(4) */
#define CHAVE_INICIO_CNPJ 6

int nfe_chave_dv(const char *chave)
{
	int soma = 0, peso = 2, i, resto;

	if (!chave)
		return E_ISNULL;
	for (i = 0; i < NFE_TAM_CHAVE - 1; i++)
		if (chave[i] == '\0')
			return E_TAMANHO;

	/* Pesos 2, 3, ..., 9, 2, 3, ... da direita para a esquerda; cada
	 * caractere vale ASCII - 48. Letras só nas posições do CNPJ */
	for (i = NFE_TAM_CHAVE - 2; i >= 0; i--) {
		int letra_ok = i >= CHAVE_INICIO_CNPJ &&
		               i < CHAVE_INICIO_CNPJ + NFE_TAM_CNPJ - 2;
		char c = chave[i];

		if (!(c >= '0' && c <= '9') &&
		    !(letra_ok && c >= 'A' && c <= 'Z'))
			return E_VALOR;
		soma += (c - '0') * peso;
		peso = peso == 9 ? 2 : peso + 1;
	}

	resto = soma % 11;
	return resto < 2 ? 0 : 11 - resto;
}

int nfe_chave_validar(const char *chave)
{
	int dv;

	if (!chave)
		return E_ISNULL;
	if (strlen(chave) != NFE_TAM_CHAVE)
		return E_TAMANHO;
	dv = nfe_chave_dv(chave);
	if (dv < 0)
		return dv;
	if (chave[NFE_TAM_CHAVE - 1] != '0' + dv)
		return E_VALOR;
	return 0;
}
