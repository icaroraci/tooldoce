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
#include <libnfe/resptec.h>
#include <libnfe/valida.h>

/* base64 de 20 bytes */
#define PADRAO_HASH "[A-Za-z0-9+/]{27}="

struct nfe_resptec {
	int informado;
	char CNPJ[NFE_TAM_ASCII(NFE_TAM_CNPJ)];
	char xContato[NFE_TAM_UTF8(NFE_TAM_XCONTATO)];
	char email[NFE_TAM_UTF8(NFE_TAM_EMAIL)];
	char fone[NFE_TAM_ASCII(NFE_TAM_FONE)];
	unsigned idCSRT;
	char hashCSRT[29]; /* "": não informado */
};

nfe_resptec *nfe_resptec_new(void)
{
	return (nfe_resptec *)calloc(1, sizeof(nfe_resptec));
}

void nfe_resptec_free(nfe_resptec *rt)
{
	free(rt);
}

int nfe_resptec_set(nfe_resptec *rt, const char *cnpj, const char *xcontato,
                    const char *email, const char *fone)
{
	int rc;

	if (!rt)
		return E_ISNULL;
	rc = nfe_cnpj_validar(cnpj);
	if (rc == 0)
		rc = nfe_valida_texto(xcontato, 2, NFE_TAM_XCONTATO);
	if (rc == 0)
		rc = nfe_valida_texto(email, 6, NFE_TAM_EMAIL);
	if (rc == 0)
		rc = nfe_valida_padrao(fone, "[0-9]{6,14}");
	if (rc != 0)
		return rc;
	strcpy(rt->CNPJ, cnpj);
	strcpy(rt->xContato, xcontato);
	strcpy(rt->email, email);
	strcpy(rt->fone, fone);
	rt->informado = 1;
	return 0;
}

int nfe_resptec_set_csrt(nfe_resptec *rt, unsigned idcsrt, const char *hashcsrt)
{
	int rc;

	if (!rt)
		return E_ISNULL;
	if (!hashcsrt) {
		rt->hashCSRT[0] = '\0';
		return 0;
	}
	if (idcsrt > 99)
		return E_VALOR;
	rc = nfe_copia_padrao(rt->hashCSRT, sizeof rt->hashCSRT, hashcsrt,
	                      PADRAO_HASH);
	if (rc != 0)
		return rc;
	rt->idCSRT = idcsrt;
	return 0;
}

int nfe_resptec_write_xml(xmlTextWriterPtr writer, const nfe_resptec *rt)
{
	int rc;

	if (!writer || !rt)
		return E_ISNULL;
	if (!rt->informado)
		return E_VALOR;
	rc = nfe_abre(writer, "infRespTec");
	if (rc != 0)
		return rc;
	NFE_ESCREVE("CNPJ", "%s", rt->CNPJ);
	NFE_ESCREVE("xContato", "%s", rt->xContato);
	NFE_ESCREVE("email", "%s", rt->email);
	NFE_ESCREVE("fone", "%s", rt->fone);
	if (rt->hashCSRT[0] != '\0') {
		NFE_ESCREVE("idCSRT", "%02u", rt->idCSRT);
		NFE_ESCREVE("hashCSRT", "%s", rt->hashCSRT);
	}
	return nfe_fecha(writer);
}
