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

#ifndef LIBNFE_ESQUEMA_H
#define LIBNFE_ESQUEMA_H

#include <libxml/xmlwriter.h>

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

enum nfe_esq_tipo { ESQ_ELEM, ESQ_SEQ, ESQ_CHOICE };

struct nfe_esq_no {
	const char *nome;           /* elemento; NULL em sequência/escolha */
	unsigned char tipo;         /* enum nfe_esq_tipo */
	unsigned char min;          /* minOccurs (0 ou 1) */
	const char *padrao;         /* xs:pattern ou NULL */
	const char *const *valores; /* xs:enumeration (lista com NULL) */
	unsigned short tmin, tmax;  /* tamanho em caracteres (0: livre) */
	unsigned char tstring;      /* texto do tipo TString */
	short pai, filho, irmao;    /* índices na tabela (-1: nenhum) */
	short folha;                /* índice do valor; -1 se não é folha */
	short ini, fim;             /* folhas do nó: [ini, fim) */
};

struct nfe_esq {
	const char *nome;
	const struct nfe_esq_no *nos;
	int nnos;
	int nfolhas;
};

/* Estruturas geradas */
extern const struct nfe_esq nfe_esq_imposto;

typedef struct nfe_grupo nfe_grupo;

NFE_INTERNO nfe_grupo *nfe_grupo_new(const struct nfe_esq *esq);
NFE_INTERNO void nfe_grupo_free(nfe_grupo *g);

/* Confere se valor é aceito no campo, sem gravar. Retorna 0, E_ISNULL,
 * E_VALOR (campo inexistente, ambíguo ou valor inválido) ou E_TAMANHO. */
NFE_INTERNO int nfe_grupo_valida(const nfe_grupo *g, const char *caminho,
                                 const char *valor);
/* Grava o campo (NULL remove). Ao gravar um campo dentro de uma escolha,
 * os campos dos outros ramos da escolha são apagados. Retorna os códigos
 * de nfe_grupo_valida ou E_MALLOC. */
NFE_INTERNO int nfe_grupo_set(nfe_grupo *g, const char *caminho,
                              const char *valor);
/* Valor do campo (o primeiro informado, se o caminho servir para mais de
 * um), ou NULL */
NFE_INTERNO const char *nfe_grupo_get(const nfe_grupo *g, const char *caminho);
/* Apaga todos os campos dentro dos elementos que casam com caminho.
 * Retorna 0, E_ISNULL ou E_VALOR (caminho inexistente). */
NFE_INTERNO int nfe_grupo_limpa(nfe_grupo *g, const char *caminho);
/* Escreve o elemento raiz. Retorna 0, E_ISNULL, E_VALOR (falta campo
 * obrigatório ou mais de um ramo de uma escolha) ou E_XML. */
NFE_INTERNO int nfe_grupo_write_xml(xmlTextWriterPtr writer,
                                    const nfe_grupo *g);

#endif
