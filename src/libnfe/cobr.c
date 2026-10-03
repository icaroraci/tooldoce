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

#include <libnfe/cobr.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

typedef char decimal[NFE_TAM_ASCII(NFE_TAM_DEC)];

struct dup_s {
	char nDup[NFE_TAM_UTF8(NFE_TAM_NFAT)]; /* "": não informado */
	char dVenc[NFE_TAM_ASCII(NFE_TAM_DATA)];
	decimal vDup;
};

struct nfe_cobr {
	int fat; /* grupo fat informado */
	char nFat[NFE_TAM_UTF8(NFE_TAM_NFAT)];
	decimal vOrig, vDesc, vLiq; /* "": não informados */
	struct dup_s dup[NFE_MAX_DUP];
	int nDup;
};

nfe_cobr *nfe_cobr_new(void)
{
	return (nfe_cobr *)calloc(1, sizeof(nfe_cobr));
}

void nfe_cobr_free(nfe_cobr *cobr)
{
	free(cobr);
}

/* Confere um valor opcional (NULL passa) */
static int confere(const char *valor, const char *padrao)
{
	return valor ? nfe_valida_padrao(valor, padrao) : 0;
}

int nfe_cobr_set_fat(nfe_cobr *cobr, const char *nfat, const char *vorig,
                     const char *vdesc, const char *vliq)
{
	int rc = 0;

	if (!cobr)
		return E_ISNULL;
	if (nfat)
		rc = nfe_valida_texto(nfat, 1, NFE_TAM_NFAT);
	if (rc == 0)
		rc = confere(vorig, NFE_PADRAO_TDec_1302);
	if (rc == 0)
		rc = confere(vdesc, NFE_PADRAO_TDec_1302);
	if (rc == 0)
		rc = confere(vliq, NFE_PADRAO_TDec_1302);
	if (rc != 0)
		return rc;
	cobr->fat = nfat || vorig || vdesc || vliq;
	strcpy(cobr->nFat, nfat ? nfat : "");
	strcpy(cobr->vOrig, vorig ? vorig : "");
	strcpy(cobr->vDesc, vdesc ? vdesc : "");
	strcpy(cobr->vLiq, vliq ? vliq : "");
	return 0;
}

int nfe_cobr_add_dup(nfe_cobr *cobr, const char *ndup, const char *dvenc,
                     const char *vdup)
{
	struct dup_s *d;
	int rc = 0;

	if (!cobr)
		return E_ISNULL;
	if (cobr->nDup >= NFE_MAX_DUP)
		return E_VALOR;
	if (ndup)
		rc = nfe_valida_texto(ndup, 1, NFE_TAM_NFAT);
	if (rc == 0)
		rc = confere(dvenc, NFE_PADRAO_TData);
	if (rc == 0)
		rc = nfe_valida_padrao(vdup, NFE_PADRAO_TDec_1302Opc);
	if (rc != 0)
		return rc;
	d = &cobr->dup[cobr->nDup++];
	strcpy(d->nDup, ndup ? ndup : "");
	strcpy(d->dVenc, dvenc ? dvenc : "");
	strcpy(d->vDup, vdup);
	return 0;
}

int nfe_cobr_remove_dup(nfe_cobr *cobr)
{
	if (!cobr)
		return E_ISNULL;
	cobr->nDup = 0;
	return 0;
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

int nfe_cobr_write_xml(xmlTextWriterPtr writer, const nfe_cobr *cobr)
{
	int i, rc;

	if (!writer || !cobr)
		return E_ISNULL;
	rc = nfe_abre(writer, "cobr");
	if (rc != 0)
		return rc;
	if (cobr->fat) {
		rc = nfe_abre(writer, "fat");
		if (rc != 0)
			return rc;
		OPCIONAL("nFat", cobr->nFat);
		OPCIONAL("vOrig", cobr->vOrig);
		OPCIONAL("vDesc", cobr->vDesc);
		OPCIONAL("vLiq", cobr->vLiq);
		rc = nfe_fecha(writer);
		if (rc != 0)
			return rc;
	}
	for (i = 0; i < cobr->nDup; i++) {
		rc = nfe_abre(writer, "dup");
		if (rc != 0)
			return rc;
		OPCIONAL("nDup", cobr->dup[i].nDup);
		OPCIONAL("dVenc", cobr->dup[i].dVenc);
		NFE_ESCREVE("vDup", "%s", cobr->dup[i].vDup);
		rc = nfe_fecha(writer);
		if (rc != 0)
			return rc;
	}
	return nfe_fecha(writer);
}
