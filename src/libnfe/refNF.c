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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/libnfe/defs.h"


struct refNF_s {
  char cUF[TAM_CUF];
  char AAMM[TAM_AAMM];
  char CNPJ[TAM_CNPJ];
  char mod[TAM_MOD_NF];
  char serie[TAM_SERIE];
  char nNF[TAM_NNF];
};

struct refNF_s *RefNFNew()
{
  struct refNF_s *nf = (struct refNF_s *)malloc(sizeof(struct refNF_s));
  return nf;
}

void RefNfDel(struct refNF_s *nf)
{
  free(nf);
}


void RefNFSetcUF(struct refNF_s *nf, enum UF_e uf)
{
  strcpy(nf->cUF, ConverteUFParaCodStr(uf));
}

char *RefNFGetcUF(struct refNF_s *nf)
{
  return nf->cUF;
}

void RefNFSetAAMM(struct refNF_s *nf, const int ano, enum MES_e mes)
{
  if (ano >= 0 && ano < 100)
    strcpy(nf->AAMM, ConverteAnoParaCodStr(ano));
  strcat(nf->AAMM, ConverteMesParaCodStr(mes));
}

char *RefNFGetAAMM(struct refNF_s *nf)
{
  return nf->AAMM;
}

void RefNFSetCNPJ(struct refNF_s *nf, const char *cnpj)
{
  strcpy(nf->CNPJ, cnpj);
}

char *RefNFGetCNPJ(struct refNF_s *nf)
{
  return nf->CNPJ;
}

void RefNFSetmod(struct refNF_s *nf, const char *mod)
{
  strcpy(nf->mod, mod);
}

char *RefNFGetmod(struct refNF_s *nf)
{
  return nf->mod;
}

void RefNFSetserie(struct refNF_s *nf, const char *serie)
{
  strcpy(nf->serie, serie);
}

char *RefNFGetserie(struct refNF_s *nf)
{
  return nf->serie;
}

void RefNFSetnNF(struct refNF_s *nf, const char *nnf)
{
  strcpy(nf->nNF, nnf);
}

char *RefNFGetnNF(struct refNF_s *nf)
{
  return nf->nNF;
}


int xmlGenRefNFNode(xmlTextWriterPtr writer, struct refNF_s *nf)
{
  int rc;
  
  rc = xmlTextWriterStartElement(writer, BAD_CAST "NFref");
  if (rc < 0){
    printf("NFref: Erro em xmlTextWriterStartElement\n");
    return -1;
  }
  rc = xmlTextWriterStartElement(writer, BAD_CAST "refNF");
  if (rc < 0){
    printf("refNF: Erro em xmlTextWriterStartElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
                                       "%s", getRefNFcUF(nf));
  if(rc < 0){
    printf("cUF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "AAMM",
                                       "%s", getRefNFAAMM(nf));
  if(rc < 0){
    printf("AAMM: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CNPJ",
                                       "%s", getRefNFCNPJ(nf));
  if(rc < 0){
    printf("CNPJ: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod",
                                       "%s", getRefNFMod(nf));
  if(rc < 0){
    printf("mod: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie",
                                       "%s", getRefNFSerie(nf));
  if(rc < 0){
    printf("serie: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF",
                                       "%s", getRefNFnNF(nf));
  if(rc < 0){
    printf("nNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0){
    printf("refNF: Erro em xmlTextWriterEndElement\n");
    return -1;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0){
    printf("NFref: Erro em xmlTextWriterEndElement\n");
    return -1;
  }

  return 0;

  
  
}

