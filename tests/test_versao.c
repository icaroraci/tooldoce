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

/* Testes da versão da biblioteca */

#include <stdio.h>

#include <libnfe/versao.h>

#include "teste.h"

int main(void)
{
	char esperado[32];

	snprintf(esperado, sizeof esperado, "%d.%d.%d", NFE_VERSAO_MAIOR,
	         NFE_VERSAO_MENOR, NFE_VERSAO_REVISAO);
	VERIFICA_STR(NFE_VERSAO, esperado);
	VERIFICA_STR(nfe_versao(), NFE_VERSAO);
	TESTE_FIM();
}
