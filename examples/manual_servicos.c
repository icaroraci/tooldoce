/* Copyright (c) 2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with tooldoce. If not, see <https://www.gnu.org/licenses/>.
 */

/* Mensagens sintéticas e determinísticas: sem certificado ou acesso à rede. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/erros.h>
#include <libnfe/evento.h>
#include <libnfe/inutilizacao.h>
#include <libnfe/sefaz.h>

int main(int argc, char **argv)
{
	const char *chave = "35100812345678000195550010000000421123456781";
	const nfe_evento_info info = { NFE_AMBIENTE_HOMOLOGACAO, chave,
		                       "12345678000195", (time_t)1791201600,
		                       NFE_TZD_BRASILIA };
	char *xml = NULL;
	size_t tam = 0;
	int rc;

	if (argc != 2) {
		fprintf(stderr,
		        "uso: %s "
		        "status|recibo|consulta|cancelamento|cce|"
		        "inutilizacao\n",
		        argv[0]);
		return EXIT_FAILURE;
	}
	if (!strcmp(argv[1], "status"))
		rc = nfe_sefaz_msg_status(NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP,
		                          &xml);
	else if (!strcmp(argv[1], "recibo"))
		rc = nfe_sefaz_msg_recibo(NFE_AMBIENTE_HOMOLOGACAO,
		                          "123456789012345", &xml);
	else if (!strcmp(argv[1], "consulta"))
		rc = nfe_sefaz_msg_consulta(NFE_AMBIENTE_HOMOLOGACAO, chave,
		                            &xml);
	else if (!strcmp(argv[1], "cancelamento"))
		rc = nfe_evento_cancelamento(
		        &info, "135260000000001",
		        "EXEMPLO SINTETICO DE CANCELAMENTO", &xml, &tam);
	else if (!strcmp(argv[1], "cce"))
		rc = nfe_evento_cce(
		        &info, 1,
		        "CORRECAO SINTETICA DE INFORMACAO COMPLEMENTAR", &xml,
		        &tam);
	else if (!strcmp(argv[1], "inutilizacao"))
		rc = nfe_inutilizacao(
		        NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP, 26,
		        "12345678000195", NFE_MODELO_NFE, 1, 10, 12,
		        "NUMEROS PULADOS EM EXEMPLO SINTETICO", &xml, &tam);
	else {
		fprintf(stderr, "mensagem desconhecida: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	if (rc != 0) {
		fprintf(stderr, "%s\n", nfe_strerror(rc));
		free(xml);
		return EXIT_FAILURE;
	}
	if (!tam)
		tam = strlen(xml);
	int escrito = fwrite(xml, 1, tam, stdout) == tam;
	free(xml);
	return escrito ? EXIT_SUCCESS : EXIT_FAILURE;
}
