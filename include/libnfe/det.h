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

#ifndef LIBNFE_DET_H
#define LIBNFE_DET_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/grupo.h>
#include <libnfe/imposto.h>
#include <libnfe/prod.h>

/*
 * Item da nota (grupo det): número do item, produto (prod) e tributos
 * (imposto).
 *
 * Os grupos opcionais impostoDevol, obsItem e DFeReferenciado são
 * preenchidos pelo grupo genérico (grupo.h) devolvido por nfe_det_grupo:
 *   nfe_grupo_set(nfe_det_grupo(det, "obsItem"), "obsCont/xCampo", "lote");
 *
 * Uso típico:
 *   nfe_det *det = nfe_det_new();
 *   nfe_det_set_nitem(det, 1);
 *   nfe_det_set_prod(det, prod);       (o det passa a ser dono de prod)
 *   nfe_det_set_imposto(det, imposto); (e de imposto)
 *   nfe_det_write_xml(writer, det);
 *   nfe_det_free(det);
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * item não é alterado.
 */

typedef struct nfe_det nfe_det;

/* Número máximo de itens em uma nota */
#define NFE_MAX_ITENS 990

/* Cria um item vazio. Retorna NULL se faltar memória. */
nfe_det *nfe_det_new(void);

/* Libera o item, o produto e o imposto que ele possui; aceita NULL */
void nfe_det_free(nfe_det *det);

/* nItem: número do item, de 1 a NFE_MAX_ITENS */
int nfe_det_set_nitem(nfe_det *det, unsigned nitem);
/* Em caso de sucesso o item passa a ser dono de prod / imposto (e libera o
 * anterior) */
int nfe_det_set_prod(nfe_det *det, nfe_prod *prod);
int nfe_det_set_imposto(nfe_det *det, nfe_imposto *imposto);
/* infAdProd: informações adicionais do produto, 1 a 500 caracteres (tipo
 * TString); NULL remove */
int nfe_det_set_infadprod(nfe_det *det, const char *infadprod);
/* vItem: valor total do item (participação no total da nota), como texto
 * com 2 casas ("15.00"); NULL remove */
int nfe_det_set_vitem(nfe_det *det, const char *vitem);

/* Grupo genérico de um grupo opcional do item: "impostoDevol", "obsItem"
 * ou "DFeReferenciado" (criado vazio na primeira chamada; pertence ao
 * item e só é escrito se tiver algum campo). NULL se o nome não existir ou
 * faltar memória. */
nfe_grupo *nfe_det_grupo(nfe_det *det, const char *nome);

/* Escreve o elemento <det nItem="...">. Retorna 0, E_ISNULL, E_VALOR (falta
 * nItem, prod ou imposto, ou prod incompleto; ver nfe_prod_write_xml) ou
 * E_XML. */
int nfe_det_write_xml(xmlTextWriterPtr writer, const nfe_det *det);

/* Uso interno: produto e imposto do item (NULL se não informados) */
NFE_INTERNO const nfe_prod *nfe_det_prod(const nfe_det *det);
NFE_INTERNO const nfe_imposto *nfe_det_imposto(const nfe_det *det);

#endif
