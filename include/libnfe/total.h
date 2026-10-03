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

#ifndef LIBNFE_TOTAL_H
#define LIBNFE_TOTAL_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/grupo.h>
#include <libnfe/utils.h>

/*
 * Totais da nota (grupo total): ICMSTot e vNFTot.
 *
 * ICMSTot e vNFTot têm setters próprios (abaixo). Os demais grupos
 * (ISSQNtot, retTrib, ISTot e IBSCBSTot) são preenchidos pelo grupo
 * genérico (grupo.h) devolvido por nfe_total_grupo:
 *   nfe_grupo_set(nfe_total_grupo(tot), "IBSCBSTot/vBCIBSCBS", "100.00");
 *
 * Uso típico:
 *   nfe_total *tot = nfe_total_new();   (campos obrigatórios em "0.00")
 *   nfe_total_set_icmstot(tot, NFE_TOT_VPROD, "15.00");
 *   nfe_total_set_icmstot(tot, NFE_TOT_VNF, "15.00");
 *   nfe_total_write_xml(writer, tot);
 *   nfe_total_free(tot);
 *
 * Valores são texto no formato do XML, inteiros ou com 2 casas ("15" ou
 * "15.00"). A biblioteca não soma os itens: os totais devem ser calculados
 * por quem chama.
 */

typedef struct nfe_total nfe_total;

/* Campos do ICMSTot, na ordem do leiaute. Os marcados como opcionais só
 * são escritos se informados; os demais começam em "0.00". */
typedef enum nfe_campo_icmstot {
	NFE_TOT_VBC,            /* base de cálculo do ICMS */
	NFE_TOT_VICMS,          /* valor do ICMS */
	NFE_TOT_VICMSDESON,     /* ICMS desonerado */
	NFE_TOT_VFCPUFDEST,     /* FCP da UF de destino (opcional) */
	NFE_TOT_VICMSUFDEST,    /* ICMS da UF de destino (opcional) */
	NFE_TOT_VICMSUFREMET,   /* ICMS da UF do remetente (opcional) */
	NFE_TOT_VFCP,           /* Fundo de Combate à Pobreza */
	NFE_TOT_VBCST,          /* base de cálculo do ICMS ST */
	NFE_TOT_VST,            /* ICMS ST */
	NFE_TOT_VFCPST,         /* FCP retido por ST */
	NFE_TOT_VFCPSTRET,      /* FCP retido anteriormente por ST */
	NFE_TOT_QBCMONO,        /* ICMS monofásico: quantidade tributada
	                           (opcional) */
	NFE_TOT_VICMSMONO,      /* ICMS monofásico próprio (opcional) */
	NFE_TOT_QBCMONORETEN,   /* ICMS monofásico sujeito a retenção:
	                           quantidade (opcional) */
	NFE_TOT_VICMSMONORETEN, /* ICMS monofásico sujeito a retenção
	                           (opcional) */
	NFE_TOT_QBCMONORET,     /* ICMS monofásico retido anteriormente:
	                           quantidade (opcional) */
	NFE_TOT_VICMSMONORET,   /* ICMS monofásico retido anteriormente
	                           (opcional) */
	NFE_TOT_VPROD,          /* valor total dos produtos e serviços */
	NFE_TOT_VFRETE,         /* frete */
	NFE_TOT_VSEG,           /* seguro */
	NFE_TOT_VDESC,          /* desconto */
	NFE_TOT_VII,            /* imposto de importação */
	NFE_TOT_VIPI,           /* IPI */
	NFE_TOT_VIPIDEVOL,      /* IPI devolvido */
	NFE_TOT_VPIS,           /* PIS */
	NFE_TOT_VCOFINS,        /* COFINS */
	NFE_TOT_VOUTRO,         /* outras despesas acessórias */
	NFE_TOT_VNF,            /* valor total da nota */
	NFE_TOT_VTOTTRIB,       /* tributos aproximados, Lei 12.741/2012
	                           (opcional) */
	NFE_TOT_QUANTIDADE      /* número de campos (não é um campo) */
} nfe_campo_icmstot;

/* Cria os totais com os campos obrigatórios em "0.00". Retorna NULL se
 * faltar memória. */
nfe_total *nfe_total_new(void);

/* Libera os totais; aceita NULL */
void nfe_total_free(nfe_total *tot);

/* Grupo genérico com todos os campos de <total> (pertence aos totais) */
nfe_grupo *nfe_total_grupo(nfe_total *tot);

/* Grava um campo do ICMSTot. NULL volta o campo ao valor inicial ("0.00"
 * nos obrigatórios; não informado nos opcionais). Retorna 0, E_ISNULL ou
 * E_VALOR (campo inexistente ou valor fora do formato). */
int nfe_total_set_icmstot(nfe_total *tot, nfe_campo_icmstot campo,
                          const char *valor);

/* vNFTot: valor total da nota com IBS, CBS e IS; NULL remove */
int nfe_total_set_vnftot(nfe_total *tot, const char *vnftot);

/* Escreve o elemento <total>. Retorna 0, E_ISNULL, E_VALOR (grupo
 * incompleto) ou E_XML. */
int nfe_total_write_xml(xmlTextWriterPtr writer, const nfe_total *tot);

/* Uso interno: valor atual do campo ("" se opcional não informado) */
NFE_INTERNO const char *nfe_total_valor(const nfe_total *tot,
                                        nfe_campo_icmstot campo);

#endif
