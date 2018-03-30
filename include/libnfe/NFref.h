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

#ifndef _H_NFREF_H_
#define _H_NFREF_H_

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

struct NFref_s;


/* Funções de alocação de memória */

struct NFref_s *NFrefNew();
void NFrefDel(struct NFref_s *nf);

/* Funções de acesso aos dados  */
void NFrefSetrefNFe(struct NFref_s *nf, char *ref);
char *NFrefGetrefNFe(struct NFref_s *nf);

/* Funções para tratamento do xml  */

int xmlGenNFrefNode(xmlTextWriterPtr writer, struct NFref_s *nf);
#endif
