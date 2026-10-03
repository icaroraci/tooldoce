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

/* Testes das notas referenciadas: refNF (modelo 1/1A) e refNFe (chave) */

#include <libnfe/erros.h>
#include <libnfe/refNF.h>
#include <libnfe/refNFe.h>

#include "teste.h"

#define CHAVE "35100812345678000195550010000000421123456781"

int main(void)
{
	struct refNF_s *nf = RefNFNew();
	struct refNFe_s *nfe = RefNFeNew();

	VERIFICA(nf != NULL);
	VERIFICA(nfe != NULL);

	/* refNF */
	VERIFICA_INT(RefNFSetcUF(nf, NFE_UF_SP), 0);
	VERIFICA_STR(RefNFGetcUF(nf), "35");
	VERIFICA_INT(RefNFSetcUF(nf, (nfe_uf)99), E_VALOR);
	VERIFICA_STR(RefNFGetcUF(nf), "35");

	VERIFICA_INT(RefNFSetAAMM(nf, 26, NFE_MES_MARCO), 0);
	VERIFICA_STR(RefNFGetAAMM(nf), "2603");
	VERIFICA_INT(RefNFSetAAMM(nf, 150, NFE_MES_MARCO), E_VALOR);
	VERIFICA_INT(RefNFSetAAMM(nf, 26, (nfe_mes)13), E_VALOR);
	VERIFICA_STR(RefNFGetAAMM(nf), "2603");

	VERIFICA_INT(RefNFSetCNPJ(nf, "12345678000195"), 0);
	VERIFICA_INT(RefNFSetCNPJ(nf, "123"), E_TAMANHO);
	VERIFICA_INT(RefNFSetCNPJ(nf, "123456780001950"), E_TAMANHO);
	/* dígito verificador errado; CNPJ alfanumérico aceito */
	VERIFICA_INT(RefNFSetCNPJ(nf, "12345678000199"), E_VALOR);
	VERIFICA_INT(RefNFSetCNPJ(nf, "12ABC34501DE35"), 0);
	VERIFICA_STR(RefNFGetCNPJ(nf), "12ABC34501DE35");
	VERIFICA_INT(RefNFSetCNPJ(nf, "12345678000195"), 0);
	VERIFICA_STR(RefNFGetCNPJ(nf), "12345678000195");

	VERIFICA_INT(RefNFSetmod(nf, "01"), 0);
	VERIFICA_INT(RefNFSetSerie(nf, "1"), 0);
	VERIFICA_INT(RefNFSetSerie(nf, "1234"), E_TAMANHO);
	VERIFICA_INT(RefNFSetnNF(nf, "123"), 0);
	VERIFICA_INT(RefNFSetnNF(nf, "1234567890"), E_TAMANHO);
	VERIFICA_INT(RefNFSetCNPJ(NULL, "12345678000195"), E_ISNULL);

	/* refNFe: a chave tem exatamente 44 caracteres */
	VERIFICA_INT(RefNFeSetrefNFe(nfe, CHAVE), 0);
	VERIFICA_STR(RefNFeGetrefNFe(nfe), CHAVE);
	VERIFICA_INT(RefNFeSetrefNFe(nfe, "351"), E_TAMANHO);
	/* dígito verificador errado */
	VERIFICA_INT(
	        RefNFeSetrefNFe(nfe,
	                        "35100812345678000195550010000000421123456782"),
	        E_VALOR);
	VERIFICA_INT(RefNFeSetrefNFe(nfe, CHAVE "0"), E_TAMANHO);
	VERIFICA_STR(RefNFeGetrefNFe(nfe), CHAVE);

	/* Objetos nulos */
	VERIFICA(RefNFGetcUF(NULL) == NULL);
	VERIFICA(RefNFeGetrefNFe(NULL) == NULL);
	VERIFICA_INT(xmlGenRefNFNode(NULL, nf), E_ISNULL);
	VERIFICA_INT(xmlGenRefNFeNode(NULL, nfe), E_ISNULL);

	RefNFDel(nf);
	RefNFeDel(nfe);
	TESTE_FIM();
}
