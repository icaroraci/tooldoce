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
#include "include/libnfe/defs.h"
#include "ide.h"

#define DHDEFAULT "%Y-%m-%d-T-%H:%M:%S-"

struct Cont_s {
  char *dhCont;                    // Data
  char xJust[TAM_JUSTIFICATIVA];   // 256 caracteres
};


struct ide_s{
  enum TIPO_UF_e cUF;              // 2 caracteres
  uint32_t cNF;                    // 8 caracteres 
  char natOp[TAM_NATOP];           // 61 caracteres
  enum TIPO_PGTO_e indPag;                  // 1 caractere
  enum TIPO_MOD_e mod;             // 2 caracteres
  uint16_t serie;                  // 3 caracteres
  uint32_t nNF;                    // 9 caracteres
  char *dhEmi;                     // Data
  char *dhSaiEnt;                  // Data
  enum TIPO_NF_e tpNF;             // 1 caractere
  enum TIPO_DESTINO_e idDest;      // 1 caractere
  uint32_t cMunFG;                 // 7 caracteres
  enum TIPO_IMPRESSAO_e tpImp;     // 1 caractere
  enum TIPO_EMISSAO_e tpEmis;      // 1 caractere
  uint8_t cDV;                     // 1 caractere
  enum TIPO_AMBIENTE_e tpAmb;      // 1 caractere
  enum TIPO_FINALIDADE_e finNFe;   // 1 caractere
  enum TIPO_OP_e indFinal;         // 1 caractere
  enum TIPO_PRES_e indPres;        // 1 caractere
  enum TIPO_PROC_EMIS_e procEmis;  // 1 caractere
  char verProc[TAM_VERSAO_APLIC];  // 20 caracteres
  struct Cont_s *cont;             // Default NULL
};
 
/* Funções auxiliares  */

// TODO: Definir enum TIPO_HVERAO_e { SIM, NAO};
//       Definir enum TZD_e {DEFAULT,FERNANDO_DE_NORONHA, BRASILIA, MANAUS}

/*Data e hora do evento no formato AAAA-MM-DDThh:mm:ssTZD (UTC - 
 * Universal Coordinated Time , onde TZD pode ser 
 * -02:00 (Fernando de Noronha), 
 * -03:00 (Brasília) ou 
 * -04:00 (Manaus), no  horário de verão serão 
 * -01:00, -02:00 e -03:00. Ex.:
 * 2010-08-19T13:00:15-03:00.
 * */
static char *DHSet(enum TZD_e tzd, enum TIPO_HVERAO_e hverao,
                   const char *str)
{
  char *aux;
  if (hverao == NAO){
    switch (tzd){
      case FERNANDO_DE_NORONHA: 
        aux = "2:00";
        break;
      case BRASILIA:
      default:    
        aux = "3:00";
        break;
      case MANAUS:
        aux = "4:00";
        break;
    }
  }else if(hverao == SIM){
    switch (tzd){
      case FERNANDO_DE_NORONHA:
        aux = "1:00";
        break;
      case BRASILIA:
      default:    
        aux = "2:00";
        break;
      case MANAUS:
        aux = "3:00";
        break;
    }
  }
  strcat(str,DHDEFAULT); // Vc alocou espaço para *str?
  return strcat(str,aux);
}
/* tzd = DEFAULT, FERNANDO_DE_NORONHA, BRASILIA, MANAUS
 * hverao = SIM, NAO
 * str = endereço de uma string
 * xJust = justificativa (até 256 caracteres)
 * newcont = rerencia 
*/

struct Cont_s *ideContNew(const struct Cont_s *this,
                          enum TIPO_TZD_e tzd, 
                          enum TIPO_HVERAO_e hverao, 
                          const char *str, 
                          const char *xjust);
{
  if(!this) 
  { 
    strcpy(this->dhCont, DHSet(tzd, hverao, str));
    strcpy(this->xJust, xjust);
    return this;
  }
  else
  {
    struct Const_s *cont = (struct Cont_s *)malloc(sizeof(struct Const_s));
    strcpy(cont->dhCont, DHSet(tzd, hverao, str));
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
                     enum TIPO_UF_e cuf, 
                     uint32_t cnf, 
                     char *natop, 
                     enum TIPO_PGTO_e indpag, 
                     enum TIPO_MOD_e mod, 
                     uint16_t serie, 
                     uint32_t nnf, 
                     char *dhemi, 
                     char *dhsaient,
                     enum TIPO_NF_e tpnf, 
                     enum TIPO_DESTINO_e iddest,
                     uint32_t cmunfg, 
                     enum TIPO_IMPRESSAO_e tpimp,
                     enum TIPO_EMISSAO_e tpemis, 
                     uint8_t cdv,
                     enum TIPO_AMBIENTE_e tpamb, 
                     enum TIPO_FINALIDADE_e finnfe,
                     enum TIPO_OP_e indfinal, 
                     enum TIPO_PRES_e indpres,
                     enum TIPO_PROC_EMIS_e procemis, 
                     char *verproc,
                     struct Cont_s *cont,
                     enum TIPO_TZD_e tzd, 
                     enum TIPO_HVERAO_e hverao,
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
    strcpy(this->dhEmi, DHSet(tzd, hverão, str)); // precisa rever isso
    strcpy(this->dhSaiEnt, DHSet(tzd, hverao, str));
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
    strcpy(ide->dhEmi, DHSet(tzd, hverão, str)); // precisa rever isso
    strcpy(ide->dhSaiEnt, DHSet(tzd, hverao, str));
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
  if (rc == -1) {
    printf("ide-: Erro em xmlTextWriterStartElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF","%02u", 
                                               ide->cUF);
  if (rc == -1) {
    printf("ide->cUF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cNF","%08lu", 
                                               ide->cNF);
  if (rc == -1) {
    printf("ide->cNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "natOp","%s", 
                                               ide->natOp);
  if (rc == -1) {
    printf("ide->natOp: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indPag","%1u", 
                                               ide->indPag);
  if (rc == -1) {
    printf("ide->indPag: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod","%02u", 
                                               ide->mod);
  if (rc == -1) {
    printf("ide->mod: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }
  
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie","%3u", 
                                               ide->serie);
  if (rc == -1) {
    printf("ide->serie: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF","%9lu", 
                                               ide->cNF);
  if (rc == -1) {
    printf("ide->nNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhEmi","%s", 
                                               ide->dhEmi);
  if (rc == -1) {
    printf("ide->dhEmi: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhSaiEnt","%s", 
                                               ide->dhSaiEnt);
  if (rc == -1) {
    printf("ide->dhSaiEnt: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpNF","%1u", 
                                               ide->tpNF);
  if (rc == -1) {
    printf("ide->tpNF: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "idDest","%1u", 
                                               ide->idDest);
  if (rc == -1) {
    printf("ide->idDest: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cMunFG","%07lu", 
                                               ide->cMunFG);
  if (rc == -1) {
    printf("ide->cMunFG: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpImp","%u", 
                                               ide->tpImp);
  if (rc == -1) {
    printf("ide->tpImp: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpEmis","%u", 
                                               ide->tpEmis);
  if (rc == -1) {
    printf("ide->tpEmis: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cDV","%u", 
                                               ide->cDV);
  if (rc == -1) {
    printf("ide->cDV: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpAmb","%u", 
                                               ide->tpAmb);
  if (rc == -1) {
    printf("ide->tpAmb: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "finNFe","%u", 
                                               ide->finNFe);
  if (rc == -1) {
    printf("ide->finNFe: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indFinal","%u", 
                                               ide->indFinal);
  if (rc == -1) {
    printf("ide->indFinal: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indPres","%u", 
                                               ide->indPres);
  if (rc == -1) {
    printf("ide->indPres: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "procEmis","%u", 
                                               ide->procEmis);
  if (rc == -1) {
    printf("ide->procEmis: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "verProc","%s", 
                                               ide->verProc);
  if (rc == -1) {
    printf("ide->verProc: Erro em xmlTextWriterWriteFormatElement\n");
    return -1;
  }

  if(!ide->cont)
     rc = xmlGenideContNode(writer, ide->cont);

  rc = xmlTextWriterEndElement(writer);
  if (rc == -1) {
    printf("ide: Erro em xmlTextWriterEndElement\n");
    return -1;
  }

  return 0;
}

