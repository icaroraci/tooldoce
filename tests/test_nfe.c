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

/* Testes da nota completa (nfe_nfe): NF-e (modelo 55) e NFC-e (modelo 65).
 * O <infNFe> gerado é validado contra o XSD oficial; o <NFe> inteiro ainda
 * não, porque falta a assinatura digital (ds:Signature).
 *
 * Uso: test_nfe <diretório tests> */

#include <time.h>

#include <libnfe/erros.h>
#include <libnfe/nfe_nfe.h>

#include "teste.h"
#include "teste_xml.h"

#define T0 ((time_t)1791014400) /* 2026-10-03T08:00:00Z */

/* Valida o elemento <infNFe> do documento */
static int valida_infnfe(const char *xml)
{
	xmlDocPtr doc =
	        xmlReadMemory(xml, (int)strlen(xml), "nfe.xml", NULL, 0);
	xmlSchemaValidCtxtPtr ctx;
	xmlNodePtr no;
	int rc = -1;

	if (!doc)
		return -1;
	no = xmlDocGetRootElement(doc);
	no = no ? xmlFirstElementChild(no) : NULL;
	if (no && xmlStrEqual(no->name, BAD_CAST "infNFe")) {
		ctx = xmlSchemaNewValidCtxt(teste_schema);
		rc = xmlSchemaValidateOneElement(ctx, no);
		xmlSchemaFreeValidCtxt(ctx);
	}
	xmlFreeDoc(doc);
	return rc;
}

static nfe_ide *ide(nfe_modelo mod)
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

static nfe_emit *emit(void)
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

static nfe_det *item(const char *cprod)
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
static nfe_nfe *nota(nfe_modelo mod)
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

static void teste_nfce(void)
{
	nfe_nfe *nfe = nota(NFE_MODELO_NFCE);
	char chave[45];
	char *xml = NULL;
	size_t tam = 0;

	VERIFICA_INT(nfe_nfe_chave(nfe, chave, sizeof chave), 0);
	VERIFICA_INT(strlen(chave), 44);
	VERIFICA(strncmp(chave + 20, "65001000000001", 14) == 0);

	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, &tam), 0);
	VERIFICA(xml != NULL);
	if (xml) {
		char id[128];

		snprintf(id, sizeof id, "<infNFe versao=\"4.00\" Id=\"NFe%s\">",
		         chave);
		VERIFICA_INT(tam, strlen(xml));
		VERIFICA(strncmp(xml,
		                 "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		                 "<NFe xmlns=\"" TESTE_NS "\">",
		                 38 + 47) == 0);
		VERIFICA(strstr(xml, id) != NULL);
		VERIFICA(strstr(xml, "<mod>65</mod>") != NULL);
		/* cDV igual ao último dígito da chave */
		{
			char cdv[16];
			snprintf(cdv, sizeof cdv, "<cDV>%c</cDV>", chave[43]);
			VERIFICA(strstr(xml, cdv) != NULL);
		}
		VERIFICA(strstr(xml, "<det nItem=\"1\"><prod><cProd>001") !=
		         NULL);
		VERIFICA(strstr(xml, "<det nItem=\"2\"><prod><cProd>002") !=
		         NULL);
		VERIFICA(strstr(xml, "</emit><det") != NULL); /* sem dest */
		VERIFICA(strstr(xml, "</det><total>") != NULL);
		VERIFICA(strstr(xml, "</total><transp>") != NULL);
		VERIFICA(strstr(xml, "</transp><pag>") != NULL);
		VERIFICA(strstr(xml, "</pag></infNFe></NFe>") != NULL);
		VERIFICA(xml[tam - 1] == '>'); /* sem quebra de linha no fim */
		VERIFICA(strchr(xml, '\n') == NULL);
		VERIFICA_INT(valida_infnfe(xml), 0);
	}
	free(xml);
	nfe_nfe_free(nfe);
}

static void teste_nfe_com_destinatario(void)
{
	nfe_nfe *nfe = nota(NFE_MODELO_NFE);
	nfe_dest *dest = nfe_dest_new();
	char *xml = NULL;
	int rc = 0;

	rc |= nfe_dest_set_cnpj(dest, "12ABC34501DE35");
	rc |= nfe_dest_set_xnome(dest, "CLIENTE S.A.");
	rc |= nfe_dest_set_indiedest(dest, NFE_IE_DEST_NAO_CONTRIBUINTE);
	rc |= nfe_nfe_set_dest(nfe, dest);
	VERIFICA_INT(rc, 0);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<mod>55</mod>") != NULL);
		VERIFICA(strstr(xml,
		                "</emit><dest><CNPJ>12ABC34501DE35</CNPJ>") !=
		         NULL);
		VERIFICA(strstr(xml, "</dest><det nItem=\"1\">") != NULL);
		VERIFICA_INT(valida_infnfe(xml), 0);
	}
	free(xml);

	/* Removendo o destinatário */
	VERIFICA_INT(nfe_nfe_set_dest(nfe, NULL), 0);
	xml = NULL;
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), 0);
	if (xml)
		VERIFICA(strstr(xml, "<dest>") == NULL);
	free(xml);
	nfe_nfe_free(nfe);
}

static void teste_salvar(const char *dir)
{
	nfe_nfe *nfe = nota(NFE_MODELO_NFE);
	char caminho[1024], *xml = NULL, lido[16384];
	size_t tam = 0, n;
	FILE *f;

	snprintf(caminho, sizeof caminho, "%s/../obj/test_nfe.xml", dir);
	VERIFICA_INT(nfe_nfe_salvar(nfe, caminho), 0);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, &tam), 0);
	f = fopen(caminho, "rb");
	VERIFICA(f != NULL);
	if (f && xml) {
		n = fread(lido, 1, sizeof lido, f);
		VERIFICA_INT(n, tam);
		VERIFICA(memcmp(lido, xml, tam) == 0);
	}
	if (f)
		fclose(f);
	remove(caminho);
	free(xml);

	/* Diretório inexistente */
	VERIFICA_INT(nfe_nfe_salvar(nfe, "/nao/existe/nota.xml"), E_ARQUIVO);
	nfe_nfe_free(nfe);
}

/* Totais calculados a partir dos itens */
static void teste_calcular_totais(void)
{
	nfe_nfe *nfe = nota(NFE_MODELO_NFE);
	nfe_det *det = nfe_det_new();
	nfe_prod *prod = nfe_prod_new();
	nfe_imposto *imp = nfe_imposto_new();
	char *xml = NULL;
	int rc = 0;

	/* Terceiro item: regime normal, com frete, desconto e tributos */
	rc |= nfe_prod_set_cprod(prod, "003");
	rc |= nfe_prod_set_xprod(prod, "CADERNO");
	rc |= nfe_prod_set_ncm(prod, "48202000");
	rc |= nfe_prod_set_cfop(prod, 5102);
	rc |= nfe_prod_set_comercial(prod, "UN", "1", "100", "100.00");
	rc |= nfe_prod_set_tributavel(prod, "UN", "1", "100");
	rc |= nfe_prod_set_vfrete(prod, "10.00");
	rc |= nfe_prod_set_vdesc(prod, "5.50");
	rc |= nfe_imposto_set_vtottrib(imp, "31.45");
	rc |= nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                             NFE_MOD_BC_VALOR_OPERACAO, "104.50",
	                             "18.00", "18.81", "2.00", "2.09");
	rc |= nfe_imposto_set_pisaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA, "100.00",
	                              "1.65", "1.65");
	rc |= nfe_imposto_set_cofinsaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA,
	                                 "100.00", "7.60", "7.60");
	rc |= nfe_det_set_prod(det, prod);
	rc |= nfe_det_set_imposto(det, imp);
	rc |= nfe_nfe_add_det(nfe, det);
	VERIFICA_INT(rc, 0);

	VERIFICA_INT(nfe_nfe_calcular_totais(nfe), 0);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), 0);
	VERIFICA(xml != NULL);
	if (xml) {
		/* vProd = 15 + 15 + 100; vNF = 130 - 5.50 + 10 */
		VERIFICA(strstr(xml, "<ICMSTot><vBC>104.50</vBC>"
		                     "<vICMS>18.81</vICMS>"
		                     "<vICMSDeson>0.00</vICMSDeson>"
		                     "<vFCP>2.09</vFCP>") != NULL);
		VERIFICA(strstr(xml,
		                "<vProd>130.00</vProd>"
		                "<vFrete>10.00</vFrete><vSeg>0.00</vSeg>"
		                "<vDesc>5.50</vDesc><vII>0.00</vII>"
		                "<vIPI>0.00</vIPI><vIPIDevol>0.00</vIPIDevol>"
		                "<vPIS>1.65</vPIS><vCOFINS>7.60</vCOFINS>"
		                "<vOutro>0.00</vOutro><vNF>134.50</vNF>"
		                "<vTotTrib>31.45</vTotTrib>"
		                "</ICMSTot>") != NULL);
		VERIFICA_INT(valida_infnfe(xml), 0);
	}
	free(xml);

	/* indTot 0: o item não entra em vProd; vIPIDevol informado entra em
	 * vNF */
	VERIFICA_INT(nfe_prod_set_indtot(prod, 0), 0);
	VERIFICA_INT(nfe_imposto_set_vtottrib(imp, NULL), 0);
	{
		nfe_total *tot = nfe_total_new();
		VERIFICA_INT(
		        nfe_total_set_icmstot(tot, NFE_TOT_VIPIDEVOL, "3.00"),
		        0);
		VERIFICA_INT(nfe_nfe_set_total(nfe, tot), 0);
	}
	VERIFICA_INT(nfe_nfe_calcular_totais(nfe), 0);
	xml = NULL;
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), 0);
	if (xml) {
		VERIFICA(strstr(xml, "<vProd>30.00</vProd>") != NULL);
		VERIFICA(strstr(xml, "<vIPIDevol>3.00</vIPIDevol>") != NULL);
		/* 30 - 5.50 + 3 + 10 */
		VERIFICA(strstr(xml, "<vNF>37.50</vNF></ICMSTot>") != NULL);
		VERIFICA_INT(valida_infnfe(xml), 0);
	}
	free(xml);

	/* Desconto maior que o total: vNF negativo é recusado */
	VERIFICA_INT(nfe_prod_set_vdesc(prod, "99.00"), 0);
	VERIFICA_INT(nfe_nfe_calcular_totais(nfe), E_VALOR);
	nfe_nfe_free(nfe);

	/* ICMS-ST, FCP-ST, IPI e II de outros grupos entram nos totais */
	nfe = nota(NFE_MODELO_NFE);
	{
		nfe_imposto *st = nfe_imposto_new();
		nfe_det *d = item("004");
		static const char *const campos[] = {
			"ICMS10/orig",
			"0",
			"ICMS10/modBC",
			"3",
			"ICMS10/vBC",
			"10.00",
			"ICMS10/pICMS",
			"18.00",
			"ICMS10/vICMS",
			"1.80",
			"ICMS10/modBCST",
			"4",
			"ICMS10/vBCST",
			"14.00",
			"ICMS10/pICMSST",
			"18.00",
			"ICMS10/vICMSST",
			"0.72",
			"ICMS10/vBCFCPST",
			"14.00",
			"ICMS10/pFCPST",
			"2.00",
			"ICMS10/vFCPST",
			"0.28",
			"IPI/cEnq",
			"999",
			"IPITrib/CST",
			"50",
			"IPITrib/vBC",
			"15.00",
			"IPITrib/pIPI",
			"10.00",
			"IPITrib/vIPI",
			"1.50",
			"II/vBC",
			"15.00",
			"II/vDespAdu",
			"0",
			"II/vII",
			"2.00",
			"II/vIOF",
			"0",
			"PISNT/CST",
			"07",
			"COFINSNT/CST",
			"07",
			NULL,
		};
		const char *const *c;

		rc = 0;
		for (c = campos; c[0]; c += 2)
			rc |= nfe_imposto_set(st, c[0], c[1]);
		rc |= nfe_det_set_imposto(d, st);
		rc |= nfe_nfe_add_det(nfe, d);
		VERIFICA_INT(rc, 0);
	}
	VERIFICA_INT(nfe_nfe_calcular_totais(nfe), 0);
	xml = NULL;
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), 0);
	if (xml) {
		/* 45 + 0.72 + 0.28 + 1.50 + 2.00 */
		VERIFICA(strstr(xml, "<vBCST>14.00</vBCST><vST>0.72</vST>"
		                     "<vFCPST>0.28</vFCPST>") != NULL);
		VERIFICA(strstr(xml, "<vII>2.00</vII><vIPI>1.50</vIPI>") !=
		         NULL);
		VERIFICA(strstr(xml, "<vNF>49.50</vNF>") != NULL);
		VERIFICA_INT(valida_infnfe(xml), 0);
	}
	free(xml);
	nfe_nfe_free(nfe);

	/* Sem itens, ou item sem produto */
	nfe = nfe_nfe_new();
	VERIFICA_INT(nfe_nfe_calcular_totais(nfe), E_VALOR);
	VERIFICA_INT(nfe_nfe_add_det(nfe, nfe_det_new()), 0);
	VERIFICA_INT(nfe_nfe_calcular_totais(nfe), E_VALOR);
	nfe_nfe_free(nfe);
	VERIFICA_INT(nfe_nfe_calcular_totais(NULL), E_ISNULL);
}

/* Endereço completo para os locais de retirada e entrega */
static nfe_endereco *endereco(void)
{
	nfe_endereco *end = nfe_endereco_new();
	int rc = 0;

	rc |= nfe_endereco_set_xlgr(end, "AV. BRASIL");
	rc |= nfe_endereco_set_nro(end, "1000");
	rc |= nfe_endereco_set_xbairro(end, "JARDIM");
	rc |= nfe_endereco_set_cmun(end, 3304557);
	rc |= nfe_endereco_set_xmun(end, "RIO DE JANEIRO");
	rc |= nfe_endereco_set_uf(end, "RJ");
	rc |= nfe_endereco_set_fone(end, "2133334444");
	VERIFICA_INT(rc, 0);
	return end;
}

/* Grupos opcionais de infNFe, na ordem do leiaute */
static void teste_grupos_opcionais(void)
{
	nfe_nfe *nfe = nota(NFE_MODELO_NFE);
	nfe_local *ret = nfe_local_new(), *ent = nfe_local_new();
	nfe_cobr *cobr = nfe_cobr_new();
	nfe_infadic *inf = nfe_infadic_new();
	nfe_resptec *rt = nfe_resptec_new();
	char *xml = NULL;
	int rc = 0;

	rc |= nfe_local_set_cnpj(ret, "12345678000195");
	rc |= nfe_local_set_xnome(ret, "DEPOSITO CENTRAL");
	rc |= nfe_local_set_endereco(ret, endereco());
	rc |= nfe_local_set_email(ret, "deposito@exemplo.com.br");
	rc |= nfe_local_set_ie(ret, "123456789");
	rc |= nfe_local_set_cpf(ent, "12345678909");
	rc |= nfe_local_set_endereco(ent, endereco());
	rc |= nfe_cobr_set_fat(cobr, "FAT-1", "30.00", "0.00", "30.00");
	rc |= nfe_cobr_add_dup(cobr, "001", "2026-11-03", "15.00");
	rc |= nfe_cobr_add_dup(cobr, "002", "2026-12-03", "15.00");
	rc |= nfe_infadic_set_infadfisco(inf,
	                                 "Documento emitido por ME ou EPP");
	rc |= nfe_infadic_set_infcpl(inf, "Pedido 123. Obrigado pela compra!");
	rc |= nfe_infadic_add_obscont(inf, "vendedor", "MARIA");
	rc |= nfe_infadic_add_obsfisco(inf, "regime", "ESPECIAL");
	rc |= nfe_infadic_add_procref(inf, "PROC-2026/1", NFE_PROCESSO_SEFAZ,
	                              NFE_ATO_REGIME_ESPECIAL);
	rc |= nfe_infadic_add_procref(inf, "0001234-56.2026.4.03.0000",
	                              NFE_PROCESSO_JUSTICA_FEDERAL,
	                              NFE_ATO_NAO_INFORMADO);
	rc |= nfe_resptec_set(rt, "12ABC34501DE35", "SUPORTE TECNICO",
	                      "suporte@exemplo.com.br", "1140028922");
	rc |= nfe_resptec_set_csrt(rt, 1, "AAECAwQFBgcICQoLDA0ODxAREhM=");
	rc |= nfe_nfe_set_retirada(nfe, ret);
	rc |= nfe_nfe_set_entrega(nfe, ent);
	rc |= nfe_nfe_add_autxml(nfe, "12345678000195");
	rc |= nfe_nfe_add_autxml(nfe, "12345678909");
	rc |= nfe_nfe_set_cobr(nfe, cobr);
	rc |= nfe_nfe_set_intermed(nfe, "12ABC34501DE35", "LOJA-42");
	rc |= nfe_nfe_set_infadic(nfe, inf);
	rc |= nfe_nfe_set_resptec(nfe, rt);
	VERIFICA_INT(rc, 0);

	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "</emit><retirada><CNPJ>12345678000195</CNPJ>"
		                "<xNome>DEPOSITO CENTRAL</xNome>"
		                "<xLgr>AV. BRASIL</xLgr>") != NULL);
		VERIFICA(strstr(xml,
		                "<fone>2133334444</fone>"
		                "<email>deposito@exemplo.com.br</email>"
		                "<IE>123456789</IE></retirada>"
		                "<entrega><CPF>12345678909</CPF>") != NULL);
		VERIFICA(strstr(xml,
		                "</entrega><autXML><CNPJ>12345678000195"
		                "</CNPJ></autXML><autXML><CPF>12345678909"
		                "</CPF></autXML><det nItem=\"1\">") != NULL);
		VERIFICA(strstr(xml, "</transp><cobr><fat><nFat>FAT-1</nFat>"
		                     "<vOrig>30.00</vOrig><vDesc>0.00</vDesc>"
		                     "<vLiq>30.00</vLiq></fat><dup><nDup>001"
		                     "</nDup><dVenc>2026-11-03</dVenc>"
		                     "<vDup>15.00</vDup></dup>") != NULL);
		VERIFICA(strstr(xml, "</cobr><pag>") != NULL);
		VERIFICA(strstr(xml,
		                "</pag><infIntermed><CNPJ>12ABC34501DE35"
		                "</CNPJ><idCadIntTran>LOJA-42</idCadIntTran>"
		                "</infIntermed><infAdic>") != NULL);
		VERIFICA(strstr(xml,
		                "<obsCont xCampo=\"vendedor\"><xTexto>MARIA"
		                "</xTexto></obsCont><obsFisco xCampo=\"regime"
		                "\"><xTexto>ESPECIAL</xTexto></obsFisco>"
		                "<procRef><nProc>PROC-2026/1</nProc>"
		                "<indProc>0</indProc><tpAto>10</tpAto>"
		                "</procRef><procRef>") != NULL);
		VERIFICA(strstr(xml,
		                "<indProc>1</indProc></procRef></infAdic>"
		                "<infRespTec><CNPJ>12ABC34501DE35</CNPJ>"
		                "<xContato>SUPORTE TECNICO</xContato>"
		                "<email>suporte@exemplo.com.br</email>"
		                "<fone>1140028922</fone><idCSRT>01</idCSRT>"
		                "<hashCSRT>AAECAwQFBgcICQoLDA0ODxAREhM="
		                "</hashCSRT></infRespTec></infNFe>") != NULL);
		VERIFICA_INT(valida_infnfe(xml), 0);
	}
	free(xml);

	/* Valores recusados */
	VERIFICA_INT(nfe_nfe_add_autxml(nfe, "123"), E_TAMANHO);
	VERIFICA_INT(nfe_nfe_add_autxml(nfe, "12345678000196"), E_VALOR);
	VERIFICA_INT(nfe_nfe_set_intermed(nfe, "12345678000195", NULL),
	             E_ISNULL);
	VERIFICA_INT(nfe_nfe_set_intermed(nfe, "12345678000195", "X"),
	             E_TAMANHO);
	VERIFICA_INT(nfe_local_set_cnpj(ret, "1"), E_TAMANHO);
	VERIFICA_INT(nfe_local_set_ie(ret, "isento"), E_VALOR);
	VERIFICA_INT(nfe_local_set_endereco(ret, NULL), E_ISNULL);
	VERIFICA_INT(nfe_cobr_set_fat(cobr, "", NULL, NULL, NULL), E_TAMANHO);
	VERIFICA_INT(nfe_cobr_set_fat(cobr, NULL, "1,00", NULL, NULL), E_VALOR);
	VERIFICA_INT(nfe_cobr_add_dup(cobr, NULL, "2026-13-01", "1.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_cobr_add_dup(cobr, NULL, NULL, "0.00"), E_VALOR);
	VERIFICA_INT(nfe_cobr_add_dup(cobr, NULL, NULL, NULL), E_ISNULL);
	VERIFICA_INT(nfe_infadic_set_infcpl(inf, " texto"), E_VALOR);
	VERIFICA_INT(
	        nfe_infadic_add_obscont(inf, "campo com mais de 20 c", "x"),
	        E_TAMANHO);
	VERIFICA_INT(nfe_infadic_add_procref(inf, "P", (nfe_origem_processo)5,
	                                     NFE_ATO_NAO_INFORMADO),
	             E_VALOR);
	VERIFICA_INT(nfe_infadic_add_procref(inf, "P", NFE_PROCESSO_SEFAZ,
	                                     (nfe_ato_concessorio)9),
	             E_VALOR);
	VERIFICA_INT(nfe_resptec_set(rt, "12345678000195", "SUPORTE", "a@b.c",
	                             "1140028922"),
	             E_TAMANHO);
	VERIFICA_INT(
	        nfe_resptec_set_csrt(rt, 100, "AAECAwQFBgcICQoLDA0ODxAREhM="),
	        E_VALOR);
	VERIFICA_INT(nfe_resptec_set_csrt(rt, 1, "curto="), E_VALOR);

	/* Removendo os grupos e limpando listas */
	VERIFICA_INT(nfe_nfe_remove_autxml(nfe), 0);
	VERIFICA_INT(nfe_nfe_set_intermed(nfe, NULL, NULL), 0);
	VERIFICA_INT(nfe_cobr_remove_dup(cobr), 0);
	VERIFICA_INT(nfe_cobr_set_fat(cobr, NULL, NULL, NULL, NULL), 0);
	VERIFICA_INT(nfe_infadic_remove_obs(inf), 0);
	VERIFICA_INT(nfe_infadic_remove_procref(inf), 0);
	VERIFICA_INT(nfe_infadic_set_infadfisco(inf, NULL), 0);
	VERIFICA_INT(nfe_resptec_set_csrt(rt, 0, NULL), 0);
	VERIFICA_INT(nfe_nfe_set_retirada(nfe, NULL), 0);
	VERIFICA_INT(nfe_nfe_set_entrega(nfe, NULL), 0);
	xml = NULL;
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), 0);
	if (xml) {
		VERIFICA(strstr(xml, "retirada") == NULL);
		VERIFICA(strstr(xml, "entrega") == NULL);
		VERIFICA(strstr(xml, "autXML") == NULL);
		VERIFICA(strstr(xml, "infIntermed") == NULL);
		VERIFICA(strstr(xml, "<cobr></cobr>") != NULL ||
		         strstr(xml, "<cobr/>") != NULL);
		VERIFICA(strstr(xml, "<infAdic><infCpl>") != NULL);
		VERIFICA(strstr(xml, "CSRT") == NULL);
		VERIFICA_INT(valida_infnfe(xml), 0);
	}
	free(xml);
	VERIFICA_INT(nfe_nfe_set_cobr(nfe, NULL), 0);
	VERIFICA_INT(nfe_nfe_set_infadic(nfe, NULL), 0);
	VERIFICA_INT(nfe_nfe_set_resptec(nfe, NULL), 0);

	/* Grupos incompletos são recusados ao gerar */
	VERIFICA_INT(nfe_nfe_set_entrega(nfe, nfe_local_new()), 0);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), E_VALOR);
	VERIFICA_INT(nfe_nfe_set_entrega(nfe, NULL), 0);
	VERIFICA_INT(nfe_nfe_set_resptec(nfe, nfe_resptec_new()), 0);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), E_VALOR);
	nfe_nfe_free(nfe);
}

static void teste_obrigatorios(void)
{
	nfe_nfe *nfe = nfe_nfe_new();
	char chave[45], *xml = NULL;
	int i, rc = 0;

	/* Vazia: falta tudo */
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), E_VALOR);
	VERIFICA(xml == NULL);
	VERIFICA_INT(nfe_nfe_chave(nfe, chave, sizeof chave), E_VALOR);
	nfe_nfe_free(nfe);

	/* Sem pag */
	nfe = nota(NFE_MODELO_NFCE);
	VERIFICA_INT(nfe_nfe_set_pag(nfe, NULL), E_ISNULL);
	nfe_nfe_free(nfe);

	/* Item incompleto: o erro do grupo é repassado */
	nfe = nota(NFE_MODELO_NFCE);
	VERIFICA_INT(nfe_nfe_add_det(nfe, nfe_det_new()), 0);
	VERIFICA_INT(nfe_nfe_xml(nfe, &xml, NULL), E_VALOR);
	nfe_nfe_free(nfe);

	/* Limite de itens */
	nfe = nfe_nfe_new();
	for (i = 0; i < NFE_MAX_ITENS; i++)
		rc |= nfe_nfe_add_det(nfe, nfe_det_new());
	VERIFICA_INT(rc, 0);
	{
		nfe_det *det = nfe_det_new();
		VERIFICA_INT(nfe_nfe_add_det(nfe, det), E_VALOR);
		nfe_det_free(det);
	}
	nfe_nfe_free(nfe);

	VERIFICA_INT(nfe_nfe_xml(NULL, &xml, NULL), E_ISNULL);
	VERIFICA_INT(nfe_nfe_add_det(NULL, NULL), E_ISNULL);
	VERIFICA_INT(nfe_nfe_salvar(NULL, "x.xml"), E_ISNULL);
	nfe_nfe_free(NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "uso: %s <diretório tests>\n", argv[0]);
		return 2;
	}
	if (teste_carrega_schema(argv[1]) != 0)
		return 2;

	teste_nfce();
	teste_nfe_com_destinatario();
	teste_salvar(argv[1]);
	teste_calcular_totais();
	teste_grupos_opcionais();
	teste_obrigatorios();

	teste_libera_schema();
	TESTE_FIM();
}
