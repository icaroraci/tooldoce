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
#include <libnfe/transp.h>
#include <libnfe/valida.h>

/* Um volume; campos vazios não são escritos */
struct vol_s {
	char qVol[NFE_TAM_ASCII(NFE_TAM_QVOL)];
	char esp[NFE_TAM_UTF8(NFE_TAM_VOL)];
	char marca[NFE_TAM_UTF8(NFE_TAM_VOL)];
	char nVol[NFE_TAM_UTF8(NFE_TAM_VOL)];
	char pesoL[NFE_TAM_ASCII(NFE_TAM_DEC)];
	char pesoB[NFE_TAM_ASCII(NFE_TAM_DEC)];
};

enum tipo_doc_e { DOC_NENHUM, DOC_CNPJ, DOC_CPF };

struct nfe_transp {
	nfe_mod_frete modFrete;
	/* transporta; campos vazios não são escritos */
	enum tipo_doc_e tipoDoc;
	char doc[NFE_TAM_ASCII(NFE_TAM_CNPJ)];
	char xNome[NFE_TAM_UTF8(NFE_TAM_XNOME)];
	char IE[NFE_TAM_ASCII(NFE_TAM_IE)];
	char xEnder[NFE_TAM_UTF8(NFE_TAM_XENDER)];
	char xMun[NFE_TAM_UTF8(NFE_TAM_XMUN)];
	char UF[NFE_TAM_ASCII(2)];
	/* vol */
	struct vol_s *vol;
	int nVol, capVol;
};

#define EXIGE_TR(tr)                                                           \
	do {                                                                   \
		if (!(tr))                                                     \
			return E_ISNULL;                                       \
	} while (0)

nfe_transp *nfe_transp_new(void)
{
	nfe_transp *tr = (nfe_transp *)calloc(1, sizeof(nfe_transp));

	if (!tr)
		return NULL;
	tr->modFrete = NFE_FRETE_SEM_TRANSPORTE;
	return tr;
}

void nfe_transp_free(nfe_transp *tr)
{
	if (!tr)
		return;
	free(tr->vol);
	free(tr);
}

int nfe_transp_set_modfrete(nfe_transp *tr, nfe_mod_frete modfrete)
{
	EXIGE_TR(tr);
	if ((modfrete < NFE_FRETE_REMETENTE ||
	     modfrete > NFE_FRETE_PROPRIO_DESTINATARIO) &&
	    modfrete != NFE_FRETE_SEM_TRANSPORTE)
		return E_VALOR;
	tr->modFrete = modfrete;
	return 0;
}

static int set_doc(nfe_transp *tr, const char *doc, enum tipo_doc_e tipo,
                   int (*valida)(const char *))
{
	int rc;

	EXIGE_TR(tr);
	if (!doc) {
		tr->tipoDoc = DOC_NENHUM;
		return 0;
	}
	rc = valida(doc);
	if (rc != 0)
		return rc;
	memcpy(tr->doc, doc, strlen(doc) + 1);
	tr->tipoDoc = tipo;
	return 0;
}

int nfe_transp_set_transporta_cnpj(nfe_transp *tr, const char *cnpj)
{
	return set_doc(tr, cnpj, DOC_CNPJ, nfe_cnpj_validar);
}

int nfe_transp_set_transporta_cpf(nfe_transp *tr, const char *cpf)
{
	return set_doc(tr, cpf, DOC_CPF, nfe_cpf_validar);
}

/* Texto opcional (TString): NULL remove */
static int texto(char *dst, size_t tam, const char *valor, size_t min,
                 size_t max)
{
	if (!valor) {
		dst[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(dst, tam, valor, min, max);
}

int nfe_transp_set_transporta_xnome(nfe_transp *tr, const char *xnome)
{
	EXIGE_TR(tr);
	return texto(tr->xNome, sizeof tr->xNome, xnome, 2, NFE_TAM_XNOME);
}

int nfe_transp_set_transporta_ie(nfe_transp *tr, const char *ie)
{
	EXIGE_TR(tr);
	if (!ie) {
		tr->IE[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(tr->IE, sizeof tr->IE, ie, NFE_PADRAO_TIeDest);
}

int nfe_transp_set_transporta_xender(nfe_transp *tr, const char *xender)
{
	EXIGE_TR(tr);
	return texto(tr->xEnder, sizeof tr->xEnder, xender, 1, NFE_TAM_XENDER);
}

int nfe_transp_set_transporta_xmun(nfe_transp *tr, const char *xmun)
{
	EXIGE_TR(tr);
	return texto(tr->xMun, sizeof tr->xMun, xmun, 1, NFE_TAM_XMUN);
}

int nfe_transp_set_transporta_uf(nfe_transp *tr, const char *uf)
{
	static const char *const ufs[] = { NFE_VALORES_TUf, NULL };
	int rc;

	EXIGE_TR(tr);
	if (!uf) {
		tr->UF[0] = '\0';
		return 0;
	}
	rc = nfe_valida_lista(uf, ufs);
	if (rc != 0)
		return rc;
	memcpy(tr->UF, uf, sizeof tr->UF);
	return 0;
}

/* Valida (se informado) e copia para dst; NULL vira "" */
static int campo_vol(char *dst, size_t tam, const char *valor,
                     const char *padrao, size_t max)
{
	if (!valor) {
		dst[0] = '\0';
		return 0;
	}
	if (padrao)
		return nfe_copia_padrao(dst, tam, valor, padrao);
	return nfe_copia_texto_validado(dst, tam, valor, 1, max);
}

int nfe_transp_add_vol(nfe_transp *tr, const char *qvol, const char *esp,
                       const char *marca, const char *nvol, const char *pesol,
                       const char *pesob)
{
	struct vol_s v;
	int rc;

	EXIGE_TR(tr);
	if (tr->nVol >= NFE_MAX_VOL)
		return E_VALOR;
	rc = campo_vol(v.qVol, sizeof v.qVol, qvol, "[0-9]{1,15}", 0);
	if (rc == 0)
		rc = campo_vol(v.esp, sizeof v.esp, esp, NULL, NFE_TAM_VOL);
	if (rc == 0)
		rc = campo_vol(v.marca, sizeof v.marca, marca, NULL,
		               NFE_TAM_VOL);
	if (rc == 0)
		rc = campo_vol(v.nVol, sizeof v.nVol, nvol, NULL, NFE_TAM_VOL);
	if (rc == 0)
		rc = campo_vol(v.pesoL, sizeof v.pesoL, pesol,
		               NFE_PADRAO_TDec_1203, 0);
	if (rc == 0)
		rc = campo_vol(v.pesoB, sizeof v.pesoB, pesob,
		               NFE_PADRAO_TDec_1203, 0);
	if (rc != 0)
		return rc;

	if (tr->nVol == tr->capVol) {
		int cap = tr->capVol ? tr->capVol * 2 : 4;
		struct vol_s *novo;

		if (cap > NFE_MAX_VOL)
			cap = NFE_MAX_VOL;
		novo = (struct vol_s *)realloc(tr->vol, cap * sizeof *novo);
		if (!novo)
			return E_MALLOC;
		tr->vol = novo;
		tr->capVol = cap;
	}
	tr->vol[tr->nVol++] = v;
	return 0;
}

int nfe_transp_remove_vol(nfe_transp *tr)
{
	EXIGE_TR(tr);
	tr->nVol = 0;
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

int nfe_transp_write_xml(xmlTextWriterPtr writer, const nfe_transp *tr)
{
	int i, rc;

	if (!writer || !tr)
		return E_ISNULL;
	rc = nfe_abre(writer, "transp");
	if (rc != 0)
		return rc;
	NFE_ESCREVE("modFrete", "%d", (int)tr->modFrete);
	if (tr->tipoDoc != DOC_NENHUM || tr->xNome[0] || tr->IE[0] ||
	    tr->xEnder[0] || tr->xMun[0] || tr->UF[0]) {
		rc = nfe_abre(writer, "transporta");
		if (rc != 0)
			return rc;
		if (tr->tipoDoc != DOC_NENHUM)
			NFE_ESCREVE(tr->tipoDoc == DOC_CNPJ ? "CNPJ" : "CPF",
			            "%s", tr->doc);
		OPCIONAL("xNome", tr->xNome);
		OPCIONAL("IE", tr->IE);
		OPCIONAL("xEnder", tr->xEnder);
		OPCIONAL("xMun", tr->xMun);
		OPCIONAL("UF", tr->UF);
		rc = nfe_fecha(writer);
		if (rc != 0)
			return rc;
	}
	for (i = 0; i < tr->nVol; i++) {
		const struct vol_s *v = &tr->vol[i];

		rc = nfe_abre(writer, "vol");
		if (rc != 0)
			return rc;
		OPCIONAL("qVol", v->qVol);
		OPCIONAL("esp", v->esp);
		OPCIONAL("marca", v->marca);
		OPCIONAL("nVol", v->nVol);
		OPCIONAL("pesoL", v->pesoL);
		OPCIONAL("pesoB", v->pesoB);
		rc = nfe_fecha(writer);
		if (rc != 0)
			return rc;
	}
	return nfe_fecha(writer);
}
