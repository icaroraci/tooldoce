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

#ifndef LIBNFE_IMPOSTO_H
#define LIBNFE_IMPOSTO_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/nfe.h>
#include <libnfe/utils.h>

/*
 * Tributos de um item da nota (grupo det/imposto).
 *
 * Implementados até agora: ICMS00 (tributação integral) e ICMSSN102
 * (Simples Nacional sem crédito) no ICMS; PISAliq e PISNT no PIS; COFINSAliq
 * e COFINSNT na COFINS. Os demais grupos (outros CST/CSOSN, IPI, II, ISSQN,
 * PISST, COFINSST, ICMSUFDest, IS e IBSCBS) ainda não.
 *
 * Uso típico:
 *   nfe_imposto *imp = nfe_imposto_new();
 *   nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
 *                          NFE_MOD_BC_VALOR_OPERACAO, "100.00", "18.00",
 *                          "18.00", NULL, NULL);
 *   nfe_imposto_set_pisaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA, "100.00",
 *                           "1.65", "1.65");
 *   nfe_imposto_set_cofinsaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA, "100.00",
 *                              "7.60", "7.60");
 *   nfe_imposto_write_xml(writer, imp);
 *   nfe_imposto_free(imp);
 *
 * Valores são texto no formato do XML, com ponto e sem zeros à esquerda:
 * bases e valores inteiros ou com 2 casas ("100" ou "100.00"), alíquotas
 * (%) inteiras ou com 2 a 4 casas ("18", "18.00", "1.6500").
 * A biblioteca confere o formato, mas não refaz as contas (vICMS = vBC x
 * pICMS etc.).
 *
 * Os setters retornam 0, E_ISNULL ou E_VALOR (valor fora do domínio ou do
 * formato); em caso de erro o grupo não é alterado. Cada setter de um
 * tributo substitui o grupo anterior do mesmo tributo.
 */

typedef struct nfe_imposto nfe_imposto;

/* Cria um imposto sem nenhum tributo. Retorna NULL se faltar memória. */
nfe_imposto *nfe_imposto_new(void);

/* Libera o imposto; aceita NULL */
void nfe_imposto_free(nfe_imposto *imp);

/* vTotTrib: valor aproximado total dos tributos (Lei 12.741/2012); NULL
 * remove */
int nfe_imposto_set_vtottrib(nfe_imposto *imp, const char *vtottrib);

/* ICMS00: tributada integralmente. pfcp e vfcp (Fundo de Combate à
 * Pobreza) são opcionais, mas vão juntos: ambos NULL ou ambos informados. */
int nfe_imposto_set_icms00(nfe_imposto *imp, nfe_origem orig, nfe_mod_bc modbc,
                           const char *vbc, const char *picms,
                           const char *vicms, const char *pfcp,
                           const char *vfcp);

/* ICMSSN102: Simples Nacional sem permissão de crédito, isenção por faixa,
 * imune ou não tributada. orig pode ser NFE_ORIGEM_NAO_INFORMADA. */
int nfe_imposto_set_icmssn102(nfe_imposto *imp, nfe_origem orig,
                              nfe_csosn_102 csosn);

/* PISAliq / COFINSAliq: CST 01 ou 02, com base, alíquota (%) e valor */
int nfe_imposto_set_pisaliq(nfe_imposto *imp, nfe_cst_pis_cofins cst,
                            const char *vbc, const char *ppis,
                            const char *vpis);
int nfe_imposto_set_cofinsaliq(nfe_imposto *imp, nfe_cst_pis_cofins cst,
                               const char *vbc, const char *pcofins,
                               const char *vcofins);

/* PISNT / COFINSNT: CST 04 a 08 (não tributado) */
int nfe_imposto_set_pisnt(nfe_imposto *imp, nfe_cst_pis_cofins cst);
int nfe_imposto_set_cofinsnt(nfe_imposto *imp, nfe_cst_pis_cofins cst);

/* Removem o grupo do tributo */
int nfe_imposto_remove_icms(nfe_imposto *imp);
int nfe_imposto_remove_pis(nfe_imposto *imp);
int nfe_imposto_remove_cofins(nfe_imposto *imp);

/* Escreve o elemento <imposto>. Retorna 0, E_ISNULL ou E_XML. */
int nfe_imposto_write_xml(xmlTextWriterPtr writer, const nfe_imposto *imp);

/* Uso interno (cálculo dos totais) */
enum nfe_imposto_valor_e {
	NFE_IMP_VBC, /* base do ICMS */
	NFE_IMP_VICMS,
	NFE_IMP_VFCP,
	NFE_IMP_VPIS,
	NFE_IMP_VCOFINS,
	NFE_IMP_VTOTTRIB
};
/* Valor do campo ("" se não informado ou se o grupo não o tem) */
NFE_INTERNO const char *nfe_imposto_valor(const nfe_imposto *imp,
                                          enum nfe_imposto_valor_e campo);

#endif
