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

/* Exemplo: consulta o status do serviço da SEFAZ em homologação, com o
 * certificado A1 do emitente. É o teste mais simples da comunicação: não
 * envia nenhuma nota.
 *
 * Compilar e executar (na raiz do projeto):
 *   make exemplos
 *   ./obj/status_sefaz <arquivo.pfx> <senha> <url> <cUF> [ca.pem]
 *
 * url é o endereço do webservice NFeStatusServico4 da UF, em homologação
 * (Portal Nacional da NF-e, Relação de Serviços Web); cUF é o código da UF
 * (35 para SP). ca.pem (opcional) tem as autoridades certificadoras em que
 * confiar para o certificado do servidor (ex.: cadeia ICP-Brasil), se as do
 * sistema não bastarem. A resposta esperada é cStat 107 (serviço em
 * operação).
 */

#include <stdio.h>
#include <stdlib.h>

#include <libnfe/erros.h>
#include <libnfe/sefaz.h>

int main(int argc, char **argv)
{
	nfe_certificado *cert;
	nfe_sefaz *s;
	char *msg = NULL, *ret = NULL, motivo[256];
	size_t tam = 0;
	int rc = 0, cstat = 0;

	if (argc != 5 && argc != 6) {
		fprintf(stderr,
		        "uso: %s <arquivo.pfx> <senha> <url> <cUF> "
		        "[ca.pem]\n",
		        argv[0]);
		return 2;
	}
	cert = nfe_certificado_pfx(argv[1], argv[2], &rc);
	if (!cert) {
		fprintf(stderr, "certificado: %s\n",
		        rc == E_VALOR ? "senha errada ou arquivo inválido"
		                      : nfe_strerror(rc));
		return 1;
	}
	s = nfe_sefaz_new(cert);
	if (!s) {
		nfe_certificado_free(cert);
		return 1;
	}
	if (argc == 6)
		nfe_sefaz_set_ca(s, argv[5]);
	rc = nfe_sefaz_msg_status(NFE_AMBIENTE_HOMOLOGACAO,
	                          (nfe_uf)atoi(argv[4]), &msg);
	if (rc != 0) {
		fprintf(stderr, "cUF inválido\n");
	} else {
		rc = nfe_sefaz_enviar(s, argv[3], NFE_SERVICO_STATUS, msg, &ret,
		                      &tam);
		if (rc != 0)
			fprintf(stderr, "erro: %s (%s)\n", nfe_strerror(rc),
			        nfe_sefaz_erro(s));
		else if (nfe_sefaz_cstat(ret, tam, &cstat, motivo,
		                         sizeof motivo) == 0)
			printf("cStat %d: %s\n", cstat, motivo);
		else
			printf("%s\n", ret);
	}
	free(msg);
	free(ret);
	nfe_sefaz_free(s);
	nfe_certificado_free(cert);
	return rc == 0 ? 0 : 1;
}
