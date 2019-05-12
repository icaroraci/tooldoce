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

#ifndef INCLUIDO_DEFS_H
#define INCLUIDO_DEFS_H

#define TAM_CHAVE      45
#define TAM_CUF         3
#define TAM_AAMM        5
#define TAM_CNPJ       15
#define TAM_NNF        10
#define TAM_SERIE       4
#define TAM_MOD_NF      3
#define TAM_CPF        12
#define TAM_IE         15
#define TAM_NECF        4
#define TAM_NCOO        7
#define TAM_RAZAO      62
#define TAM_NATOP           62
#define TAM_VERSAO_APLIC    21

#define NFE_strlen(a) (a == NULL ? 0 : strlen(a))
#define NFE_free  (a) (a == NULL ? NULL : free(a))
#define NFE_memcpy(a, b) (a == NULL  || b == NULL ? NULL : memcpy(a, b))
#define NFE_strdup(a) (a == NULL ? NULL : strdup(a))
#define NFE_null  (a) (a == NULL ? E_ISNULL : 0)
#define GetCEP    (a) (NFE_null(a) == E_ISNULL ? 0 : a->CEP)
#define SetCEP    (a, b) (NFE_null(a) == E_ISNULL ? 0 : a->CEP = b)
#define SetFone   (a, b) (NFE_null(a) == E_ISNULL ? 0 : a->fone = b)
#define GetFone   (a) (NFE_null(a) == E_ISNULL ? 0 : a->fone)


#endif


