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

/* Testes dos eventos (evento.h) e das mensagens de evento (sefaz.h),
 * validados contra os schemas oficiais dos eventos.
 *
 * Uso: test_evento <diretório tests> */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/assinatura.h>
#include <libnfe/chave.h>
#include <libnfe/erros.h>
#include <libnfe/evento.h>
#include <libnfe/sefaz.h>

#include "teste.h"
#include "teste_xml.h"

#define CHAVE "35261012345678000195650010000000011123456784"
#define PROT  "135260000000001"
#define JUST  "ERRO NA DIGITACAO DO PEDIDO"

/* 2026-10-03T10:00:00-03:00 */
#define INSTANTE ((time_t)1791032400)

static int contem(const char *s, const char *trecho)
{
	return s && strstr(s, trecho) != NULL;
}

/* Retorno da SEFAZ com o registro do evento */
static const char RETORNO[] =
        "<retEnvEvento xmlns=\"http://www.portalfiscal.inf.br/nfe\" "
        "versao=\"1.00\"><idLote>1</idLote><tpAmb>2</tpAmb><verAplic>TESTE"
        "</verAplic><cOrgao>35</cOrgao><cStat>128</cStat><xMotivo>Lote de "
        "evento processado</xMotivo><retEvento versao=\"1.00\"><infEvento>"
        "<tpAmb>2</tpAmb><verAplic>TESTE</verAplic><cOrgao>35</cOrgao>"
        "<cStat>135</cStat><xMotivo>Evento registrado e vinculado a NF-e"
        "</xMotivo><chNFe>" CHAVE "</chNFe><tpEvento>110111</tpEvento>"
        "<xEvento>Cancelamento registrado</xEvento><nSeqEvento>1"
        "</nSeqEvento><dhRegEvento>2026-10-03T10:00:05-03:00</dhRegEvento>"
        "<nProt>135260000000002</nProt></infEvento></retEvento>"
        "</retEnvEvento>";

static void testa_erros(void)
{
	nfe_evento_info info = { NFE_AMBIENTE_HOMOLOGACAO, CHAVE,
		                 "12345678000195", INSTANTE, NFE_TZD_BRASILIA };
	nfe_evento_info ruim = info;
	char *xml = NULL;

	VERIFICA_INT(nfe_evento_cancelamento(NULL, PROT, JUST, &xml, NULL),
	             E_ISNULL);
	VERIFICA_INT(nfe_evento_cancelamento(&info, NULL, JUST, &xml, NULL),
	             E_ISNULL);
	VERIFICA_INT(nfe_evento_cancelamento(&info, PROT, JUST, NULL, NULL),
	             E_ISNULL);
	/* Justificativa curta, protocolo e chave inválidos */
	VERIFICA_INT(nfe_evento_cancelamento(&info, PROT, "CURTA", &xml, NULL),
	             E_TAMANHO);
	VERIFICA_INT(nfe_evento_cancelamento(&info, "1234", JUST, &xml, NULL),
	             E_VALOR);
	ruim.chave = "35261012345678000195650010000000011123456785";
	VERIFICA_INT(nfe_evento_cancelamento(&ruim, PROT, JUST, &xml, NULL),
	             E_VALOR);
	ruim = info;
	ruim.cnpjcpf = "12345678000196";
	VERIFICA_INT(nfe_evento_cancelamento(&ruim, PROT, JUST, &xml, NULL),
	             E_VALOR);
	ruim = info;
	ruim.amb = (nfe_ambiente)3;
	VERIFICA_INT(nfe_evento_cancelamento(&ruim, PROT, JUST, &xml, NULL),
	             E_VALOR);
	ruim = info;
	ruim.tzd = (nfe_tzd)0;
	VERIFICA_INT(nfe_evento_cancelamento(&ruim, PROT, JUST, &xml, NULL),
	             E_VALOR);
	ruim = info;
	ruim.chave = NULL;
	VERIFICA_INT(nfe_evento_cancelamento(&ruim, PROT, JUST, &xml, NULL),
	             E_ISNULL);
	/* Substituição: a própria nota, nota que não é NFC-e */
	VERIFICA_INT(nfe_evento_cancelamento_subst(&info, PROT, JUST, CHAVE,
	                                           "app 1.0", &xml, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_evento_cancelamento_subst(&info, PROT, JUST, CHAVE, "",
	                                           &xml, NULL),
	             E_TAMANHO);
	VERIFICA(xml == NULL);
}

int main(int argc, char **argv)
{
	nfe_evento_info info = { NFE_AMBIENTE_HOMOLOGACAO, CHAVE,
		                 "12345678000195", INSTANTE, NFE_TZD_BRASILIA };
	char pfx[1024], subst[45], *ev = NULL, *assinado = NULL, *msg = NULL,
	                           *proc = NULL;
	const char *lote[2];
	size_t tam = 0, tam_assinado = 0, tam_proc = 0;
	nfe_certificado *cert;
	int rc, cstat = 0;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	testa_erros();
	snprintf(pfx, sizeof pfx, "%s/certificados/teste.pfx", argv[1]);
	cert = nfe_certificado_pfx(pfx, "teste", &rc);
	VERIFICA(cert != NULL);
	if (!cert)
		TESTE_FIM();

	/* Cancelamento */
	VERIFICA_INT(nfe_evento_cancelamento(&info, PROT,
	                                     "ERRO & <TESTE> NO "
	                                     "PEDIDO",
	                                     &ev, &tam),
	             0);
	VERIFICA(ev && strlen(ev) == tam);
	VERIFICA(contem(ev, "<infEvento Id=\"ID110111" CHAVE "01\">"
	                    "<cOrgao>35</cOrgao><tpAmb>2</tpAmb><CNPJ>"
	                    "12345678000195</CNPJ><chNFe>" CHAVE "</chNFe>"
	                    "<dhEvento>2026-10-03T10:00:00-03:00</dhEvento>"
	                    "<tpEvento>110111</tpEvento><nSeqEvento>1"
	                    "</nSeqEvento><verEvento>1.00</verEvento>"));
	VERIFICA(contem(ev, "<xJust>ERRO &amp; &lt;TESTE&gt; NO PEDIDO"));
	VERIFICA_INT(nfe_assinar_xml(cert, ev, tam, &assinado, &tam_assinado),
	             0);
	VERIFICA_INT(nfe_verificar_assinatura(assinado, tam_assinado), 0);
	VERIFICA_INT(teste_valida_xsd(argv[1],
	                              "evento_canc/eventoCancNFe_v1.00.xsd",
	                              assinado, tam_assinado),
	             0);

	/* Lote de eventos, validado com o schema do cancelamento e com o
	 * schema genérico */
	lote[0] = assinado;
	VERIFICA_INT(nfe_sefaz_msg_evento("1", lote, 1, &msg), 0);
	VERIFICA(contem(msg, "<envEvento xmlns=\"http://www.portalfiscal.inf."
	                     "br/nfe\" versao=\"1.00\"><idLote>1</idLote>"
	                     "<evento"));
	VERIFICA_INT(teste_valida_xsd(argv[1],
	                              "evento_canc/envEventoCancNFe_v1.00.xsd",
	                              msg, strlen(msg)),
	             0);
	VERIFICA_INT(teste_valida_xsd(argv[1], "evento/envEvento_v1.00.xsd",
	                              msg, strlen(msg)),
	             0);
	free(msg);
	msg = NULL;
	VERIFICA_INT(nfe_sefaz_msg_evento("1", lote, 21, &msg), E_VALOR);
	VERIFICA_INT(nfe_sefaz_msg_evento("x", lote, 1, &msg), E_VALOR);
	lote[1] = "<NFe/>";
	VERIFICA_INT(nfe_sefaz_msg_evento("1", lote, 2, &msg), E_VALOR);
	lote[1] = NULL;
	VERIFICA_INT(nfe_sefaz_msg_evento("1", lote, 2, &msg), E_ISNULL);

	/* Retorno e procEventoNFe */
	VERIFICA_INT(
	        teste_valida_xsd(argv[1],
	                         "evento_canc/retEnvEventoCancNFe_v1.00.xsd",
	                         RETORNO, strlen(RETORNO)),
	        0);
	VERIFICA_INT(nfe_sefaz_cstat(RETORNO, strlen(RETORNO), &cstat, NULL, 0),
	             0);
	VERIFICA_INT(cstat, 128);
	VERIFICA_INT(nfe_sefaz_proc_evento(assinado, tam_assinado, RETORNO,
	                                   strlen(RETORNO), &proc, &tam_proc),
	             0);
	VERIFICA(proc && strlen(proc) == tam_proc);
	VERIFICA(contem(proc, "<procEventoNFe xmlns=\"http://www.portalfiscal."
	                      "inf.br/nfe\" versao=\"1.00\"><evento"));
	VERIFICA(contem(proc, strstr(assinado, "<evento")));
	if (proc) {
		VERIFICA_INT(teste_valida_xsd(argv[1],
		                              "evento_canc/"
		                              "procEventoCancNFe_v1.00.xsd",
		                              proc, tam_proc),
		             0);
		VERIFICA_INT(teste_valida_xsd(argv[1],
		                              "evento/procEventoNFe_v1.00.xsd",
		                              proc, tam_proc),
		             0);
		/* cStat do evento, dentro de retEvento */
		VERIFICA_INT(
		        nfe_sefaz_cstat(strstr(proc, "<retEvento"),
		                        strlen(strstr(proc, "<retEvento")) -
		                                strlen("</procEventoNFe>"),
		                        &cstat, NULL, 0),
		        0);
		VERIFICA_INT(cstat, 135);
	}
	free(proc);
	proc = NULL;
	/* Retorno de outro evento */
	{
		char *outro = strdup(RETORNO), *p = strstr(outro, "110111");

		p[5] = '0'; /* 110110 */
		VERIFICA_INT(nfe_sefaz_proc_evento(assinado, tam_assinado,
		                                   outro, strlen(outro), &proc,
		                                   NULL),
		             E_VALOR);
		free(outro);
	}
	VERIFICA_INT(nfe_sefaz_proc_evento(RETORNO, strlen(RETORNO), RETORNO,
	                                   strlen(RETORNO), &proc, NULL),
	             E_XML);
	VERIFICA_INT(nfe_sefaz_proc_evento(NULL, 0, RETORNO, 1, &proc, NULL),
	             E_ISNULL);
	free(assinado);
	assinado = NULL;
	free(ev);
	ev = NULL;

	/* Cancelamento por substituição (NFC-e), por CPF */
	memcpy(subst, CHAVE, 43);
	subst[33] = '2'; /* outra nNF */
	subst[43] = (char)('0' + nfe_chave_dv(subst));
	subst[44] = '\0';
	info.cnpjcpf = "52998224725";
	VERIFICA_INT(nfe_evento_cancelamento_subst(&info, PROT, JUST, subst,
	                                           "app 1.0", &ev, &tam),
	             0);
	VERIFICA(contem(ev, "<CPF>52998224725</CPF>"));
	VERIFICA(contem(ev,
	                "<descEvento>Cancelamento por substituicao"
	                "</descEvento><cOrgaoAutor>35</cOrgaoAutor>"
	                "<tpAutor>1</tpAutor><verAplic>app 1.0</verAplic>"));
	VERIFICA_INT(nfe_assinar_xml(cert, ev, tam, &assinado, &tam_assinado),
	             0);
	VERIFICA_INT(teste_valida_xsd(argv[1],
	                              "evento_cancsubst/"
	                              "eventoCancSubst_v1.00.xsd",
	                              assinado, tam_assinado),
	             0);
	free(assinado);
	free(ev);

	/* Carta de correção, segunda da nota */
	info.cnpjcpf = "12345678000195";
	VERIFICA_INT(nfe_evento_cce(&info, 2,
	                            "ONDE SE LE RUA DAS FLORES, LEIA-SE RUA "
	                            "DAS ROSAS",
	                            &ev, &tam),
	             0);
	VERIFICA(contem(ev, "Id=\"ID110110" CHAVE "02\""));
	VERIFICA(contem(ev, "<nSeqEvento>2</nSeqEvento>"));
	VERIFICA(contem(ev, "<descEvento>Carta de Correcao</descEvento>"
	                    "<xCorrecao>ONDE SE LE"));
	VERIFICA_INT(nfe_assinar_xml(cert, ev, tam, &assinado, &tam_assinado),
	             0);
	VERIFICA_INT(teste_valida_xsd(argv[1], "evento_cce/CCe_v1.00.xsd",
	                              assinado, tam_assinado),
	             0);
	lote[0] = assinado;
	VERIFICA_INT(nfe_sefaz_msg_evento("2", lote, 1, &msg), 0);
	VERIFICA_INT(teste_valida_xsd(argv[1], "evento_cce/envCCe_v1.00.xsd",
	                              msg, strlen(msg)),
	             0);
	free(msg);
	free(assinado);
	free(ev);
	ev = NULL;
	VERIFICA_INT(
	        nfe_evento_cce(&info, 0, "CORRECAO DE TESTE OK", &ev, NULL),
	        E_VALOR);
	VERIFICA_INT(
	        nfe_evento_cce(&info, 21, "CORRECAO DE TESTE OK", &ev, NULL),
	        E_VALOR);
	VERIFICA_INT(nfe_evento_cce(&info, 1, "CURTA", &ev, NULL), E_TAMANHO);
	VERIFICA_INT(nfe_evento_cce(&info, 1, NULL, &ev, NULL), E_ISNULL);
	VERIFICA(ev == NULL);

	nfe_certificado_free(cert);
	xmlCleanupParser();
	TESTE_FIM();
}
