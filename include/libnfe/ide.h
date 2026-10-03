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



#ifndef LIBNFE_IDE_H
#define LIBNFE_IDE_H

#include <stdint.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/nfe.h>

struct Cont_s;
struct ide_s;


/* this = um objeto struct Cont_s
 * tzd = fuso horário (nfe_tzd)
 * str = endereço de uma string
 * xJust = justificativa (até 256 caracteres)
 *  
*/ 

struct Cont_s *ideContNew(const struct Cont_s *this,
                          nfe_tzd tzd, 
                          const char *str, 
                          const char *xjust);

void ideContDel(const struct Cont_s *cont);

/* Cria um objeto "ide" para identificação da NF-e
 *
 *            ** Parâmetros  **
 * cont     = ponteiro para struct Cont_s, default NULL;
 * cuf      = UF, código IBGE (nfe_uf)
 * cnf      = chave de acesso - 8 caracteres
 * natop    = Descrição natureza da operação: string 60 caracteres;
 * indpag   = forma de pagamento (nfe_forma_pagamento);
 * mod      = modelo do documento (nfe_modelo);
 * serie    = serie do documento fiscal - 3 algarismos
 * dhemi    = data e hora de emissao
 * dhSaiEnt = data e hora da saída ou entrada da mercadoria/produto;
 * tpnf     = tipo de operação (nfe_tipo_operacao);
 * iddest   = destino da operação (nfe_destino);
 * cmunfg   = Municipio do fato gerador: Tabela IBGE Municipios :
 *            7 algarismos;
 * tpImp    = formato da DANFE (nfe_danfe);
 * tpemis   = tipo de emissão (nfe_emissao);
 * cdv      = digito verificador, calculado externamente;
 * tpamb    = ambiente (nfe_ambiente);
 * finnfe   = finalidade (nfe_finalidade);
 * indfinal = consumidor final (nfe_consumidor);
 * indpres  = presença do comprador (nfe_presenca);
 * procemis = processo de emissão (nfe_processo_emissao);
 * verproc  = Versão do protocolo de emissao: 20 caracteres
 * tzd      = fuso horário (nfe_tzd);
 * str      = string indicativa da hora
 * 
**/
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
                     const char *str );
void ideDel(struct ide_s *ide);

/*  Gera o Nó xml para o respectivo objeto
 *  
 *  xmlGenideNode(writer, ide) chama internamente
 *  xmlGenideContNode(writer, cont) se este for definido.
 ***/

int xmlGenideContNode(xmlTextWriterPtr writer,struct Cont_s *cont);

int xmlGenideNode(xmlTextWriterPtr writer,struct ide_s *ide);

#endif
