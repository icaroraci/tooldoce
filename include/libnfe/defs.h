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

#ifndef INCLUIDO_DEFS
#define INCLUIDO_DEFS

#define TAM_CHAVE      45
#define TAM_CUF         3
#define TAM_AAMM        5
#define TAM_CNPJ       15
#define TAM_NNF        10
#define TAM_SERIE       4
#define TAM_MOD_NF      3
#define TAM_CPF        12
#define TAM_IE         15
#define TAM_NECF        4
#define TAM_NCOO        7

#define TAM_NATOP           62
#define TAM_VERSAO_APLIC    21

enum TIPO_UF_e{
  /* Região Norte */
  RO = 11,
  AC = 12,
  AM = 13,
  RR = 14,
  PA = 15,
  AP = 16,
  TO = 17,
  /* Região Nordeste */
  MA = 21,
  PI = 22,
  CE = 23,
  RN = 24,
  PB = 25,
  PE = 26,
  AL = 27,
  SE = 28,
  BA = 29,
  /* Região Sudeste */
  MG = 31,
  ES = 32,
  RJ = 33,
  SP = 35,
  /* Região Sul */
  PR = 41,
  SC = 42,
  RS = 43,
  /* Região Centro-Oeste */
  MS = 50,
  MT = 51,
  GO = 52,
  DF = 53
};

enum TIPO_MES_e {
  JANEIRO = 1,
  FEVEREIRO = 2,
  MARCO = 3,
  ABRIL = 4,
  MAIO = 5,
  JUNHO = 6,
  JULHO = 7,
  AGOSTO = 8,
  SETEMBRO = 9,
  OUTUBRO = 10,
  NOVEMBRO = 11,
  DEZEMBRO = 12
};

enum TIPO_PGTO_e         {AVISTA, APRAZO, OUTROS};

enum TIPO_MOD_e          {NFE = 55, NFCE = 65};

enum TIPO_NF_e           {ENTRADA, SAIDA};

enum TIPO_DESTINO_e      {OP_INTERNA, OP_INTERESTADUAL, OP_EXTERIOR};

enum TIPO_IMPRESSAO_e    {SEM_DANFE, DANFE_RETRATO, DANFE_PAISAGEM, 
                          DANFE_SIMPLIFICADO, DANFE_NFCE, DANFE_NFCE_MSG};

enum TIPO_EMISSAO_e      {NORMAL = 1, FSIA = 2, SCAN =3, DPEC = 4
                          FSDA = 5, SVCAN = 6, SVCRS = 7, NFCE_OFFLINE=9};

enum TIPO_AMBIENTE_e     {PRODUCAO = 1, HOMOLOGACAO = 2};

enum TIPO_FINALIDADE_e   {NORMAL = 1,COMPLEMENTAR = 2, AJUSTE = 3, 
                          DEVOLUCAO = 4};

enum TIPO_OP_e           {NORMAL, CONSUMIDOR};

enum TIPO_PRES_e         {NAO_APLICA = 0, PRESENCIAL = 1 NAO_PRESENCIAL = 2,
                          TELEATENDIMENTO = 3, ENTREGA_DOMICILIO = 4, 
                          OUTROS = 9};

enum TIPO_PROC_EMIS_e    {APL_CONTRIBUINTE, AVULSA_FISCO, 
                          AVULSA_CONTRIBUINTE_SITE_FISCO,
                          CONTRIBUINTE_APL_FISCO};

enum TIPO_TZD_e          {DEFAULT, FERNANDO_DE_NORONHA, BRASILIA, MANAUS};

enum TIPO_HVERAO_e       {SIM, NAO};

#endif


