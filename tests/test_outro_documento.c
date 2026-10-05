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

/* Motor de grupos com as tabelas de um documento que não é a NF-e, geradas
 * por tools/gerar_esquemas.py --config tests/outro_documento/documento.json
 * a partir do schema de teste tests/schemas/exemplo (fictício), como uma
 * biblioteca de outro documento (libmdf, libcte...) faria. Usa só a API
 * pública (<libnfe/esquema.h>, <libnfe/grupo.h>, <libnfe/validar.h>). */

#include <stdio.h>
#include <string.h>

#include <libxml/xmlwriter.h>

#include <libnfe/erros.h>
#include <libnfe/esquema.h>
#include <libnfe/grupo.h>
#include <libnfe/validar.h>

#include "outro_documento/esquemas.c"
#include "outro_documento/padroes.h"
#include "teste.h"

#define NS "http://www.tooldoce.org/exemplo"

/* XML do documento: <exemplo> com o grupo infEx; *xml é liberado pelo
 * chamador com xmlBufferFree */
static int escreve(const nfe_grupo *g, xmlBufferPtr *xml)
{
	xmlBufferPtr buf = xmlBufferCreate();
	xmlTextWriterPtr w = xmlNewTextWriterMemory(buf, 0);
	int rc;

	xmlTextWriterStartDocument(w, NULL, "UTF-8", NULL);
	xmlTextWriterStartElementNS(w, NULL, BAD_CAST "exemplo", BAD_CAST NS);
	rc = nfe_grupo_write_xml(w, g);
	xmlTextWriterEndDocument(w);
	xmlFreeTextWriter(w);
	*xml = buf;
	return rc;
}

static int valida(nfe_validador *v, xmlBufferPtr xml)
{
	return nfe_validar_xsd(v, (const char *)xmlBufferContent(xml),
	                       (size_t)xmlBufferLength(xml), 0, NULL);
}

int main(int argc, char **argv)
{
	char caminho[1024];
	nfe_validador *v;
	nfe_grupo *g, *c, *veic;
	xmlBufferPtr xml;

	snprintf(caminho, sizeof caminho,
	         "%s/schemas/exemplo/leiauteExemplo_v1.00.xsd",
	         argc > 1 ? argv[1] : "tests");
	v = nfe_validador_xsd(caminho);
	VERIFICA(v != NULL);
	if (!v)
		TESTE_FIM();

	VERIFICA(nfe_grupo_new(NULL) == NULL);
	g = nfe_grupo_new(&ex_esq_infEx);
	VERIFICA(g != NULL);
	VERIFICA(nfe_grupo_vazio(g));

	/* Padrões, listas de valores e tamanhos vêm do schema */
	VERIFICA_INT(nfe_grupo_set(g, "versao", "2.00"), E_VALOR);
	VERIFICA_INT(nfe_grupo_set(g, "cUF", "MG"), E_VALOR);
	VERIFICA_INT(nfe_grupo_set(g, "ide/xNome", "A"), E_TAMANHO);
	VERIFICA_INT(nfe_grupo_set(g, "placa", "abc1234"), E_VALOR);
	VERIFICA_INT(nfe_grupo_set(g, "naoexiste", "1"), E_VALOR);
	VERIFICA_INT(nfe_grupo_valida(g, "vCarga", "10.5"), E_VALOR);
	VERIFICA_INT(nfe_grupo_valida(g, "vCarga", "10.50"), 0);

	VERIFICA_INT(nfe_grupo_set(g, "versao", "1.00"), 0);
	VERIFICA_INT(nfe_grupo_set(g, "cUF", "RJ"), 0);
	VERIFICA_INT(nfe_grupo_set(g, "ide/xNome", "Transportes Exemplo"), 0);
	VERIFICA_INT(nfe_grupo_set(g, "placa", "ABC1D23"), 0);
	VERIFICA_INT(nfe_grupo_set(g, "vCarga", "1500.00"), 0);

	/* Dois campos casam com "veiculo/UF" (veiculo/UF e veiculo/prop/UF):
	 * vale o que casa exatamente, com os elementos consecutivos */
	VERIFICA_INT(nfe_grupo_set(g, "veiculo/UF", "RJ"), 0);
	VERIFICA(nfe_grupo_get(g, "prop/UF") == NULL);
	VERIFICA_INT(nfe_grupo_set(g, "prop/UF", "SP"), 0);
	VERIFICA_STR(nfe_grupo_get(g, "veiculo/UF"), "RJ");
	VERIFICA_STR(nfe_grupo_get(g, "veiculo/prop/UF"), "SP");
	/* Só a UF do proprietário é apagada; prop fica sem CPF e é retirado */
	VERIFICA_INT(nfe_grupo_remove(g, "prop/UF"), 0);
	VERIFICA_STR(nfe_grupo_get(g, "veiculo/UF"), "RJ");
	VERIFICA(nfe_grupo_get(g, "prop/UF") == NULL);
	VERIFICA(!nfe_grupo_vazio(g));

	/* Falta o condutor (1..10): não escreve */
	VERIFICA_INT(escreve(g, &xml), E_VALOR);
	xmlBufferFree(xml);

	VERIFICA_INT(nfe_grupo_add(g, "condutor", &c), 0);
	VERIFICA_INT(nfe_grupo_set(c, "xNome", "Fulano de Tal"), 0);
	VERIFICA_INT(nfe_grupo_set(c, "CPF", "123"), E_VALOR);
	VERIFICA_INT(nfe_grupo_set(c, "CPF", "12345678909"), 0);
	VERIFICA_INT(nfe_grupo_quantidade(g, "condutor"), 1);

	/* Escolha: gravar CPF apaga CNPJ */
	VERIFICA_INT(nfe_grupo_set(g, "contratante/CNPJ", "12345678000195"), 0);
	VERIFICA_INT(nfe_grupo_set(g, "contratante/CPF", "12345678909"), 0);
	VERIFICA(nfe_grupo_get(g, "contratante/CNPJ") == NULL);
	VERIFICA_STR(nfe_grupo_get(g, "contratante/CPF"), "12345678909");

	VERIFICA_INT(escreve(g, &xml), 0);
	VERIFICA_INT(valida(v, xml), 0);
	VERIFICA(strstr((const char *)xmlBufferContent(xml),
	                "<infEx versao=\"1.00\"><ide><cUF>RJ</cUF>") != NULL);
	VERIFICA(strstr((const char *)xmlBufferContent(xml),
	                "<placa>ABC1D23</placa><UF>RJ</UF></veiculo>") != NULL);
	xmlBufferFree(xml);

	/* Limite da lista vem de maxOccurs */
	while (nfe_grupo_quantidade(g, "condutor") < 10)
		VERIFICA_INT(nfe_grupo_add(g, "condutor", &c), 0);
	VERIFICA_INT(nfe_grupo_add(g, "condutor", &c), E_VALOR);
	nfe_grupo_remove_ultimo(g, "condutor");
	VERIFICA_INT(nfe_grupo_quantidade(g, "condutor"), 9);

	/* Segunda raiz da configuração, com nome C próprio */
	veic = nfe_grupo_new(&ex_esq_veic);
	VERIFICA(veic != NULL);
	VERIFICA_INT(nfe_grupo_set(veic, "tara", "8000.00"), 0);
	VERIFICA_STR(nfe_grupo_get(veic, "tara"), "8000.00");
	/* No grupo do próprio veículo, "UF" casa com UF e com prop/UF: vale o
	 * filho direto do grupo */
	VERIFICA_INT(nfe_grupo_set(veic, "UF", "RJ"), 0);
	VERIFICA(nfe_grupo_get(veic, "prop/UF") == NULL);
	VERIFICA_INT(nfe_grupo_set(veic, "prop/UF", "SP"), 0);
	VERIFICA_STR(nfe_grupo_get(veic, "UF"), "RJ");
	/* Só a UF do veículo é apagada */
	VERIFICA_INT(nfe_grupo_remove(veic, "UF"), 0);
	VERIFICA_STR(nfe_grupo_get(veic, "prop/UF"), "SP");
	VERIFICA_INT(nfe_grupo_set(veic, "UF", "RJ"), 0);
	VERIFICA_STR(nfe_grupo_get(veic, "UF"), "RJ");
	VERIFICA_STR(nfe_grupo_get(veic, "prop/UF"), "SP");

	/* Header de padrões gerado com o prefixo do documento */
	VERIFICA_STR(EX_PADRAO_TPlaca, "[A-Z]{3}[0-9][A-Z0-9][0-9]{2}");
	VERIFICA_INT(EX_TAM_MAX_TCpf, 11);

	nfe_grupo_free(veic);
	nfe_grupo_free(g);
	nfe_validador_free(v);
	TESTE_FIM();
}
