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

#ifndef LIBNFE_PROD_H
#define LIBNFE_PROD_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/nfe.h>

/*
 * Produto ou serviço de um item da nota (grupo det/prod).
 *
 * Ainda não implementados: gCred, DI, detExport, rastro, infProdNFF,
 * infProdEmb e os grupos específicos (veicProd, med, arma, comb, nRECOPI).
 *
 * Uso típico:
 *   nfe_prod *prod = nfe_prod_new();
 *   nfe_prod_set_cprod(prod, "001");
 *   nfe_prod_set_xprod(prod, "CANETA AZUL");
 *   nfe_prod_set_ncm(prod, "96081000");
 *   nfe_prod_set_cfop(prod, 5102);
 *   nfe_prod_set_comercial(prod, "UN", "10", "1.50", "15.00");
 *   nfe_prod_set_tributavel(prod, "UN", "10", "1.50");
 *   nfe_prod_write_xml(writer, prod);
 *   nfe_prod_free(prod);
 *
 * Os setters validam o valor contra o leiaute e retornam 0, E_ISNULL,
 * E_TAMANHO (texto fora dos limites) ou E_VALOR (valor fora do domínio ou
 * do formato); em caso de erro o campo não é alterado. Textos: limites em
 * caracteres UTF-8, só caracteres de U+0020 a U+00FF e sem espaço no início
 * ou no fim (tipo TString). Valores decimais são texto no formato do XML,
 * com ponto ("1.50"). Campos opcionais são removidos com NULL.
 */

typedef struct nfe_prod nfe_prod;

/* Valor de cEAN/cEANTrib para produto sem código de barras GTIN */
#define NFE_SEM_GTIN "SEM GTIN"

/* Número máximo de códigos NVE por produto */
#define NFE_MAX_NVE 8

/* Cria um produto com cEAN e cEANTrib "SEM GTIN" e indTot 1 (o valor do
 * item compõe o total da nota). Retorna NULL se faltar memória. */
nfe_prod *nfe_prod_new(void);

/* Libera o produto; aceita NULL */
void nfe_prod_free(nfe_prod *prod);

int nfe_prod_set_cprod(nfe_prod *prod, const char *cprod); /* 1 a 60 */
/* cEAN: GTIN com 8, 12, 13 ou 14 dígitos, "" ou NFE_SEM_GTIN */
int nfe_prod_set_cean(nfe_prod *prod, const char *cean);
/* cBarra: código de barras diferente do GTIN, 3 a 30 caracteres */
int nfe_prod_set_cbarra(nfe_prod *prod, const char *cbarra);
int nfe_prod_set_xprod(nfe_prod *prod, const char *xprod); /* 1 a 120 */
/* NCM: 8 dígitos, ou só o capítulo (2 dígitos) para serviços e itens sem
 * NCM */
int nfe_prod_set_ncm(nfe_prod *prod, const char *ncm);
/* NVE: até NFE_MAX_NVE códigos (2 letras maiúsculas e 4 dígitos), na ordem
 * de inclusão. nfe_prod_remove_nve apaga todos. */
int nfe_prod_add_nve(nfe_prod *prod, const char *nve);
int nfe_prod_remove_nve(nfe_prod *prod);
/* CEST: 7 dígitos. indEscala e CNPJFab só são escritos junto com o CEST;
 * CNPJFab é o CNPJ do fabricante (obrigatório na produção em escala não
 * relevante). */
int nfe_prod_set_cest(nfe_prod *prod, const char *cest);
int nfe_prod_set_indescala(nfe_prod *prod, nfe_escala indescala);
int nfe_prod_set_cnpjfab(nfe_prod *prod, const char *cnpjfab);
/* cBenef: código de benefício fiscal na UF (8 ou 10 caracteres) ou
 * "SEM CBENEF" */
int nfe_prod_set_cbenef(nfe_prod *prod, const char *cbenef);
int nfe_prod_set_tpcredpresibszfm(nfe_prod *prod, nfe_cred_pres_zfm tipo);
/* EXTIPI: exceção da TIPI, 2 ou 3 dígitos */
int nfe_prod_set_extipi(nfe_prod *prod, const char *extipi);
/* CFOP: 4 dígitos, começando por 1, 2, 3, 5, 6 ou 7 */
int nfe_prod_set_cfop(nfe_prod *prod, unsigned cfop);
/* Unidade comercial (uCom, 1 a 6 caracteres), quantidade (qCom, até 4
 * casas), valor unitário (vUnCom, até 10 casas) e valor total bruto (vProd,
 * 2 casas). Todos obrigatórios; só grava se os quatro forem válidos. */
int nfe_prod_set_comercial(nfe_prod *prod, const char *ucom, const char *qcom,
                           const char *vuncom, const char *vprod);
/* cEANTrib: como cEAN, para a unidade tributável */
int nfe_prod_set_ceantrib(nfe_prod *prod, const char *ceantrib);
int nfe_prod_set_cbarratrib(nfe_prod *prod, const char *cbarratrib);
/* Unidade tributável (uTrib), quantidade (qTrib) e valor unitário
 * (vUnTrib), com os mesmos formatos da unidade comercial */
int nfe_prod_set_tributavel(nfe_prod *prod, const char *utrib,
                            const char *qtrib, const char *vuntrib);
/* vFrete, vSeg, vDesc e vOutro: maiores que zero, 2 casas */
int nfe_prod_set_vfrete(nfe_prod *prod, const char *vfrete);
int nfe_prod_set_vseg(nfe_prod *prod, const char *vseg);
int nfe_prod_set_vdesc(nfe_prod *prod, const char *vdesc);
int nfe_prod_set_voutro(nfe_prod *prod, const char *voutro);
/* indTot: 1 se o valor do item (vProd) compõe o total da nota, 0 se não */
int nfe_prod_set_indtot(nfe_prod *prod, int indtot);
/* indBemMovelUsado: 1 para bem móvel usado, 0 remove */
int nfe_prod_set_indbemmovelusado(nfe_prod *prod, int usado);
/* xPed: pedido de compra (1 a 15); nItemPed: item do pedido (1 a 6
 * dígitos) */
int nfe_prod_set_xped(nfe_prod *prod, const char *xped);
int nfe_prod_set_nitemped(nfe_prod *prod, const char *nitemped);
/* nFCI: número da FCI, GUID em maiúsculas com hífens */
int nfe_prod_set_nfci(nfe_prod *prod, const char *nfci);

/* Escreve o elemento <prod>. Retorna 0, E_ISNULL, E_VALOR (falta cProd,
 * xProd, NCM, CFOP ou os dados comercial/tributável; ou indEscala/CNPJFab
 * sem CEST) ou E_XML. */
int nfe_prod_write_xml(xmlTextWriterPtr writer, const nfe_prod *prod);

#endif
