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

/* Testes da validação de CNPJ (numérico e alfanumérico) e CPF */

#include <libnfe/cnpjcpf.h>
#include <libnfe/erros.h>

#include "teste.h"

int main(void)
{
	/* CNPJ alfanumérico: exemplo oficial da Receita (12.ABC.345/01DE-35) */
	VERIFICA_INT(nfe_cnpj_validar("12ABC34501DE35"), 0);
	VERIFICA_INT(nfe_cnpj_validar("12ABC34501DE36"), E_VALOR);
	VERIFICA_INT(nfe_cnpj_validar("12abc34501de35"), E_VALOR);
	VERIFICA_INT(nfe_cnpj_validar("12ABC34501DEA5"), E_VALOR);

	/* CNPJ numérico */
	VERIFICA_INT(nfe_cnpj_validar("12345678000195"), 0);
	VERIFICA_INT(nfe_cnpj_validar("12345678000199"), E_VALOR);
	VERIFICA_INT(nfe_cnpj_validar("00000000000000"), E_VALOR);
	VERIFICA_INT(nfe_cnpj_validar("1234567800019"), E_TAMANHO);
	VERIFICA_INT(nfe_cnpj_validar("12.345.678/0001-95"), E_TAMANHO);
	VERIFICA_INT(nfe_cnpj_validar(NULL), E_ISNULL);

	/* CPF */
	VERIFICA_INT(nfe_cpf_validar("12345678909"), 0);
	VERIFICA_INT(nfe_cpf_validar("12345678900"), E_VALOR);
	VERIFICA_INT(nfe_cpf_validar("11111111111"), E_VALOR);
	VERIFICA_INT(nfe_cpf_validar("1234567890A"), E_VALOR);
	VERIFICA_INT(nfe_cpf_validar("1234567890"), E_TAMANHO);
	VERIFICA_INT(nfe_cpf_validar(NULL), E_ISNULL);

	TESTE_FIM();
}
