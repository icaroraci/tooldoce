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
#include <libnfe/padroes.h>
#include <libnfe/total.h>
#include <libnfe/valida.h>

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

typedef char decimal[NFE_TAM_ASCII(NFE_TAM_DEC)];

struct nfe_total {
	decimal icmstot[NFE_TOT_QUANTIDADE]; /* "": não informado */
	decimal vNFTot;                      /* "": não informado */
};

/* Valor inicial do campo: "0.00" nos obrigatórios, "" nos opcionais */
static void inicial(nfe_total *tot, int campo)
{
	strcpy(tot->icmstot[campo], campos[campo].opcional ? "" : "0.00");
}

nfe_total *nfe_total_new(void)
{
	nfe_total *tot = (nfe_total *)calloc(1, sizeof(nfe_total));
	int i;

	if (!tot)
		return NULL;
	for (i = 0; i < NFE_TOT_QUANTIDADE; i++)
		inicial(tot, i);
	return tot;
}

void nfe_total_free(nfe_total *tot)
{
	free(tot);
}

int nfe_total_set_icmstot(nfe_total *tot, nfe_campo_icmstot campo,
                          const char *valor)
{
	if (!tot)
		return E_ISNULL;
	if ((int)campo < 0 || campo >= NFE_TOT_QUANTIDADE)
		return E_VALOR;
	if (!valor) {
		inicial(tot, (int)campo);
		return 0;
	}
	return nfe_copia_padrao(tot->icmstot[campo], sizeof tot->icmstot[0],
	                        valor, NFE_PADRAO_TDec_1302);
}

int nfe_total_set_vnftot(nfe_total *tot, const char *vnftot)
{
	if (!tot)
		return E_ISNULL;
	if (!vnftot) {
		tot->vNFTot[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(tot->vNFTot, sizeof tot->vNFTot, vnftot,
	                        NFE_PADRAO_TDec_1302);
}

const char *nfe_total_valor(const nfe_total *tot, nfe_campo_icmstot campo)
{
	return tot->icmstot[campo];
}

int nfe_total_write_xml(xmlTextWriterPtr writer, const nfe_total *tot)
{
	int i, rc;

	if (!writer || !tot)
		return E_ISNULL;
	rc = nfe_abre(writer, "total");
	if (rc == 0)
		rc = nfe_abre(writer, "ICMSTot");
	if (rc != 0)
		return rc;
	for (i = 0; i < NFE_TOT_QUANTIDADE; i++)
		if (tot->icmstot[i][0] != '\0')
			NFE_ESCREVE(campos[i].tag, "%s", tot->icmstot[i]);
	rc = nfe_fecha(writer);
	if (rc != 0)
		return rc;
	if (tot->vNFTot[0] != '\0')
		NFE_ESCREVE("vNFTot", "%s", tot->vNFTot);
	return nfe_fecha(writer);
}
