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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <libnfe/defs.h>
#include <libnfe/ide.h>


struct Cont_s {
  char *dhCont;                    // Data
  char xJust[NFE_TAM_UTF8(NFE_TAM_XJUST)];     // 256 caracteres
};


struct ide_s{
  nfe_uf cUF;              // 2 caracteres
  uint32_t cNF;                    // 8 caracteres 
  char natOp[NFE_TAM_UTF8(NFE_TAM_NATOP)];     // 60 caracteres
  nfe_forma_pagamento indPag;                  // 1 caractere
  nfe_modelo mod;             // 2 caracteres
  uint16_t serie;                  // 3 caracteres
  uint32_t nNF;                    // 9 caracteres
  char *dhEmi;                     // Data
  char *dhSaiEnt;                  // Data
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
};
 
/* Funções auxiliares  */


/*Data e hora do evento no formato AAAA-MM-DDThh:mm:ssTZD (UTC - 
 * Universal Coordinated Time), onde TZD pode ser 
 * -02:00 (Fernando de Noronha), 
 * -03:00 (Brasília), 
 * -04:00 (Manaus) ou 
 * -05:00 (Acre). Ex.:
 * 2010-08-19T13:00:15-03:00.
 * Não há horário de verão no Brasil desde 2019.
 * */
static char *DHSet(nfe_tzd tzd, const char *str)
{
  char *aux;
  switch (tzd){
    case NFE_TZD_FERNANDO_NORONHA: 
      aux = "-02:00";
      break;
    case NFE_TZD_BRASILIA:
    default:    
      aux = "-03:00";
      break;
    case NFE_TZD_MANAUS:
      aux = "-04:00";
      break;
    case NFE_TZD_ACRE:
      aux = "-05:00";
      break;
  }
  strcat(str,NFE_FORMATO_DATA_HORA); // Vc alocou espaço para *str?
  return strcat(str,aux);
}
/* tzd = fuso horário (nfe_tzd)
 * str = endereço de uma string
 * xJust = justificativa (até 256 caracteres)
 * newcont = rerencia 
*/

struct Cont_s *ideContNew(const struct Cont_s *this,
                          nfe_tzd tzd, 
                          const char *str, 
                          const char *xjust)
{
  if(!this) 
  { 
    strcpy(this->dhCont, DHSet(tzd, str));
    strcpy(this->xJust, xjust);
    return this;
  }
  else
  {
    struct Cont_s *cont = (struct Cont_s *)malloc(sizeof(struct Cont_s));
    strcpy(cont->dhCont, DHSet(tzd, str));
    strcpy(cont->xJust, xjust);
    return cont;
  }
}

void ideContDel(const struct Cont_s *cont)
{
  free(cont);
}

int xmlGenideContNode(xmlTextWriterPtr writer,struct Cont_s *cont)
{
  int rc;
  rc = xmlTextWriterStartElement(writer, BAD_CAST "-x-");
  if (rc < 0) {
    printf("ide--x-: Erro em xmlTextWriterStartElement\n");
    return -1;
  }
  
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


  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) {
    printf("ide--x-: Erro em xmlTextWriterEndElement\n");
    return -1;
  }

  return 0;
}



struct ide_s *ideNew(struct ide_s *this, 
                     nfe_uf cuf, 
                     uint32_t cnf, 
                     char *natop, 
                     nfe_forma_pagamento indpag, 
                     nfe_modelo mod, 
                     uint16_t serie, 
                     uint32_t nnf, 
                     char *dhemi, 
                     char *dhsaient,
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
                     nfe_tzd tzd, 
                     const char *str )
{
  if(!this)
  {
    this->cUF = cuf;
    this->cNF = cnf;
    strcpy(this->natOp, natop);
    this->indPag = indpag;
    this->mod = mod;
    this->serir = serie;
    this->nNF = nnf;
    strcpy(this->dhEmi, DHSet(tzd, str)); // precisa rever isso
    strcpy(this->dhSaiEnt, DHSet(tzd, str));
    this->tpNF = tpnf;
    this->ideDest = idedest;
    this->cMunFG = cmunfg;
    this->tpImp = tpimp;
    this->tpEmis = tpemis;
    this->cDV = cdv;
    this->tpAmb = tpamb;
    this->finNFe = finnfe;
    this->indFinal = indfinal;
    this->indPres = indpres;
    this->proEmis = procemis;
    strcpy(this->verProc, verproc);
    this->cont = cont;
    return this;
  }
  else
 {
    struct ide_s *ide = (struct ide_s *)malloc(sizeof(struct ide_s));
    ide->cUF = cuf;
    ide->cNF = cnf;
    strcpy(ide->natOp, natop);
    ide->indPag = indpag;
    ide->mod = mod;
    ide->serir = serie;
    ide->nNF = nnf;
    strcpy(ide->dhEmi, DHSet(tzd, str)); // precisa rever isso
    strcpy(ide->dhSaiEnt, DHSet(tzd, str));
    ide->tpNF = tpnf;
    ide->ideDest = idedest;
    ide->cMunFG = cmunfg;
    ide->tpImp = tpimp;
    ide->tpEmis = tpemis;
    ide->cDV = cdv;
    ide->tpAmb = tpamb;
    ide->finNFe = finnfe;
    ide->indFinal = indfinal;
    ide->indPres = indpres;
    ide->proEmis = procemis;
    strcpy(ide->verProc, verproc);
    ide->cont = cont;
    ide->newide = newide; 
    return ide;
 }
  
}

void ideDel(struct ide_s *ide)
{
  if(!ide->cont)
    ideContDel(ide->cont);

  free(ide); 
}

int xmlGenideNode(xmlTextWriterPtr writer,struct ide_s *ide)
{
  int rc;
  rc = xmlTextWriterStartElement(writer, BAD_CAST "ide");
  if (rc < 0) {
    printf("ide-: Erro em xmlTextWriterStartElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF","%02u", 
                                               ide->cUF);
  if (rc < 0) {
    printf("ide->cUF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cNF","%08lu", 
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

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indPag","%1u", 
                                               ide->indPag);
  if (rc < 0) {
    printf("ide->indPag: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod","%02u", 
                                               ide->mod);
  if (rc < 0) {
    printf("ide->mod: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie","%3u", 
                                               ide->serie);
  if (rc < 0) {
    printf("ide->serie: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF","%9lu", 
                                               ide->cNF);
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

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhSaiEnt","%s", 
                                               ide->dhSaiEnt);
  if (rc < 0) {
    printf("ide->dhSaiEnt: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpNF","%1u", 
                                               ide->tpNF);
  if (rc < 0) {
    printf("ide->tpNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "idDest","%1u", 
                                               ide->idDest);
  if (rc < 0) {
    printf("ide->idDest: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cMunFG","%07lu", 
                                               ide->cMunFG);
  if (rc < 0) {
    printf("ide->cMunFG: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpImp","%u", 
                                               ide->tpImp);
  if (rc < 0) {
    printf("ide->tpImp: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpEmis","%u", 
                                               ide->tpEmis);
  if (rc < 0) {
    printf("ide->tpEmis: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cDV","%u", 
                                               ide->cDV);
  if (rc < 0) {
    printf("ide->cDV: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpAmb","%u", 
                                               ide->tpAmb);
  if (rc < 0) {
    printf("ide->tpAmb: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "finNFe","%u", 
                                               ide->finNFe);
  if (rc < 0) {
    printf("ide->finNFe: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indFinal","%u", 
                                               ide->indFinal);
  if (rc < 0) {
    printf("ide->indFinal: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indPres","%u", 
                                               ide->indPres);
  if (rc < 0) {
    printf("ide->indPres: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "procEmis","%u", 
                                               ide->procEmis);
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

  if(!ide->cont)
     rc = xmlGenideContNode(writer, ide->cont);

  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) {
    printf("ide: Erro em xmlTextWriterEndElement\n");
    return -1;
  }

  return 0;
}

