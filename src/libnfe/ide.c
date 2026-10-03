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

/* gmtime_r (POSIX) */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/utils.h>
#include <libnfe/ide.h>
#include <libnfe/refNF.h>
#include <libnfe/refNFe.h>


struct Cont_s {
  char dhCont[NFE_TAM_ASCII(NFE_TAM_DATA_HORA)];   // Data e hora
  char xJust[NFE_TAM_UTF8(NFE_TAM_XJUST)];     // 256 caracteres
};


/* Documento referenciado: um item do grupo NFref */
enum tipo_ref_e {
  REF_NFE,  /* refNFe: NF-e ou NFC-e, pela chave de acesso */
  REF_NF    /* refNF: nota fiscal modelo 1/1A */
};

struct ref_s {
  enum tipo_ref_e tipo;
  union {
    struct refNFe_s *nfe;
    struct refNF_s *nf;
  } doc;
  struct ref_s *prox;
};

struct ide_s{
  nfe_uf cUF;              // 2 caracteres
  uint32_t cNF;                    // 8 caracteres 
  char natOp[NFE_TAM_UTF8(NFE_TAM_NATOP)];     // 60 caracteres
  nfe_modelo mod;             // 2 caracteres
  uint16_t serie;                  // 3 caracteres
  uint32_t nNF;                    // 9 caracteres
  char dhEmi[NFE_TAM_ASCII(NFE_TAM_DATA_HORA)];    // Data e hora
  char dhSaiEnt[NFE_TAM_ASCII(NFE_TAM_DATA_HORA)]; // Data e hora
  nfe_tipo_operacao tpNF;             // 1 caractere
  nfe_destino idDest;      // 1 caractere
  uint32_t cMunFG;                 // 7 caracteres
  nfe_danfe tpImp;     // 1 caractere
  nfe_emissao tpEmis;      // 1 caractere
  uint8_t cDV;                     // 1 caractere
  nfe_ambiente tpAmb;      // 1 caractere
  nfe_finalidade finNFe;   // 1 caractere
  nfe_consumidor indFinal;         // 1 caractere
  nfe_presenca indPres;        // 1 caractere
  nfe_processo_emissao procEmis;  // 1 caractere
  char verProc[NFE_TAM_UTF8(NFE_TAM_VERPROC)]; // 20 caracteres
  struct Cont_s *cont;             // Default NULL
  struct ref_s *refs;              // NFref, na ordem de inclusão
  struct ref_s *refsFim;
  int nRefs;
};
 
/* Funções auxiliares  */


/* Escreve em dst (tam bytes) o instante t no formato AAAA-MM-DDThh:mm:ssTZD,
 * no fuso tzd. Ex.: 2010-08-19T13:00:15-03:00.
 * Não há horário de verão no Brasil desde 2019.
 * Com t == NFE_SEM_DATA, grava texto vazio.
 * Retorna 0, E_VALOR (fuso inválido) ou E_TAMANHO (buffer pequeno).
 * */
static int DHSet(char *dst, size_t tam, time_t t, nfe_tzd tzd)
{
  struct tm tm;
  time_t local;
  size_t n;
  int r;

  switch (tzd){
    case NFE_TZD_FERNANDO_NORONHA:
    case NFE_TZD_BRASILIA:
    case NFE_TZD_MANAUS:
    case NFE_TZD_ACRE:
      break;
    default:
      return E_VALOR;
  }

  if (t == NFE_SEM_DATA){
    dst[0] = '\0';
    return 0;
  }

  /* Hora local = UTC deslocado pelo fuso; gmtime_r não depende do fuso
   * configurado na máquina */
  local = t + (time_t)tzd * 3600;
  if (!gmtime_r(&local, &tm))
    return E_VALOR;

  n = strftime(dst, tam, NFE_FORMATO_DATA_HORA, &tm);
  if (n == 0)
    return E_TAMANHO;

  r = snprintf(dst + n, tam - n, "-%02d:00", -(int)tzd);
  if (r < 0 || (size_t)r >= tam - n)
    return E_TAMANHO;

  return 0;
}

struct Cont_s *ideContNew(struct Cont_s *this,
                          time_t dhcont,
                          nfe_tzd tzd, 
                          const char *xjust)
{
  /* Reaproveita o objeto informado; se for NULL, aloca um novo */
  struct Cont_s *cont = this;
  if (!cont){
    cont = (struct Cont_s *)calloc(1, sizeof(struct Cont_s));
    if (!cont)
      return NULL;
  }
  if (DHSet(cont->dhCont, sizeof cont->dhCont, dhcont, tzd) != 0 ||
      nfe_copia_texto(cont->xJust, sizeof cont->xJust, xjust,
                      15, NFE_TAM_XJUST) != 0){
    if (!this)
      free(cont);
    return NULL;
  }
  return cont;
}

void ideContDel(struct Cont_s *cont)
{
  free(cont);
}

/* Escreve dhCont e xJust; no leiaute são filhos diretos de <ide>, sem
 * grupo próprio, logo após verProc */
int xmlGenideContNode(xmlTextWriterPtr writer,struct Cont_s *cont)
{
  int rc;
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhCont","%s", 
                                               cont->dhCont);
  if (rc < 0) {
    printf("ide->cont->dhCont: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xJust","%s", 
                                               cont->xJust);
  if (rc < 0) {
    printf("ide->cont->xJust: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  return 0;
}



struct ide_s *ideNew(struct ide_s *this, 
                     nfe_uf cuf, 
                     uint32_t cnf, 
                     char *natop, 
                     nfe_modelo mod, 
                     uint16_t serie, 
                     uint32_t nnf, 
                     time_t dhemi, 
                     time_t dhsaient,
                     nfe_tipo_operacao tpnf, 
                     nfe_destino iddest,
                     uint32_t cmunfg, 
                     nfe_danfe tpimp,
                     nfe_emissao tpemis, 
                     uint8_t cdv,
                     nfe_ambiente tpamb, 
                     nfe_finalidade finnfe,
                     nfe_consumidor indfinal, 
                     nfe_presenca indpres,
                     nfe_processo_emissao procemis, 
                     char *verproc,
                     struct Cont_s *cont,
                     nfe_tzd tzd )
{
  /* Reaproveita o objeto informado; se for NULL, aloca um novo */
  struct ide_s *ide = this;
  if (!ide){
    ide = (struct ide_s *)calloc(1, sizeof(struct ide_s));
    if (!ide)
      return NULL;
  }
  ide->cUF = cuf;
  ide->cNF = cnf;
  ide->mod = mod;
  ide->serie = serie;
  ide->nNF = nnf;
  if (DHSet(ide->dhEmi, sizeof ide->dhEmi, dhemi, tzd) != 0 ||
      DHSet(ide->dhSaiEnt, sizeof ide->dhSaiEnt, dhsaient, tzd) != 0 ||
      nfe_copia_texto(ide->natOp, sizeof ide->natOp, natop,
                      1, NFE_TAM_NATOP) != 0 ||
      nfe_copia_texto(ide->verProc, sizeof ide->verProc, verproc,
                      1, NFE_TAM_VERPROC) != 0){
    if (!this)
      free(ide);
    return NULL;
  }
  ide->tpNF = tpnf;
  ide->idDest = iddest;
  ide->cMunFG = cmunfg;
  ide->tpImp = tpimp;
  ide->tpEmis = tpemis;
  ide->cDV = cdv;
  ide->tpAmb = tpamb;
  ide->finNFe = finnfe;
  ide->indFinal = indfinal;
  ide->indPres = indpres;
  ide->procEmis = procemis;
  ide->cont = cont;
  return ide;
}

void ideDel(struct ide_s *ide)
{
  if (!ide)
    return;

  if (ide->cont)
    ideContDel(ide->cont);

  while (ide->refs) {
    struct ref_s *prox = ide->refs->prox;
    if (ide->refs->tipo == REF_NFE)
      RefNFeDel(ide->refs->doc.nfe);
    else
      RefNFDel(ide->refs->doc.nf);
    free(ide->refs);
    ide->refs = prox;
  }

  free(ide); 
}

/* Acrescenta uma referência ao fim da lista */
static int ideAddRef(struct ide_s *ide, struct ref_s *novo)
{
  if (ide->nRefs >= NFE_MAX_NFREF)
    return E_VALOR;
  novo->prox = NULL;
  if (ide->refsFim)
    ide->refsFim->prox = novo;
  else
    ide->refs = novo;
  ide->refsFim = novo;
  ide->nRefs++;
  return 0;
}

int ideAddRefNFe(struct ide_s *ide, struct refNFe_s *ref)
{
  struct ref_s *novo;
  int rc;

  if (!ide || !ref)
    return E_ISNULL;
  novo = (struct ref_s *)malloc(sizeof(struct ref_s));
  if (!novo)
    return E_MALLOC;
  novo->tipo = REF_NFE;
  novo->doc.nfe = ref;
  rc = ideAddRef(ide, novo);
  if (rc != 0)
    free(novo);
  return rc;
}

int ideAddRefNF(struct ide_s *ide, struct refNF_s *ref)
{
  struct ref_s *novo;
  int rc;

  if (!ide || !ref)
    return E_ISNULL;
  novo = (struct ref_s *)malloc(sizeof(struct ref_s));
  if (!novo)
    return E_MALLOC;
  novo->tipo = REF_NF;
  novo->doc.nf = ref;
  rc = ideAddRef(ide, novo);
  if (rc != 0)
    free(novo);
  return rc;
}

int xmlGenideNode(xmlTextWriterPtr writer,struct ide_s *ide)
{
  int rc;
  struct ref_s *ref;
  rc = xmlTextWriterStartElement(writer, BAD_CAST "ide");
  if (rc < 0) {
    printf("ide-: Erro em xmlTextWriterStartElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF","%02d", 
                                               (int)ide->cUF);
  if (rc < 0) {
    printf("ide->cUF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cNF","%08" PRIu32, 
                                               ide->cNF);
  if (rc < 0) {
    printf("ide->cNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "natOp","%s", 
                                               ide->natOp);
  if (rc < 0) {
    printf("ide->natOp: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod","%02d", 
                                               (int)ide->mod);
  if (rc < 0) {
    printf("ide->mod: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie","%u", 
                                               (unsigned)ide->serie);
  if (rc < 0) {
    printf("ide->serie: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF","%" PRIu32, 
                                               ide->nNF);
  if (rc < 0) {
    printf("ide->nNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhEmi","%s", 
                                               ide->dhEmi);
  if (rc < 0) {
    printf("ide->dhEmi: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  /* dhSaiEnt é opcional */
  if (ide->dhSaiEnt[0] != '\0') {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhSaiEnt","%s", 
                                                 ide->dhSaiEnt);
    if (rc < 0) {
      printf("ide->dhSaiEnt: Erro em xmlTextWriterWriteFormatElement\n");
      return -1;
    }
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpNF","%d", 
                                               (int)ide->tpNF);
  if (rc < 0) {
    printf("ide->tpNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "idDest","%d", 
                                               (int)ide->idDest);
  if (rc < 0) {
    printf("ide->idDest: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cMunFG","%07" PRIu32, 
                                               ide->cMunFG);
  if (rc < 0) {
    printf("ide->cMunFG: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpImp","%d", 
                                               (int)ide->tpImp);
  if (rc < 0) {
    printf("ide->tpImp: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpEmis","%d", 
                                               (int)ide->tpEmis);
  if (rc < 0) {
    printf("ide->tpEmis: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cDV","%u", 
                                               (unsigned)ide->cDV);
  if (rc < 0) {
    printf("ide->cDV: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpAmb","%d", 
                                               (int)ide->tpAmb);
  if (rc < 0) {
    printf("ide->tpAmb: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "finNFe","%d", 
                                               (int)ide->finNFe);
  if (rc < 0) {
    printf("ide->finNFe: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indFinal","%d", 
                                               (int)ide->indFinal);
  if (rc < 0) {
    printf("ide->indFinal: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indPres","%d", 
                                               (int)ide->indPres);
  if (rc < 0) {
    printf("ide->indPres: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "procEmis","%d", 
                                               (int)ide->procEmis);
  if (rc < 0) {
    printf("ide->procEmis: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "verProc","%s", 
                                               ide->verProc);
  if (rc < 0) {
    printf("ide->verProc: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  if(ide->cont){
    rc = xmlGenideContNode(writer, ide->cont);
    if (rc < 0)
      return rc;
  }

  /* NFref: um grupo para cada documento referenciado */
  for (ref = ide->refs; ref; ref = ref->prox) {
    rc = xmlTextWriterStartElement(writer, BAD_CAST "NFref");
    if (rc < 0) {
      printf("ide->NFref: Erro em xmlTextWriterStartElement\n");
      return -1;
    }
    if (ref->tipo == REF_NFE)
      rc = xmlGenRefNFeNode(writer, ref->doc.nfe);
    else
      rc = xmlGenRefNFNode(writer, ref->doc.nf);
    if (rc < 0)
      return rc;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
      printf("ide->NFref: Erro em xmlTextWriterEndElement\n");
      return -1;
    }
  }

  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) {
    printf("ide: Erro em xmlTextWriterEndElement\n");
    return -1;
  }

  return 0;
}

