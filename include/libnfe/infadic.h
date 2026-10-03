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

#ifndef LIBNFE_INFADIC_H
#define LIBNFE_INFADIC_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

/*
 * Informações adicionais da nota (grupo infAdic): texto ao fisco, texto
 * complementar ao contribuinte, observações (campo/texto) e processos
 * referenciados.
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO, E_VALOR ou E_MALLOC; em caso
 * de erro o objeto não é alterado. Textos seguem o tipo TString (sem espaço no
 * início ou no fim); NULL remove os opcionais.
 */

typedef struct nfe_infadic nfe_infadic;

/* Limites de ocorrências do leiaute */
#define NFE_MAX_OBS     10 /* obsCont e obsFisco, cada um */
#define NFE_MAX_PROCREF 100

/* indProc: origem do processo referenciado */
typedef enum nfe_origem_processo {
	NFE_PROCESSO_SEFAZ = 0,
	NFE_PROCESSO_JUSTICA_FEDERAL = 1,
	NFE_PROCESSO_JUSTICA_ESTADUAL = 2,
	NFE_PROCESSO_SECEX_RFB = 3,
	NFE_PROCESSO_CONFAZ = 4
} nfe_origem_processo;

/* tpAto: tipo do ato concessório (processos da SEFAZ) */
typedef enum nfe_ato_concessorio {
	NFE_ATO_NAO_INFORMADO = 0,
	NFE_ATO_TERMO_ACORDO = 8,
	NFE_ATO_REGIME_ESPECIAL = 10,
	NFE_ATO_AUTORIZACAO_ESPECIFICA = 12,
	NFE_ATO_AJUSTE_SINIEF = 14,
	NFE_ATO_CONVENIO_ICMS = 15
} nfe_ato_concessorio;

/* Cria as informações adicionais vazias. Retorna NULL se faltar memória. */
nfe_infadic *nfe_infadic_new(void);

/* Libera; aceita NULL */
void nfe_infadic_free(nfe_infadic *inf);

/* infAdFisco: de interesse do fisco, 1 a 2000 caracteres */
int nfe_infadic_set_infadfisco(nfe_infadic *inf, const char *texto);
/* infCpl: de interesse do contribuinte, 1 a 5000 caracteres */
int nfe_infadic_set_infcpl(nfe_infadic *inf, const char *texto);
/* obsCont / obsFisco: observação com identificação do campo (xCampo, 1 a
 * 20) e texto (xTexto, 1 a 60); até NFE_MAX_OBS de cada */
int nfe_infadic_add_obscont(nfe_infadic *inf, const char *xcampo,
                            const char *xtexto);
int nfe_infadic_add_obsfisco(nfe_infadic *inf, const char *xcampo,
                             const char *xtexto);
/* procRef: processo referenciado (nProc, 1 a 60), origem e, para processos
 * da SEFAZ, o tipo de ato (NFE_ATO_NAO_INFORMADO omite); até
 * NFE_MAX_PROCREF */
int nfe_infadic_add_procref(nfe_infadic *inf, const char *nproc,
                            nfe_origem_processo indproc,
                            nfe_ato_concessorio tpato);
/* Apagam todas as observações / processos */
int nfe_infadic_remove_obs(nfe_infadic *inf);
int nfe_infadic_remove_procref(nfe_infadic *inf);

/* Escreve o elemento <infAdic>. Retorna 0, E_ISNULL ou E_XML. */
int nfe_infadic_write_xml(xmlTextWriterPtr writer, const nfe_infadic *inf);

#endif
