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

#ifndef _H_IDE_H_
#define _H_IDE_H_

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

struct Cont_s;
struct ide_s;

/****************************************
** Funções para manipulação de memória **
****************************************/
// Struct Cont_s  

struct Cont_s *ideContNew(const struct Cont_s *this,
                          enum TZD_e tzd, enum TIPO_HVERAO_e hverao, 
                          const char *str, const char *xjust)
void ideContDel(const struct Cont_s *cont);

int xmlGenideContNode(xmlTextWriterPtr writer,struct Cont_s *cont);

// Struct ide_s
struct ide_s *ideNew(struct Cont_s *cont, enum UF_e cuf, uint32_t cnf, 
                     char *natop, uint8_t indpag, enum MOD_e mod, 
                     uint16_t serie, uint32_t nnf, char *dhemi, char *dhsaient,
                     enum TIPO_NF_e tpnf, enum TIPO_DESTINO_e iddest,
                     uint32_t cmunfg, enum TIPO_IMPRESSAO_e tpimp,
                     enum TIPO_EMISSAO_e tpemis, uint8_t cdv,
                     enum TIPO_AMBIENTE_e tpamb, enum TIPO_FINALIDADE_e finnfe,
                     enum TIPO_OP_e indfinal, enum TIPO_PRES_e indpres,        // 1 caractere
                     enum TIPO_PROC_EMIS_e procemis, char *verproc,
                     TZD_e tzd, TIPO_HVERAO_e hverao,
                     const char *str, struct ide_s *newide)
void ideDel(struct ide_s *ide);

#endif
