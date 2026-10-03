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

/* Testes de nfe_copia_texto (cópia de texto com limite), nfe_strerror e
 * das contas com decimais (decimal.h) */

#include <libnfe/decimal.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/utils.h>

#include "teste.h"

/* Conversão de valores monetários para centavos e de volta */
static void teste_decimal(void)
{
	long long c = -1;
	char txt[32];

	VERIFICA_INT(nfe_dec2_ler("15", &c), 0);
	VERIFICA(c == 1500);
	VERIFICA_INT(nfe_dec2_ler("15.5", &c), 0);
	VERIFICA(c == 1550);
	VERIFICA_INT(nfe_dec2_ler("0.07", &c), 0);
	VERIFICA(c == 7);
	VERIFICA_INT(nfe_dec2_ler("", &c), 0);
	VERIFICA(c == 0);
	VERIFICA_INT(nfe_dec2_ler("9999999999999.99", &c), 0);
	VERIFICA(c == 999999999999999LL);
	VERIFICA_INT(nfe_dec2_ler("10000000000000.00", &c), E_VALOR);
	VERIFICA_INT(nfe_dec2_ler("1.234", &c), E_VALOR);
	VERIFICA_INT(nfe_dec2_ler("1,50", &c), E_VALOR);
	VERIFICA_INT(nfe_dec2_ler("15.", &c), E_VALOR);
	VERIFICA_INT(nfe_dec2_ler(".5", &c), E_VALOR);
	VERIFICA_INT(nfe_dec2_ler("1.2.3", &c), E_VALOR);
	VERIFICA_INT(nfe_dec2_ler("-1", &c), E_VALOR);
	VERIFICA_INT(nfe_dec2_ler(NULL, &c), E_ISNULL);

	VERIFICA_INT(nfe_dec2_escreve(1550, txt, sizeof txt), 0);
	VERIFICA_STR(txt, "15.50");
	VERIFICA_INT(nfe_dec2_escreve(7, txt, sizeof txt), 0);
	VERIFICA_STR(txt, "0.07");
	VERIFICA_INT(nfe_dec2_escreve(0, txt, sizeof txt), 0);
	VERIFICA_STR(txt, "0.00");
	VERIFICA_INT(nfe_dec2_escreve(-1, txt, sizeof txt), E_VALOR);
	VERIFICA_INT(nfe_dec2_escreve(1550, txt, 5), E_TAMANHO);
}

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
	VERIFICA_STR(nfe_strerror(E_XML),
	             "XML malformado ou falha ao gerar o XML");
	VERIFICA_STR(nfe_strerror(E_ARQUIVO), "falha ao gravar o arquivo");
	VERIFICA_STR(nfe_strerror(E_REDE), "falha na comunicação com a SEFAZ");
	VERIFICA_STR(nfe_strerror(12345), "erro desconhecido");

	teste_decimal();
	TESTE_FIM();
}
