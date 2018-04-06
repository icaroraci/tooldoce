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

#ifndef _H_REFNF_H_
#define _H_REFNF_H_

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>


struct refNF_s;

/* Alocação de memória para os dados */
struct refNF_s *RefNFNew();
void RefNfDel(struct refNF_s *nf);

/* Funções de acesso ao dado  */
void RefNFSetcUF(struct refNF_s *nf, enum UF_e uf);
char *RefNFGetcUF(struct refNF_s *nf);

void RefNFSetAAMM(struct refNF_s *nf, const int ano, enum MES_e mes);
char *RefNFGetAAMM(struct refNF_s *nf);

void RefNFSetCNPJ(struct refNF_s *nf, const char *cnpj);
char *RefNFGetCNPJ(struct refNF_s *nf);

void RefNFSetmod(struct refNF_s *nf, const char *mod);
char *RefNFGetmod(struct refNF_s *nf);

void RefNFSetSerie(struct refNF_s *nf, const char *serie);
char *RefNFGetSerie(struct refNF_s *nf);

void RefNFSetnNF(struct refNF_s *nf, const char *nnf);
char *RefNFGetnNF(struct refNF_s *nf);

/* Funções de manipulação do xml  */

int xmlGenRefNFNode(xmlTextWriterPtr writer, struct refNF_s *nf);

#endif
