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
#include <libnfe/prod.h>
#include <libnfe/valida.h>

#define PADRAO_GTIN   "SEM GTIN|[0-9]{0}|[0-9]{8}|[0-9]{12,14}"
#define PADRAO_CBENEF "([!-\xC3\xBF]{8}|[!-\xC3\xBF]{10}|SEM CBENEF)?"

struct nfe_prod {
	char cProd[NFE_TAM_UTF8(NFE_TAM_CPROD)];
	char cEAN[NFE_TAM_ASCII(NFE_TAM_GTIN)];
	char cBarra[NFE_TAM_UTF8(NFE_TAM_CBARRA)]; /* "": não informado */
	char xProd[NFE_TAM_UTF8(NFE_TAM_XPROD)];
	char NCM[NFE_TAM_ASCII(NFE_TAM_NCM)];
	char NVE[NFE_MAX_NVE][NFE_TAM_ASCII(NFE_TAM_NVE)];
	int nNVE;
	char CEST[NFE_TAM_ASCII(NFE_TAM_CEST)]; /* "": não informado */
	nfe_escala indEscala;
	char CNPJFab[NFE_TAM_ASCII(NFE_TAM_CNPJ)]; /* "": não informado */
	char cBenef[NFE_TAM_UTF8(NFE_TAM_CBENEF)]; /* "": não informado */
	int cBenefInformado; /* cBenef aceita "" como valor */
	nfe_cred_pres_zfm tpCredPresIBSZFM;
	char EXTIPI[NFE_TAM_ASCII(NFE_TAM_EXTIPI)]; /* "": não informado */
	unsigned CFOP;
	char uCom[NFE_TAM_UTF8(NFE_TAM_UNIDADE)];
	char qCom[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char vUnCom[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char vProd[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char cEANTrib[NFE_TAM_ASCII(NFE_TAM_GTIN)];
	char cBarraTrib[NFE_TAM_UTF8(NFE_TAM_CBARRA)]; /* "": não informado */
	char uTrib[NFE_TAM_UTF8(NFE_TAM_UNIDADE)];
	char qTrib[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char vUnTrib[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char vFrete[NFE_TAM_ASCII(NFE_TAM_DEC)]; /* "": não informado */
	char vSeg[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char vDesc[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char vOutro[NFE_TAM_ASCII(NFE_TAM_DEC)];
	int indTot;
	int indBemMovelUsado;
	char xPed[NFE_TAM_UTF8(NFE_TAM_XPED)];          /* "": não informado */
	char nItemPed[NFE_TAM_ASCII(NFE_TAM_NITEMPED)]; /* "": não informado */
	char nFCI[NFE_TAM_ASCII(NFE_TAM_GUID)];         /* "": não informado */
};

nfe_prod *nfe_prod_new(void)
{
	nfe_prod *prod = (nfe_prod *)calloc(1, sizeof(nfe_prod));

	if (!prod)
		return NULL;
	strcpy(prod->cEAN, NFE_SEM_GTIN);
	strcpy(prod->cEANTrib, NFE_SEM_GTIN);
	prod->tpCredPresIBSZFM = NFE_CRED_PRES_ZFM_NAO_INFORMADO;
	prod->indTot = 1;
	return prod;
}

void nfe_prod_free(nfe_prod *prod)
{
	free(prod);
}

#define EXIGE_PROD(prod)                                                       \
	do {                                                                   \
		if (!(prod))                                                   \
			return E_ISNULL;                                       \
	} while (0)

/* Campo validado por padrão; com opcional, NULL remove */
static int padrao(char *dst, size_t tam, const char *valor, const char *pad,
                  int opcional)
{
	if (!valor && opcional) {
		dst[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(dst, tam, valor, pad);
}

/* Texto TString; com opcional, NULL remove */
static int texto(char *dst, size_t tam, const char *valor, size_t min,
                 size_t max, int opcional)
{
	if (!valor && opcional) {
		dst[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(dst, tam, valor, min, max);
}

int nfe_prod_set_cprod(nfe_prod *prod, const char *cprod)
{
	EXIGE_PROD(prod);
	return texto(prod->cProd, sizeof prod->cProd, cprod, 1, NFE_TAM_CPROD,
	             0);
}

int nfe_prod_set_cean(nfe_prod *prod, const char *cean)
{
	EXIGE_PROD(prod);
	return padrao(prod->cEAN, sizeof prod->cEAN, cean, PADRAO_GTIN, 0);
}

int nfe_prod_set_cbarra(nfe_prod *prod, const char *cbarra)
{
	EXIGE_PROD(prod);
	return texto(prod->cBarra, sizeof prod->cBarra, cbarra, 3,
	             NFE_TAM_CBARRA, 1);
}

int nfe_prod_set_xprod(nfe_prod *prod, const char *xprod)
{
	EXIGE_PROD(prod);
	return texto(prod->xProd, sizeof prod->xProd, xprod, 1, NFE_TAM_XPROD,
	             0);
}

int nfe_prod_set_ncm(nfe_prod *prod, const char *ncm)
{
	EXIGE_PROD(prod);
	return padrao(prod->NCM, sizeof prod->NCM, ncm, "[0-9]{2}|[0-9]{8}", 0);
}

int nfe_prod_add_nve(nfe_prod *prod, const char *nve)
{
	int rc;

	EXIGE_PROD(prod);
	if (prod->nNVE >= NFE_MAX_NVE)
		return E_VALOR;
	rc = padrao(prod->NVE[prod->nNVE], sizeof prod->NVE[0], nve,
	            "[A-Z]{2}[0-9]{4}", 0);
	if (rc != 0)
		return rc;
	prod->nNVE++;
	return 0;
}

int nfe_prod_remove_nve(nfe_prod *prod)
{
	EXIGE_PROD(prod);
	prod->nNVE = 0;
	return 0;
}

int nfe_prod_set_cest(nfe_prod *prod, const char *cest)
{
	EXIGE_PROD(prod);
	return padrao(prod->CEST, sizeof prod->CEST, cest, "[0-9]{7}", 1);
}

int nfe_prod_set_indescala(nfe_prod *prod, nfe_escala indescala)
{
	EXIGE_PROD(prod);
	if (indescala < NFE_ESCALA_NAO_INFORMADA ||
	    indescala > NFE_ESCALA_NAO_RELEVANTE)
		return E_VALOR;
	prod->indEscala = indescala;
	return 0;
}

int nfe_prod_set_cnpjfab(nfe_prod *prod, const char *cnpjfab)
{
	int rc;

	EXIGE_PROD(prod);
	if (!cnpjfab) {
		prod->CNPJFab[0] = '\0';
		return 0;
	}
	rc = nfe_cnpj_validar(cnpjfab);
	if (rc != 0)
		return rc;
	memcpy(prod->CNPJFab, cnpjfab, sizeof prod->CNPJFab);
	return 0;
}

int nfe_prod_set_cbenef(nfe_prod *prod, const char *cbenef)
{
	int rc;

	EXIGE_PROD(prod);
	rc = padrao(prod->cBenef, sizeof prod->cBenef, cbenef, PADRAO_CBENEF,
	            1);
	if (rc == 0)
		prod->cBenefInformado = cbenef != NULL;
	return rc;
}

int nfe_prod_set_tpcredpresibszfm(nfe_prod *prod, nfe_cred_pres_zfm tipo)
{
	EXIGE_PROD(prod);
	if (tipo < NFE_CRED_PRES_ZFM_NAO_INFORMADO ||
	    tipo > NFE_CRED_PRES_ZFM_INFORMATICA)
		return E_VALOR;
	prod->tpCredPresIBSZFM = tipo;
	return 0;
}

int nfe_prod_set_extipi(nfe_prod *prod, const char *extipi)
{
	EXIGE_PROD(prod);
	return padrao(prod->EXTIPI, sizeof prod->EXTIPI, extipi, "[0-9]{2,3}",
	              1);
}

int nfe_prod_set_cfop(nfe_prod *prod, unsigned cfop)
{
	unsigned grupo = cfop / 1000;

	EXIGE_PROD(prod);
	if (cfop < 1000 || cfop > 7999 || grupo == 4)
		return E_VALOR;
	prod->CFOP = cfop;
	return 0;
}

/* Unidade (TString 1 a 6), quantidade e valor unitário; com vtotal (não
 * NULL), também o valor total. Valida tudo antes de gravar. */
static int unidade(char *u, char *q, char *vun, char *vtot, const char *uval,
                   const char *qval, const char *vunval, const char *vtotval)
{
	int rc;

	rc = nfe_valida_texto(uval, 1, NFE_TAM_UNIDADE);
	if (rc == 0)
		rc = nfe_valida_padrao(qval, NFE_PADRAO_TDec_1104v);
	if (rc == 0)
		rc = nfe_valida_padrao(vunval, NFE_PADRAO_TDec_1110v);
	if (rc == 0 && vtot)
		rc = nfe_valida_padrao(vtotval, NFE_PADRAO_TDec_1302);
	if (rc != 0)
		return rc;
	/* Os padrões limitam o tamanho: as cópias sempre cabem */
	strcpy(u, uval);
	strcpy(q, qval);
	strcpy(vun, vunval);
	if (vtot)
		strcpy(vtot, vtotval);
	return 0;
}

int nfe_prod_set_comercial(nfe_prod *prod, const char *ucom, const char *qcom,
                           const char *vuncom, const char *vprod)
{
	EXIGE_PROD(prod);
	return unidade(prod->uCom, prod->qCom, prod->vUnCom, prod->vProd, ucom,
	               qcom, vuncom, vprod);
}

int nfe_prod_set_ceantrib(nfe_prod *prod, const char *ceantrib)
{
	EXIGE_PROD(prod);
	return padrao(prod->cEANTrib, sizeof prod->cEANTrib, ceantrib,
	              PADRAO_GTIN, 0);
}

int nfe_prod_set_cbarratrib(nfe_prod *prod, const char *cbarratrib)
{
	EXIGE_PROD(prod);
	return texto(prod->cBarraTrib, sizeof prod->cBarraTrib, cbarratrib, 3,
	             NFE_TAM_CBARRA, 1);
}

int nfe_prod_set_tributavel(nfe_prod *prod, const char *utrib,
                            const char *qtrib, const char *vuntrib)
{
	EXIGE_PROD(prod);
	return unidade(prod->uTrib, prod->qTrib, prod->vUnTrib, NULL, utrib,
	               qtrib, vuntrib, NULL);
}

int nfe_prod_set_vfrete(nfe_prod *prod, const char *vfrete)
{
	EXIGE_PROD(prod);
	return padrao(prod->vFrete, sizeof prod->vFrete, vfrete,
	              NFE_PADRAO_TDec_1302Opc, 1);
}

int nfe_prod_set_vseg(nfe_prod *prod, const char *vseg)
{
	EXIGE_PROD(prod);
	return padrao(prod->vSeg, sizeof prod->vSeg, vseg,
	              NFE_PADRAO_TDec_1302Opc, 1);
}

int nfe_prod_set_vdesc(nfe_prod *prod, const char *vdesc)
{
	EXIGE_PROD(prod);
	return padrao(prod->vDesc, sizeof prod->vDesc, vdesc,
	              NFE_PADRAO_TDec_1302Opc, 1);
}

int nfe_prod_set_voutro(nfe_prod *prod, const char *voutro)
{
	EXIGE_PROD(prod);
	return padrao(prod->vOutro, sizeof prod->vOutro, voutro,
	              NFE_PADRAO_TDec_1302Opc, 1);
}

int nfe_prod_set_indtot(nfe_prod *prod, int indtot)
{
	EXIGE_PROD(prod);
	if (indtot != 0 && indtot != 1)
		return E_VALOR;
	prod->indTot = indtot;
	return 0;
}

int nfe_prod_set_indbemmovelusado(nfe_prod *prod, int usado)
{
	EXIGE_PROD(prod);
	if (usado != 0 && usado != 1)
		return E_VALOR;
	prod->indBemMovelUsado = usado;
	return 0;
}

int nfe_prod_set_xped(nfe_prod *prod, const char *xped)
{
	EXIGE_PROD(prod);
	return texto(prod->xPed, sizeof prod->xPed, xped, 1, NFE_TAM_XPED, 1);
}

int nfe_prod_set_nitemped(nfe_prod *prod, const char *nitemped)
{
	EXIGE_PROD(prod);
	return padrao(prod->nItemPed, sizeof prod->nItemPed, nitemped,
	              "[0-9]{1,6}", 1);
}

int nfe_prod_set_nfci(nfe_prod *prod, const char *nfci)
{
	EXIGE_PROD(prod);
	return padrao(prod->nFCI, sizeof prod->nFCI, nfci, NFE_PADRAO_TGuid, 1);
}

const char *nfe_prod_valor(const nfe_prod *prod, enum nfe_prod_valor_e campo)
{
	switch (campo) {
	case NFE_PROD_VPROD:
		return prod->vProd;
	case NFE_PROD_VFRETE:
		return prod->vFrete;
	case NFE_PROD_VSEG:
		return prod->vSeg;
	case NFE_PROD_VDESC:
		return prod->vDesc;
	case NFE_PROD_VOUTRO:
		return prod->vOutro;
	}
	return "";
}

int nfe_prod_indtot(const nfe_prod *prod)
{
	return prod->indTot;
}

/* Escreve <tag>valor</tag> se valor não for vazio */
static int opcional(xmlTextWriterPtr writer, const char *tag, const char *valor)
{
	if (valor[0] == '\0')
		return 0;
	return nfe_escreve(writer, tag, "%s", valor);
}

#define OPCIONAL(tag, valor)                                                   \
	do {                                                                   \
		rc = opcional(writer, (tag), (valor));                         \
		if (rc != 0)                                                   \
			return rc;                                             \
	} while (0)

int nfe_prod_write_xml(xmlTextWriterPtr writer, const nfe_prod *prod)
{
	int i, rc;

	if (!writer || !prod)
		return E_ISNULL;
	if (prod->cProd[0] == '\0' || prod->xProd[0] == '\0' ||
	    prod->NCM[0] == '\0' || prod->CFOP == 0 || prod->uCom[0] == '\0' ||
	    prod->uTrib[0] == '\0')
		return E_VALOR;
	/* indEscala e CNPJFab fazem parte da sequência do CEST */
	if (prod->CEST[0] == '\0' &&
	    (prod->indEscala != NFE_ESCALA_NAO_INFORMADA ||
	     prod->CNPJFab[0] != '\0'))
		return E_VALOR;

	rc = nfe_abre(writer, "prod");
	if (rc != 0)
		return rc;
	NFE_ESCREVE("cProd", "%s", prod->cProd);
	NFE_ESCREVE("cEAN", "%s", prod->cEAN);
	OPCIONAL("cBarra", prod->cBarra);
	NFE_ESCREVE("xProd", "%s", prod->xProd);
	NFE_ESCREVE("NCM", "%s", prod->NCM);
	for (i = 0; i < prod->nNVE; i++)
		NFE_ESCREVE("NVE", "%s", prod->NVE[i]);
	if (prod->CEST[0] != '\0') {
		NFE_ESCREVE("CEST", "%s", prod->CEST);
		if (prod->indEscala != NFE_ESCALA_NAO_INFORMADA)
			NFE_ESCREVE("indEscala", "%s",
			            prod->indEscala == NFE_ESCALA_RELEVANTE
			                    ? "S"
			                    : "N");
		OPCIONAL("CNPJFab", prod->CNPJFab);
	}
	if (prod->cBenefInformado)
		NFE_ESCREVE("cBenef", "%s", prod->cBenef);
	if (prod->tpCredPresIBSZFM != NFE_CRED_PRES_ZFM_NAO_INFORMADO)
		NFE_ESCREVE("tpCredPresIBSZFM", "%d",
		            (int)prod->tpCredPresIBSZFM);
	OPCIONAL("EXTIPI", prod->EXTIPI);
	NFE_ESCREVE("CFOP", "%u", prod->CFOP);
	NFE_ESCREVE("uCom", "%s", prod->uCom);
	NFE_ESCREVE("qCom", "%s", prod->qCom);
	NFE_ESCREVE("vUnCom", "%s", prod->vUnCom);
	NFE_ESCREVE("vProd", "%s", prod->vProd);
	NFE_ESCREVE("cEANTrib", "%s", prod->cEANTrib);
	OPCIONAL("cBarraTrib", prod->cBarraTrib);
	NFE_ESCREVE("uTrib", "%s", prod->uTrib);
	NFE_ESCREVE("qTrib", "%s", prod->qTrib);
	NFE_ESCREVE("vUnTrib", "%s", prod->vUnTrib);
	OPCIONAL("vFrete", prod->vFrete);
	OPCIONAL("vSeg", prod->vSeg);
	OPCIONAL("vDesc", prod->vDesc);
	OPCIONAL("vOutro", prod->vOutro);
	NFE_ESCREVE("indTot", "%d", prod->indTot);
	if (prod->indBemMovelUsado)
		NFE_ESCREVE("indBemMovelUsado", "1");
	OPCIONAL("xPed", prod->xPed);
	OPCIONAL("nItemPed", prod->nItemPed);
	OPCIONAL("nFCI", prod->nFCI);
	return nfe_fecha(writer);
}
