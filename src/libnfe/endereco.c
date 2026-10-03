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

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/defs.h>
#include <libnfe/endereco.h>
#include <libnfe/erros.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

struct nfe_endereco {
	char xLgr[NFE_TAM_UTF8(NFE_TAM_XLGR)];
	char nro[NFE_TAM_UTF8(NFE_TAM_NRO)];
	char xCpl[NFE_TAM_UTF8(NFE_TAM_XCPL)]; /* "": não informado */
	char xBairro[NFE_TAM_UTF8(NFE_TAM_XBAIRRO)];
	uint32_t cMun;
	char xMun[NFE_TAM_UTF8(NFE_TAM_XMUN)];
	char UF[NFE_TAM_ASCII(2)];
	char CEP[NFE_TAM_ASCII(NFE_TAM_CEP)];    /* "": não informado */
	unsigned cPais;                          /* 0: não informado */
	char xPais[NFE_TAM_UTF8(NFE_TAM_XPAIS)]; /* "": não informado */
	char fone[NFE_TAM_ASCII(NFE_TAM_FONE)];  /* "": não informado */
};

nfe_endereco *nfe_endereco_new(void)
{
	return (nfe_endereco *)calloc(1, sizeof(nfe_endereco));
}

void nfe_endereco_free(nfe_endereco *end)
{
	free(end);
}

#define EXIGE_END(end)                                                         \
	do {                                                                   \
		if (!(end))                                                    \
			return E_ISNULL;                                       \
	} while (0)

int nfe_endereco_set_xlgr(nfe_endereco *end, const char *xlgr)
{
	EXIGE_END(end);
	return nfe_copia_texto_validado(end->xLgr, sizeof end->xLgr, xlgr, 2,
	                                NFE_TAM_XLGR);
}

int nfe_endereco_set_nro(nfe_endereco *end, const char *nro)
{
	EXIGE_END(end);
	return nfe_copia_texto_validado(end->nro, sizeof end->nro, nro, 1,
	                                NFE_TAM_NRO);
}

int nfe_endereco_set_xcpl(nfe_endereco *end, const char *xcpl)
{
	EXIGE_END(end);
	if (!xcpl) {
		end->xCpl[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(end->xCpl, sizeof end->xCpl, xcpl, 1,
	                                NFE_TAM_XCPL);
}

int nfe_endereco_set_xbairro(nfe_endereco *end, const char *xbairro)
{
	EXIGE_END(end);
	return nfe_copia_texto_validado(end->xBairro, sizeof end->xBairro,
	                                xbairro, 2, NFE_TAM_XBAIRRO);
}

int nfe_endereco_set_cmun(nfe_endereco *end, uint32_t cmun)
{
	EXIGE_END(end);
	if (cmun < 1000000u || cmun > 9999999u)
		return E_VALOR;
	end->cMun = cmun;
	return 0;
}

int nfe_endereco_set_xmun(nfe_endereco *end, const char *xmun)
{
	EXIGE_END(end);
	return nfe_copia_texto_validado(end->xMun, sizeof end->xMun, xmun, 2,
	                                NFE_TAM_XMUN);
}

int nfe_endereco_set_uf(nfe_endereco *end, const char *uf)
{
	static const char *const ufs[] = { NFE_VALORES_TUf, NULL };
	int rc;

	EXIGE_END(end);
	rc = nfe_valida_lista(uf, ufs);
	if (rc != 0)
		return rc;
	memcpy(end->UF, uf, sizeof end->UF);
	return 0;
}

/* Campo opcional só com dígitos: NULL remove */
static int copia_opcional(char *dst, size_t tam, const char *valor,
                          const char *padrao)
{
	if (!valor) {
		dst[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(dst, tam, valor, padrao);
}

int nfe_endereco_set_cep(nfe_endereco *end, const char *cep)
{
	EXIGE_END(end);
	return copia_opcional(end->CEP, sizeof end->CEP, cep, "[0-9]{8}");
}

int nfe_endereco_set_cpais(nfe_endereco *end, unsigned cpais)
{
	EXIGE_END(end);
	if (cpais > 9999u)
		return E_VALOR;
	end->cPais = cpais;
	return 0;
}

int nfe_endereco_set_xpais(nfe_endereco *end, const char *xpais)
{
	EXIGE_END(end);
	if (!xpais) {
		end->xPais[0] = '\0';
		return 0;
	}
	return nfe_copia_texto_validado(end->xPais, sizeof end->xPais, xpais, 2,
	                                NFE_TAM_XPAIS);
}

int nfe_endereco_set_fone(nfe_endereco *end, const char *fone)
{
	EXIGE_END(end);
	return copia_opcional(end->fone, sizeof end->fone, fone, "[0-9]{6,14}");
}

/* Escreve <tag>valor</tag>; retorna 0 ou E_XML */
static int escreve(xmlTextWriterPtr writer, const char *tag,
                   const char *formato, ...)
{
	va_list ap;
	int rc;

	va_start(ap, formato);
	rc = xmlTextWriterWriteVFormatElement(writer, BAD_CAST tag, formato,
	                                      ap);
	va_end(ap);
	return rc < 0 ? E_XML : 0;
}

#define ESCREVE(...)                                                           \
	do {                                                                   \
		rc = escreve(writer, __VA_ARGS__);                             \
		if (rc != 0)                                                   \
			return rc;                                             \
	} while (0)

int nfe_endereco_write_xml(xmlTextWriterPtr writer, nfe_endereco_tipo tipo,
                           const nfe_endereco *end)
{
	const char *tag;
	int rc;

	if (!writer || !end)
		return E_ISNULL;
	switch (tipo) {
	case NFE_ENDERECO_EMITENTE:
		tag = "enderEmit";
		break;
	case NFE_ENDERECO_DESTINATARIO:
		tag = "enderDest";
		break;
	default:
		return E_VALOR;
	}

	/* Campos obrigatórios */
	if (end->xLgr[0] == '\0' || end->nro[0] == '\0' ||
	    end->xBairro[0] == '\0' || end->cMun == 0 || end->xMun[0] == '\0' ||
	    end->UF[0] == '\0')
		return E_VALOR;

	/* Emitente (TEnderEmi): CEP obrigatório, UF sem "EX" e país só o
	 * Brasil */
	if (tipo == NFE_ENDERECO_EMITENTE &&
	    (end->CEP[0] == '\0' || strcmp(end->UF, NFE_UF_EXTERIOR) == 0 ||
	     (end->cPais != 0 && end->cPais != NFE_CPAIS_BRASIL) ||
	     (end->xPais[0] != '\0' && strcmp(end->xPais, "Brasil") != 0 &&
	      strcmp(end->xPais, "BRASIL") != 0)))
		return E_VALOR;

	if (xmlTextWriterStartElement(writer, BAD_CAST tag) < 0)
		return E_XML;
	ESCREVE("xLgr", "%s", end->xLgr);
	ESCREVE("nro", "%s", end->nro);
	if (end->xCpl[0] != '\0')
		ESCREVE("xCpl", "%s", end->xCpl);
	ESCREVE("xBairro", "%s", end->xBairro);
	ESCREVE("cMun", "%07u", (unsigned)end->cMun);
	ESCREVE("xMun", "%s", end->xMun);
	ESCREVE("UF", "%s", end->UF);
	if (end->CEP[0] != '\0')
		ESCREVE("CEP", "%s", end->CEP);
	if (end->cPais != 0)
		ESCREVE("cPais", "%u", end->cPais);
	if (end->xPais[0] != '\0')
		ESCREVE("xPais", "%s", end->xPais);
	if (end->fone[0] != '\0')
		ESCREVE("fone", "%s", end->fone);
	if (xmlTextWriterEndElement(writer) < 0)
		return E_XML;
	return 0;
}
