/* Copyright (c) 2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
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

/* ^ e $ como âncoras num xs:pattern de um schema incluído (como a série da
 * DPS da NFS-e): o validador os tira ao carregar o schema (#291).
 *
 * Uso: test_ancoras <diretório tests> */

#include <stdio.h>

#include <libnfe/erros.h>
#include <libnfe/validar.h>
#include <libxml/parser.h>

#include "teste.h"

int main(int argc, char **argv)
{
	static const char *const docs[][2] = {
		{ "<serie>1</serie>", "0" },
		{ "<serie>00001</serie>", "0" },
		{ "<serie>123456</serie>", "1" },
		{ "<serie>^1$</serie>", "1" },
		{ "<serie>1</serie><preco>10.00</preco>", "0" },
		{ "<serie>1</serie><preco>10</preco>", "1" },
		{ "<serie>1</serie><sigla>AB</sigla>", "0" },
		{ "<serie>1</serie><sigla>ab</sigla>", "1" },
	};
	char caminho[1024], doc[256];
	nfe_validador *x;
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	snprintf(caminho, sizeof caminho, "%s/schemas/ancoras/ancoras.xsd",
	         argv[1]);
	x = nfe_validador_xsd(caminho);
	VERIFICA(x != NULL);
	for (i = 0; x && i < sizeof docs / sizeof docs[0]; i++) {
		int n = snprintf(doc, sizeof doc,
		                 "<doc xmlns=\"urn:teste:ancoras\">%s</doc>",
		                 docs[i][0]);
		int esperado = docs[i][1][0] == '0' ? 0 : E_VALOR;
		int rc = nfe_validar_xsd(x, doc, (size_t)n, 0, NULL);

		if (rc != esperado)
			fprintf(stderr, "âncoras: %s -> %d\n", docs[i][0], rc);
		VERIFICA_INT(rc, esperado);
	}
	nfe_validador_free(x);
	xmlCleanupParser();
	TESTE_FIM();
}
