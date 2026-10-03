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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/entities.h>

#include <libnfe/cnpjcpf.h>
#include <libnfe/erros.h>
#include <libnfe/inutilizacao.h>
#include <libnfe/valida.h>

#define NS_NFE "http://www.portalfiscal.inf.br/nfe"

int nfe_inutilizacao(nfe_ambiente amb, nfe_uf uf, int ano, const char *cnpj,
                     nfe_modelo mod, int serie, long nini, long nfin,
                     const char *xjust, char **xml, size_t *tam)
{
	static const char cabecalho[] =
	        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	xmlChar *just;
	char *doc;
	size_t n;
	int rc;

	if (!cnpj || !xjust || !xml)
		return E_ISNULL;
	if ((amb != NFE_AMBIENTE_PRODUCAO && amb != NFE_AMBIENTE_HOMOLOGACAO) ||
	    nfe_uf_valida((int)uf) != 0 || ano < 0 || ano > 99 ||
	    strlen(cnpj) != 14 || nfe_cnpj_validar(cnpj) != 0 ||
	    (mod != NFE_MODELO_NFE && mod != NFE_MODELO_NFCE) || serie < 0 ||
	    serie > 999 || nini < 1 || nfin > 999999999L || nini > nfin)
		return E_VALOR;
	rc = nfe_valida_texto(xjust, 15, 255);
	if (rc != 0)
		return rc;
	/* A justificativa pode ter &, < e >: escapados para o XML */
	just = xmlEncodeSpecialChars(NULL, BAD_CAST xjust);
	if (!just)
		return E_MALLOC;
	n = sizeof cabecalho + strlen((const char *)just) + 512;
	doc = (char *)malloc(n);
	if (!doc) {
		xmlFree(just);
		return E_MALLOC;
	}
	/* Id: "ID" + cUF + ano + CNPJ + mod + série (3) + nNFIni (9) +
	 * nNFFin (9) */
	snprintf(doc, n,
	         "%s<inutNFe xmlns=\"" NS_NFE "\" versao=\"4.00\"><infInut "
	         "Id=\"ID%02d%02d%s%02d%03d%09ld%09ld\"><tpAmb>%d</tpAmb>"
	         "<xServ>INUTILIZAR</xServ><cUF>%02d</cUF><ano>%02d</ano>"
	         "<CNPJ>%s</CNPJ><mod>%02d</mod><serie>%d</serie><nNFIni>%ld"
	         "</nNFIni><nNFFin>%ld</nNFFin><xJust>%s</xJust></infInut>"
	         "</inutNFe>",
	         cabecalho, (int)uf, ano, cnpj, (int)mod, serie, nini, nfin,
	         (int)amb, (int)uf, ano, cnpj, (int)mod, serie, nini, nfin,
	         (const char *)just);
	xmlFree(just);
	*xml = doc;
	if (tam)
		*tam = strlen(doc);
	return 0;
}
