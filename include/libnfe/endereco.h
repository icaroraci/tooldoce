/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 **
 ** This file is part of tooldoce.
 **
 ** tooldoce is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** tooldoce is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 ** */

#ifndef LIBNFE_ENDERECO_H
#define LIBNFE_ENDERECO_H

#include <stdint.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/utils.h>

/*
 * Endereço do emitente (enderEmit, tipo TEnderEmi) ou do destinatário
 * (enderDest, tipo TEndereco).
 *
 * Uso típico:
 *   nfe_endereco *end = nfe_endereco_new();
 *   nfe_endereco_set_xlgr(end, "RUA DAS FLORES");
 *   ...
 *   nfe_endereco_write_xml(writer, NFE_ENDERECO_EMITENTE, end);
 *   nfe_endereco_free(end);
 *
 * Os setters validam o valor contra o leiaute e retornam 0, E_ISNULL,
 * E_TAMANHO (texto fora dos limites) ou E_VALOR (valor fora do domínio ou
 * do formato); em caso de erro o campo não é alterado. Textos: limites em
 * caracteres UTF-8, só caracteres de U+0020 a U+00FF e sem espaço no início
 * ou no fim (tipo TString). Campos opcionais são removidos com NULL (ou 0,
 * nos numéricos).
 */

typedef struct nfe_endereco nfe_endereco;

/* Qual grupo nfe_endereco_write_xml escreve */
typedef enum nfe_endereco_tipo {
	NFE_ENDERECO_EMITENTE,    /* <enderEmit> */
	NFE_ENDERECO_DESTINATARIO /* <enderDest> */
} nfe_endereco_tipo;

/* Código e nome de município e UF para operações com o exterior
 * (destinatário) */
#define NFE_CMUN_EXTERIOR 9999999u
#define NFE_XMUN_EXTERIOR "EXTERIOR"
#define NFE_UF_EXTERIOR   "EX"

/* Código do Brasil na tabela de países (cPais) */
#define NFE_CPAIS_BRASIL 1058u

/* Cria um endereço vazio. Retorna NULL se faltar memória. */
nfe_endereco *nfe_endereco_new(void);

/* Libera o endereço; aceita NULL */
void nfe_endereco_free(nfe_endereco *end);

int nfe_endereco_set_xlgr(nfe_endereco *end, const char *xlgr); /* 2 a 60 */
int nfe_endereco_set_nro(nfe_endereco *end, const char *nro);   /* 1 a 60 */
int nfe_endereco_set_xcpl(nfe_endereco *end, const char *xcpl); /* 1 a 60 */
int nfe_endereco_set_xbairro(nfe_endereco *end,
                             const char *xbairro); /* 2 a 60 */
/* cMun: código IBGE do município, 7 dígitos (1000000 a 9999999) */
int nfe_endereco_set_cmun(nfe_endereco *end, uint32_t cmun);
int nfe_endereco_set_xmun(nfe_endereco *end, const char *xmun); /* 2 a 60 */
/* UF: sigla ("SP") ou "EX" (exterior, só para o destinatário) */
int nfe_endereco_set_uf(nfe_endereco *end, const char *uf);
/* CEP: 8 dígitos, como texto ("01001000") */
int nfe_endereco_set_cep(nfe_endereco *end, const char *cep);
/* cPais: código do país, 1 a 9999 (no emitente, só NFE_CPAIS_BRASIL) */
int nfe_endereco_set_cpais(nfe_endereco *end, unsigned cpais);
/* xPais: nome do país, 2 a 60 (no emitente, só "Brasil" ou "BRASIL") */
int nfe_endereco_set_xpais(nfe_endereco *end, const char *xpais);
/* fone: DDD e número, 6 a 14 dígitos (no exterior, código do país e da
 * localidade e número) */
int nfe_endereco_set_fone(nfe_endereco *end, const char *fone);

/* Escreve <enderEmit> ou <enderDest>, conforme tipo. Retorna 0, E_ISNULL,
 * E_VALOR ou E_XML. E_VALOR quando falta um campo obrigatório (xLgr, nro,
 * xBairro, cMun, xMun e UF; no emitente, também o CEP) ou, no emitente,
 * quando UF é "EX" ou o país não é o Brasil. */
int nfe_endereco_write_xml(xmlTextWriterPtr writer, nfe_endereco_tipo tipo,
                           const nfe_endereco *end);

/* Uso interno: escreve só os campos (xLgr a fone), sem o elemento que os
 * envolve, com as regras do destinatário (grupos retirada e entrega).
 * Retorna 0, E_VALOR (falta campo obrigatório) ou E_XML. */
NFE_INTERNO int nfe_endereco_write_campos(xmlTextWriterPtr writer,
                                          const nfe_endereco *end);

#endif
