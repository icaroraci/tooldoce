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

#include <stdlib.h>
#include <string.h>

#include <libnfe/defs.h>
#include <libnfe/det.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/esquema.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

struct nfe_det {
	unsigned nItem; /* 0: não informado */
	nfe_prod *prod;
	nfe_imposto *imposto;
	char infAdProd[NFE_TAM_UTF8(NFE_TAM_INFADPROD)]; /* "": não informado */
	char vItem[NFE_TAM_ASCII(NFE_TAM_DEC)]; /* "": não informado */
	nfe_grupo *grupo[3]; /* impostoDevol, obsItem, DFeReferenciado */
};

/* Grupos genéricos do item, na ordem de nfe_det.grupo */
static const struct {
	const char *nome;
	const struct nfe_esq *esq;
} grupos[3] = {
	{ "impostoDevol", &esq_impostoDevol },
	{ "obsItem", &esq_obsItem },
	{ "DFeReferenciado", &esq_DFeReferenciado },
};

nfe_det *nfe_det_new(void)
{
	return (nfe_det *)calloc(1, sizeof(nfe_det));
}

void nfe_det_free(nfe_det *det)
{
	if (!det)
		return;
	nfe_prod_free(det->prod);
	nfe_imposto_free(det->imposto);
	nfe_grupo_free(det->grupo[0]);
	nfe_grupo_free(det->grupo[1]);
	nfe_grupo_free(det->grupo[2]);
	free(det);
}

#define EXIGE_DET(det)                                                         \
	do {                                                                   \
		if (!(det))                                                    \
			return E_ISNULL;                                       \
	} while (0)

int nfe_det_set_nitem(nfe_det *det, unsigned nitem)
{
	EXIGE_DET(det);
	if (nitem < 1 || nitem > NFE_MAX_ITENS)
		return E_VALOR;
	det->nItem = nitem;
	return 0;
}

int nfe_det_set_prod(nfe_det *det, nfe_prod *prod)
{
	EXIGE_DET(det);
	if (!prod)
		return E_ISNULL;
	if (det->prod != prod) {
		nfe_prod_free(det->prod);
		det->prod = prod;
	}
	return 0;
}

int nfe_det_set_imposto(nfe_det *det, nfe_imposto *imposto)
{
	EXIGE_DET(det);
	if (!imposto)
		return E_ISNULL;
	if (det->imposto != imposto) {
		nfe_imposto_free(det->imposto);
		det->imposto = imposto;
	}
	return 0;
}

int nfe_det_set_infadprod(nfe_det *det, const char *infadprod)
{
	EXIGE_DET(det);
	if (!infadprod) {
		det->infAdProd[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(det->infAdProd, sizeof det->infAdProd,
	                                infadprod, 1, NFE_TAM_INFADPROD);
}

int nfe_det_set_vitem(nfe_det *det, const char *vitem)
{
	EXIGE_DET(det);
	if (!vitem) {
		det->vItem[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(det->vItem, sizeof det->vItem, vitem,
	                        NFE_PADRAO_TDec_1302);
}

const nfe_prod *nfe_det_prod(const nfe_det *det)
{
	return det->prod;
}

const nfe_imposto *nfe_det_imposto(const nfe_det *det)
{
	return det->imposto;
}

nfe_grupo *nfe_det_grupo(nfe_det *det, const char *nome)
{
	int i;

	if (!det || !nome)
		return NULL;
	for (i = 0; i < 3; i++) {
		if (strcmp(nome, grupos[i].nome) != 0)
			continue;
		if (!det->grupo[i])
			det->grupo[i] = nfe_grupo_new(grupos[i].esq);
		return det->grupo[i];
	}
	return NULL;
}

/* Escreve o grupo genérico i, se tiver algum campo */
static int escreve_grupo(xmlTextWriterPtr writer, const nfe_det *det, int i)
{
	if (nfe_grupo_vazio(det->grupo[i]))
		return 0;
	return nfe_grupo_write_xml(writer, det->grupo[i]);
}

int nfe_det_write_xml(xmlTextWriterPtr writer, const nfe_det *det)
{
	int rc;

	if (!writer || !det)
		return E_ISNULL;
	if (det->nItem == 0 || !det->prod || !det->imposto)
		return E_VALOR;

	rc = nfe_abre(writer, "det");
	if (rc != 0)
		return rc;
	if (xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "nItem", "%u",
	                                      det->nItem) < 0)
		return E_XML;
	rc = nfe_prod_write_xml(writer, det->prod);
	if (rc != 0)
		return rc;
	rc = nfe_imposto_write_xml(writer, det->imposto);
	if (rc == 0)
		rc = escreve_grupo(writer, det, 0);
	if (rc != 0)
		return rc;
	if (det->infAdProd[0] != '\0')
		NFE_ESCREVE("infAdProd", "%s", det->infAdProd);
	rc = escreve_grupo(writer, det, 1);
	if (rc != 0)
		return rc;
	if (det->vItem[0] != '\0')
		NFE_ESCREVE("vItem", "%s", det->vItem);
	rc = escreve_grupo(writer, det, 2);
	if (rc != 0)
		return rc;
	return nfe_fecha(writer);
}
