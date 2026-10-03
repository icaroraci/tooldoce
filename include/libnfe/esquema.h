/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 **
 ** This file is part of tooldoce.
 **
 ** tooldoce is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published
 ** by the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** tooldoce is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with tooldoce.  If not, see <https://www.gnu.org/licenses/>.
 ** */

#ifndef LIBNFE_ESQUEMA_H
#define LIBNFE_ESQUEMA_H

#include <libxml/xmlwriter.h>

#include <libnfe/grupo.h>
#include <libnfe/utils.h>

/*
 * Uso interno: motor genérico de grupos do leiaute.
 *
 * A estrutura de um grupo (elementos, sequências e escolhas do XSD, com as
 * regras de cada campo) vem de tabelas geradas por tools/gerar_esquemas.py
 * (src/libnfe/esquemas.c). Um nfe_grupo guarda os valores dos campos
 * (folhas) de uma dessas estruturas, valida cada valor ao gravar e escreve
 * o XML na ordem do leiaute, conferindo campos obrigatórios e escolhas.
 *
 * Campos são indicados por caminho: nomes de elementos separados por "/",
 * que devem aparecer, nessa ordem, no caminho do campo a partir da raiz
 * (ex.: "ICMS10/vBC", ou só "vBC" quando não há ambiguidade).
 */

enum nfe_esq_tipo { ESQ_ELEM, ESQ_SEQ, ESQ_CHOICE, ESQ_ATTR, ESQ_LISTA };

struct nfe_esq;

struct nfe_esq_no {
	const char *nome;           /* elemento ou atributo; NULL em
	                               sequência/escolha */
	unsigned char tipo;         /* enum nfe_esq_tipo */
	unsigned char min;          /* minOccurs (0 ou 1) / use="required" */
	unsigned short max;         /* maxOccurs das listas (0: sem limite) */
	const char *padrao;         /* xs:pattern ou NULL */
	const char *const *valores; /* xs:enumeration (lista com NULL) */
	unsigned short tmin, tmax;  /* tamanho em caracteres (0: livre) */
	unsigned char tstring;      /* texto do tipo TString */
	short pai, filho, irmao;    /* índices na tabela (-1: nenhum) */
	short folha;                /* índice do valor; -1 se não é folha */
	short ini, fim;             /* folhas do nó: [ini, fim) */
	short lista;                /* índice da lista; -1 se não é lista */
	short lini, lfim;           /* listas do nó: [lini, lfim) */
	const struct nfe_esq *sub;  /* estrutura dos itens da lista */
};

struct nfe_esq {
	const char *nome;
	const struct nfe_esq_no *nos;
	int nnos;
	int nfolhas;
	int nlistas;
};

/* Estruturas geradas */
extern const struct nfe_esq esq_imposto, esq_prod, esq_transp, esq_total,
        esq_NFref, esq_impostoDevol, esq_obsItem, esq_DFeReferenciado,
        esq_avulsa, esq_exporta, esq_compra, esq_cana, esq_infSolicNFF,
        esq_agropecuario, esq_infPAA, esq_infNFeSupl;

NFE_INTERNO nfe_grupo *nfe_grupo_new(const struct nfe_esq *esq);
NFE_INTERNO void nfe_grupo_free(nfe_grupo *g);

/* Confere se valor é aceito no campo, sem gravar. Retorna 0, E_ISNULL,
 * E_VALOR (campo inexistente ou valor inválido) ou E_TAMANHO. */
NFE_INTERNO int nfe_grupo_valida(const nfe_grupo *g, const char *caminho,
                                 const char *valor);
/* Apaga o último item da lista do caminho (desfaz um nfe_grupo_add) */
NFE_INTERNO void nfe_grupo_remove_ultimo(nfe_grupo *g, const char *caminho);
/* Algum campo informado (ou item de lista) no grupo */
NFE_INTERNO int nfe_grupo_vazio(const nfe_grupo *g);
/* Escreve o elemento raiz. Retorna 0, E_ISNULL, E_VALOR (falta campo
 * obrigatório ou mais de um ramo de uma escolha) ou E_XML. */
NFE_INTERNO int nfe_grupo_write_xml(xmlTextWriterPtr writer,
                                    const nfe_grupo *g);

#endif
