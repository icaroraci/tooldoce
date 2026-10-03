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

/* Verificações mínimas para os testes: cada falha é contada e informada com
 * arquivo e linha; o programa termina com código 1 se houver alguma. */

#ifndef TOOLDOCE_TESTE_H
#define TOOLDOCE_TESTE_H

#include <stdio.h>
#include <string.h>

static int teste_falhas = 0;
static int teste_total = 0;

#define VERIFICA(cond) do { \
	teste_total++; \
	if (!(cond)) { \
		teste_falhas++; \
		fprintf(stderr, "%s:%d: FALHOU: %s\n", __FILE__, __LINE__, #cond); \
	} \
} while (0)

#define VERIFICA_INT(obtido, esperado) do { \
	long teste_o = (long)(obtido), teste_e = (long)(esperado); \
	teste_total++; \
	if (teste_o != teste_e) { \
		teste_falhas++; \
		fprintf(stderr, "%s:%d: FALHOU: %s == %ld (obtido %ld)\n", \
		        __FILE__, __LINE__, #obtido, teste_e, teste_o); \
	} \
} while (0)

#define VERIFICA_STR(obtido, esperado) do { \
	const char *teste_o = (obtido), *teste_e = (esperado); \
	teste_total++; \
	if (teste_o == NULL || strcmp(teste_o, teste_e) != 0) { \
		teste_falhas++; \
		fprintf(stderr, "%s:%d: FALHOU: %s == \"%s\" (obtido \"%s\")\n", \
		        __FILE__, __LINE__, #obtido, teste_e, \
		        teste_o ? teste_o : "(NULL)"); \
	} \
} while (0)

/* Encerra o teste com o resumo */
#define TESTE_FIM() do { \
	printf("%d verificações, %d falha(s)\n", teste_total, teste_falhas); \
	return teste_falhas ? 1 : 0; \
} while (0)

#endif
