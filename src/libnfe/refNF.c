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


#include <libnfe/refNF.h>


#include <libnfe/defs.h>



struct refNF_s {
  char cUF[NFE_TAM_ASCII(NFE_TAM_CUF)];
  char AAMM[NFE_TAM_ASCII(NFE_TAM_AAMM)];
  char CNPJ[NFE_TAM_ASCII(NFE_TAM_CNPJ)];
  char mod[NFE_TAM_ASCII(NFE_TAM_MOD)];
  char serie[NFE_TAM_ASCII(NFE_TAM_SERIE)];
  char nNF[NFE_TAM_ASCII(NFE_TAM_NNF)];
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


void RefNFSetcUF(struct refNF_s *nf, nfe_uf uf)
{
  snprintf(nf->cUF, sizeof nf->cUF, "%02d", (int)uf);
}

char *RefNFGetcUF(struct refNF_s *nf)
{
  return nf->cUF;
}

/* ano: dois dígitos (0 a 99); valores fora da faixa são ignorados */
void RefNFSetAAMM(struct refNF_s *nf, const int ano, nfe_mes mes)
{
  if (ano >= 0 && ano < 100 &&
      mes >= NFE_MES_JANEIRO && mes <= NFE_MES_DEZEMBRO)
    snprintf(nf->AAMM, sizeof nf->AAMM, "%02d%02d", ano, (int)mes);
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
                                       "%s", nf->cUF);
  if(rc < 0){
    printf("cUF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "AAMM",
                                       "%s", nf->AAMM);
  if(rc < 0){
    printf("AAMM: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CNPJ",
                                       "%s", nf->CNPJ);
  if(rc < 0){
    printf("CNPJ: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod",
                                       "%s", nf->mod);
  if(rc < 0){
    printf("mod: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie",
                                       "%s", nf->serie);
  if(rc < 0){
    printf("serie: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF",
                                       "%s", nf->nNF);
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

