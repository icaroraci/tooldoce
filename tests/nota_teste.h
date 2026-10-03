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

/* Nota de exemplo para os testes: NF-e ou NFC-e de homologação, Simples
 * Nacional, com dois itens pagos em dinheiro. Requer teste.h. */

#ifndef TOOLDOCE_NOTA_TESTE_H
#define TOOLDOCE_NOTA_TESTE_H

#include <time.h>

#include <libnfe/nfe_nfe.h>

#define T0 ((time_t)1791014400) /* 2026-10-03T08:00:00Z */

static inline nfe_ide *ide(nfe_modelo mod)
{
	nfe_ide *ide = nfe_ide_new();
	int rc = 0;

	rc |= nfe_ide_set_cuf(ide, NFE_UF_SP);
	rc |= nfe_ide_set_cnf(ide, 12345678);
	rc |= nfe_ide_set_natop(ide, "VENDA");
	rc |= nfe_ide_set_mod(ide, mod);
	rc |= nfe_ide_set_serie(ide, 1);
	rc |= nfe_ide_set_nnf(ide, 1);
	rc |= nfe_ide_set_dhemi(ide, T0);
	rc |= nfe_ide_set_cmunfg(ide, 3550308);
	rc |= nfe_ide_set_verproc(ide, "tooldoce");
	if (mod == NFE_MODELO_NFCE) {
		rc |= nfe_ide_set_tpimp(ide, NFE_DANFE_NFCE);
		rc |= nfe_ide_set_indfinal(ide, NFE_CONSUMIDOR_FINAL);
		rc |= nfe_ide_set_indpres(ide, NFE_PRESENCA_PRESENCIAL);
	}
	VERIFICA_INT(rc, 0);
	return ide;
}

static inline nfe_emit *emit(void)
{
	nfe_emit *emit = nfe_emit_new();
	nfe_endereco *end = nfe_endereco_new();
	int rc = 0;

	rc |= nfe_endereco_set_xlgr(end, "RUA DAS FLORES");
	rc |= nfe_endereco_set_nro(end, "123");
	rc |= nfe_endereco_set_xbairro(end, "CENTRO");
	rc |= nfe_endereco_set_cmun(end, 3550308);
	rc |= nfe_endereco_set_xmun(end, "SAO PAULO");
	rc |= nfe_endereco_set_uf(end, "SP");
	rc |= nfe_endereco_set_cep(end, "01001000");
	rc |= nfe_emit_set_cnpj(emit, "12345678000195");
	rc |= nfe_emit_set_xnome(emit, "EMPRESA EXEMPLO LTDA");
	rc |= nfe_emit_set_endereco(emit, end);
	rc |= nfe_emit_set_ie(emit, "123456789012");
	rc |= nfe_emit_set_crt(emit, NFE_CRT_SIMPLES_NACIONAL);
	VERIFICA_INT(rc, 0);
	return emit;
}

static inline nfe_det *item(const char *cprod)
{
	nfe_det *det = nfe_det_new();
	nfe_prod *prod = nfe_prod_new();
	nfe_imposto *imp = nfe_imposto_new();
	int rc = 0;

	rc |= nfe_prod_set_cprod(prod, cprod);
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
	VERIFICA_INT(rc, 0);
	return det;
}

/* Nota completa com dois itens, sem destinatário */
static inline nfe_nfe *nota(nfe_modelo mod)
{
	nfe_nfe *nfe = nfe_nfe_new();
	nfe_total *tot = nfe_total_new();
	nfe_pag *pag = nfe_pag_new();
	nfe_detpag *dp = nfe_detpag_new();
	int rc = 0;

	rc |= nfe_total_set_icmstot(tot, NFE_TOT_VPROD, "30.00");
	rc |= nfe_total_set_icmstot(tot, NFE_TOT_VNF, "30.00");
	rc |= nfe_detpag_set_tpag(dp, NFE_MEIO_DINHEIRO);
	rc |= nfe_detpag_set_vpag(dp, "30.00");
	rc |= nfe_pag_add_detpag(pag, dp);
	rc |= nfe_nfe_set_ide(nfe, ide(mod));
	rc |= nfe_nfe_set_emit(nfe, emit());
	rc |= nfe_nfe_add_det(nfe, item("001"));
	rc |= nfe_nfe_add_det(nfe, item("002"));
	rc |= nfe_nfe_set_total(nfe, tot);
	rc |= nfe_nfe_set_transp(nfe, nfe_transp_new());
	rc |= nfe_nfe_set_pag(nfe, pag);
	VERIFICA_INT(rc, 0);
	return nfe;
}

#endif
