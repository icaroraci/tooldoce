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

#include<libnfe/erros.h>

/* Funções de uso interno da biblioteca: não são exportadas na libnfe.so */
#if defined(__GNUC__) && __GNUC__ >= 4
#define NFE_INTERNO __attribute__((visibility("hidden")))
#else
#define NFE_INTERNO
#endif

NFE_INTERNO int nfe_error(const char *msg, int codErro);
NFE_INTERNO int nfe_ptrnull(const void *ptr);
#endif
