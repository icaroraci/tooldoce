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

#include <libnfe/cnpjcpf.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>

/* Dígito verificador (módulo 11, pesos 2 a peso_max da direita para a
 * esquerda) dos n primeiros caracteres; caractere vale ASCII - 48 */
static int dv_mod11(const char *s, int n, int peso_max)
{
	int soma = 0, peso = 2, i, resto;

	for (i = n - 1; i >= 0; i--) {
		soma += (s[i] - '0') * peso;
		peso = peso == peso_max ? 2 : peso + 1;
	}
	resto = soma % 11;
	return resto < 2 ? 0 : 11 - resto;
}

static int digito(char c)
{
	return c >= '0' && c <= '9';
}

static int todos_iguais(const char *s, int n)
{
	int i;

	for (i = 1; i < n; i++)
		if (s[i] != s[0])
			return 0;
	return 1;
}

int nfe_cnpj_validar(const char *cnpj)
{
	int i;

	if (!cnpj)
		return E_ISNULL;
	if (strlen(cnpj) != NFE_TAM_CNPJ)
		return E_TAMANHO;
	for (i = 0; i < NFE_TAM_CNPJ - 2; i++)
		if (!digito(cnpj[i]) && !(cnpj[i] >= 'A' && cnpj[i] <= 'Z'))
			return E_VALOR;
	if (!digito(cnpj[12]) || !digito(cnpj[13]))
		return E_VALOR;
	if (todos_iguais(cnpj, NFE_TAM_CNPJ))
		return E_VALOR;
	if (dv_mod11(cnpj, 12, 9) != cnpj[12] - '0' ||
	    dv_mod11(cnpj, 13, 9) != cnpj[13] - '0')
		return E_VALOR;
	return 0;
}

int nfe_cpf_validar(const char *cpf)
{
	int i;

	if (!cpf)
		return E_ISNULL;
	if (strlen(cpf) != NFE_TAM_CPF)
		return E_TAMANHO;
	for (i = 0; i < NFE_TAM_CPF; i++)
		if (!digito(cpf[i]))
			return E_VALOR;
	if (todos_iguais(cpf, NFE_TAM_CPF))
		return E_VALOR;
	/* CPF: pesos crescentes sem reinício (2 a 10 e 2 a 11) */
	if (dv_mod11(cpf, 9, 10) != cpf[9] - '0' ||
	    dv_mod11(cpf, 10, 11) != cpf[10] - '0')
		return E_VALOR;
	return 0;
}
