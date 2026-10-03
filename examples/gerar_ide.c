/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 * */

/* Exemplo: monta o grupo <ide> de uma NF-e e escreve o XML na saída padrão.
 *
 * Compilar e executar (na raiz do projeto):
 *   make exemplos
 *   ./obj/gerar_ide
 */

#include <stdio.h>
#include <time.h>

#include <libxml/xmlwriter.h>

#include <libnfe/erros.h>
#include <libnfe/ide.h>
#include <libnfe/refNFe.h>

/* Interrompe o exemplo mostrando a descrição do erro */
static int falha(const char *etapa, int rc)
{
	fprintf(stderr, "erro em %s: %s\n", etapa, nfe_strerror(rc));
	return 1;
}

int main(void)
{
	nfe_ide *ide;
	struct refNFe_s *ref;
	xmlTextWriterPtr writer;
	int rc = 0;

	ide = nfe_ide_new();
	if (!ide)
		return falha("nfe_ide_new", E_MALLOC);

	/* Campos obrigatórios sem valor padrão */
	rc |= nfe_ide_set_cuf(ide, NFE_UF_SP);
	rc |= nfe_ide_set_natop(ide, "VENDA DE MERCADORIA");
	rc |= nfe_ide_set_nnf(ide, 1);
	rc |= nfe_ide_set_dhemi(ide, time(NULL));
	rc |= nfe_ide_set_cmunfg(ide, 3550308); /* São Paulo */
	rc |= nfe_ide_set_verproc(ide, "exemplo 1.0");

	/* Demais campos (os não informados ficam com o valor padrão; o
	 * ambiente padrão é homologação) */
	rc |= nfe_ide_set_cnf(ide, 12345678);
	rc |= nfe_ide_set_serie(ide, 1);
	rc |= nfe_ide_set_indfinal(ide, NFE_CONSUMIDOR_FINAL);
	rc |= nfe_ide_set_indpres(ide, NFE_PRESENCA_PRESENCIAL);
	if (rc != 0) {
		nfe_ide_free(ide);
		return falha("preenchimento do ide", rc);
	}

	/* Uma NF-e referenciada, pela chave de acesso */
	ref = RefNFeNew();
	rc = RefNFeSetrefNFe(ref,
	                     "35100812345678000195550010000000421123456781");
	if (rc == 0)
		rc = nfe_ide_add_refnfe(ide,
		                        ref); /* o ide passa a ser o dono */
	if (rc != 0) {
		RefNFeDel(ref);
		nfe_ide_free(ide);
		return falha("referência", rc);
	}

	/* Escreve o XML na saída padrão */
	writer = xmlNewTextWriterFilename("-", 0);
	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	rc = nfe_ide_write_xml(writer, ide);
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);

	nfe_ide_free(ide);
	if (rc != 0)
		return falha("nfe_ide_write_xml", rc);
	return 0;
}
