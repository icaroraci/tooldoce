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
#include <libnfe/erros.h>
#include <libnfe/utils.h>



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
  /* calloc: campos começam vazios; NULL se faltar memória */
  struct refNF_s *nf = (struct refNF_s *)calloc(1, sizeof(struct refNF_s));
  return nf;
}

void RefNFDel(struct refNF_s *nf)
{
  free(nf);
}


int RefNFSetcUF(struct refNF_s *nf, nfe_uf uf)
{
  if (!nf)
    return E_ISNULL;
  if (uf < 11 || uf > 53)
    return E_VALOR;
  snprintf(nf->cUF, sizeof nf->cUF, "%02d", (int)uf);
  return 0;
}

char *RefNFGetcUF(struct refNF_s *nf)
{
  return nf->cUF;
}

/* ano: dois dígitos (0 a 99) */
int RefNFSetAAMM(struct refNF_s *nf, const int ano, nfe_mes mes)
{
  if (!nf)
    return E_ISNULL;
  if (ano < 0 || ano > 99 ||
      mes < NFE_MES_JANEIRO || mes > NFE_MES_DEZEMBRO)
    return E_VALOR;
  snprintf(nf->AAMM, sizeof nf->AAMM, "%02d%02d", ano, (int)mes);
  return 0;
}

char *RefNFGetAAMM(struct refNF_s *nf)
{
  return nf->AAMM;
}

int RefNFSetCNPJ(struct refNF_s *nf, const char *cnpj)
{
  if (!nf)
    return E_ISNULL;
  return nfe_copia_texto(nf->CNPJ, sizeof nf->CNPJ, cnpj,
                         NFE_TAM_CNPJ, NFE_TAM_CNPJ);
}

char *RefNFGetCNPJ(struct refNF_s *nf)
{
  return nf->CNPJ;
}

int RefNFSetmod(struct refNF_s *nf, const char *mod)
{
  if (!nf)
    return E_ISNULL;
  return nfe_copia_texto(nf->mod, sizeof nf->mod, mod,
                         NFE_TAM_MOD, NFE_TAM_MOD);
}

char *RefNFGetmod(struct refNF_s *nf)
{
  return nf->mod;
}

int RefNFSetSerie(struct refNF_s *nf, const char *serie)
{
  if (!nf)
    return E_ISNULL;
  return nfe_copia_texto(nf->serie, sizeof nf->serie, serie,
                         1, NFE_TAM_SERIE);
}

char *RefNFGetSerie(struct refNF_s *nf)
{
  return nf->serie;
}

int RefNFSetnNF(struct refNF_s *nf, const char *nnf)
{
  if (!nf)
    return E_ISNULL;
  return nfe_copia_texto(nf->nNF, sizeof nf->nNF, nnf,
                         1, NFE_TAM_NNF);
}

char *RefNFGetnNF(struct refNF_s *nf)
{
  return nf->nNF;
}


/* Escreve o grupo <refNF>; o grupo <NFref> que o contém é aberto por quem
 * chama (nfe_ide_write_xml) */
int xmlGenRefNFNode(xmlTextWriterPtr writer, struct refNF_s *nf)
{
  int rc;
  
  rc = xmlTextWriterStartElement(writer, BAD_CAST "refNF");
  if (rc < 0){
    return E_XML;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
                                       "%s", nf->cUF);
  if(rc < 0){
    return E_XML;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "AAMM",
                                       "%s", nf->AAMM);
  if(rc < 0){
    return E_XML;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CNPJ",
                                       "%s", nf->CNPJ);
  if(rc < 0){
    return E_XML;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod",
                                       "%s", nf->mod);
  if(rc < 0){
    return E_XML;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie",
                                       "%s", nf->serie);
  if(rc < 0){
    return E_XML;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF",
                                       "%s", nf->nNF);
  if(rc < 0){
    return E_XML;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0){
    return E_XML;
  }

  return 0;

  
  
}

