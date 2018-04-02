/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of LibDoce.
 *
 * LibDoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LibDoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LibDoce.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include<stdint.h>
#include<libnfe/defs.h>

struct retirada_entrega_s{
	CNPJouCPF CPF_CNPJ;
	struct endereco_s endereco;
}
