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
#include <libnfe/imposto.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

typedef char decimal[NFE_TAM_ASCII(NFE_TAM_DEC)];

enum tipo_icms_e { ICMS_NENHUM, ICMS_00, ICMS_SN102 };
enum tipo_pc_e { PC_NENHUM, PC_ALIQ, PC_NT };

/* Grupo do PIS ou da COFINS */
struct pis_cofins_s {
	enum tipo_pc_e tipo;
	nfe_cst_pis_cofins CST;
	decimal vBC, p, v;
};

struct nfe_imposto {
	decimal vTotTrib; /* "": não informado */
	enum tipo_icms_e tipoICMS;
	nfe_origem orig;
	int CST; /* CST (ICMS00) ou CSOSN (ICMSSN102) */
	nfe_mod_bc modBC;
	decimal vBC, pICMS, vICMS;
	decimal pFCP, vFCP; /* "": não informados */
	struct pis_cofins_s PIS, COFINS;
};

nfe_imposto *nfe_imposto_new(void)
{
	return (nfe_imposto *)calloc(1, sizeof(nfe_imposto));
}

void nfe_imposto_free(nfe_imposto *imp)
{
	free(imp);
}

#define EXIGE_IMP(imp)                                                         \
	do {                                                                   \
		if (!(imp))                                                    \
			return E_ISNULL;                                       \
	} while (0)

/* Copia um decimal já validado (o padrão limita o tamanho) */
static void copia(decimal dst, const char *valor)
{
	strcpy(dst, valor);
}

int nfe_imposto_set_vtottrib(nfe_imposto *imp, const char *vtottrib)
{
	EXIGE_IMP(imp);
	if (!vtottrib) {
		imp->vTotTrib[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(imp->vTotTrib, sizeof imp->vTotTrib, vtottrib,
	                        NFE_PADRAO_TDec_1302);
}

int nfe_imposto_set_icms00(nfe_imposto *imp, nfe_origem orig, nfe_mod_bc modbc,
                           const char *vbc, const char *picms,
                           const char *vicms, const char *pfcp,
                           const char *vfcp)
{
	int rc;

	EXIGE_IMP(imp);
	if (orig < NFE_ORIGEM_NACIONAL ||
	    orig > NFE_ORIGEM_NACIONAL_IMPORTACAO_ACIMA_70 ||
	    modbc < NFE_MOD_BC_MVA || modbc > NFE_MOD_BC_VALOR_OPERACAO ||
	    (pfcp == NULL) != (vfcp == NULL))
		return E_VALOR;
	rc = nfe_valida_padrao(vbc, NFE_PADRAO_TDec_1302);
	if (rc == 0)
		rc = nfe_valida_padrao(picms, NFE_PADRAO_TDec_0302a04);
	if (rc == 0)
		rc = nfe_valida_padrao(vicms, NFE_PADRAO_TDec_1302);
	if (rc == 0 && pfcp)
		rc = nfe_valida_padrao(pfcp, NFE_PADRAO_TDec_0302a04Opc);
	if (rc == 0 && vfcp)
		rc = nfe_valida_padrao(vfcp, NFE_PADRAO_TDec_1302);
	if (rc != 0)
		return rc;

	imp->tipoICMS = ICMS_00;
	imp->orig = orig;
	imp->CST = 0;
	imp->modBC = modbc;
	copia(imp->vBC, vbc);
	copia(imp->pICMS, picms);
	copia(imp->vICMS, vicms);
	copia(imp->pFCP, pfcp ? pfcp : "");
	copia(imp->vFCP, vfcp ? vfcp : "");
	return 0;
}

int nfe_imposto_set_icmssn102(nfe_imposto *imp, nfe_origem orig,
                              nfe_csosn_102 csosn)
{
	EXIGE_IMP(imp);
	if (orig < NFE_ORIGEM_NAO_INFORMADA ||
	    orig > NFE_ORIGEM_NACIONAL_IMPORTACAO_ACIMA_70)
		return E_VALOR;
	switch (csosn) {
	case NFE_CSOSN_102:
	case NFE_CSOSN_103:
	case NFE_CSOSN_300:
	case NFE_CSOSN_400:
		break;
	default:
		return E_VALOR;
	}
	imp->tipoICMS = ICMS_SN102;
	imp->orig = orig;
	imp->CST = (int)csosn;
	return 0;
}

static int set_aliq(struct pis_cofins_s *pc, nfe_cst_pis_cofins cst,
                    const char *vbc, const char *p, const char *v)
{
	int rc;

	if (cst != NFE_CST_PC_ALIQUOTA_BASICA &&
	    cst != NFE_CST_PC_ALIQUOTA_DIFERENCIADA)
		return E_VALOR;
	rc = nfe_valida_padrao(vbc, NFE_PADRAO_TDec_1302);
	if (rc == 0)
		rc = nfe_valida_padrao(p, NFE_PADRAO_TDec_0302a04);
	if (rc == 0)
		rc = nfe_valida_padrao(v, NFE_PADRAO_TDec_1302);
	if (rc != 0)
		return rc;
	pc->tipo = PC_ALIQ;
	pc->CST = cst;
	copia(pc->vBC, vbc);
	copia(pc->p, p);
	copia(pc->v, v);
	return 0;
}

static int set_nt(struct pis_cofins_s *pc, nfe_cst_pis_cofins cst)
{
	if (cst < NFE_CST_PC_MONOFASICA_ZERO || cst > NFE_CST_PC_SEM_INCIDENCIA)
		return E_VALOR;
	pc->tipo = PC_NT;
	pc->CST = cst;
	return 0;
}

int nfe_imposto_set_pisaliq(nfe_imposto *imp, nfe_cst_pis_cofins cst,
                            const char *vbc, const char *ppis, const char *vpis)
{
	EXIGE_IMP(imp);
	return set_aliq(&imp->PIS, cst, vbc, ppis, vpis);
}

int nfe_imposto_set_cofinsaliq(nfe_imposto *imp, nfe_cst_pis_cofins cst,
                               const char *vbc, const char *pcofins,
                               const char *vcofins)
{
	EXIGE_IMP(imp);
	return set_aliq(&imp->COFINS, cst, vbc, pcofins, vcofins);
}

int nfe_imposto_set_pisnt(nfe_imposto *imp, nfe_cst_pis_cofins cst)
{
	EXIGE_IMP(imp);
	return set_nt(&imp->PIS, cst);
}

int nfe_imposto_set_cofinsnt(nfe_imposto *imp, nfe_cst_pis_cofins cst)
{
	EXIGE_IMP(imp);
	return set_nt(&imp->COFINS, cst);
}

int nfe_imposto_remove_icms(nfe_imposto *imp)
{
	EXIGE_IMP(imp);
	imp->tipoICMS = ICMS_NENHUM;
	return 0;
}

int nfe_imposto_remove_pis(nfe_imposto *imp)
{
	EXIGE_IMP(imp);
	imp->PIS.tipo = PC_NENHUM;
	return 0;
}

int nfe_imposto_remove_cofins(nfe_imposto *imp)
{
	EXIGE_IMP(imp);
	imp->COFINS.tipo = PC_NENHUM;
	return 0;
}

static int escreve_icms(xmlTextWriterPtr writer, const nfe_imposto *imp)
{
	int rc;

	rc = nfe_abre(writer, "ICMS");
	if (rc != 0)
		return rc;
	if (imp->tipoICMS == ICMS_00) {
		rc = nfe_abre(writer, "ICMS00");
		if (rc != 0)
			return rc;
		NFE_ESCREVE("orig", "%d", (int)imp->orig);
		NFE_ESCREVE("CST", "00");
		NFE_ESCREVE("modBC", "%d", (int)imp->modBC);
		NFE_ESCREVE("vBC", "%s", imp->vBC);
		NFE_ESCREVE("pICMS", "%s", imp->pICMS);
		NFE_ESCREVE("vICMS", "%s", imp->vICMS);
		if (imp->pFCP[0] != '\0') {
			NFE_ESCREVE("pFCP", "%s", imp->pFCP);
			NFE_ESCREVE("vFCP", "%s", imp->vFCP);
		}
	} else {
		rc = nfe_abre(writer, "ICMSSN102");
		if (rc != 0)
			return rc;
		if (imp->orig != NFE_ORIGEM_NAO_INFORMADA)
			NFE_ESCREVE("orig", "%d", (int)imp->orig);
		NFE_ESCREVE("CSOSN", "%d", imp->CST);
	}
	rc = nfe_fecha(writer);
	if (rc != 0)
		return rc;
	return nfe_fecha(writer);
}

/* Escreve <PIS> ou <COFINS>; tributo é "PIS" ou "COFINS" */
static int escreve_pc(xmlTextWriterPtr writer, const struct pis_cofins_s *pc,
                      const char *tributo)
{
	char tag[16];
	int rc;

	rc = nfe_abre(writer, tributo);
	if (rc != 0)
		return rc;
	strcpy(tag, tributo);
	strcat(tag, pc->tipo == PC_ALIQ ? "Aliq" : "NT");
	rc = nfe_abre(writer, tag);
	if (rc != 0)
		return rc;
	NFE_ESCREVE("CST", "%02d", (int)pc->CST);
	if (pc->tipo == PC_ALIQ) {
		NFE_ESCREVE("vBC", "%s", pc->vBC);
		strcpy(tag, "p");
		strcat(tag, tributo);
		NFE_ESCREVE(tag, "%s", pc->p);
		tag[0] = 'v';
		NFE_ESCREVE(tag, "%s", pc->v);
	}
	rc = nfe_fecha(writer);
	if (rc != 0)
		return rc;
	return nfe_fecha(writer);
}

int nfe_imposto_write_xml(xmlTextWriterPtr writer, const nfe_imposto *imp)
{
	int rc;

	if (!writer || !imp)
		return E_ISNULL;
	rc = nfe_abre(writer, "imposto");
	if (rc != 0)
		return rc;
	if (imp->vTotTrib[0] != '\0')
		NFE_ESCREVE("vTotTrib", "%s", imp->vTotTrib);
	if (imp->tipoICMS != ICMS_NENHUM) {
		rc = escreve_icms(writer, imp);
		if (rc != 0)
			return rc;
	}
	if (imp->PIS.tipo != PC_NENHUM) {
		rc = escreve_pc(writer, &imp->PIS, "PIS");
		if (rc != 0)
			return rc;
	}
	if (imp->COFINS.tipo != PC_NENHUM) {
		rc = escreve_pc(writer, &imp->COFINS, "COFINS");
		if (rc != 0)
			return rc;
	}
	return nfe_fecha(writer);
}
