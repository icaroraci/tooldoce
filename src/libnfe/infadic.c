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
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/infadic.h>
#include <libnfe/valida.h>

struct obs_s {
	char xCampo[NFE_TAM_UTF8(NFE_TAM_XCAMPO)];
	char xTexto[NFE_TAM_UTF8(NFE_TAM_XTEXTO)];
};

struct procref_s {
	char nProc[NFE_TAM_UTF8(NFE_TAM_NPROC)];
	nfe_origem_processo indProc;
	nfe_ato_concessorio tpAto;
};

struct nfe_infadic {
	char *infAdFisco; /* NULL: não informado */
	char *infCpl;
	struct obs_s obsCont[NFE_MAX_OBS], obsFisco[NFE_MAX_OBS];
	int nObsCont, nObsFisco;
	struct procref_s procRef[NFE_MAX_PROCREF];
	int nProcRef;
};

nfe_infadic *nfe_infadic_new(void)
{
	return (nfe_infadic *)calloc(1, sizeof(nfe_infadic));
}

void nfe_infadic_free(nfe_infadic *inf)
{
	if (!inf)
		return;
	free(inf->infAdFisco);
	free(inf->infCpl);
	free(inf);
}

/* Texto longo alocado sob medida; NULL remove */
static int texto_longo(char **dst, const char *valor, size_t max)
{
	char *novo;
	int rc;

	if (!valor) {
		free(*dst);
		*dst = NULL;
		return 0;
	}
	rc = nfe_valida_texto(valor, 1, max);
	if (rc != 0)
		return rc;
	novo = (char *)malloc(strlen(valor) + 1);
	if (!novo)
		return E_MALLOC;
	strcpy(novo, valor);
	free(*dst);
	*dst = novo;
	return 0;
}

int nfe_infadic_set_infadfisco(nfe_infadic *inf, const char *texto)
{
	if (!inf)
		return E_ISNULL;
	return texto_longo(&inf->infAdFisco, texto, NFE_TAM_INFADFISCO);
}

int nfe_infadic_set_infcpl(nfe_infadic *inf, const char *texto)
{
	if (!inf)
		return E_ISNULL;
	return texto_longo(&inf->infCpl, texto, NFE_TAM_INFCPL);
}

static int add_obs(struct obs_s *lista, int *n, const char *xcampo,
                   const char *xtexto)
{
	int rc;

	if (*n >= NFE_MAX_OBS)
		return E_VALOR;
	rc = nfe_valida_texto(xcampo, 1, NFE_TAM_XCAMPO);
	if (rc == 0)
		rc = nfe_valida_texto(xtexto, 1, NFE_TAM_XTEXTO);
	if (rc != 0)
		return rc;
	strcpy(lista[*n].xCampo, xcampo);
	strcpy(lista[*n].xTexto, xtexto);
	(*n)++;
	return 0;
}

int nfe_infadic_add_obscont(nfe_infadic *inf, const char *xcampo,
                            const char *xtexto)
{
	if (!inf)
		return E_ISNULL;
	return add_obs(inf->obsCont, &inf->nObsCont, xcampo, xtexto);
}

int nfe_infadic_add_obsfisco(nfe_infadic *inf, const char *xcampo,
                             const char *xtexto)
{
	if (!inf)
		return E_ISNULL;
	return add_obs(inf->obsFisco, &inf->nObsFisco, xcampo, xtexto);
}

int nfe_infadic_add_procref(nfe_infadic *inf, const char *nproc,
                            nfe_origem_processo indproc,
                            nfe_ato_concessorio tpato)
{
	struct procref_s *p;
	int rc;

	if (!inf)
		return E_ISNULL;
	if (inf->nProcRef >= NFE_MAX_PROCREF || indproc < NFE_PROCESSO_SEFAZ ||
	    indproc > NFE_PROCESSO_CONFAZ)
		return E_VALOR;
	switch (tpato) {
	case NFE_ATO_NAO_INFORMADO:
	case NFE_ATO_TERMO_ACORDO:
	case NFE_ATO_REGIME_ESPECIAL:
	case NFE_ATO_AUTORIZACAO_ESPECIFICA:
	case NFE_ATO_AJUSTE_SINIEF:
	case NFE_ATO_CONVENIO_ICMS:
		break;
	default:
		return E_VALOR;
	}
	rc = nfe_valida_texto(nproc, 1, NFE_TAM_NPROC);
	if (rc != 0)
		return rc;
	p = &inf->procRef[inf->nProcRef++];
	strcpy(p->nProc, nproc);
	p->indProc = indproc;
	p->tpAto = tpato;
	return 0;
}

int nfe_infadic_remove_obs(nfe_infadic *inf)
{
	if (!inf)
		return E_ISNULL;
	inf->nObsCont = inf->nObsFisco = 0;
	return 0;
}

int nfe_infadic_remove_procref(nfe_infadic *inf)
{
	if (!inf)
		return E_ISNULL;
	inf->nProcRef = 0;
	return 0;
}

static int escreve_obs(xmlTextWriterPtr writer, const char *tag,
                       const struct obs_s *obs)
{
	int rc;

	rc = nfe_abre(writer, tag);
	if (rc != 0)
		return rc;
	if (xmlTextWriterWriteAttribute(writer, BAD_CAST "xCampo",
	                                BAD_CAST obs->xCampo) < 0)
		return E_XML;
	NFE_ESCREVE("xTexto", "%s", obs->xTexto);
	return nfe_fecha(writer);
}

int nfe_infadic_write_xml(xmlTextWriterPtr writer, const nfe_infadic *inf)
{
	int i, rc;

	if (!writer || !inf)
		return E_ISNULL;
	rc = nfe_abre(writer, "infAdic");
	if (rc != 0)
		return rc;
	if (inf->infAdFisco)
		NFE_ESCREVE("infAdFisco", "%s", inf->infAdFisco);
	if (inf->infCpl)
		NFE_ESCREVE("infCpl", "%s", inf->infCpl);
	for (i = 0; rc == 0 && i < inf->nObsCont; i++)
		rc = escreve_obs(writer, "obsCont", &inf->obsCont[i]);
	for (i = 0; rc == 0 && i < inf->nObsFisco; i++)
		rc = escreve_obs(writer, "obsFisco", &inf->obsFisco[i]);
	for (i = 0; rc == 0 && i < inf->nProcRef; i++) {
		const struct procref_s *p = &inf->procRef[i];

		rc = nfe_abre(writer, "procRef");
		if (rc != 0)
			return rc;
		NFE_ESCREVE("nProc", "%s", p->nProc);
		NFE_ESCREVE("indProc", "%d", (int)p->indProc);
		if (p->tpAto != NFE_ATO_NAO_INFORMADO)
			NFE_ESCREVE("tpAto", "%02d", (int)p->tpAto);
		rc = nfe_fecha(writer);
	}
	if (rc != 0)
		return rc;
	return nfe_fecha(writer);
}
