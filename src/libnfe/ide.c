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
#include <string.h>
#include <time.h>
#include "include/libnfe/defs.h"
#include "ide.h"

struct ide_c{
  enum UF_e cUF;
  unsigned long int cNF;           // 8 caracteres 
  char natOp[TAM_NATOP];           // 61 caracteres
  unsigned char indPag;            // 1 caractere
  enum MOD_e mod;                  // 2 caracteres
  unsigned short int serie;        // 3 caracteres
  unsigned long int nNF;           // 9 caracteres
  time_t dhEmi;                    // Data
  time_t dhSaiEnt;                 // Data
  enum TIPO_NF_e tpNF;             // 1 caractere
  enum TIPO_DESTINO_e idDest;      // 1 caractere
  unsigned int cMunFG;             // 7 caracteres
  enum TIPO_IMPRESSAO_e tpImp;     // 1 caractere
  enum TIPO_EMISSAO_e;             // 1 caractere
  unsigned char cDV;               // 1 caractere
  enum TIPO_AMBIENTE_e tpAmb;      // 1 caractere
  enum TIPO_FINALIDADE_e finNFe;   // 1 caractere
  enum TIPO_OP_e indFinal;         // 1 caractere
  enum TIPO_PRES_e indPres;        // 1 caractere
  enum TIPO_PROC_EMIS_e procEmis;  // 1 caractere
  char verProc[TAM_VERSAO_APLIC];  // 20 caracteres
  xmlTextWriterPtr writer;         // default NULL - sequencia xml;
  time_t dhCont;                   // Data
  char xJust[TAM_JUSTIFICATIVA];   // 256 caracteres
};
  
  
}
