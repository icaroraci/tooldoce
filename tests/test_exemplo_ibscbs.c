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

/* Testa a nota do exemplo examples/nfe_ibscbs.c (NF-e com IBS/CBS): valida
 * contra o schema e as regras da SEFAZ, confere os valores do item e dos
 * totais e a assinatura.
 *
 * Uso: test_exemplo_ibscbs <diretório tests> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/assinatura.h>
#include <libnfe/validar.h>

#include "teste.h"

/* O exemplo, com o main renomeado */
#define main exemplo_main
#include "../examples/nfe_ibscbs.c"
#undef main

#define T0 ((time_t)1791014400) /* 2026-10-03T08:00:00Z */

static int contem(const char *s, const char *trecho)
{
	return s && strstr(s, trecho) != NULL;
}

int main(int argc, char **argv)
{
	char dir[1024], pfx[1024], *xml = NULL, *assinado = NULL;
	size_t tam = 0, tam_assinado = 0;
	nfe_validador *v;
	nfe_erros *erros = nfe_erros_new();
	nfe_certificado *cert;
	nfe_nfe *nota;
	int i, rc;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	nota = monta_nota(T0);
	VERIFICA(nota != NULL);
	if (!nota)
		TESTE_FIM();
	VERIFICA_INT(nfe_nfe_xml(nota, &xml, &tam), 0);

	/* Schema e regras da SEFAZ (chave, totais, UF) */
	snprintf(dir, sizeof dir, "%s/schemas/nfe", argv[1]);
	v = nfe_validador_new(dir);
	VERIFICA(v != NULL);
	rc = nfe_validar_xml(v, xml, tam, erros);
	VERIFICA_INT(rc, 0);
	for (i = 0; i < nfe_erros_qtd(erros); i++)
		fprintf(stderr, "%s: %s\n", nfe_erros_campo(erros, i),
		        nfe_erros_msg(erros, i));

	/* Item: ICMS20, PIS/COFINS alíquota zero e IBS/CBS */
	VERIFICA(contem(xml,
	                "<ICMS20><orig>0</orig><CST>20</CST><modBC>3"
	                "</modBC><pRedBC>60.00</pRedBC><vBC>15.20</vBC>"
	                "<pICMS>12.00</pICMS><vICMS>1.82</vICMS></ICMS20>"));
	VERIFICA(contem(xml, "<IBSCBS><CST>200</CST><cClassTrib>200038"
	                     "</cClassTrib><gIBSCBS><vBC>36.18</vBC><gIBSUF>"
	                     "<pIBSUF>0.10</pIBSUF><gRed><pRedAliq>60.00"
	                     "</pRedAliq><pAliqEfet>0.04</pAliqEfet></gRed>"
	                     "<vIBSUF>0.01</vIBSUF></gIBSUF>"));
	VERIFICA(contem(xml, "<vIBS>0.01</vIBS><gCBS><pCBS>0.90</pCBS><gRed>"
	                     "<pRedAliq>60.00</pRedAliq><pAliqEfet>0.36"
	                     "</pAliqEfet></gRed><vCBS>0.13</vCBS></gCBS>"));
	/* Totais: ICMSTot calculado e IBSCBSTot informado */
	VERIFICA(contem(xml, "<ICMSTot><vBC>15.20</vBC><vICMS>1.82</vICMS>"));
	VERIFICA(contem(xml, "<vProd>38.00</vProd>"));
	VERIFICA(contem(xml, "<vNF>38.00</vNF></ICMSTot><IBSCBSTot><vBCIBSCBS>"
	                     "36.18</vBCIBSCBS>"));
	VERIFICA(contem(xml, "<vNFTot>38.00</vNFTot></total>"));

	/* Assinada com o certificado de teste */
	snprintf(pfx, sizeof pfx, "%s/certificados/teste.pfx", argv[1]);
	cert = nfe_certificado_pfx(pfx, "teste", &rc);
	VERIFICA(cert != NULL);
	if (cert) {
		VERIFICA_INT(
		        nfe_nfe_assinar(nota, cert, &assinado, &tam_assinado),
		        0);
		VERIFICA_INT(nfe_verificar_assinatura(assinado, tam_assinado),
		             0);
		if (v && assinado)
			VERIFICA_INT(nfe_validar_xml(v, assinado, tam_assinado,
			                             erros),
			             0);
	}

	free(assinado);
	free(xml);
	nfe_certificado_free(cert);
	nfe_nfe_free(nota);
	nfe_validador_free(v);
	nfe_erros_free(erros);
	xmlCleanupParser();
	TESTE_FIM();
}
