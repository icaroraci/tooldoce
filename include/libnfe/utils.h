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

#ifndef LIBNFE_UTILS_H
#define LIBNFE_UTILS_H

#include<stddef.h>

#include<libnfe/erros.h>

/* Funções de uso interno da biblioteca: não são exportadas na libnfe.so */
#if defined(__GNUC__) && __GNUC__ >= 4
#define NFE_INTERNO __attribute__((visibility("hidden")))
#else
#define NFE_INTERNO
#endif

/* Retorna E_ISNULL se ptr for nulo, ou 0 */
NFE_INTERNO int nfe_ptrnull(const void *ptr);

/* Copia o texto src para dst (tam bytes, incluindo o terminador).
 * min e max são os limites do campo em caracteres UTF-8 (max == 0: sem
 * limite além do buffer). Em caso de erro dst não é alterado.
 * Retorna 0, E_ISNULL (dst ou src NULL) ou E_TAMANHO. */
NFE_INTERNO int nfe_copia_texto(char *dst, size_t tam, const char *src,
                                size_t min, size_t max);
#endif
