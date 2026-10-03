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

#ifndef LIBNFE_REFNFE_H
#define LIBNFE_REFNFE_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

struct refNFe_s;

/* Funções de alocação de memória */

struct refNFe_s *RefNFeNew(void);
void RefNFeDel(struct refNFe_s *nf);

/* Funções de acesso aos dados  */
/* ref: chave de acesso com 44 dígitos; retorna 0, E_ISNULL ou E_TAMANHO */
int RefNFeSetrefNFe(struct refNFe_s *nf, const char *ref);
const char *RefNFeGetrefNFe(const struct refNFe_s *nf);

/* Funções para tratamento do xml  */

int xmlGenRefNFeNode(xmlTextWriterPtr writer, const struct refNFe_s *nf);
#endif
