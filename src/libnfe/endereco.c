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

#include<stdint.h>
#include"endereco.h"

/**
 * pais_s:
 * @cPais: Código do país
 * @xPais: Nome do país
 *
 * País
 */
struct pais_s{
	const char *xPais;
	uint16_t cPais;
};

/**
 * uf_s:
 * @cUF: Código IBGE da UF 
 * @xUF: Nome da UF
 * @pais: Pais
 *
 * Unidade federada
 */
struct uf_s{
	const char *xUF;
	uint8_t cUF;
	struct pais_s *pais;
};

/**
 * MUNICIPIO:
 * @xMun: Nome do municício 
 * @cMun: Código IBGE do municício 
 * @uf: UF
 *
 * Informação do Município
 */
struct municipio_s{
	const char *xMun;
	uint16_t cMun;
	struct uf_s *uf;
} ;

/**
 * ENDERECO:
 * @xLgr: Rua do endereço
 * @nro: Número do endereço na rua
 * @Cpl: Complemento do endereço
 * @xBairro: Bairro do endereço
 * @municipio: Município do endereço
 * @CEP: CEP do endereço
 *
 * Endereço
 */
typedef struct endereco_s{
	const char *xLgr;
	uint32_t nro;
	const char *Cpl;
	const char *xBairro;
	uint32_t CEP;
	struct municipio_s *municipio;
} ;
