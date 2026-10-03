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

#include <stdio.h>
#include <stdlib.h>

#include <libnfe/erros.h>
#include <libnfe/esquema.h>
#include <libnfe/total.h>

/* Tag e obrigatoriedade de cada campo, na ordem de nfe_campo_icmstot */
static const struct {
	const char *tag;
	int opcional;
} campos[NFE_TOT_QUANTIDADE] = {
	{ "vBC", 0 },        { "vICMS", 0 },        { "vICMSDeson", 0 },
	{ "vFCPUFDest", 1 }, { "vICMSUFDest", 1 },  { "vICMSUFRemet", 1 },
	{ "vFCP", 0 },       { "vBCST", 0 },        { "vST", 0 },
	{ "vFCPST", 0 },     { "vFCPSTRet", 0 },    { "qBCMono", 1 },
	{ "vICMSMono", 1 },  { "qBCMonoReten", 1 }, { "vICMSMonoReten", 1 },
	{ "qBCMonoRet", 1 }, { "vICMSMonoRet", 1 }, { "vProd", 0 },
	{ "vFrete", 0 },     { "vSeg", 0 },         { "vDesc", 0 },
	{ "vII", 0 },        { "vIPI", 0 },         { "vIPIDevol", 0 },
	{ "vPIS", 0 },       { "vCOFINS", 0 },      { "vOutro", 0 },
	{ "vNF", 0 },        { "vTotTrib", 1 },
};

/* Os campos ficam em um grupo genérico (esquema.h) com a estrutura de
 * <total> do leiaute; os setters específicos são atalhos para ele. */
struct nfe_total {
	nfe_grupo *g;
};

/* Caminho "ICMSTot/<tag>" do campo */
static void caminho(char *dst, size_t tam, int campo)
{
	snprintf(dst, tam, "ICMSTot/%s", campos[campo].tag);
}

/* Valor inicial do campo: "0.00" nos obrigatórios, nenhum nos opcionais */
static int inicial(nfe_total *tot, int campo)
{
	char c[32];

	caminho(c, sizeof c, campo);
	return nfe_grupo_set(tot->g, c, campos[campo].opcional ? NULL : "0.00");
}

nfe_total *nfe_total_new(void)
{
	nfe_total *tot = (nfe_total *)calloc(1, sizeof(nfe_total));
	int i;

	if (!tot)
		return NULL;
	tot->g = nfe_grupo_new(&esq_total);
	if (!tot->g) {
		free(tot);
		return NULL;
	}
	for (i = 0; i < NFE_TOT_QUANTIDADE; i++) {
		if (inicial(tot, i) != 0) {
			nfe_total_free(tot);
			return NULL;
		}
	}
	return tot;
}

void nfe_total_free(nfe_total *tot)
{
	if (!tot)
		return;
	nfe_grupo_free(tot->g);
	free(tot);
}

nfe_grupo *nfe_total_grupo(nfe_total *tot)
{
	return tot ? tot->g : NULL;
}

int nfe_total_set_icmstot(nfe_total *tot, nfe_campo_icmstot campo,
                          const char *valor)
{
	char c[32];

	if (!tot)
		return E_ISNULL;
	if ((int)campo < 0 || campo >= NFE_TOT_QUANTIDADE)
		return E_VALOR;
	if (!valor)
		return inicial(tot, (int)campo);
	caminho(c, sizeof c, (int)campo);
	return nfe_grupo_set(tot->g, c, valor);
}

int nfe_total_set_vnftot(nfe_total *tot, const char *vnftot)
{
	if (!tot)
		return E_ISNULL;
	return nfe_grupo_set(tot->g, "vNFTot", vnftot);
}

const char *nfe_total_valor(const nfe_total *tot, nfe_campo_icmstot campo)
{
	char c[32];
	const char *v;

	caminho(c, sizeof c, (int)campo);
	v = nfe_grupo_get(tot->g, c);
	return v ? v : "";
}

int nfe_total_write_xml(xmlTextWriterPtr writer, const nfe_total *tot)
{
	if (!writer || !tot)
		return E_ISNULL;
	return nfe_grupo_write_xml(writer, tot->g);
}
