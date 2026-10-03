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

/* Testes do endereço */

#include <libnfe/erros.h>
#include <libnfe/endereco.h>

#include "teste.h"

int main(void)
{
	char buf[64];
	char longo[62];
	Endereco *e = NewEndereco();

	VERIFICA(e != NULL);

	/* Campos começam vazios */
	VERIFICA_STR(GetLgr(e), "");
	VERIFICA_INT(GetCEP(e), 0);

	/* O texto é copiado: alterar o original não muda o endereço */
	strcpy(buf, "Rua das Flores");
	VERIFICA_INT(SetLgr(e, buf), 0);
	strcpy(buf, "XXXX");
	VERIFICA_STR(GetLgr(e), "Rua das Flores");

	VERIFICA_INT(SetNro(e, "123"), 0);
	VERIFICA_INT(SetCpl(e, "Apto 4"), 0);
	VERIFICA_INT(SetBairro(e, "Centro"), 0);
	VERIFICA_STR(GetNro(e), "123");
	VERIFICA_STR(GetCpl(e), "Apto 4");
	VERIFICA_STR(GetBairro(e), "Centro");

	/* Limites do leiaute: xLgr de 2 a 60 caracteres */
	memset(longo, 'a', sizeof longo - 1);
	longo[sizeof longo - 1] = '\0';
	VERIFICA_INT(SetLgr(e, longo), E_TAMANHO);
	VERIFICA_INT(SetLgr(e, "R"), E_TAMANHO);
	VERIFICA_STR(GetLgr(e), "Rua das Flores");

	/* CEP e telefone */
	VERIFICA_INT(SetCEP(e, 1001000), 0);
	VERIFICA_INT(GetCEP(e), 1001000);
	VERIFICA_INT(SetFone(e, 1133334444ULL), 0);
	VERIFICA(GetFone(e) == 1133334444ULL);

	/* Município: o próprio município pode ser informado de novo */
	VERIFICA(GetMunicipio(e) != NULL);
	VERIFICA_INT(SetMunicipio(e, GetMunicipio(e)), 0);

	DelEndereco(e);
	TESTE_FIM();
}
