/* Copyright (c) 2017-2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include <libnfe/refNF.h>
#include <libnfe/cnpjcpf.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/utils.h>

/* Campos do grupo, na ordem em que o leiaute os exige no XML */
enum campo_refnf {
	CAMPO_CUF,
	CAMPO_AAMM,
	CAMPO_CNPJ,
	CAMPO_MOD,
	CAMPO_SERIE,
	CAMPO_NNF,
	N_CAMPOS_REFNF
};

static const char *const tag_refnf[N_CAMPOS_REFNF] = {
	"cUF", "AAMM", "CNPJ", "mod", "serie", "nNF",
};

/* Todos os campos cabem em NFE_TAM_ASCII(NFE_TAM_CNPJ), o maior deles */
struct refNF_s {
	char campo[N_CAMPOS_REFNF][NFE_TAM_ASCII(NFE_TAM_CNPJ)];
};

static const char *campo_de(const struct refNF_s *nf, enum campo_refnf c)
{
	return nf ? nf->campo[c] : NULL;
}

/* Grava texto com min a max caracteres no campo c */
static int grava_texto(struct refNF_s *nf, enum campo_refnf c,
                       const char *texto, size_t min, size_t max)
{
	if (!nf)
		return E_ISNULL;
	return nfe_copia_texto(nf->campo[c], sizeof nf->campo[c], texto, min,
	                       max);
}

struct refNF_s *RefNFNew()
{
	return calloc(1, sizeof(struct refNF_s));
}

void RefNFDel(struct refNF_s *nf)
{
	free(nf);
}

int RefNFSetcUF(struct refNF_s *nf, nfe_uf uf)
{
	if (!nf)
		return E_ISNULL;
	/* Códigos IBGE de UF vão de 11 (RO) a 53 (DF) */
	if (uf < NFE_UF_RO || uf > NFE_UF_DF)
		return E_VALOR;
	snprintf(nf->campo[CAMPO_CUF], sizeof nf->campo[CAMPO_CUF], "%02d",
	         (int)uf);
	return 0;
}

const char *RefNFGetcUF(const struct refNF_s *nf)
{
	return campo_de(nf, CAMPO_CUF);
}

int RefNFSetAAMM(struct refNF_s *nf, const int ano, nfe_mes mes)
{
	if (!nf)
		return E_ISNULL;
	if (ano < 0 || ano > 99)
		return E_VALOR;
	if (mes < NFE_MES_JANEIRO || mes > NFE_MES_DEZEMBRO)
		return E_VALOR;
	snprintf(nf->campo[CAMPO_AAMM], sizeof nf->campo[CAMPO_AAMM],
	         "%02d%02d", ano, (int)mes);
	return 0;
}

const char *RefNFGetAAMM(const struct refNF_s *nf)
{
	return campo_de(nf, CAMPO_AAMM);
}

int RefNFSetCNPJ(struct refNF_s *nf, const char *cnpj)
{
	int erro;

	if (!nf)
		return E_ISNULL;
	erro = nfe_cnpj_validar(cnpj);
	if (erro)
		return erro;
	return grava_texto(nf, CAMPO_CNPJ, cnpj, NFE_TAM_CNPJ, NFE_TAM_CNPJ);
}

const char *RefNFGetCNPJ(const struct refNF_s *nf)
{
	return campo_de(nf, CAMPO_CNPJ);
}

int RefNFSetmod(struct refNF_s *nf, const char *mod)
{
	return grava_texto(nf, CAMPO_MOD, mod, NFE_TAM_MOD, NFE_TAM_MOD);
}

const char *RefNFGetmod(const struct refNF_s *nf)
{
	return campo_de(nf, CAMPO_MOD);
}

int RefNFSetSerie(struct refNF_s *nf, const char *serie)
{
	return grava_texto(nf, CAMPO_SERIE, serie, 1, NFE_TAM_SERIE);
}

const char *RefNFGetSerie(const struct refNF_s *nf)
{
	return campo_de(nf, CAMPO_SERIE);
}

int RefNFSetnNF(struct refNF_s *nf, const char *nnf)
{
	return grava_texto(nf, CAMPO_NNF, nnf, 1, NFE_TAM_NNF);
}

const char *RefNFGetnNF(const struct refNF_s *nf)
{
	return campo_de(nf, CAMPO_NNF);
}

int xmlGenRefNFNode(xmlTextWriterPtr writer, const struct refNF_s *nf)
{
	int c, rc;

	if (!writer || !nf)
		return E_ISNULL;
	rc = nfe_abre(writer, "refNF");
	if (rc)
		return rc;
	for (c = 0; c < N_CAMPOS_REFNF; c++)
		NFE_ESCREVE(tag_refnf[c], "%s", nf->campo[c]);
	return nfe_fecha(writer);
}
