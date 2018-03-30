/* TODO: Falta fazer o cabeçalho da documentação  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilidades.h"
#include "refNF.h"


struct refNF_s {
  char cUF[3];
  char AAMM[5];
  char CNPJ[15];
  char mod[03];
  char serie[4];
  char nNF[10];
};

struct refNF_s *newRefNF()
{
  struct refNF_s *nf = (struct refNF_s *)malloc(sizeof(struct refNF_s));
  return nf;
}

void delRefNf(struct refNF_s *nf)
{
  free(nf);
}


void setRefNFcUF(struct refNF_s *nf, enum UF_e uf)
{
  strcpy(nf->cUF, ConverteUFParaCodStr(uf));
}

char *getRefNFcUF(struct refNF_s *nf)
{
  return nf->cUF;
}

void setRefNFAAMM(struct refNF_s *nf, const int ano, enum MES_e mes)
{
  if (ano >= 0 && ano < 100)
    strcpy(nf->AAMM, ConverteAnoParaCodStr(ano));
  strcat(nf->AAMM, ConverteMesParaCodStr(mes));
}

char *getRefNFAAMM(struct refNF_s *nf)
{
  return nf->AAMM;
}

void setRefNFCNPJ(struct refNF_s *nf, const char *cnpj)
{
  strcpy(nf->CNPJ, cnpj);
}

char *getRefNFCNPJ(struct refNF_s *nf)
{
  return nf->CNPJ;
}

void setRefNFMod(struct refNF_s *nf, const char *mod)
{
  strcpy(nf->mod, mod);
}

char *getRefNFMod(struct refNF_s *nf)
{
  return nf->mod;
}

void setRefNFSerie(struct refNF_s *nf, const char *serie)
{
  strcpy(nf->serie, serie);
}

char *getRefNFSerie(struct refNF_s *nf)
{
  return nf->serie;
}

void setRefNFnNF(struct refNF_s *nf, const char *nnf)
{
  strcpy(nf->nNF, nnf);
}

char *getRefNFnNF(struct refNF_s *nf)
{
  return nf->nNF;
}

/* TODO: verificar os retornos dos testes. Atualmente eles retornam
 * um objeto writer e não para a execução do programa. Isto pode
 * gerar má formação futura do xml  */

xmlTextWriterPtr xmlGenRefNFNode(xmlTextWriterPtr writer, struct refNF_s *nf)
{
  int rc;
  
  rc = xmlTextWriterStartElement(writer, BAD_CAST "NFref");
  if (rc < 0){
    printf("NFref: Erro em xmlTextWriterStartElement\n");
    return writer;
  }
  rc = xmlTextWriterStartElement(writer, BAD_CAST "refNF");
  if (rc < 0){
    printf("refNF: Erro em xmlTextWriterStartElement\n");
    return writer;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
                                       "%s", getRefNFcUF(nf));
  if(rc < 0){
    printf("cUF: Erro em xmlTextWriterWriteFormatElement\n");
    return writer;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "AAMM",
                                       "%s", getRefNFAAMM(nf));
  if(rc < 0){
    printf("AAMM: Erro em xmlTextWriterWriteFormatElement\n");
    return writer;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CNPJ",
                                       "%s", getRefNFCNPJ(nf));
  if(rc < 0){
    printf("CNPJ: Erro em xmlTextWriterWriteFormatElement\n");
    return writer;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod",
                                       "%s", getRefNFMod(nf));
  if(rc < 0){
    printf("mod: Erro em xmlTextWriterWriteFormatElement\n");
    return writer;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie",
                                       "%s", getRefNFSerie(nf));
  if(rc < 0){
    printf("serie: Erro em xmlTextWriterWriteFormatElement\n");
    return writer;
  }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF",
                                       "%s", getRefNFnNF(nf));
  if(rc < 0){
    printf("nNF: Erro em xmlTextWriterWriteFormatElement\n");
    return writer;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0){
    printf("refNF: Erro em xmlTextWriterEndElement\n");
    return writer;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0){
    printf("NFref: Erro em xmlTextWriterEndElement\n");
    return writer;
  }

  return writer;

  
  
}

