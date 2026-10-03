/* Copyright (c) 2017-2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with tooldoce.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * Grupo refNF (dentro de NFref): nota fiscal modelo 1 ou 1A referenciada.
 *
 * Os setters retornam 0 ou um código de erro (E_ISNULL, E_TAMANHO, E_VALOR)
 * e não mudam o campo quando o valor é recusado. Os getters devolvem o texto
 * como será escrito no XML ("" se ainda não informado) ou NULL se nf for
 * NULL.
 */

#ifndef LIBNFE_REFNF_H
#define LIBNFE_REFNF_H

#include <libxml/xmlwriter.h>

#include <libnfe/nfe.h>

struct refNF_s;

/* Cria um refNF vazio; NULL se faltar memória */
struct refNF_s *RefNFNew();
/* Libera o refNF (aceita NULL) */
void RefNFDel(struct refNF_s *nf);

/* cUF: código IBGE da UF do emitente */
int RefNFSetcUF(struct refNF_s *nf, nfe_uf uf);
const char *RefNFGetcUF(const struct refNF_s *nf);

/* AAMM: ano com dois dígitos (0 a 99) e mês da emissão */
int RefNFSetAAMM(struct refNF_s *nf, const int ano, nfe_mes mes);
const char *RefNFGetAAMM(const struct refNF_s *nf);

/* CNPJ do emitente, validado */
int RefNFSetCNPJ(struct refNF_s *nf, const char *cnpj);
const char *RefNFGetCNPJ(const struct refNF_s *nf);

/* mod: modelo do documento, dois caracteres ("01") */
int RefNFSetmod(struct refNF_s *nf, const char *mod);
const char *RefNFGetmod(const struct refNF_s *nf);

/* serie: de 1 a 3 caracteres */
int RefNFSetSerie(struct refNF_s *nf, const char *serie);
const char *RefNFGetSerie(const struct refNF_s *nf);

/* nNF: de 1 a 9 caracteres */
int RefNFSetnNF(struct refNF_s *nf, const char *nnf);
const char *RefNFGetnNF(const struct refNF_s *nf);

/* Escreve <refNF> com seus filhos; quem chama abre e fecha <NFref> */
int xmlGenRefNFNode(xmlTextWriterPtr writer, const struct refNF_s *nf);

#endif
