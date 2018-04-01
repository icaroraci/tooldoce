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

#ifndef _H_IDE_H_
#define _H_IDE_H_

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

struct Cont_s;
struct ide_s;


/* this = um objeto struct Cont_s
 * tzd = DEFAULT, FERNANDO_DE_NORONHA, BRASILIA, MANAUS
 * hverao = SIM, NAO
 * str = endereço de uma string
 * xJust = justificativa (até 256 caracteres)
 *  
*/ 

struct Cont_s *ideContNew(const struct Cont_s *this,
                          enum TIPO_TZD_e tzd, enum TIPO_HVERAO_e hverao, 
                          const char *str, const char *xjust);

void ideContDel(const struct Cont_s *cont);

/* Cria um objeto "ide" para identificação da NF-e
 *
 *            ** Parâmetros  **
 * cont     = ponteiro para struct Cont_s, default NULL;
 * cuf      = tabela IBGE Unidades Federativas
 * cnf      = chave de acesso - 8 caracteres
 * natop    = Descrição natureza da operação: string 60 caracteres;
 * indpag   = AVISTA, APRAZO, OUTROS;
 * mod      = NFE para 55, NFCE para 65;
 * serie    = serie do documento fiscal - 3 algarismos
 * dhemi    = data e hora de emissao
 * dhSaiEnt = data e hora da saída ou entrada da mercadoria/produto;
 * tpnf     = ENTRADA, SAIDA;
 * iddest   = OP_INTERNA, OP_INTERESTADUAL, OP_EXTERIOR;
 * cmunfg   = Municipio do fato gerador: Tabela IBGE Municipios :
 *            7 algarismos;
 * tpImp    = SEM_DANFE, DANFE_RETRATO, DANFE_PAISAGEM, 
 *            DANFE_SIMPLIFICADO, DANFE_NFCE, DANFE_NFCE_MSG;
 * tpemis   = NORMAL = 1, FSIA = 2, SCAN =3, DPEC = 4
 *            FSDA = 5, SVCAN = 6, SVCRS = 7, NFCE_OFFLINE=9;
 * cdv      = digito verificador, calculado externamente;
 * tpamb    = PRODUCAO = 1, HOMOLOGACAO = 2;
 * finnfe   = NORMAL = 1,COMPLEMENTAR = 2, AJUSTE = 3, 
 *            DEVOLUCAO = 4;
 * indfinal = NORMAL, CONSUMIDOR;
 * indpres  = NAO_APLICA = 0, PRESENCIAL = 1 NAO_PRESENCIAL = 2,
 *            TELEATENDIMENTO = 3, ENTREGA_DOMICILIO = 4, 
 *            OUTROS = 9;
 * procemis = APL_CONTRIBUINTE, AVULSA_FISCO, 
 *            AVULSA_CONTRIBUINTE_SITE_FISCO,
 *            CONTRIBUINTE_APL_FISCO;
 * verproc  = Versão do protocolo de emissao: 20 caracteres
 * tzd      = DEFAULT, FERNANDO_DE_NORONHA, BRASILIA, MANAUS;
 * hverao   = SIM, NAO
 * str      = string indicativa da hora
 * 
**/
struct ide_s *ideNew(struct ide_s *this, enum TIPO_UF_e cuf, uint32_t cnf, 
                     char *natop, enum TIPO_PGTO_e indpag, enum TIPO_MOD_e mod, 
                     uint16_t serie, uint32_t nnf, char *dhemi, char *dhsaient,
                     enum TIPO_NF_e tpnf, enum TIPO_DESTINO_e iddest,
                     uint32_t cmunfg, enum TIPO_IMPRESSAO_e tpimp,
                     enum TIPO_EMISSAO_e tpemis, uint8_t cdv,
                     enum TIPO_AMBIENTE_e tpamb, enum TIPO_FINALIDADE_e finnfe,
                     enum TIPO_OP_e indfinal, enum TIPO_PRES_e indpres,        // 1 caractere
                     enum TIPO_PROC_EMIS_e procemis, char *verproc,
                     enum TIPO_TZD_e tzd, enum TIPO_HVERAO_e hverao,
                     const char *str, struct Cont_s *cont)
{
void ideDel(struct ide_s *ide);

/*  Gera o Nó xml para o respectivo objeto
 *  
 *  xmlGenideNode(writer, ide) chama internamente
 *  xmlGenideContNode(writer, cont) se este for definido.
 ***/

int xmlGenideContNode(xmlTextWriterPtr writer,struct Cont_s *cont);

int xmlGenideNode(xmlTextWriterPtr writer,struct ide_s *ide);

#endif
