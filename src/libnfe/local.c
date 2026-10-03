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
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/local.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

enum tipo_doc_e { DOC_NENHUM, DOC_CNPJ, DOC_CPF };

struct nfe_local {
	enum tipo_doc_e tipoDoc;
	char doc[NFE_TAM_ASCII(NFE_TAM_CNPJ)];
	char xNome[NFE_TAM_UTF8(NFE_TAM_XNOME)]; /* "": não informado */
	nfe_endereco *end;
	char email[NFE_TAM_UTF8(NFE_TAM_EMAIL)]; /* "": não informado */
	char IE[NFE_TAM_ASCII(NFE_TAM_IE)];      /* "": não informado */
};

nfe_local *nfe_local_new(void)
{
	return (nfe_local *)calloc(1, sizeof(nfe_local));
}

void nfe_local_free(nfe_local *local)
{
	if (!local)
		return;
	nfe_endereco_free(local->end);
	free(local);
}

static int set_doc(nfe_local *local, const char *doc, enum tipo_doc_e tipo,
                   int (*valida)(const char *))
{
	int rc;

	if (!local)
		return E_ISNULL;
	rc = valida(doc);
	if (rc != 0)
		return rc;
	memcpy(local->doc, doc, strlen(doc) + 1);
	local->tipoDoc = tipo;
	return 0;
}

int nfe_local_set_cnpj(nfe_local *local, const char *cnpj)
{
	return set_doc(local, cnpj, DOC_CNPJ, nfe_cnpj_validar);
}

int nfe_local_set_cpf(nfe_local *local, const char *cpf)
{
	return set_doc(local, cpf, DOC_CPF, nfe_cpf_validar);
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

int nfe_local_set_xnome(nfe_local *local, const char *xnome)
{
	if (!local)
		return E_ISNULL;
	return texto(local->xNome, sizeof local->xNome, xnome, 2,
	             NFE_TAM_XNOME);
}

int nfe_local_set_endereco(nfe_local *local, nfe_endereco *end)
{
	if (!local || !end)
		return E_ISNULL;
	if (local->end != end) {
		nfe_endereco_free(local->end);
		local->end = end;
	}
	return 0;
}

int nfe_local_set_email(nfe_local *local, const char *email)
{
	if (!local)
		return E_ISNULL;
	return texto(local->email, sizeof local->email, email, 1,
	             NFE_TAM_EMAIL);
}

int nfe_local_set_ie(nfe_local *local, const char *ie)
{
	if (!local)
		return E_ISNULL;
	if (!ie) {
		local->IE[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(local->IE, sizeof local->IE, ie,
	                        NFE_PADRAO_TIe);
}

int nfe_local_write_xml(xmlTextWriterPtr writer, nfe_local_tipo tipo,
                        const nfe_local *local)
{
	int rc;

	if (!writer || !local)
		return E_ISNULL;
	if ((tipo != NFE_LOCAL_RETIRADA && tipo != NFE_LOCAL_ENTREGA) ||
	    local->tipoDoc == DOC_NENHUM || !local->end)
		return E_VALOR;
	rc = nfe_abre(writer,
	              tipo == NFE_LOCAL_RETIRADA ? "retirada" : "entrega");
	if (rc != 0)
		return rc;
	NFE_ESCREVE(local->tipoDoc == DOC_CNPJ ? "CNPJ" : "CPF", "%s",
	            local->doc);
	if (local->xNome[0] != '\0')
		NFE_ESCREVE("xNome", "%s", local->xNome);
	rc = nfe_endereco_write_campos(writer, local->end);
	if (rc != 0)
		return rc;
	if (local->email[0] != '\0')
		NFE_ESCREVE("email", "%s", local->email);
	if (local->IE[0] != '\0')
		NFE_ESCREVE("IE", "%s", local->IE);
	return nfe_fecha(writer);
}
