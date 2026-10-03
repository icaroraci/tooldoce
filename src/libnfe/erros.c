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

#include <libnfe/erros.h>

const char *nfe_strerror(int codigo)
{
	switch (codigo) {
	case 0:
		return "sucesso";
	case E_ISNULL:
		return "ponteiro nulo";
	case E_TAMANHO:
		return "texto fora dos limites do campo";
	case E_VALOR:
		return "valor fora da faixa permitida";
	case E_XML:
		return "XML malformado ou falha ao gerar o XML";
	case E_ARQUIVO:
		return "falha ao gravar o arquivo";
	case E_REDE:
		return "falha na comunicação com a SEFAZ";
	case E_MALLOC:
		return "falta de memória";
	default:
		return "erro desconhecido";
	}
}
