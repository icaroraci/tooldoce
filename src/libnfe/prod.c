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

#include <libnfe/cnpjcpf.h>
#include <libnfe/erros.h>
#include <libnfe/esquema.h>
#include <libnfe/prod.h>

/* Os campos ficam em um grupo genérico (esquema.h) com a estrutura de
 * <prod> do leiaute; os setters específicos são atalhos para ele. */
struct nfe_prod {
	nfe_grupo *g;
};

nfe_prod *nfe_prod_new(void)
{
	nfe_prod *prod = (nfe_prod *)calloc(1, sizeof(nfe_prod));

	if (!prod)
		return NULL;
	prod->g = nfe_grupo_new(&esq_prod);
	if (!prod->g || nfe_grupo_set(prod->g, "cEAN", NFE_SEM_GTIN) != 0 ||
	    nfe_grupo_set(prod->g, "cEANTrib", NFE_SEM_GTIN) != 0 ||
	    nfe_grupo_set(prod->g, "indTot", "1") != 0) {
		nfe_prod_free(prod);
		return NULL;
	}
	return prod;
}

void nfe_prod_free(nfe_prod *prod)
{
	if (!prod)
		return;
	nfe_grupo_free(prod->g);
	free(prod);
}

nfe_grupo *nfe_prod_grupo(nfe_prod *prod)
{
	return prod ? prod->g : NULL;
}

/* Campo obrigatório: NULL é recusado */
static int obrigatorio(nfe_prod *prod, const char *caminho, const char *v)
{
	if (!prod || !v)
		return E_ISNULL;
	return nfe_grupo_set(prod->g, caminho, v);
}

/* Campo opcional: NULL remove */
static int opcional(nfe_prod *prod, const char *caminho, const char *v)
{
	if (!prod)
		return E_ISNULL;
	return nfe_grupo_set(prod->g, caminho, v);
}

int nfe_prod_set_cprod(nfe_prod *prod, const char *cprod)
{
	return obrigatorio(prod, "cProd", cprod);
}

int nfe_prod_set_cean(nfe_prod *prod, const char *cean)
{
	return obrigatorio(prod, "cEAN", cean);
}

int nfe_prod_set_cbarra(nfe_prod *prod, const char *cbarra)
{
	return opcional(prod, "cBarra", cbarra);
}

int nfe_prod_set_xprod(nfe_prod *prod, const char *xprod)
{
	return obrigatorio(prod, "xProd", xprod);
}

int nfe_prod_set_ncm(nfe_prod *prod, const char *ncm)
{
	return obrigatorio(prod, "NCM", ncm);
}

int nfe_prod_add_nve(nfe_prod *prod, const char *nve)
{
	nfe_grupo *item;
	int rc;

	if (!prod || !nve)
		return E_ISNULL;
	rc = nfe_grupo_add(prod->g, "NVE", &item);
	if (rc != 0)
		return rc;
	rc = nfe_grupo_set(item, "NVE", nve);
	if (rc != 0)
		nfe_grupo_remove_ultimo(prod->g, "NVE");
	return rc;
}

int nfe_prod_remove_nve(nfe_prod *prod)
{
	if (!prod)
		return E_ISNULL;
	return nfe_grupo_remove(prod->g, "NVE");
}

int nfe_prod_set_cest(nfe_prod *prod, const char *cest)
{
	return opcional(prod, "CEST", cest);
}

int nfe_prod_set_indescala(nfe_prod *prod, nfe_escala indescala)
{
	switch (indescala) {
	case NFE_ESCALA_NAO_INFORMADA:
		return opcional(prod, "indEscala", NULL);
	case NFE_ESCALA_RELEVANTE:
		return opcional(prod, "indEscala", "S");
	case NFE_ESCALA_NAO_RELEVANTE:
		return opcional(prod, "indEscala", "N");
	}
	return prod ? E_VALOR : E_ISNULL;
}

int nfe_prod_set_cnpjfab(nfe_prod *prod, const char *cnpjfab)
{
	int rc;

	if (!prod)
		return E_ISNULL;
	if (cnpjfab) {
		rc = nfe_cnpj_validar(cnpjfab);
		if (rc != 0)
			return rc;
	}
	return opcional(prod, "CNPJFab", cnpjfab);
}

int nfe_prod_set_cbenef(nfe_prod *prod, const char *cbenef)
{
	return opcional(prod, "cBenef", cbenef);
}

int nfe_prod_set_tpcredpresibszfm(nfe_prod *prod, nfe_cred_pres_zfm tipo)
{
	char v[4];

	if (!prod)
		return E_ISNULL;
	if (tipo == NFE_CRED_PRES_ZFM_NAO_INFORMADO)
		return opcional(prod, "tpCredPresIBSZFM", NULL);
	if ((int)tipo < 0 || (int)tipo > 9)
		return E_VALOR;
	snprintf(v, sizeof v, "%d", (int)tipo);
	return opcional(prod, "tpCredPresIBSZFM", v);
}

int nfe_prod_set_extipi(nfe_prod *prod, const char *extipi)
{
	return opcional(prod, "EXTIPI", extipi);
}

int nfe_prod_set_cfop(nfe_prod *prod, unsigned cfop)
{
	char v[16];

	if (!prod)
		return E_ISNULL;
	if (cfop > 9999)
		return E_VALOR;
	snprintf(v, sizeof v, "%u", cfop);
	return nfe_grupo_set(prod->g, "CFOP", v);
}

/* Grava os campos só se todos forem válidos */
static int varios(nfe_prod *prod, const char *const *caminhos,
                  const char *const *valores, int n)
{
	int i, rc;

	if (!prod)
		return E_ISNULL;
	for (i = 0; i < n; i++) {
		rc = nfe_grupo_valida(prod->g, caminhos[i], valores[i]);
		if (rc != 0)
			return rc;
	}
	for (i = 0; i < n; i++) {
		rc = nfe_grupo_set(prod->g, caminhos[i], valores[i]);
		if (rc != 0)
			return rc;
	}
	return 0;
}

int nfe_prod_set_comercial(nfe_prod *prod, const char *ucom, const char *qcom,
                           const char *vuncom, const char *vprod)
{
	static const char *const c[] = { "uCom", "qCom", "vUnCom", "vProd" };
	const char *v[4];

	v[0] = ucom;
	v[1] = qcom;
	v[2] = vuncom;
	v[3] = vprod;
	return varios(prod, c, v, 4);
}

int nfe_prod_set_ceantrib(nfe_prod *prod, const char *ceantrib)
{
	return obrigatorio(prod, "cEANTrib", ceantrib);
}

int nfe_prod_set_cbarratrib(nfe_prod *prod, const char *cbarratrib)
{
	return opcional(prod, "cBarraTrib", cbarratrib);
}

int nfe_prod_set_tributavel(nfe_prod *prod, const char *utrib,
                            const char *qtrib, const char *vuntrib)
{
	static const char *const c[] = { "uTrib", "qTrib", "vUnTrib" };
	const char *v[3];

	v[0] = utrib;
	v[1] = qtrib;
	v[2] = vuntrib;
	return varios(prod, c, v, 3);
}

int nfe_prod_set_vfrete(nfe_prod *prod, const char *vfrete)
{
	return opcional(prod, "vFrete", vfrete);
}

int nfe_prod_set_vseg(nfe_prod *prod, const char *vseg)
{
	return opcional(prod, "vSeg", vseg);
}

int nfe_prod_set_vdesc(nfe_prod *prod, const char *vdesc)
{
	return opcional(prod, "vDesc", vdesc);
}

int nfe_prod_set_voutro(nfe_prod *prod, const char *voutro)
{
	return opcional(prod, "vOutro", voutro);
}

int nfe_prod_set_indtot(nfe_prod *prod, int indtot)
{
	if (!prod)
		return E_ISNULL;
	if (indtot != 0 && indtot != 1)
		return E_VALOR;
	return nfe_grupo_set(prod->g, "indTot", indtot ? "1" : "0");
}

int nfe_prod_set_indbemmovelusado(nfe_prod *prod, int usado)
{
	if (!prod)
		return E_ISNULL;
	if (usado != 0 && usado != 1)
		return E_VALOR;
	return nfe_grupo_set(prod->g, "indBemMovelUsado", usado ? "1" : NULL);
}

int nfe_prod_set_xped(nfe_prod *prod, const char *xped)
{
	return opcional(prod, "xPed", xped);
}

int nfe_prod_set_nitemped(nfe_prod *prod, const char *nitemped)
{
	return opcional(prod, "nItemPed", nitemped);
}

int nfe_prod_set_nfci(nfe_prod *prod, const char *nfci)
{
	return opcional(prod, "nFCI", nfci);
}

const char *nfe_prod_valor(const nfe_prod *prod, enum nfe_prod_valor_e campo)
{
	static const char *const caminhos[] = { "vProd", "vFrete", "vSeg",
		                                "vDesc", "vOutro" };
	const char *v;

	if ((int)campo < 0 || (int)campo > NFE_PROD_VOUTRO)
		return "";
	v = nfe_grupo_get(prod->g, caminhos[campo]);
	return v ? v : "";
}

int nfe_prod_indtot(const nfe_prod *prod)
{
	const char *v = nfe_grupo_get(prod->g, "indTot");

	return v && v[0] == '1';
}

int nfe_prod_write_xml(xmlTextWriterPtr writer, const nfe_prod *prod)
{
	if (!writer || !prod)
		return E_ISNULL;
	return nfe_grupo_write_xml(writer, prod->g);
}
