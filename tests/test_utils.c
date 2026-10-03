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

/* Testes de nfe_copia_texto (cópia de texto com limite) e nfe_strerror */

#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/utils.h>

#include "teste.h"

int main(void)
{
	char buf[NFE_TAM_UTF8(5)];
	char grande[NFE_TAM_UTF8(60)];
	char texto[256];
	int i;

	strcpy(buf, "orig");

	/* Acentos contam como um caractere cada */
	VERIFICA_INT(nfe_copia_texto(buf, sizeof buf, "ação", 1, 5), 0);
	VERIFICA_STR(buf, "ação");

	/* Fora dos limites: recusa sem alterar o destino */
	VERIFICA_INT(nfe_copia_texto(buf, sizeof buf, "abcdef", 1, 5),
	             E_TAMANHO);
	VERIFICA_INT(nfe_copia_texto(buf, sizeof buf, "", 1, 5), E_TAMANHO);
	VERIFICA_STR(buf, "ação");

	/* Ponteiros nulos */
	VERIFICA_INT(nfe_copia_texto(buf, sizeof buf, NULL, 1, 5), E_ISNULL);
	VERIFICA_INT(nfe_copia_texto(NULL, sizeof buf, "a", 1, 5), E_ISNULL);

	/* Texto que não cabe no buffer, mesmo sem limite de caracteres */
	VERIFICA_INT(nfe_copia_texto(buf, 4, "abcd", 0, 0), E_TAMANHO);
	VERIFICA_INT(nfe_copia_texto(buf, 4, "abc", 0, 0), 0);

	/* 60 caracteres de 2 bytes cabem; 61 não */
	texto[0] = '\0';
	for (i = 0; i < 60; i++)
		strcat(texto, "ç");
	VERIFICA_INT(nfe_copia_texto(grande, sizeof grande, texto, 2, 60), 0);
	VERIFICA_INT((long)strlen(grande), 120);
	strcat(texto, "ç");
	VERIFICA_INT(nfe_copia_texto(grande, sizeof grande, texto, 2, 60),
	             E_TAMANHO);

	/* Descrição dos códigos de erro */
	VERIFICA_STR(nfe_strerror(0), "sucesso");
	VERIFICA_STR(nfe_strerror(E_TAMANHO),
	             "texto fora dos limites do campo");
	VERIFICA_STR(nfe_strerror(E_XML), "falha ao escrever o XML");
	VERIFICA_STR(nfe_strerror(12345), "erro desconhecido");

	TESTE_FIM();
}
