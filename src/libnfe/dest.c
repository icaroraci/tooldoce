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

#include <stdlib.h>
#include <string.h>

#include <libnfe/cnpjcpf.h>
#include <libnfe/defs.h>
#include <libnfe/dest.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

enum tipo_doc_e { DOC_NENHUM, DOC_CNPJ, DOC_CPF, DOC_ESTRANGEIRO };

/* idEstrangeiro: até 20 caracteres de U+0021 a U+00FF (até 2 bytes cada) */
#define TAM_ID_ESTRANGEIRO 20

struct nfe_dest {
	enum tipo_doc_e tipoDoc;
	char doc[NFE_TAM_UTF8(TAM_ID_ESTRANGEIRO)]; /* CNPJ, CPF ou
	                                              idEstrangeiro */
	char xNome[NFE_TAM_UTF8(NFE_TAM_XNOME)];    /* "": não informado */
	nfe_endereco *end;                          /* NULL: não informado */
	nfe_ind_ie_dest indIEDest;
	char IE[NFE_TAM_ASCII(NFE_TAM_IE)];      /* "": não informado */
	char ISUF[NFE_TAM_ASCII(NFE_TAM_ISUF)];  /* "": não informado */
	char IM[NFE_TAM_UTF8(NFE_TAM_IM)];       /* "": não informado */
	char email[NFE_TAM_UTF8(NFE_TAM_EMAIL)]; /* "": não informado */
};

nfe_dest *nfe_dest_new(void)
{
	return (nfe_dest *)calloc(1, sizeof(nfe_dest));
}

void nfe_dest_free(nfe_dest *dest)
{
	if (!dest)
		return;
	nfe_endereco_free(dest->end);
	free(dest);
}

#define EXIGE_DEST(dest)                                                       \
	do {                                                                   \
		if (!(dest))                                                   \
			return E_ISNULL;                                       \
	} while (0)

/* Grava o documento já validado por valida() */
static int set_doc(nfe_dest *dest, const char *doc, enum tipo_doc_e tipo,
                   int (*valida)(const char *))
{
	int rc;

	EXIGE_DEST(dest);
	rc = valida(doc);
	if (rc != 0)
		return rc;
	memcpy(dest->doc, doc, strlen(doc) + 1);
	dest->tipoDoc = tipo;
	return 0;
}

int nfe_dest_set_cnpj(nfe_dest *dest, const char *cnpj)
{
	return set_doc(dest, cnpj, DOC_CNPJ, nfe_cnpj_validar);
}

int nfe_dest_set_cpf(nfe_dest *dest, const char *cpf)
{
	return set_doc(dest, cpf, DOC_CPF, nfe_cpf_validar);
}

int nfe_dest_set_idestrangeiro(nfe_dest *dest, const char *idestrangeiro)
{
	int rc;

	EXIGE_DEST(dest);
	rc = nfe_copia_padrao(dest->doc, sizeof dest->doc, idestrangeiro,
	                      "([!-\xC3\xBF]{0}|[!-\xC3\xBF]{5,20})?");
	if (rc != 0)
		return rc;
	dest->tipoDoc = DOC_ESTRANGEIRO;
	return 0;
}

int nfe_dest_set_xnome(nfe_dest *dest, const char *xnome)
{
	EXIGE_DEST(dest);
	if (!xnome) {
		dest->xNome[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(dest->xNome, sizeof dest->xNome, xnome,
	                                2, NFE_TAM_XNOME);
}

int nfe_dest_set_endereco(nfe_dest *dest, nfe_endereco *end)
{
	EXIGE_DEST(dest);
	if (dest->end != end) {
		nfe_endereco_free(dest->end);
		dest->end = end;
	}
	return 0;
}

int nfe_dest_set_indiedest(nfe_dest *dest, nfe_ind_ie_dest indiedest)
{
	EXIGE_DEST(dest);
	if (indiedest != NFE_IE_DEST_CONTRIBUINTE &&
	    indiedest != NFE_IE_DEST_ISENTO &&
	    indiedest != NFE_IE_DEST_NAO_CONTRIBUINTE)
		return E_VALOR;
	dest->indIEDest = indiedest;
	return 0;
}

/* Campo opcional validado por padrão: NULL remove */
static int copia_opcional(char *dst, size_t tam, const char *valor,
                          const char *padrao)
{
	if (!valor) {
		dst[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(dst, tam, valor, padrao);
}

/* Texto opcional (TString): NULL remove */
static int texto_opcional(char *dst, size_t tam, const char *valor, size_t min,
                          size_t max)
{
	if (!valor) {
		dst[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(dst, tam, valor, min, max);
}

int nfe_dest_set_ie(nfe_dest *dest, const char *ie)
{
	EXIGE_DEST(dest);
	return copia_opcional(dest->IE, sizeof dest->IE, ie,
	                      NFE_PADRAO_TIeDestNaoIsento);
}

int nfe_dest_set_isuf(nfe_dest *dest, const char *isuf)
{
	EXIGE_DEST(dest);
	return copia_opcional(dest->ISUF, sizeof dest->ISUF, isuf,
	                      "[0-9]{8,9}");
}

int nfe_dest_set_im(nfe_dest *dest, const char *im)
{
	EXIGE_DEST(dest);
	return texto_opcional(dest->IM, sizeof dest->IM, im, 1, NFE_TAM_IM);
}

int nfe_dest_set_email(nfe_dest *dest, const char *email)
{
	EXIGE_DEST(dest);
	return texto_opcional(dest->email, sizeof dest->email, email, 1,
	                      NFE_TAM_EMAIL);
}

int nfe_dest_write_xml(xmlTextWriterPtr writer, const nfe_dest *dest)
{
	static const char *const tags[] = { "", "CNPJ", "CPF",
		                            "idEstrangeiro" };
	int rc;

	if (!writer || !dest)
		return E_ISNULL;
	if (dest->tipoDoc == DOC_NENHUM ||
	    dest->indIEDest == NFE_IE_DEST_NAO_INFORMADO)
		return E_VALOR;

	rc = nfe_abre(writer, "dest");
	if (rc != 0)
		return rc;
	NFE_ESCREVE(tags[dest->tipoDoc], "%s", dest->doc);
	if (dest->xNome[0] != '\0')
		NFE_ESCREVE("xNome", "%s", dest->xNome);
	if (dest->end) {
		rc = nfe_endereco_write_xml(writer, NFE_ENDERECO_DESTINATARIO,
		                            dest->end);
		if (rc != 0)
			return rc;
	}
	NFE_ESCREVE("indIEDest", "%d", (int)dest->indIEDest);
	if (dest->IE[0] != '\0')
		NFE_ESCREVE("IE", "%s", dest->IE);
	if (dest->ISUF[0] != '\0')
		NFE_ESCREVE("ISUF", "%s", dest->ISUF);
	if (dest->IM[0] != '\0')
		NFE_ESCREVE("IM", "%s", dest->IM);
	if (dest->email[0] != '\0')
		NFE_ESCREVE("email", "%s", dest->email);
	return nfe_fecha(writer);
}
