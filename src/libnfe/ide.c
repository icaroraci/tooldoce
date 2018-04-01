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
  struct Cont_s *newCont;
};


struct ide_s{
  enum UF_e cUF;                   // 2 caracteres
  uint32_t cNF;                    // 8 caracteres 
  char natOp[TAM_NATOP];           // 61 caracteres
  uint8_t indPag;                  // 1 caractere
  enum MOD_e mod;                  // 2 caracteres
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
  struct ide_s *newide;
};
 
/* Funções auxiliares  */

// TODO: Definir enum TIPO_BOOL_e { VERDADE, FALSO};
//       Definir enum TZD_e {DEFAULT,FERNANDO_DE_NORONHA, BRASILIA, MANAUS}

/*Data e hora do evento no formato AAAA-MM-DDThh:mm:ssTZD (UTC - 
 * Universal Coordinated Time , onde TZD pode ser 
 * -02:00 (Fernando de Noronha), 
 * -03:00 (Brasília) ou 
 * -04:00 (Manaus), no  horário de verão serão 
 * -01:00, -02:00 e -03:00. Ex.:
 * 2010-08-19T13:00:15-03:00.
 * */
static char *DHSet(enum TZD_e tzd, enum TIPO_BOOL_e hverao,
                   const char *str)
{
  char *aux;
  if (hverao == FALSO){
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
  }else if(hverao == VERDADE){
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


// Funções de Cont_s


static void ContSetdhCont(const struct Cont_s *cont, enum TZD_e tzd,
                   enum TIPO_BOOL_e hverao, const char *str )
{
  // devemos passar str ou alocar aqui?
  cont->dhCont = DHSet(tzd, hverao, str);
}

static char *ContGetdhCont(const struct Cont_s *cont)
{
  return cont->dhCont;
}

static void ContSetxJust(const struct Cont_s *cont, char *xjust)
{
  strcpy(cont->xJust, xjust);
}

static char *ContGetxJust(const struct Cont_s *cont)
{
  return cont->xJust;
}

struct Cont_s *ideContNew(enum TZD_e tzd, enum TIPO_BOOL_e hverao, 
                          const char *str, const char *xjust,
                          const struct Cont_s *newcont)
{
  struct Const_s *cont = (struct Cont_s *)malloc(sizeof(struct Const_s));
  ContSetdhCont(cont, tzd, hverao, str);
  ContSetxJust(cont, xjust);
  cont->newCont = newcont;
  return cont;
}

void ideContDel(const struct Cont_s *cont)
{
  free(cont);
}


// Funcçoes de ide_s
 
static void ideSetcUF(const struct ide_s *ide, enum UF_e cuf)
{
  ide->cUF = cuf;
}

static enum UF_e ideGetcUF(const struct ide_s *ide)
{
  return ide->cUF;
}

static void ideSetcNF(const struct ide_s *ide, const uint32_t cnf)
{
  ide->cNF = cnf;
}

static uint32_t ideGetcNF(const struct ide_s *ide)
{
  return ide->cNF;
}

static void ideSetnatOp(const struct ide_s *ide, const char *natop)
{
  strcpy(ide->natOp, natop); 
}
static char *ideGetnatOp(const struct ide_s *ide)
{
  return ide->natOp;
}

static void ideSetindPag(const struct ide_s *ide, const uint8_t indpag)
{
  ide->indPag = indpag;
}
static char *ideGetindPag(const struct ide_s *ide)
{
  return ide->indPag;
}

static void ideSetmod(const struct ide_s *ide, enum MOD_e mod)
{
  ide->mod = mod;
}
static enum MOD_e ideGetmod(const struct ide_s *ide)
{
  return ide->mod;
}

static void ideSetserie(const struct ide_s *ide, const uint16_t serie)
{
  ide->serie = serie;
}
static uint16_t ideGetserie(const struct ide_s *ide)
{
  return ide->serie;
}

static void ideSetnNF(const struct ide_s *ide, const uint32_t nnf)
{
  ide->nNF = nnf;
}
static uint32_t ideGetnNF(const struct ide_s *ide)
{
  return ide->nNF;
}

static void ideSetdhEmi(const struct ide_s *ide, enum TZD_e tzd,
                        enum TIPO_BOOL_e hverao, const char *str)
{
  ide->dhEmi = DHSet(tzd, hverao, str);
}
static char *ideGetdhEmi(const struct ide_s *ide)
{
  return ide->dhEmi;
}

static void ideSetdhSaiEnt(const struct ide_s *ide, enum TZD_e tzd,
                           enum TIPO_BOOL_e hverao, const char *str)
{
  ide->dhSaiEnt = DHSet(tzd, hverao, str);
}
static char *ideGetdhSaiEnt(const struct ide_s *ide)
{
  return ide->dhSaiEnt;
}

static void ideSettpNF(const struct ide_s *ide, enum TIPO_NF_e tpnf)
{
  ide->tpNF = tpnf;
}
static enum TIPO_NF_e ideGettpNF(const struct ide_s *ide)
{
  return ide->tpNF;
}

static void ideSetidDest(const struct ide_s *ide, enum TIPO_DESTINO_e iddest)
{
  ide->idDest = iddest;
}
static enum TIPO_DESTINO_e ideGetidDest(const struct ide_s *ide)
{
  return ide->idDest;
}

static void ideSetcMunFG(const struct ide_s *ide, const uint32_t cmunfg)
{
  ide->cMunFG = cmunfg;
}
static uint32_t ideGetcMunFG(const struct ide_s *ide)
{
  return ide->cMunFG;
}

static void ideSettpImp(const struct ide_s *ide, enum TIPO_IMPRESSAO_e tpimp)
{
  ide->tpImp = tpimp;
}
static enum TIPO_IMPRESSAO_e ideGettpImp(const struct ide_s *ide)
{
  return ide->tpImp;
}

static void ideSettpEmis(const struct ide_s *ide, enum TIPO_EMISSAO_e tpemis)
{
  ide->tpEmis = tpemis;
}
static enum TIPO_EMISSAO_e ideGettpEmis(const struct ide_s *ide)
{
  return ide->tpEmis;
}

static void ideSetcDV(const struct ide_s *ide, const uint8_t cdv)
{
  ide->cDV = cdv;
}
static uint8_t ideGetcDV(const struct ide_s *ide)
{
  return ide->cDV;
}

static void ideSettpAmb(const struct ide_s *ide, enum TIPO_AMBIENTE_e tpamb)
{
  ide->tpAmb = tpamp;
}
static enum TIPO_AMBIENTE_e ideGettpAmb(const struct ide_s *ide)
{
  return ide->tpAmb;
}

static void ideSetfinNFe(const struct ide_s *ide, enum TIPO_FINALIDADE_e finnfe)
{
  ide->finNFe = finnfe;
}
static enum TIPO_FINALIDADE_e ideGetfinNFe(const struct ide_s *ide)
{
  return ide->finNFe;
}

static void ideSetindFinal(const struct ide_s *ide, enum TIPO_OP_e indfinal)
{
  ide->indFinal = indfinal;
}
static enum TIPO_OP_e ideGetindFinal(const struct ide_s *ide)
{
  return ide->indFinal;
}

static void ideSetindPres(const struct ide_s *ide, enum TIPO_PRES_e indpres)
{
  ide->indPres = indpres;
}
static enum TIPO_PRES_e ideGetindPres(const struct ide_s *ide)
{
  return ide->indPres;
}

static void ideSetprocEmis(const struct ide_s *ide, 
                                   enum TIPO_PROC_EMIS_e procemis)
{
  ide->procEmis = procemis;
}
static enum TIPO_PROC_EMIS_e ideGetprocEmis(const struct ide_s *ide)
{
  return ide->procEmis;
}

static void ideSetverProc(const struct ide_s *ide, const char *verproc)
{
  strcpy(ide->verProc, verproc);
}
static char *ideGetverProc(const struct ide_s *ide)
{
  return ide->verProc;
}

static void ideSetcont(const struct ide_s *ide, const struct Cont_s *const,
                       enum TZD_e tzd, enum TIPO_BOOL_e hverao, 
                       const char *str, const char *xjust)
{
  if(!const)
    ide->cont = ideContNew(tzd, hverao, str, xjust);
  else
    ide->cont = NULL;
}
static struct Cont_t *ideGetcont(const struct ide_s *ide)
{
  return ide->cont;
}


struct ide_s *ideNew(enum UF_e cuf, uint32_t cnf, char *natop, 
                     uint8_t indpag, enum MOD_e mod, uint16_t serie,
                     uint32_t nnf, char *dhemi, char *dhsaient,
                     enum TIPO_NF_e tpnf, enum TIPO_DESTINO_e iddest,
                     uint32_t cmunfg, enum TIPO_IMPRESSAO_e tpimp,
                     enum TIPO_EMISSAO_e tpemis, uint8_t cdv,
                     enum TIPO_AMBIENTE_e tpamb, enum TIPO_FINALIDADE_e finnfe,
                     enum TIPO_OP_e indfinal, enum TIPO_PRES_e indpres,        // 1 caractere
                     enum TIPO_PROC_EMIS_e procemis, char *verproc,
                     struct Cont_s *cont, TZD_e tzd, TIPO_BOOL_e hverao,
                     const char *str, struct ide_s *newide)
{
  struct ide_s *ide = (struct ide_s *)malloc(sizeof(struct ide_s));
  ideSetcUF(ide, cuf);
  ideSetcNF(ide, nnf);
  ideSetnatOp(ide, natop);
  ideSetindPag(ide, indpag);
  ideSetmod(ide, mod);
  ideSetserie(ide, serie);
  ideSetnNF(ide, nnf);
  ideSetdhEmi(ide, tzd. hverao, str);
  ideSetdhSaiEnt(ide, tzd, hverao, str);
  ideSettpNF(ide, tpnf);
  ideSetidDest(ide, idedest);
  ideSetcMunFG(ide, cmunfg);
  ideSettpImp(ide, tpimp);
  ideSettpEmis(ide, tpemis);
  ideSetcDV(ide, cdv);
  ideSettpAmb(ide, tpamb);
  ideSetfinNFe(ide, finnfe);
  ideSetindFinal(ide, indfinal);
  ideSetindPres(ide, indpres);
  ideSetprocEmis(ide, procemis);
  ideSetverProc(ide, verproc);
  ideSetcont(ide, cont); // precisa rever
  ide->newide = newide;
  return ide;
}

/* Se houver alocação de memoria da struct Cont_s esta deve ser liberada
 * antes de liberar memória da struct ide_s */
void ideDel(struct ide_s *ide)
{
  if(!ide->cont)
  {
    ideContDel(ide->cont); 
    free(ide);
  }
  else
    free(ide);
}

