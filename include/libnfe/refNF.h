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

#ifndef LIBNFE_REFNF_H
#define LIBNFE_REFNF_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/nfe.h>

struct refNF_s;

/* Alocação de memória para os dados */
struct refNF_s *RefNFNew();
void RefNFDel(struct refNF_s *nf);

/* Funções de acesso ao dado
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro
 * o campo não é alterado. */
int RefNFSetcUF(struct refNF_s *nf, nfe_uf uf);
const char *RefNFGetcUF(const struct refNF_s *nf);

int RefNFSetAAMM(struct refNF_s *nf, const int ano, nfe_mes mes);
const char *RefNFGetAAMM(const struct refNF_s *nf);

int RefNFSetCNPJ(struct refNF_s *nf, const char *cnpj);
const char *RefNFGetCNPJ(const struct refNF_s *nf);

int RefNFSetmod(struct refNF_s *nf, const char *mod);
const char *RefNFGetmod(const struct refNF_s *nf);

int RefNFSetSerie(struct refNF_s *nf, const char *serie);
const char *RefNFGetSerie(const struct refNF_s *nf);

int RefNFSetnNF(struct refNF_s *nf, const char *nnf);
const char *RefNFGetnNF(const struct refNF_s *nf);

/* Funções de manipulação do xml  */

int xmlGenRefNFNode(xmlTextWriterPtr writer, const struct refNF_s *nf);

#endif
