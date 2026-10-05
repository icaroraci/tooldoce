# ide.h — Identificação, chave, referências e dados das operações governamentais/antecipação. Os construtores fornecem padrões; campos sem padrão continuam obrigatórios.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/ide.h>`. Identificação, chave, referências e dados das operações governamentais/antecipação. Os construtores fornecem padrões; campos sem padrão continuam obrigatórios.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <libxml/xmlwriter.h>
#include <libnfe/grupo.h>
#include <libnfe/nfe.h>

/*
 * Grupo ide: identificação da NF-e.
 *
 * Uso:
 *   nfe_ide *ide = nfe_ide_new();
 *   nfe_ide_set_cuf(ide, NFE_UF_SP);
 *   nfe_ide_set_natop(ide, "VENDA");
 *   ...
 *   nfe_ide_write_xml(writer, ide);
 *   nfe_ide_free(ide);
 *
 * Os setters validam o valor contra o leiaute 4.00 e retornam 0, E_ISNULL,
 * E_VALOR (valor fora do domínio) ou E_TAMANHO (texto fora dos limites);
 * em caso de erro o campo não é alterado.
 */

typedef struct nfe_ide nfe_ide;

struct refNFe_s;
struct refNF_s;

/* Data/hora opcional não informada (ex.: dhSaiEnt) */
#define NFE_SEM_DATA ((time_t) - 1)

/* Número máximo de documentos referenciados (grupo NFref: maxOccurs="999"
 * no leiauteNFe_v4.00.xsd) */
#define NFE_MAX_NFREF 999

/* Cria um ide com os valores padrão: mod 55, tpNF saída, idDest interna,
 * tpImp DANFE retrato, tpEmis normal, tpAmb HOMOLOGAÇÃO, finNFe normal,
 * indFinal normal, indPres não se aplica, procEmi aplicativo do
 * contribuinte, fuso de Brasília. Retorna NULL se faltar memória. */
nfe_ide *nfe_ide_new(void);

/* Libera o ide e as referências que ele possui; aceita NULL */
void nfe_ide_free(nfe_ide *ide);

int nfe_ide_set_cuf(nfe_ide *ide, nfe_uf cuf);
int nfe_ide_set_cnf(nfe_ide *ide, uint32_t cnf); /* 0 a 99999999 */
/* Textos (natOp, verProc, xJust): limites em caracteres UTF-8, só
 * caracteres de U+0020 a U+00FF e sem espaço no início ou no fim (tipo
 * TString do leiaute); fora disso, E_TAMANHO ou E_VALOR */
int nfe_ide_set_natop(nfe_ide *ide, const char *natop); /* 1 a 60 caracteres */
int nfe_ide_set_mod(nfe_ide *ide, nfe_modelo mod);
int nfe_ide_set_serie(nfe_ide *ide, unsigned serie); /* 0 a 999 */
int nfe_ide_set_nnf(nfe_ide *ide, uint32_t nnf);     /* 1 a 999999999 */
int nfe_ide_set_dhemi(nfe_ide *ide, time_t dhemi);
int nfe_ide_set_dhsaient(nfe_ide *ide,
                         time_t dhsaient); /* aceita NFE_SEM_DATA */
int nfe_ide_set_tpnf(nfe_ide *ide, nfe_tipo_operacao tpnf);
int nfe_ide_set_iddest(nfe_ide *ide, nfe_destino iddest);
int nfe_ide_set_cmunfg(nfe_ide *ide, uint32_t cmunfg); /* 7 dígitos */
int nfe_ide_set_tpimp(nfe_ide *ide, nfe_danfe tpimp);
int nfe_ide_set_tpemis(nfe_ide *ide, nfe_emissao tpemis);
int nfe_ide_set_cdv(nfe_ide *ide, unsigned cdv); /* 0 a 9 */
int nfe_ide_set_tpamb(nfe_ide *ide, nfe_ambiente tpamb);
int nfe_ide_set_finnfe(nfe_ide *ide, nfe_finalidade finnfe);
int nfe_ide_set_indfinal(nfe_ide *ide, nfe_consumidor indfinal);
int nfe_ide_set_indpres(nfe_ide *ide, nfe_presenca indpres);
int nfe_ide_set_procemi(nfe_ide *ide, nfe_processo_emissao procemi);
int nfe_ide_set_verproc(nfe_ide *ide,
                        const char *verproc); /* 1 a 20 caracteres */

/* Campos opcionais incluídos pela Reforma Tributária (PL_010f) e pela NT
 * 2020.006. Cada um só é escrito no XML se tiver sido informado. */

/* dPrevEntrega: data prevista de entrega (só a data, no fuso do ide);
 * NFE_SEM_DATA remove */
int nfe_ide_set_dpreventrega(nfe_ide *ide, time_t dpreventrega);
/* cMunFGIBS: município do fato gerador do IBS/CBS (7 dígitos); 0 remove */
int nfe_ide_set_cmunfgibs(nfe_ide *ide, uint32_t cmunfgibs);
/* tpNFDebito / tpNFCredito: tipo da nota de débito ou de crédito;
 * NFE_DEBITO_NAO_INFORMADO / NFE_CREDITO_NAO_INFORMADO removem */
int nfe_ide_set_tpnfdebito(nfe_ide *ide, nfe_tipo_debito tpnfdebito);
int nfe_ide_set_tpnfcredito(nfe_ide *ide, nfe_tipo_credito tpnfcredito);
/* indIntermed: intermediador; NFE_INTERMEDIADOR_NAO_INFORMADO remove */
int nfe_ide_set_indintermed(nfe_ide *ide, nfe_intermediador indintermed);
/* cIndOp: código indicador do local da operação (6 dígitos); NULL remove */
int nfe_ide_set_cindop(nfe_ide *ide, const char *cindop);

/* Fuso horário em que as datas (dhEmi, dhSaiEnt, dhCont) são escritas */
int nfe_ide_set_tzd(nfe_ide *ide, nfe_tzd tzd);

/* Entrada em contingência: instante e justificativa (15 a 256 caracteres).
 * Gera dhCont e xJust no XML. */
int nfe_ide_set_contingencia(nfe_ide *ide, time_t dhcont, const char *xjust);

/* Gera a chave de acesso da nota em chave (tam >= 45 bytes) a partir de
 * cUF, dhEmi (ano e mês no fuso do ide), mod, serie, nNF, tpEmis e cNF do
 * ide, e do CNPJ (14 posições, numérico ou alfanumérico) ou CPF (11
 * dígitos) do emitente. Calcula o dígito verificador e o grava em cDV.
 * Defina todos esses campos antes: alterá-los depois invalida a chave.
 * Retorna 0, E_ISNULL, E_TAMANHO (buffer pequeno ou CNPJ/CPF com tamanho
 * errado) ou E_VALOR (cUF, nNF ou dhEmi não informados, ou CNPJ/CPF
 * inválido; ver cnpjcpf.h). */
int nfe_ide_gerar_chave(nfe_ide *ide, const char *cnpjcpf, char *chave,
                        size_t tam);

/* Documentos fiscais referenciados (grupo NFref, até NFE_MAX_NFREF), gerados
 * na ordem em que foram adicionados. Em caso de sucesso, o ide passa a ser
 * dono da referência e a libera em nfe_ide_free. Retornam 0, E_ISNULL,
 * E_VALOR (limite atingido) ou E_MALLOC. */
int nfe_ide_add_refnfe(nfe_ide *ide, struct refNFe_s *ref);
int nfe_ide_add_refnf(nfe_ide *ide, struct refNF_s *ref);
/* Referência genérica: acrescenta um grupo NFref vazio (pertence ao ide) e
 * o devolve em *nfref, para ser preenchido pelo grupo genérico (grupo.h)
 * com qualquer tipo de documento: refNFe, refNFeSig, refNF, refNFP
 * (produtor rural), refCTe ou refECF (cupom fiscal). Ex.:
 *   nfe_grupo_set(nfref, "refECF/mod", "2D");
 * Retorna 0, E_ISNULL, E_VALOR (limite atingido) ou E_MALLOC. */
int nfe_ide_add_nfref(nfe_ide *ide, nfe_grupo **nfref);

/* Número máximo de chaves em gCompraGov/refDFeAnt e em gPagAntecipado/refNFe
 * (maxOccurs="99" no XSD) */
#define NFE_MAX_REF_RTC 99

/* gCompraGov: compra governamental. tpentegov NFE_ENTE_GOV_NAO_INFORMADO
 * remove o grupo (e as chaves anteriores). predutor: percentual de redução
 * de alíquota, como texto no formato do XML (tipo TDec_0302_04RTC: "0",
 * "10.50", "100.0000"...). Retorna 0, E_ISNULL ou E_VALOR. */
int nfe_ide_set_compragov(nfe_ide *ide, nfe_ente_gov tpentegov,
                          const char *predutor, nfe_oper_gov tpopergov);
/* refDFeAnt: chave de acesso de um documento anterior da compra
 * governamental (44 posições, dígito verificador correto). Exigida para
 * tpOperGov 2 (exatamente uma) e 3 (uma ou mais) e vedada para 1 e 4; a
 * regra é conferida em nfe_ide_write_xml. Retorna 0, E_ISNULL, E_TAMANHO ou
 * E_VALOR (chave inválida, grupo gCompraGov não informado ou limite
 * NFE_MAX_REF_RTC atingido). */
int nfe_ide_add_compragov_refdfeant(nfe_ide *ide, const char *chave);

/* gPagAntecipado: chaves das NF-e de antecipação de pagamento que esta nota
 * abate. Retorna 0, E_ISNULL, E_TAMANHO ou E_VALOR (chave inválida ou limite
 * NFE_MAX_REF_RTC atingido). nfe_ide_remove_pagantecipado apaga todas. */
int nfe_ide_add_pagantecipado(nfe_ide *ide, const char *refnfe);
int nfe_ide_remove_pagantecipado(nfe_ide *ide);

/* Escreve o elemento <ide>. Retorna 0, E_ISNULL, E_VALOR (campo obrigatório
 * sem valor padrão não informado: cUF, natOp, nNF, dhEmi, cMunFG ou verProc;
 * ou chaves anteriores de gCompraGov em desacordo com tpOperGov) ou E_XML. */
int nfe_ide_write_xml(xmlTextWriterPtr writer, const nfe_ide *ide);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/ide.h).
- [Implementação ide.c](../../../src/libnfe/ide.c).
- [Programa de testes compilável](../../../tests/test_ide.c): `make obj/test_ide` e `./obj/test_ide tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
