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

/* Exemplo: NF-e (modelo 55) de regime normal com os tributos da Reforma
 * Tributária (IBS e CBS) no item e nos totais, para homologação.
 *
 * Desde 2026 a SEFAZ rejeita a NF-e de regime normal sem o grupo IBSCBS
 * (rejeição 1115, "IBS/CBS não informado"). Este exemplo mostra como
 * preencher o grupo no item (nfe_imposto_set) e o total IBSCBSTot
 * (nfe_grupo_set em nfe_total_grupo).
 *
 * Compilar e executar (na raiz do projeto):
 *   make exemplos
 *   ./obj/nfe_ibscbs                        (XML sem assinatura)
 *   ./obj/nfe_ibscbs <arquivo.pfx> <senha>  (XML assinado)
 *
 * CENÁRIO (dados fictícios, só para homologação, referência 2026):
 * venda interna no RJ de sementes (NCM 12099100, CFOP 5102) a um
 * consumidor final não contribuinte, com:
 *   - ICMS com base reduzida em 60% (ICMS20): base 15,20, alíquota 12%,
 *     ICMS 1,82;
 *   - PIS e COFINS com alíquota zero (CST 06);
 *   - IBS/CBS CST 200 (alíquota reduzida), cClassTrib 200038, com redução
 *     de 60% das alíquotas (insumos agropecuários);
 *   - base do IBS/CBS = valor do produto menos o ICMS (38,00 - 1,82 =
 *     36,18), como no período de transição;
 *   - alíquotas de teste de 2026: IBS da UF 0,1% (efetiva 0,04%), IBS do
 *     município 0% e CBS 0,9% (efetiva 0,36%); IBS 0,01 e CBS 0,13.
 * Um cenário igual a este foi autorizado (cStat 100) na homologação da
 * SVRS em 03/10/2026. Isso mostra que o leiaute está certo, mas não que o
 * enquadramento fiscal vale para operações reais: a classificação
 * (cClassTrib), as reduções e as alíquotas de cada produto e de cada ano
 * devem ser conferidas na legislação e na tabela oficial de
 * classificação tributária (portal da SVRS) antes de emitir em produção.
 *
 * nfe_nfe_calcular_totais soma os itens só no ICMSTot; o IBSCBSTot (e o
 * vNFTot) são preenchidos por quem usa a biblioteca, como abaixo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <libnfe/erros.h>
#include <libnfe/nfe_nfe.h>

/* Um campo de um grupo genérico: caminho e valor */
struct campo {
	const char *caminho;
	const char *valor;
};

/* Grava os campos no grupo; devolve 0 ou o primeiro erro (e o campo em
 * *ruim) */
static int grava(nfe_grupo *g, const struct campo *c, int n, const char **ruim)
{
	int i, rc;

	for (i = 0; i < n; i++) {
		rc = nfe_grupo_set(g, c[i].caminho, c[i].valor);
		if (rc != 0) {
			*ruim = c[i].caminho;
			return rc;
		}
	}
	return 0;
}

/* Tributos do item */
static const struct campo IMPOSTO[] = {
	/* ICMS com redução de base de cálculo (CST 20) */
	{ "ICMS20/orig", "0" },
	{ "ICMS20/modBC", "3" }, /* valor da operação */
	{ "ICMS20/pRedBC", "60.00" },
	{ "ICMS20/vBC", "15.20" },
	{ "ICMS20/pICMS", "12.00" },
	{ "ICMS20/vICMS", "1.82" },
	/* PIS e COFINS com alíquota zero */
	{ "PISNT/CST", "06" },
	{ "COFINSNT/CST", "06" },
	/* IBS e CBS */
	{ "IBSCBS/CST", "200" },
	{ "IBSCBS/cClassTrib", "200038" },
	{ "IBSCBS/gIBSCBS/vBC", "36.18" },
	{ "gIBSCBS/gIBSUF/pIBSUF", "0.10" },
	{ "gIBSUF/gRed/pRedAliq", "60.00" },
	{ "gIBSUF/gRed/pAliqEfet", "0.04" },
	{ "gIBSUF/vIBSUF", "0.01" },
	{ "gIBSMun/pIBSMun", "0.00" },
	{ "gIBSMun/vIBSMun", "0.00" },
	{ "gIBSCBS/vIBS", "0.01" },
	{ "gCBS/pCBS", "0.90" },
	{ "gCBS/gRed/pRedAliq", "60.00" },
	{ "gCBS/gRed/pAliqEfet", "0.36" },
	{ "gCBS/vCBS", "0.13" },
};

/* Totais de IBS e CBS: as somas dos itens */
static const struct campo TOTAL[] = {
	{ "IBSCBSTot/vBCIBSCBS", "36.18" },
	{ "gIBS/gIBSUF/vDif", "0.00" },
	{ "gIBS/gIBSUF/vDevTrib", "0.00" },
	{ "gIBS/gIBSUF/vIBSUF", "0.01" },
	{ "gIBS/gIBSMun/vDif", "0.00" },
	{ "gIBS/gIBSMun/vDevTrib", "0.00" },
	{ "gIBS/gIBSMun/vIBSMun", "0.00" },
	{ "gIBS/vIBS", "0.01" },
	{ "gIBS/vCredPres", "0.00" },
	{ "gIBS/vCredPresCondSus", "0.00" },
	{ "gCBS/vDif", "0.00" },
	{ "gCBS/vDevTrib", "0.00" },
	{ "gCBS/vCBS", "0.13" },
	{ "gCBS/vCredPres", "0.00" },
	{ "gCBS/vCredPresCondSus", "0.00" },
	/* Total com IBS, CBS e IS: em 2026 eles ainda não são somados */
	{ "vNFTot", "38.00" },
};

#define N(v) ((int)(sizeof(v) / sizeof((v)[0])))

/* Monta a nota do cenário acima; devolve NULL em caso de erro (com a
 * mensagem em stderr) */
nfe_nfe *monta_nota(time_t dhemi)
{
	nfe_nfe *nota = nfe_nfe_new();
	nfe_ide *ide = nfe_ide_new();
	nfe_emit *emit = nfe_emit_new();
	nfe_endereco *end_emit = nfe_endereco_new();
	nfe_dest *dest = nfe_dest_new();
	nfe_endereco *end_dest = nfe_endereco_new();
	nfe_det *det = nfe_det_new();
	nfe_prod *prod = nfe_prod_new();
	nfe_imposto *imp = nfe_imposto_new();
	nfe_total *tot = nfe_total_new();
	nfe_pag *pag = nfe_pag_new();
	nfe_detpag *dinheiro = nfe_detpag_new();
	const char *ruim = NULL;
	int rc = 0;

	if (!nota || !ide || !emit || !end_emit || !dest || !end_dest || !det ||
	    !prod || !imp || !tot || !pag || !dinheiro) {
		fprintf(stderr, "erro: %s\n", nfe_strerror(E_MALLOC));
		return NULL; /* exemplo: não libera em caso de falta de memória
		              */
	}

	/* Identificação: NF-e 55, série 1, número 1, venda interna no RJ */
	rc |= nfe_ide_set_cuf(ide, NFE_UF_RJ);
	rc |= nfe_ide_set_cnf(ide, 20260001);
	rc |= nfe_ide_set_natop(ide, "VENDA DE MERCADORIA");
	rc |= nfe_ide_set_mod(ide, NFE_MODELO_NFE);
	rc |= nfe_ide_set_serie(ide, 1);
	rc |= nfe_ide_set_nnf(ide, 1);
	rc |= nfe_ide_set_dhemi(ide, dhemi);
	rc |= nfe_ide_set_iddest(ide, NFE_DESTINO_INTERNO);
	rc |= nfe_ide_set_cmunfg(ide, 3304557); /* Rio de Janeiro */
	rc |= nfe_ide_set_tpimp(ide, NFE_DANFE_NORMAL_RETRATO);
	rc |= nfe_ide_set_indfinal(ide, NFE_CONSUMIDOR_FINAL);
	rc |= nfe_ide_set_indpres(ide, NFE_PRESENCA_PRESENCIAL);
	rc |= nfe_ide_set_verproc(ide, "exemplo 1.0");

	/* Emitente fictício, regime normal */
	rc |= nfe_endereco_set_xlgr(end_emit, "RUA DO MERCADO");
	rc |= nfe_endereco_set_nro(end_emit, "100");
	rc |= nfe_endereco_set_xbairro(end_emit, "CENTRO");
	rc |= nfe_endereco_set_cmun(end_emit, 3304557);
	rc |= nfe_endereco_set_xmun(end_emit, "RIO DE JANEIRO");
	rc |= nfe_endereco_set_uf(end_emit, "RJ");
	rc |= nfe_endereco_set_cep(end_emit, "20010000");
	rc |= nfe_emit_set_cnpj(emit, "12345678000195");
	rc |= nfe_emit_set_xnome(emit, "EMPRESA EXEMPLO LTDA");
	rc |= nfe_emit_set_endereco(emit, end_emit);
	rc |= nfe_emit_set_ie(emit, "12345678");
	rc |= nfe_emit_set_crt(emit, NFE_CRT_REGIME_NORMAL);

	/* Destinatário fictício: consumidor final não contribuinte */
	rc |= nfe_endereco_set_xlgr(end_dest, "RUA DAS ACACIAS");
	rc |= nfe_endereco_set_nro(end_dest, "10");
	rc |= nfe_endereco_set_xbairro(end_dest, "TIJUCA");
	rc |= nfe_endereco_set_cmun(end_dest, 3304557);
	rc |= nfe_endereco_set_xmun(end_dest, "RIO DE JANEIRO");
	rc |= nfe_endereco_set_uf(end_dest, "RJ");
	rc |= nfe_endereco_set_cep(end_dest, "20520000");
	rc |= nfe_dest_set_cpf(dest, "52998224725");
	rc |= nfe_dest_set_xnome(dest, "NF-E EMITIDA EM AMBIENTE DE "
	                               "HOMOLOGACAO - SEM VALOR FISCAL");
	rc |= nfe_dest_set_endereco(dest, end_dest);
	rc |= nfe_dest_set_indiedest(dest, NFE_IE_DEST_NAO_CONTRIBUINTE);

	/* Item: 2 kg de sementes a R$ 19,00 */
	rc |= nfe_prod_set_cprod(prod, "SEM001");
	rc |= nfe_prod_set_xprod(prod, "SEMENTES DE HORTALICAS");
	rc |= nfe_prod_set_ncm(prod, "12099100");
	rc |= nfe_prod_set_cfop(prod, 5102);
	rc |= nfe_prod_set_comercial(prod, "KG", "2", "19.00", "38.00");
	rc |= nfe_prod_set_tributavel(prod, "KG", "2", "19.00");
	if (rc != 0) {
		fprintf(stderr, "erro ao preencher a nota\n");
		return NULL;
	}
	rc = grava(nfe_imposto_grupo(imp), IMPOSTO, N(IMPOSTO), &ruim);
	if (rc == 0)
		rc = grava(nfe_total_grupo(tot), TOTAL, N(TOTAL), &ruim);
	if (rc != 0) {
		fprintf(stderr, "campo %s: %s\n", ruim, nfe_strerror(rc));
		return NULL;
	}
	rc |= nfe_det_set_prod(det, prod);
	rc |= nfe_det_set_imposto(det, imp);

	/* Pagamento: dinheiro */
	rc |= nfe_detpag_set_tpag(dinheiro, NFE_MEIO_DINHEIRO);
	rc |= nfe_detpag_set_vpag(dinheiro, "38.00");
	rc |= nfe_pag_add_detpag(pag, dinheiro);

	/* A nota passa a ser dona de todos os grupos. O IBSCBSTot já está no
	 * total; nfe_nfe_calcular_totais acrescenta o ICMSTot. */
	rc |= nfe_nfe_set_ide(nota, ide);
	rc |= nfe_nfe_set_emit(nota, emit);
	rc |= nfe_nfe_set_dest(nota, dest);
	rc |= nfe_nfe_add_det(nota, det);
	rc |= nfe_nfe_set_total(nota, tot);
	rc |= nfe_nfe_set_transp(nota, nfe_transp_new()); /* sem frete */
	rc |= nfe_nfe_set_pag(nota, pag);
	rc |= nfe_nfe_calcular_totais(nota);
	if (rc != 0) {
		fprintf(stderr, "erro ao montar a nota\n");
		nfe_nfe_free(nota);
		return NULL;
	}
	return nota;
}

int main(int argc, char **argv)
{
	nfe_certificado *cert = NULL;
	nfe_nfe *nota;
	char *xml = NULL;
	int rc = 0;

	if (argc != 1 && argc != 3) {
		fprintf(stderr, "uso: %s [<arquivo.pfx> <senha>]\n", argv[0]);
		return 2;
	}
	if (argc == 3) {
		cert = nfe_certificado_pfx(argv[1], argv[2], &rc);
		if (!cert) {
			fprintf(stderr, "certificado: %s\n", nfe_strerror(rc));
			return 1;
		}
	}
	nota = monta_nota(time(NULL));
	if (!nota) {
		nfe_certificado_free(cert);
		return 1;
	}
	rc = cert ? nfe_nfe_assinar(nota, cert, &xml, NULL)
	          : nfe_nfe_xml(nota, &xml, NULL);
	if (rc != 0)
		fprintf(stderr, "erro ao gerar o XML: %s\n", nfe_strerror(rc));
	else
		printf("%s\n", xml);
	free(xml);
	nfe_nfe_free(nota);
	nfe_certificado_free(cert);
	return rc == 0 ? 0 : 1;
}
