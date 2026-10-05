# imposto.h — Tributos do item, motor de grupos e atalhos mais usados. Formatos e presença são conferidos; o enquadramento e as contas continuam sob responsabilidade do emissor.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/imposto.h>`. Tributos do item, motor de grupos e atalhos mais usados. Formatos e presença são conferidos; o enquadramento e as contas continuam sob responsabilidade do emissor.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/grupo.h>
#include <libnfe/nfe.h>
#include <libnfe/utils.h>

/*
 * Tributos de um item da nota (grupo det/imposto).
 *
 * Todos os grupos do leiaute são aceitos por nfe_imposto_set, que grava um
 * campo pelo caminho (nomes de elementos do leiaute separados por "/"):
 *   nfe_imposto_set(imp, "ICMS10/orig", "0");
 *   nfe_imposto_set(imp, "ICMS10/modBC", "3");
 *   nfe_imposto_set(imp, "ICMS10/vBC", "100.00");
 *   ...
 *   nfe_imposto_set(imp, "IBSCBS/gIBSCBS/vBC", "100.00");
 * O caminho só precisa ter os nomes suficientes para identificar o campo,
 * na ordem (ex.: "ICMS10/vBC" em vez de "ICMS/ICMS10/vBC"). Ao gravar um
 * campo de um grupo que é uma escolha no leiaute (ICMS00, ICMS10... dentro
 * de ICMS), os campos dos outros grupos da escolha são apagados. Campos com
 * um único valor possível (como o CST de ICMS10) são preenchidos sozinhos.
 * Os campos obrigatórios de cada grupo são conferidos ao gerar o XML.
 *
 * Há também atalhos para os grupos mais usados: ICMS00, ICMSSN102,
 * PISAliq/PISNT e COFINSAliq/COFINSNT.
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
 * Os setters retornam 0, E_ISNULL, E_TAMANHO, E_VALOR (campo inexistente
 * ou valor fora do domínio ou do formato) ou E_MALLOC; em caso de erro o
 * imposto não é alterado. Cada atalho substitui o grupo anterior do mesmo
 * tributo.
 */

typedef struct nfe_imposto nfe_imposto;

/* Cria um imposto sem nenhum tributo. Retorna NULL se faltar memória. */
nfe_imposto *nfe_imposto_new(void);

/* Libera o imposto; aceita NULL */
void nfe_imposto_free(nfe_imposto *imp);

/* Grupo genérico com todos os campos de <imposto> (pertence ao imposto) */
nfe_grupo *nfe_imposto_grupo(nfe_imposto *imp);

/* Grava um campo qualquer de <imposto> pelo caminho; NULL apaga o campo */
int nfe_imposto_set(nfe_imposto *imp, const char *caminho, const char *valor);
/* Valor do campo, ou NULL se não informado (texto pertencente ao imposto) */
const char *nfe_imposto_get(const nfe_imposto *imp, const char *caminho);
/* Apaga todos os campos de um grupo (ex.: "ICMS", "IPI", "IBSCBS").
 * Retorna 0, E_ISNULL ou E_VALOR (grupo inexistente). */
int nfe_imposto_remove(nfe_imposto *imp, const char *caminho);

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

/* PISNT / COFINSNT: CST 04 a 09 (não tributado) */
int nfe_imposto_set_pisnt(nfe_imposto *imp, nfe_cst_pis_cofins cst);
int nfe_imposto_set_cofinsnt(nfe_imposto *imp, nfe_cst_pis_cofins cst);

/* Removem o grupo do tributo */
int nfe_imposto_remove_icms(nfe_imposto *imp);
int nfe_imposto_remove_pis(nfe_imposto *imp);
int nfe_imposto_remove_cofins(nfe_imposto *imp);

/* Escreve o elemento <imposto>. Retorna 0, E_ISNULL, E_VALOR (falta campo
 * obrigatório em algum grupo informado, ou grupos incompatíveis) ou
 * E_XML. */
int nfe_imposto_write_xml(xmlTextWriterPtr writer, const nfe_imposto *imp);

/* Uso interno (cálculo dos totais) */
enum nfe_imposto_valor_e {
	NFE_IMP_VBC, /* base do ICMS */
	NFE_IMP_VICMS,
	NFE_IMP_VFCP,
	NFE_IMP_VPIS,
	NFE_IMP_VCOFINS,
	NFE_IMP_VTOTTRIB,
	NFE_IMP_VST, /* ICMS ST */
	NFE_IMP_VFCPST,
	NFE_IMP_VICMSDESON,
	NFE_IMP_VII,
	NFE_IMP_VIPI,
	NFE_IMP_VBCST
};
/* Valor do campo ("" se não informado ou se o grupo não o tem) */
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/imposto.h).
- [Implementação imposto.c](../../../src/libnfe/imposto.c).
- [Programa de testes compilável](../../../tests/test_imposto.c): `make obj/test_imposto` e `./obj/test_imposto tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
