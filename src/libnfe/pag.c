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

#include <libnfe/cnpjcpf.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/padroes.h>
#include <libnfe/pag.h>
#include <libnfe/valida.h>

struct nfe_detpag {
	nfe_forma_pagamento indPag;
	int tPag;                                  /* -1: não informado */
	char xPag[NFE_TAM_UTF8(NFE_TAM_XPAG)];     /* "": não informado */
	char vPag[NFE_TAM_ASCII(NFE_TAM_DEC)];     /* "": não informado */
	char dPag[NFE_TAM_ASCII(NFE_TAM_DATA)];    /* "": não informado */
	char CNPJPag[NFE_TAM_ASCII(NFE_TAM_CNPJ)]; /* "": não informado */
	char UFPag[NFE_TAM_ASCII(2)];
	/* card */
	int card; /* grupo card informado */
	nfe_integracao tpIntegra;
	char CNPJ[NFE_TAM_ASCII(NFE_TAM_CNPJ)]; /* "": não informado */
	unsigned tBand;                         /* 0: não informado */
	char cAut[NFE_TAM_UTF8(NFE_TAM_CAUT)];  /* "": não informado */
	char CNPJReceb[NFE_TAM_ASCII(NFE_TAM_CNPJ)];
	char idTermPag[NFE_TAM_UTF8(NFE_TAM_IDTERMPAG)];
};

struct nfe_pag {
	nfe_detpag *detPag[NFE_MAX_DETPAG];
	int nDetPag;
	char vTroco[NFE_TAM_ASCII(NFE_TAM_DEC)]; /* "": não informado */
};

#define EXIGE(obj)                                                             \
	do {                                                                   \
		if (!(obj))                                                    \
			return E_ISNULL;                                       \
	} while (0)

nfe_detpag *nfe_detpag_new(void)
{
	nfe_detpag *dp = (nfe_detpag *)calloc(1, sizeof(nfe_detpag));

	if (!dp)
		return NULL;
	dp->indPag = NFE_PAGAMENTO_NAO_INFORMADO;
	dp->tPag = -1;
	return dp;
}

void nfe_detpag_free(nfe_detpag *dp)
{
	free(dp);
}

int nfe_detpag_set_indpag(nfe_detpag *dp, nfe_forma_pagamento indpag)
{
	EXIGE(dp);
	if (indpag < NFE_PAGAMENTO_NAO_INFORMADO ||
	    indpag > NFE_PAGAMENTO_PRAZO)
		return E_VALOR;
	dp->indPag = indpag;
	return 0;
}

int nfe_detpag_set_tpag(nfe_detpag *dp, nfe_meio_pagamento tpag)
{
	EXIGE(dp);
	if ((int)tpag < 0 || (int)tpag > 99)
		return E_VALOR;
	dp->tPag = (int)tpag;
	return 0;
}

int nfe_detpag_set_xpag(nfe_detpag *dp, const char *xpag)
{
	EXIGE(dp);
	if (!xpag) {
		dp->xPag[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(dp->xPag, sizeof dp->xPag, xpag, 2,
	                                NFE_TAM_XPAG);
}

int nfe_detpag_set_vpag(nfe_detpag *dp, const char *vpag)
{
	EXIGE(dp);
	return nfe_copia_padrao(dp->vPag, sizeof dp->vPag, vpag,
	                        NFE_PADRAO_TDec_1302);
}

int nfe_detpag_set_dpag(nfe_detpag *dp, const char *dpag)
{
	EXIGE(dp);
	if (!dpag) {
		dp->dPag[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(dp->dPag, sizeof dp->dPag, dpag,
	                        NFE_PADRAO_TData);
}

/* CNPJ opcional: NULL vira "" */
static int cnpj_opcional(const char *cnpj)
{
	return cnpj ? nfe_cnpj_validar(cnpj) : 0;
}

int nfe_detpag_set_local(nfe_detpag *dp, const char *cnpjpag, const char *ufpag)
{
	static const char *const ufs[] = { NFE_VALORES_TUfEmi, NULL };
	int rc;

	EXIGE(dp);
	if (!cnpjpag && !ufpag) {
		dp->CNPJPag[0] = '\0';
		dp->UFPag[0] = '\0';
		return 0;
	}
	if (!cnpjpag || !ufpag)
		return E_VALOR;
	rc = nfe_cnpj_validar(cnpjpag);
	if (rc == 0)
		rc = nfe_valida_lista(ufpag, ufs);
	if (rc != 0)
		return rc;
	memcpy(dp->CNPJPag, cnpjpag, sizeof dp->CNPJPag);
	memcpy(dp->UFPag, ufpag, sizeof dp->UFPag);
	return 0;
}

int nfe_detpag_set_card(nfe_detpag *dp, nfe_integracao tpintegra,
                        const char *cnpj, unsigned tband, const char *caut,
                        const char *cnpjreceb, const char *idtermpag)
{
	int rc;

	EXIGE(dp);
	if (tpintegra != NFE_INTEGRACAO_TEF && tpintegra != NFE_INTEGRACAO_POS)
		return E_VALOR;
	if (tband > 99)
		return E_VALOR;
	rc = cnpj_opcional(cnpj);
	if (rc == 0)
		rc = cnpj_opcional(cnpjreceb);
	if (rc == 0 && caut)
		rc = nfe_valida_texto(caut, 1, NFE_TAM_CAUT);
	if (rc == 0 && idtermpag)
		rc = nfe_valida_texto(idtermpag, 1, NFE_TAM_IDTERMPAG);
	if (rc != 0)
		return rc;
	/* Tudo validado; os tamanhos já foram conferidos */
	dp->card = 1;
	dp->tpIntegra = tpintegra;
	strcpy(dp->CNPJ, cnpj ? cnpj : "");
	dp->tBand = tband;
	strcpy(dp->cAut, caut ? caut : "");
	strcpy(dp->CNPJReceb, cnpjreceb ? cnpjreceb : "");
	strcpy(dp->idTermPag, idtermpag ? idtermpag : "");
	return 0;
}

int nfe_detpag_remove_card(nfe_detpag *dp)
{
	EXIGE(dp);
	dp->card = 0;
	return 0;
}

nfe_pag *nfe_pag_new(void)
{
	return (nfe_pag *)calloc(1, sizeof(nfe_pag));
}

void nfe_pag_free(nfe_pag *pag)
{
	int i;

	if (!pag)
		return;
	for (i = 0; i < pag->nDetPag; i++)
		nfe_detpag_free(pag->detPag[i]);
	free(pag);
}

int nfe_pag_add_detpag(nfe_pag *pag, nfe_detpag *dp)
{
	EXIGE(pag);
	EXIGE(dp);
	if (pag->nDetPag >= NFE_MAX_DETPAG)
		return E_VALOR;
	pag->detPag[pag->nDetPag++] = dp;
	return 0;
}

int nfe_pag_set_vtroco(nfe_pag *pag, const char *vtroco)
{
	EXIGE(pag);
	if (!vtroco) {
		pag->vTroco[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(pag->vTroco, sizeof pag->vTroco, vtroco,
	                        NFE_PADRAO_TDec_1302);
}

/* Escreve <tag>valor</tag> se valor não for vazio */
static int opcional(xmlTextWriterPtr writer, const char *tag, const char *valor)
{
	return valor[0] ? nfe_escreve(writer, tag, "%s", valor) : 0;
}

#define OPCIONAL(tag, valor)                                                   \
	do {                                                                   \
		rc = opcional(writer, (tag), (valor));                         \
		if (rc != 0)                                                   \
			return rc;                                             \
	} while (0)

static int escreve_detpag(xmlTextWriterPtr writer, const nfe_detpag *dp)
{
	int rc;

	rc = nfe_abre(writer, "detPag");
	if (rc != 0)
		return rc;
	if (dp->indPag != NFE_PAGAMENTO_NAO_INFORMADO)
		NFE_ESCREVE("indPag", "%d", (int)dp->indPag);
	NFE_ESCREVE("tPag", "%02d", dp->tPag);
	OPCIONAL("xPag", dp->xPag);
	NFE_ESCREVE("vPag", "%s", dp->vPag);
	OPCIONAL("dPag", dp->dPag);
	if (dp->CNPJPag[0] != '\0') {
		NFE_ESCREVE("CNPJPag", "%s", dp->CNPJPag);
		NFE_ESCREVE("UFPag", "%s", dp->UFPag);
	}
	if (dp->card) {
		rc = nfe_abre(writer, "card");
		if (rc != 0)
			return rc;
		NFE_ESCREVE("tpIntegra", "%d", (int)dp->tpIntegra);
		OPCIONAL("CNPJ", dp->CNPJ);
		if (dp->tBand != 0)
			NFE_ESCREVE("tBand", "%02u", dp->tBand);
		OPCIONAL("cAut", dp->cAut);
		OPCIONAL("CNPJReceb", dp->CNPJReceb);
		OPCIONAL("idTermPag", dp->idTermPag);
		rc = nfe_fecha(writer);
		if (rc != 0)
			return rc;
	}
	return nfe_fecha(writer);
}

int nfe_pag_write_xml(xmlTextWriterPtr writer, const nfe_pag *pag)
{
	int i, rc;

	if (!writer || !pag)
		return E_ISNULL;
	if (pag->nDetPag == 0)
		return E_VALOR;
	for (i = 0; i < pag->nDetPag; i++)
		if (pag->detPag[i]->tPag < 0 || pag->detPag[i]->vPag[0] == '\0')
			return E_VALOR;

	rc = nfe_abre(writer, "pag");
	if (rc != 0)
		return rc;
	for (i = 0; i < pag->nDetPag; i++) {
		rc = escreve_detpag(writer, pag->detPag[i]);
		if (rc != 0)
			return rc;
	}
	OPCIONAL("vTroco", pag->vTroco);
	return nfe_fecha(writer);
}
