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

/* Testes da comunicação com a SEFAZ (sefaz.h), contra o servidor falso
 * tests/servidor_sefaz.py (HTTPS com autenticação mútua, em 127.0.0.1).
 *
 * Uso: test_sefaz <diretório tests> */

#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <libnfe/assinatura.h>
#include <libnfe/chave.h>
#include <libnfe/erros.h>
#include <libnfe/nfe_nfe.h>
#include <libnfe/sefaz.h>

#include "teste.h"
#include "nota_teste.h"
#include "teste_xml.h"

/* Inicia o servidor falso; devolve o pid e a porta em *porta (0 se não
 * foi possível) */
static pid_t inicia_servidor(const char *dir, int *porta)
{
	char script[1024], linha[32];
	int fd[2];
	pid_t pid;
	FILE *f;

	*porta = 0;
	snprintf(script, sizeof script, "%s/servidor_sefaz.py", dir);
	if (pipe(fd) != 0)
		return -1;
	pid = fork();
	if (pid < 0)
		return -1;
	if (pid == 0) {
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execlp("python3", "python3", script, dir, (char *)NULL);
		_exit(127);
	}
	close(fd[1]);
	f = fdopen(fd[0], "r");
	if (f && fgets(linha, sizeof linha, f))
		*porta = atoi(linha);
	if (f)
		fclose(f);
	else
		close(fd[0]);
	return pid;
}

/* Contém o texto? */
static int contem(const char *s, const char *trecho)
{
	return s && strstr(s, trecho) != NULL;
}

static void testa_mensagens(const char *dir)
{
	char *msg = NULL;
	const char *um[1];
	const char *dois[2];

	VERIFICA_INT(
	        nfe_sefaz_msg_status(NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP, &msg),
	        0);
	VERIFICA_STR(msg, "<consStatServ xmlns=\"http://www.portalfiscal.inf."
	                  "br/nfe\" versao=\"4.00\"><tpAmb>2</tpAmb><cUF>35"
	                  "</cUF><xServ>STATUS</xServ></consStatServ>");
	free(msg);
	VERIFICA_INT(nfe_sefaz_msg_status((nfe_ambiente)3, NFE_UF_SP, &msg),
	             E_VALOR);
	VERIFICA_INT(
	        nfe_sefaz_msg_status(NFE_AMBIENTE_PRODUCAO, (nfe_uf)34, &msg),
	        E_VALOR);
	VERIFICA_INT(
	        nfe_sefaz_msg_status(NFE_AMBIENTE_PRODUCAO, NFE_UF_SP, NULL),
	        E_ISNULL);

	VERIFICA_INT(nfe_sefaz_msg_recibo(NFE_AMBIENTE_PRODUCAO,
	                                  "351000000000001", &msg),
	             0);
	VERIFICA_STR(msg, "<consReciNFe xmlns=\"http://www.portalfiscal.inf."
	                  "br/nfe\" versao=\"4.00\"><tpAmb>1</tpAmb><nRec>"
	                  "351000000000001</nRec></consReciNFe>");
	VERIFICA_INT(
	        teste_valida_xsd(dir, "nfe/tipos_v4.00.xsd", msg, strlen(msg)),
	        0);
	free(msg);
	VERIFICA_INT(nfe_sefaz_msg_recibo(NFE_AMBIENTE_PRODUCAO, "3510", &msg),
	             E_VALOR);

	VERIFICA_INT(nfe_sefaz_msg_consulta(
	                     NFE_AMBIENTE_HOMOLOGACAO,
	                     "35261012345678000195650010000000011123456784",
	                     &msg),
	             0);
	VERIFICA(contem(msg, "<xServ>CONSULTAR</xServ><chNFe>352610123456780"
	                     "00195650010000000011123456784</chNFe>"));
	VERIFICA_INT(teste_valida_xsd(dir, "nfe/consSitNFe_v4.00.xsd", msg,
	                              strlen(msg)),
	             0);
	free(msg);
	/* CNPJ alfanumérico na chave */
	{
		char alfa[45] = "3526101"
		                "2ABC3450001"
		                "9565001000000001112345678";

		alfa[43] = (char)('0' + nfe_chave_dv(alfa));
		alfa[44] = '\0';
		VERIFICA_INT(nfe_sefaz_msg_consulta(NFE_AMBIENTE_HOMOLOGACAO,
		                                    alfa, &msg),
		             0);
		VERIFICA(contem(msg, alfa));
		free(msg);
	}
	/* Dígito verificador errado */
	VERIFICA_INT(nfe_sefaz_msg_consulta(
	                     NFE_AMBIENTE_HOMOLOGACAO,
	                     "35261012345678000195650010000000011123456785",
	                     &msg),
	             E_VALOR);

	um[0] = "<?xml version=\"1.0\"?>\n<NFe xmlns=\"x\"><a/></NFe>\n";
	VERIFICA_INT(nfe_sefaz_msg_lote("42", 1, um, 1, &msg), 0);
	VERIFICA_STR(msg, "<enviNFe xmlns=\"http://www.portalfiscal.inf.br/"
	                  "nfe\" versao=\"4.00\"><idLote>42</idLote><indSinc>1"
	                  "</indSinc><NFe xmlns=\"x\"><a/></NFe></enviNFe>");
	free(msg);
	dois[0] = um[0];
	dois[1] = "<NFe><b/></NFe>";
	VERIFICA_INT(nfe_sefaz_msg_lote("1", 0, dois, 2, &msg), 0);
	VERIFICA(contem(msg, "<indSinc>0</indSinc><NFe xmlns=\"x\"><a/></NFe>"
	                     "<NFe><b/></NFe></enviNFe>"));
	free(msg);
	/* Síncrono só com uma nota; lote e documentos inválidos */
	VERIFICA_INT(nfe_sefaz_msg_lote("1", 1, dois, 2, &msg), E_VALOR);
	VERIFICA_INT(nfe_sefaz_msg_lote("1a", 0, um, 1, &msg), E_VALOR);
	VERIFICA_INT(nfe_sefaz_msg_lote("1234567890123456", 0, um, 1, &msg),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_msg_lote("1", 0, um, 0, &msg), E_VALOR);
	dois[1] = "<NFeX/>";
	VERIFICA_INT(nfe_sefaz_msg_lote("1", 0, dois, 2, &msg), E_VALOR);
	dois[1] = NULL;
	VERIFICA_INT(nfe_sefaz_msg_lote("1", 0, dois, 2, &msg), E_ISNULL);
}

static void testa_retorno(void)
{
	static const char ret[] =
	        "<retConsStatServ xmlns=\"http://www.portalfiscal.inf.br/nfe\" "
	        "versao=\"4.00\"><tpAmb>2</tpAmb><cStat>107</cStat><xMotivo>"
	        "Servico em Operacao</xMotivo></retConsStatServ>";
	char motivo[8];
	int cstat = 0;

	VERIFICA_INT(nfe_sefaz_cstat(ret, strlen(ret), &cstat, motivo,
	                             sizeof motivo),
	             0);
	VERIFICA_INT(cstat, 107);
	VERIFICA_STR(motivo, "Servico"); /* truncado */
	VERIFICA_INT(nfe_sefaz_cstat(ret, strlen(ret), &cstat, NULL, 0), 0);
	VERIFICA_INT(nfe_sefaz_cstat("<a/>", 4, &cstat, NULL, 0), E_XML);
	VERIFICA_INT(nfe_sefaz_cstat("<a", 2, &cstat, NULL, 0), E_XML);
	VERIFICA_INT(nfe_sefaz_cstat(NULL, 0, &cstat, NULL, 0), E_ISNULL);
}

int main(int argc, char **argv)
{
	char pfx[1024], ca[1024], url[256], *xml = NULL, *msg = NULL,
	                                    *ret = NULL, *prot = NULL,
	                                    *proc = NULL, chave[45];
	const char *lote[1];
	nfe_certificado *cert;
	nfe_sefaz *s;
	nfe_nfe *nfe;
	size_t tam = 0, tam_ret = 0, tam_prot = 0, tam_proc = 0;
	int porta, rc, cstat = 0;
	pid_t pid;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	testa_mensagens(argv[1]);
	testa_retorno();

	snprintf(pfx, sizeof pfx, "%s/certificados/teste.pfx", argv[1]);
	snprintf(ca, sizeof ca, "%s/certificados/servidor.pem", argv[1]);
	cert = nfe_certificado_pfx(pfx, "teste", &rc);
	VERIFICA(cert != NULL);
	if (!cert)
		TESTE_FIM();
	VERIFICA(nfe_sefaz_new(NULL) == NULL);
	s = nfe_sefaz_new(cert);
	VERIFICA(s != NULL);
	VERIFICA_STR(nfe_sefaz_erro(s), "");
	VERIFICA_INT(nfe_sefaz_set_timeout(s, 0), E_VALOR);
	VERIFICA_INT(nfe_sefaz_set_timeout(s, 20), 0);
	VERIFICA_INT(nfe_sefaz_set_ca(NULL, ca), E_ISNULL);

	/* Nota assinada, para o lote */
	nfe = nota(NFE_MODELO_NFCE);
	VERIFICA_INT(nfe_nfe_assinar(nfe, cert, &xml, &tam), 0);
	VERIFICA_INT(nfe_nfe_chave(nfe, chave, sizeof chave), 0);

	/* Só https; serviço inválido */
	VERIFICA_INT(
	        nfe_sefaz_msg_status(NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP, &msg),
	        0);
	VERIFICA_INT(nfe_sefaz_enviar(s, "http://127.0.0.1:1/",
	                              NFE_SERVICO_STATUS, msg, &ret, NULL),
	             E_REDE);
	VERIFICA(nfe_sefaz_erro(s)[0] != '\0');
	VERIFICA_INT(nfe_sefaz_enviar(s, "https://127.0.0.1:1/",
	                              (nfe_servico)99, msg, &ret, NULL),
	             E_VALOR);
	VERIFICA_INT(
	        nfe_sefaz_enviar(s, NULL, NFE_SERVICO_STATUS, msg, &ret, NULL),
	        E_ISNULL);

	pid = inicia_servidor(argv[1], &porta);
	VERIFICA(porta > 0);
	if (porta > 0) {
		snprintf(url, sizeof url, "https://127.0.0.1:%d/ws", porta);

		/* Sem confiar no certificado do servidor: recusado */
		VERIFICA_INT(nfe_sefaz_enviar(s, url, NFE_SERVICO_STATUS, msg,
		                              &ret, NULL),
		             E_REDE);
		VERIFICA(nfe_sefaz_erro(s)[0] != '\0');

		VERIFICA_INT(nfe_sefaz_set_ca(s, ca), 0);
		rc = nfe_sefaz_enviar(s, url, NFE_SERVICO_STATUS, msg, &ret,
		                      &tam_ret);
		VERIFICA_INT(rc, 0);
		if (rc != 0)
			fprintf(stderr, "erro: %s\n", nfe_sefaz_erro(s));
		VERIFICA(contem(ret, "<retConsStatServ"));
		VERIFICA_INT(nfe_sefaz_cstat(ret, tam_ret, &cstat, NULL, 0), 0);
		VERIFICA_INT(cstat, 107);
		VERIFICA_STR(nfe_sefaz_erro(s), "");
		free(ret);
		ret = NULL;

		/* Ação errada para a mensagem: SOAP Fault */
		VERIFICA_INT(nfe_sefaz_enviar(s, url, NFE_SERVICO_CONSULTA, msg,
		                              &ret, NULL),
		             E_REDE);
		VERIFICA(contem(nfe_sefaz_erro(s), "acao desconhecida"));

		/* Lote síncrono com a nota assinada: autorizada */
		free(msg);
		lote[0] = xml;
		VERIFICA_INT(nfe_sefaz_msg_lote("1", 1, lote, 1, &msg), 0);
		VERIFICA_INT(teste_valida_xsd(argv[1], "nfe/tipos_v4.00.xsd",
		                              msg, strlen(msg)),
		             0);
		rc = nfe_sefaz_enviar(s, url, NFE_SERVICO_AUTORIZACAO, msg,
		                      &ret, &tam_ret);
		VERIFICA_INT(rc, 0);
		VERIFICA_INT(nfe_sefaz_cstat(ret, tam_ret, &cstat, NULL, 0), 0);
		VERIFICA_INT(cstat, 104);
		VERIFICA_INT(nfe_sefaz_protocolo(ret, tam_ret, chave, &prot,
		                                 &tam_prot),
		             0);
		/* cStat da nota, dentro de infProt */
		VERIFICA_INT(nfe_sefaz_cstat(prot, tam_prot, &cstat, NULL, 0),
		             0);
		VERIFICA_INT(cstat, 100);
		VERIFICA_INT(teste_valida_xsd(argv[1], "nfe/tipos_v4.00.xsd",
		                              prot, tam_prot),
		             0);
		VERIFICA(contem(prot, "<n:chNFe>"));
		VERIFICA_INT(
		        nfe_sefaz_protocolo(ret, tam_ret, "123", &proc, NULL),
		        E_VALOR);

		/* nfeProc: nota sem alterações + protocolo */
		VERIFICA_INT(nfe_sefaz_proc(xml, tam, prot, tam_prot, &proc,
		                            &tam_proc),
		             0);
		VERIFICA_INT(teste_valida_xsd(argv[1], "nfe/tipos_v4.00.xsd",
		                              proc, tam_proc),
		             0);
		VERIFICA(contem(proc, "<nfeProc xmlns=\"http://www.portalfiscal"
		                      ".inf.br/nfe\" versao=\"4.00\"><NFe"));
		VERIFICA(contem(proc, "</NFe><n:protNFe"));
		VERIFICA(proc && strlen(proc) == tam_proc);
		{
			size_t n;
			const char *ini = strstr(xml, "<NFe");

			n = tam - (size_t)(ini - xml);
			while (n > 0 && (ini[n - 1] == '\n'))
				n--;
			VERIFICA(proc && strstr(proc, "<NFe") &&
			         strncmp(strstr(proc, "<NFe"), ini, n) == 0);
		}
		free(proc);
		proc = NULL;
		free(ret);
		ret = NULL;

		/* Erros HTTP e resposta sem retorno */
		snprintf(url, sizeof url, "https://127.0.0.1:%d/fault", porta);
		VERIFICA_INT(nfe_sefaz_enviar(s, url, NFE_SERVICO_STATUS, msg,
		                              &ret, NULL),
		             E_REDE);
		VERIFICA(contem(nfe_sefaz_erro(s), "Erro de teste"));
		VERIFICA(contem(nfe_sefaz_erro(s), "500"));
		snprintf(url, sizeof url, "https://127.0.0.1:%d/http404",
		         porta);
		VERIFICA_INT(nfe_sefaz_enviar(s, url, NFE_SERVICO_STATUS, msg,
		                              &ret, NULL),
		             E_REDE);
		VERIFICA(contem(nfe_sefaz_erro(s), "404"));
		snprintf(url, sizeof url, "https://127.0.0.1:%d/vazio", porta);
		VERIFICA_INT(nfe_sefaz_enviar(s, url, NFE_SERVICO_STATUS, msg,
		                              &ret, NULL),
		             E_XML);
	}
	if (pid > 0) {
		kill(pid, SIGTERM);
		waitpid(pid, NULL, 0);
	}

	/* Protocolo de outra nota; documentos errados */
	if (prot) {
		/* Mesmo protocolo com a chave de outra nota (nNF diferente) */
		char *outro = strdup(prot),
		     *p = outro ? strstr(outro, "<n:chNFe>") : NULL;

		if (p) {
			p += 9;
			p[33] = p[33] == '1' ? '2' : '1';
			p[43] = (char)('0' + nfe_chave_dv(p));
			VERIFICA_INT(nfe_sefaz_proc(xml, tam, outro,
			                            strlen(outro), &proc, NULL),
			             E_VALOR);
		}
		free(outro);
		VERIFICA_INT(nfe_sefaz_proc(prot, tam_prot, prot, tam_prot,
		                            &proc, NULL),
		             E_XML);
		VERIFICA_INT(nfe_sefaz_proc(xml, tam, xml, tam, &proc, NULL),
		             E_XML);
	}
	VERIFICA_INT(nfe_sefaz_proc(NULL, 0, "", 0, &proc, NULL), E_ISNULL);
	{
		char *p = NULL;

		VERIFICA_INT(nfe_sefaz_protocolo("<a/>", 4, NULL, &p, NULL),
		             E_VALOR);
		VERIFICA_INT(nfe_sefaz_protocolo("<a", 2, NULL, &p, NULL),
		             E_XML);
	}

	free(prot);
	free(msg);
	free(xml);
	nfe_nfe_free(nfe);
	nfe_sefaz_free(s);
	nfe_sefaz_free(NULL);
	nfe_certificado_free(cert);
	TESTE_FIM();
}
