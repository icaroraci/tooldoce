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
#include <string.h>

#include <libnfe/erros.h>
#include <libnfe/esquema.h>
#include <libnfe/imposto.h>

/* Os tributos ficam em um grupo genérico (esquema.h) com a estrutura de
 * <imposto> do leiaute; os setters específicos são atalhos para ele. */
struct nfe_imposto {
	nfe_grupo *g;
};

/* Um campo a gravar: caminho e valor (NULL: pula) */
struct campo_s {
	const char *caminho;
	const char *valor;
};

nfe_imposto *nfe_imposto_new(void)
{
	nfe_imposto *imp = (nfe_imposto *)calloc(1, sizeof(nfe_imposto));

	if (!imp)
		return NULL;
	imp->g = nfe_grupo_new(&esq_imposto);
	if (!imp->g) {
		free(imp);
		return NULL;
	}
	return imp;
}

void nfe_imposto_free(nfe_imposto *imp)
{
	if (!imp)
		return;
	nfe_grupo_free(imp->g);
	free(imp);
}

nfe_grupo *nfe_imposto_grupo(nfe_imposto *imp)
{
	return imp ? imp->g : NULL;
}

int nfe_imposto_set(nfe_imposto *imp, const char *caminho, const char *valor)
{
	if (!imp)
		return E_ISNULL;
	return nfe_grupo_set(imp->g, caminho, valor);
}

const char *nfe_imposto_get(const nfe_imposto *imp, const char *caminho)
{
	if (!imp)
		return NULL;
	return nfe_grupo_get(imp->g, caminho);
}

int nfe_imposto_remove(nfe_imposto *imp, const char *caminho)
{
	if (!imp)
		return E_ISNULL;
	return nfe_grupo_remove(imp->g, caminho);
}

/* Valida todos os campos; se todos forem aceitos, apaga o tributo e grava
 * os campos. Assim o imposto não muda em caso de erro. */
static int grava_grupo(nfe_imposto *imp, const char *tributo,
                       const struct campo_s *campos, int n)
{
	int i, rc;

	for (i = 0; i < n; i++) {
		if (!campos[i].valor)
			continue;
		rc = nfe_grupo_valida(imp->g, campos[i].caminho,
		                      campos[i].valor);
		if (rc != 0)
			return rc;
	}
	nfe_grupo_remove(imp->g, tributo);
	for (i = 0; i < n; i++) {
		if (!campos[i].valor)
			continue;
		rc = nfe_grupo_set(imp->g, campos[i].caminho, campos[i].valor);
		if (rc != 0) {
			nfe_grupo_remove(imp->g, tributo);
			return rc;
		}
	}
	return 0;
}

int nfe_imposto_set_vtottrib(nfe_imposto *imp, const char *vtottrib)
{
	return nfe_imposto_set(imp, "vTotTrib", vtottrib);
}

int nfe_imposto_set_icms00(nfe_imposto *imp, nfe_origem orig, nfe_mod_bc modbc,
                           const char *vbc, const char *picms,
                           const char *vicms, const char *pfcp,
                           const char *vfcp)
{
	char o[4], m[4];

	if (!imp || !vbc || !picms || !vicms)
		return E_ISNULL;
	if ((int)orig < 0 || (int)orig > 99 || (int)modbc < 0 ||
	    (int)modbc > 99 || (pfcp == NULL) != (vfcp == NULL))
		return E_VALOR;
	snprintf(o, sizeof o, "%d", (int)orig);
	snprintf(m, sizeof m, "%d", (int)modbc);
	{
		const struct campo_s c[] = {
			{ "ICMS00/orig", o },      { "ICMS00/modBC", m },
			{ "ICMS00/vBC", vbc },     { "ICMS00/pICMS", picms },
			{ "ICMS00/vICMS", vicms }, { "ICMS00/pFCP", pfcp },
			{ "ICMS00/vFCP", vfcp },
		};
		return grava_grupo(imp, "ICMS", c, 7);
	}
}

int nfe_imposto_set_icmssn102(nfe_imposto *imp, nfe_origem orig,
                              nfe_csosn_102 csosn)
{
	char o[4], c[8];

	if (!imp)
		return E_ISNULL;
	if ((int)orig < NFE_ORIGEM_NAO_INFORMADA || (int)orig > 99 ||
	    (int)csosn < 0 || (int)csosn > 999)
		return E_VALOR;
	snprintf(o, sizeof o, "%d", (int)orig);
	snprintf(c, sizeof c, "%d", (int)csosn);
	{
		const struct campo_s campos[] = {
			{ "ICMSSN102/orig",
			  orig == NFE_ORIGEM_NAO_INFORMADA ? NULL : o },
			{ "ICMSSN102/CSOSN", c },
		};
		return grava_grupo(imp, "ICMS", campos, 2);
	}
}

/* PISAliq/COFINSAliq; tributo é "PIS" ou "COFINS" */
static int set_aliq(nfe_imposto *imp, const char *tributo,
                    nfe_cst_pis_cofins cst, const char *vbc, const char *p,
                    const char *v)
{
	char c[4], cam[4][32];

	if (!imp || !vbc || !p || !v)
		return E_ISNULL;
	if ((int)cst < 0 || (int)cst > 99)
		return E_VALOR;
	snprintf(c, sizeof c, "%02d", (int)cst);
	snprintf(cam[0], sizeof cam[0], "%sAliq/CST", tributo);
	snprintf(cam[1], sizeof cam[1], "%sAliq/vBC", tributo);
	snprintf(cam[2], sizeof cam[2], "%sAliq/p%s", tributo, tributo);
	snprintf(cam[3], sizeof cam[3], "%sAliq/v%s", tributo, tributo);
	{
		const struct campo_s campos[] = {
			{ cam[0], c },
			{ cam[1], vbc },
			{ cam[2], p },
			{ cam[3], v },
		};
		return grava_grupo(imp, tributo, campos, 4);
	}
}

static int set_nt(nfe_imposto *imp, const char *tributo, nfe_cst_pis_cofins cst)
{
	char c[4], cam[32];

	if (!imp)
		return E_ISNULL;
	if ((int)cst < 0 || (int)cst > 99)
		return E_VALOR;
	snprintf(c, sizeof c, "%02d", (int)cst);
	snprintf(cam, sizeof cam, "%sNT/CST", tributo);
	{
		const struct campo_s campos[] = { { cam, c } };
		return grava_grupo(imp, tributo, campos, 1);
	}
}

int nfe_imposto_set_pisaliq(nfe_imposto *imp, nfe_cst_pis_cofins cst,
                            const char *vbc, const char *ppis, const char *vpis)
{
	return set_aliq(imp, "PIS", cst, vbc, ppis, vpis);
}

int nfe_imposto_set_cofinsaliq(nfe_imposto *imp, nfe_cst_pis_cofins cst,
                               const char *vbc, const char *pcofins,
                               const char *vcofins)
{
	return set_aliq(imp, "COFINS", cst, vbc, pcofins, vcofins);
}

int nfe_imposto_set_pisnt(nfe_imposto *imp, nfe_cst_pis_cofins cst)
{
	return set_nt(imp, "PIS", cst);
}

int nfe_imposto_set_cofinsnt(nfe_imposto *imp, nfe_cst_pis_cofins cst)
{
	return set_nt(imp, "COFINS", cst);
}

int nfe_imposto_remove_icms(nfe_imposto *imp)
{
	return nfe_imposto_remove(imp, "ICMS");
}

int nfe_imposto_remove_pis(nfe_imposto *imp)
{
	return nfe_imposto_remove(imp, "PIS");
}

int nfe_imposto_remove_cofins(nfe_imposto *imp)
{
	return nfe_imposto_remove(imp, "COFINS");
}

const char *nfe_imposto_valor(const nfe_imposto *imp,
                              enum nfe_imposto_valor_e campo)
{
	static const char *const caminhos[] = {
		"ICMS/vBC",        "ICMS/vICMS", "ICMS/vFCP",    "PIS/vPIS",
		"COFINS/vCOFINS",  "vTotTrib",   "ICMS/vICMSST", "ICMS/vFCPST",
		"ICMS/vICMSDeson", "II/vII",     "IPI/vIPI",     "ICMS/vBCST",
	};
	const char *v;

	if ((int)campo < 0 ||
	    (int)campo >= (int)(sizeof caminhos / sizeof caminhos[0]))
		return "";
	v = nfe_grupo_get(imp->g, caminhos[campo]);
	return v ? v : "";
}

int nfe_imposto_write_xml(xmlTextWriterPtr writer, const nfe_imposto *imp)
{
	if (!writer || !imp)
		return E_ISNULL;
	return nfe_grupo_write_xml(writer, imp->g);
}
