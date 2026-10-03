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

/* Exemplo: monta uma NFC-e completa (homologação, Simples Nacional, um item
 * pago em dinheiro) e escreve o XML na saída padrão.
 *
 * O XML ainda não tem a assinatura digital, que a SEFAZ exige.
 *
 * Compilar e executar (na raiz do projeto):
 *   make exemplos
 *   ./obj/gerar_nfe
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <libnfe/erros.h>
#include <libnfe/nfe_nfe.h>

int main(void)
{
	nfe_nfe *nota = nfe_nfe_new();
	nfe_ide *ide = nfe_ide_new();
	nfe_emit *emit = nfe_emit_new();
	nfe_endereco *end = nfe_endereco_new();
	nfe_det *det = nfe_det_new();
	nfe_prod *prod = nfe_prod_new();
	nfe_imposto *imp = nfe_imposto_new();
	nfe_total *total = nfe_total_new();
	nfe_pag *pag = nfe_pag_new();
	nfe_detpag *dinheiro = nfe_detpag_new();
	char *xml = NULL;
	int rc = 0;

	if (!nota || !ide || !emit || !end || !det || !prod || !imp || !total ||
	    !pag || !dinheiro) {
		fprintf(stderr, "erro: %s\n", nfe_strerror(E_MALLOC));
		return 1;
	}

	/* Identificação: NFC-e, série 1, número 1 */
	rc |= nfe_ide_set_cuf(ide, NFE_UF_SP);
	rc |= nfe_ide_set_cnf(ide, 12345678);
	rc |= nfe_ide_set_natop(ide, "VENDA");
	rc |= nfe_ide_set_mod(ide, NFE_MODELO_NFCE);
	rc |= nfe_ide_set_serie(ide, 1);
	rc |= nfe_ide_set_nnf(ide, 1);
	rc |= nfe_ide_set_dhemi(ide, time(NULL));
	rc |= nfe_ide_set_cmunfg(ide, 3550308); /* São Paulo */
	rc |= nfe_ide_set_tpimp(ide, NFE_DANFE_NFCE);
	rc |= nfe_ide_set_indfinal(ide, NFE_CONSUMIDOR_FINAL);
	rc |= nfe_ide_set_indpres(ide, NFE_PRESENCA_PRESENCIAL);
	rc |= nfe_ide_set_verproc(ide, "exemplo 1.0");

	/* Emitente */
	rc |= nfe_endereco_set_xlgr(end, "RUA DAS FLORES");
	rc |= nfe_endereco_set_nro(end, "123");
	rc |= nfe_endereco_set_xbairro(end, "CENTRO");
	rc |= nfe_endereco_set_cmun(end, 3550308);
	rc |= nfe_endereco_set_xmun(end, "SAO PAULO");
	rc |= nfe_endereco_set_uf(end, "SP");
	rc |= nfe_endereco_set_cep(end, "01001000");
	rc |= nfe_emit_set_cnpj(emit, "12345678000195");
	rc |= nfe_emit_set_xnome(emit, "EMPRESA EXEMPLO LTDA");
	rc |= nfe_emit_set_endereco(emit, end); /* o emit passa a ser o dono */
	rc |= nfe_emit_set_ie(emit, "123456789012");
	rc |= nfe_emit_set_crt(emit, NFE_CRT_SIMPLES_NACIONAL);

	/* Item: 10 canetas a R$ 1,50 */
	rc |= nfe_prod_set_cprod(prod, "001");
	rc |= nfe_prod_set_xprod(prod, "CANETA AZUL");
	rc |= nfe_prod_set_ncm(prod, "96081000");
	rc |= nfe_prod_set_cfop(prod, 5102);
	rc |= nfe_prod_set_comercial(prod, "UN", "10", "1.50", "15.00");
	rc |= nfe_prod_set_tributavel(prod, "UN", "10", "1.50");
	rc |= nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NACIONAL,
	                                NFE_CSOSN_102);
	rc |= nfe_imposto_set_pisnt(imp, NFE_CST_PC_SEM_INCIDENCIA);
	rc |= nfe_imposto_set_cofinsnt(imp, NFE_CST_PC_SEM_INCIDENCIA);
	rc |= nfe_det_set_prod(det, prod);
	rc |= nfe_det_set_imposto(det, imp);

	/* Totais e pagamento: R$ 20,00 em dinheiro, R$ 5,00 de troco */
	rc |= nfe_total_set_icmstot(total, NFE_TOT_VPROD, "15.00");
	rc |= nfe_total_set_icmstot(total, NFE_TOT_VNF, "15.00");
	rc |= nfe_detpag_set_tpag(dinheiro, NFE_MEIO_DINHEIRO);
	rc |= nfe_detpag_set_vpag(dinheiro, "20.00");
	rc |= nfe_pag_add_detpag(pag, dinheiro);
	rc |= nfe_pag_set_vtroco(pag, "5.00");

	/* A nota passa a ser dona de todos os grupos */
	rc |= nfe_nfe_set_ide(nota, ide);
	rc |= nfe_nfe_set_emit(nota, emit);
	rc |= nfe_nfe_add_det(nota, det);
	rc |= nfe_nfe_set_total(nota, total);
	rc |= nfe_nfe_set_transp(nota, nfe_transp_new()); /* sem transporte */
	rc |= nfe_nfe_set_pag(nota, pag);
	if (rc != 0) {
		/* Em um programa real, confira cada retorno para saber qual
		 * campo foi recusado */
		fprintf(stderr, "erro ao preencher a nota\n");
		nfe_nfe_free(nota);
		return 1;
	}

	rc = nfe_nfe_xml(nota, &xml, NULL);
	if (rc != 0) {
		fprintf(stderr, "erro em nfe_nfe_xml: %s\n", nfe_strerror(rc));
		nfe_nfe_free(nota);
		return 1;
	}
	printf("%s\n", xml);
	free(xml);
	nfe_nfe_free(nota);
	return 0;
}
