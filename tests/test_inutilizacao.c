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

/* Testes da inutilização de numeração (inutilizacao.h) e do ProcInutNFe
 * (sefaz.h), validados contra os schemas oficiais.
 *
 * Uso: test_inutilizacao <diretório tests> */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/assinatura.h>
#include <libnfe/erros.h>
#include <libnfe/inutilizacao.h>
#include <libnfe/sefaz.h>

#include "teste.h"
#include "teste_xml.h"

#define CNPJ "12345678000195"
#define JUST "NUMEROS PULADOS POR FALHA DO SISTEMA"

/* Retorno da SEFAZ: inutilização homologada */
static const char RETORNO[] =
        "<retInutNFe xmlns=\"http://www.portalfiscal.inf.br/nfe\" "
        "versao=\"4.00\"><infInut><tpAmb>2</tpAmb><verAplic>TESTE"
        "</verAplic><cStat>102</cStat><xMotivo>Inutilizacao de numero "
        "homologado</xMotivo><cUF>35</cUF><ano>26</ano><CNPJ>" CNPJ
        "</CNPJ><mod>55</mod><serie>1</serie><nNFIni>10</nNFIni><nNFFin>12"
        "</nNFFin><dhRecbto>2026-10-03T10:00:00-03:00</dhRecbto><nProt>"
        "135260000000003</nProt></infInut></retInutNFe>";

static int contem(const char *s, const char *trecho)
{
	return s && strstr(s, trecho) != NULL;
}

static int inut(int ano, const char *cnpj, int mod, int serie, long ini,
                long fin, const char *just)
{
	char *xml = NULL;
	int rc = nfe_inutilizacao(NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP, ano,
	                          cnpj, (nfe_modelo)mod, serie, ini, fin, just,
	                          &xml, NULL);

	free(xml);
	return rc;
}

int main(int argc, char **argv)
{
	char pfx[1024], *xml = NULL, *assinado = NULL, *proc = NULL;
	size_t tam = 0, tam_assinado = 0, tam_proc = 0;
	nfe_certificado *cert;
	int rc, cstat = 0;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}

	/* Valores inválidos */
	VERIFICA_INT(inut(26, CNPJ, 55, 1, 10, 12, JUST), 0);
	VERIFICA_INT(inut(100, CNPJ, 55, 1, 10, 12, JUST), E_VALOR);
	VERIFICA_INT(inut(26, "12345678000196", 55, 1, 10, 12, JUST), E_VALOR);
	VERIFICA_INT(inut(26, "52998224725", 55, 1, 10, 12, JUST), E_VALOR);
	VERIFICA_INT(inut(26, CNPJ, 57, 1, 10, 12, JUST), E_VALOR);
	VERIFICA_INT(inut(26, CNPJ, 55, 1000, 10, 12, JUST), E_VALOR);
	VERIFICA_INT(inut(26, CNPJ, 55, 1, 0, 12, JUST), E_VALOR);
	VERIFICA_INT(inut(26, CNPJ, 55, 1, 12, 10, JUST), E_VALOR);
	VERIFICA_INT(inut(26, CNPJ, 55, 1, 10, 1000000000L, JUST), E_VALOR);
	VERIFICA_INT(inut(26, CNPJ, 55, 1, 10, 12, "CURTA"), E_TAMANHO);
	VERIFICA_INT(inut(26, NULL, 55, 1, 10, 12, JUST), E_ISNULL);
	VERIFICA_INT(nfe_inutilizacao(NFE_AMBIENTE_HOMOLOGACAO, (nfe_uf)34, 26,
	                              CNPJ, NFE_MODELO_NFE, 1, 10, 12, JUST,
	                              &xml, NULL),
	             E_VALOR);
	VERIFICA(xml == NULL);

	VERIFICA_INT(nfe_inutilizacao(NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP, 26,
	                              CNPJ, NFE_MODELO_NFE, 1, 10, 12,
	                              "FALHA & <TESTE> NO SISTEMA", &xml, &tam),
	             0);
	VERIFICA(xml && strlen(xml) == tam);
	VERIFICA(contem(
	        xml, "<inutNFe xmlns=\"http://www.portalfiscal.inf.br/"
	             "nfe\" versao=\"4.00\"><infInut Id=\"ID3526" CNPJ
	             "55001000000010000000012\"><tpAmb>2</tpAmb><xServ>"
	             "INUTILIZAR</xServ><cUF>35</cUF><ano>26</ano><CNPJ>" CNPJ
	             "</CNPJ><mod>55</mod><serie>1</serie><nNFIni>"
	             "10</nNFIni><nNFFin>12</nNFFin><xJust>FALHA &amp; "
	             "&lt;TESTE&gt; NO SISTEMA</xJust></infInut>"
	             "</inutNFe>"));

	snprintf(pfx, sizeof pfx, "%s/certificados/teste.pfx", argv[1]);
	cert = nfe_certificado_pfx(pfx, "teste", &rc);
	VERIFICA(cert != NULL);
	if (!cert) {
		free(xml);
		TESTE_FIM();
	}
	VERIFICA_INT(nfe_assinar_xml(cert, xml, tam, &assinado, &tam_assinado),
	             0);
	VERIFICA_INT(nfe_verificar_assinatura(assinado, tam_assinado), 0);

	/* Retorno e ProcInutNFe, validado com o schema oficial */
	VERIFICA_INT(nfe_sefaz_cstat(RETORNO, strlen(RETORNO), &cstat, NULL, 0),
	             0);
	VERIFICA_INT(cstat, 102);
	VERIFICA_INT(nfe_sefaz_proc_inutilizacao(assinado, tam_assinado,
	                                         RETORNO, strlen(RETORNO),
	                                         &proc, &tam_proc),
	             0);
	VERIFICA(proc && strlen(proc) == tam_proc);
	VERIFICA(contem(proc, strstr(assinado, "<inutNFe")));
	if (proc)
		VERIFICA_INT(teste_valida_xsd(argv[1],
		                              "nfe/procInutNFe_v4.00.xsd", proc,
		                              tam_proc),
		             0);
	free(proc);
	proc = NULL;

	/* Retorno de outra faixa; documentos errados */
	{
		char *outro = strdup(RETORNO), *p = strstr(outro, "<nNFFin>12");

		p[9] = '3';
		VERIFICA_INT(nfe_sefaz_proc_inutilizacao(assinado, tam_assinado,
		                                         outro, strlen(outro),
		                                         &proc, NULL),
		             E_VALOR);
		free(outro);
	}
	VERIFICA_INT(nfe_sefaz_proc_inutilizacao(assinado, tam_assinado, "<a/>",
	                                         4, &proc, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_proc_inutilizacao(RETORNO, strlen(RETORNO),
	                                         RETORNO, strlen(RETORNO),
	                                         &proc, NULL),
	             E_XML);
	VERIFICA_INT(
	        nfe_sefaz_proc_inutilizacao(NULL, 0, RETORNO, 1, &proc, NULL),
	        E_ISNULL);

	free(assinado);
	free(xml);
	nfe_certificado_free(cert);
	xmlCleanupParser();
	TESTE_FIM();
}
