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
#include <libnfe/emit.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

enum tipo_doc_e { DOC_NENHUM, DOC_CNPJ, DOC_CPF };

struct nfe_emit {
	enum tipo_doc_e tipoDoc;
	char doc[NFE_TAM_ASCII(NFE_TAM_CNPJ)]; /* CNPJ ou CPF */
	char xNome[NFE_TAM_UTF8(NFE_TAM_XNOME)];
	char xFant[NFE_TAM_UTF8(NFE_TAM_XFANT)]; /* "": não informado */
	nfe_endereco *end;
	char IE[NFE_TAM_ASCII(NFE_TAM_IE)];     /* "": não informado */
	char IEST[NFE_TAM_ASCII(NFE_TAM_IE)];   /* "": não informado */
	char IM[NFE_TAM_UTF8(NFE_TAM_IM)];      /* "": não informado */
	char CNAE[NFE_TAM_ASCII(NFE_TAM_CNAE)]; /* "": não informado */
	nfe_crt CRT;
	char ISUF[NFE_TAM_ASCII(NFE_TAM_ISUF)]; /* "": não informado */
};

nfe_emit *nfe_emit_new(void)
{
	return (nfe_emit *)calloc(1, sizeof(nfe_emit));
}

void nfe_emit_free(nfe_emit *emit)
{
	if (!emit)
		return;
	nfe_endereco_free(emit->end);
	free(emit);
}

#define EXIGE_EMIT(emit)                                                       \
	do {                                                                   \
		if (!(emit))                                                   \
			return E_ISNULL;                                       \
	} while (0)

/* Grava o documento já validado por valida() */
static int set_doc(nfe_emit *emit, const char *doc, enum tipo_doc_e tipo,
                   int (*valida)(const char *))
{
	int rc;

	EXIGE_EMIT(emit);
	rc = valida(doc);
	if (rc != 0)
		return rc;
	memcpy(emit->doc, doc, strlen(doc) + 1);
	emit->tipoDoc = tipo;
	return 0;
}

int nfe_emit_set_cnpj(nfe_emit *emit, const char *cnpj)
{
	return set_doc(emit, cnpj, DOC_CNPJ, nfe_cnpj_validar);
}

int nfe_emit_set_cpf(nfe_emit *emit, const char *cpf)
{
	return set_doc(emit, cpf, DOC_CPF, nfe_cpf_validar);
}

int nfe_emit_set_xnome(nfe_emit *emit, const char *xnome)
{
	EXIGE_EMIT(emit);
	return nfe_copia_texto_validado(emit->xNome, sizeof emit->xNome, xnome,
	                                2, NFE_TAM_XNOME);
}

int nfe_emit_set_xfant(nfe_emit *emit, const char *xfant)
{
	EXIGE_EMIT(emit);
	if (!xfant) {
		emit->xFant[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(emit->xFant, sizeof emit->xFant, xfant,
	                                1, NFE_TAM_XFANT);
}

int nfe_emit_set_endereco(nfe_emit *emit, nfe_endereco *end)
{
	EXIGE_EMIT(emit);
	if (!end)
		return E_ISNULL;
	if (emit->end != end) {
		nfe_endereco_free(emit->end);
		emit->end = end;
	}
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

int nfe_emit_set_ie(nfe_emit *emit, const char *ie)
{
	EXIGE_EMIT(emit);
	return copia_opcional(emit->IE, sizeof emit->IE, ie, NFE_PADRAO_TIe);
}

int nfe_emit_set_iest(nfe_emit *emit, const char *iest)
{
	EXIGE_EMIT(emit);
	return copia_opcional(emit->IEST, sizeof emit->IEST, iest,
	                      NFE_PADRAO_TIeST);
}

int nfe_emit_set_im(nfe_emit *emit, const char *im)
{
	EXIGE_EMIT(emit);
	if (!im) {
		emit->IM[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(emit->IM, sizeof emit->IM, im, 1,
	                                NFE_TAM_IM);
}

int nfe_emit_set_cnae(nfe_emit *emit, const char *cnae)
{
	EXIGE_EMIT(emit);
	return copia_opcional(emit->CNAE, sizeof emit->CNAE, cnae, "[0-9]{7}");
}

int nfe_emit_set_crt(nfe_emit *emit, nfe_crt crt)
{
	EXIGE_EMIT(emit);
	if (crt < NFE_CRT_SIMPLES_NACIONAL || crt > NFE_CRT_MEI)
		return E_VALOR;
	emit->CRT = crt;
	return 0;
}

int nfe_emit_set_isufemit(nfe_emit *emit, const char *isufemit)
{
	EXIGE_EMIT(emit);
	return copia_opcional(emit->ISUF, sizeof emit->ISUF, isufemit,
	                      "[0-9]{8,9}");
}

const char *nfe_emit_documento(const nfe_emit *emit)
{
	if (!emit || emit->tipoDoc == DOC_NENHUM)
		return NULL;
	return emit->doc;
}

int nfe_emit_write_xml(xmlTextWriterPtr writer, const nfe_emit *emit)
{
	int rc;

	if (!writer || !emit)
		return E_ISNULL;
	if (emit->tipoDoc == DOC_NENHUM || emit->xNome[0] == '\0' ||
	    !emit->end || emit->CRT == NFE_CRT_NAO_INFORMADO ||
	    (emit->CNAE[0] != '\0' && emit->IM[0] == '\0'))
		return E_VALOR;

	rc = nfe_abre(writer, "emit");
	if (rc != 0)
		return rc;
	NFE_ESCREVE(emit->tipoDoc == DOC_CNPJ ? "CNPJ" : "CPF", "%s",
	            emit->doc);
	NFE_ESCREVE("xNome", "%s", emit->xNome);
	if (emit->xFant[0] != '\0')
		NFE_ESCREVE("xFant", "%s", emit->xFant);
	rc = nfe_endereco_write_xml(writer, NFE_ENDERECO_EMITENTE, emit->end);
	if (rc != 0)
		return rc;
	if (emit->IE[0] != '\0')
		NFE_ESCREVE("IE", "%s", emit->IE);
	if (emit->IEST[0] != '\0')
		NFE_ESCREVE("IEST", "%s", emit->IEST);
	if (emit->IM[0] != '\0') {
		NFE_ESCREVE("IM", "%s", emit->IM);
		if (emit->CNAE[0] != '\0')
			NFE_ESCREVE("CNAE", "%s", emit->CNAE);
	}
	NFE_ESCREVE("CRT", "%d", (int)emit->CRT);
	if (emit->ISUF[0] != '\0')
		NFE_ESCREVE("ISUFEmit", "%s", emit->ISUF);
	return nfe_fecha(writer);
}
