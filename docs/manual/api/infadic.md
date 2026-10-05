# infadic.h — Informações ao Fisco/contribuinte, observações e processos. Cada tipo tem limites próprios; texto livre não substitui campos estruturados.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/infadic.h>`. Informações ao Fisco/contribuinte, observações e processos. Cada tipo tem limites próprios; texto livre não substitui campos estruturados.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

/*
 * Informações adicionais da nota (grupo infAdic): texto ao fisco, texto
 * complementar ao contribuinte, observações (campo/texto) e processos
 * referenciados.
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO, E_VALOR ou E_MALLOC; em caso
 * de erro o objeto não é alterado. Textos seguem o tipo TString (sem espaço no
 * início ou no fim); NULL remove os opcionais.
 */

typedef struct nfe_infadic nfe_infadic;

/* Limites de ocorrências do leiaute */
#define NFE_MAX_OBS     10 /* obsCont e obsFisco, cada um */
#define NFE_MAX_PROCREF 100

/* indProc: origem do processo referenciado */
typedef enum nfe_origem_processo {
	NFE_PROCESSO_SEFAZ = 0,
	NFE_PROCESSO_JUSTICA_FEDERAL = 1,
	NFE_PROCESSO_JUSTICA_ESTADUAL = 2,
	NFE_PROCESSO_SECEX_RFB = 3,
	NFE_PROCESSO_CONFAZ = 4
} nfe_origem_processo;

/* tpAto: tipo do ato concessório (processos da SEFAZ) */
typedef enum nfe_ato_concessorio {
	NFE_ATO_NAO_INFORMADO = 0,
	NFE_ATO_TERMO_ACORDO = 8,
	NFE_ATO_REGIME_ESPECIAL = 10,
	NFE_ATO_AUTORIZACAO_ESPECIFICA = 12,
	NFE_ATO_AJUSTE_SINIEF = 14,
	NFE_ATO_CONVENIO_ICMS = 15
} nfe_ato_concessorio;

/* Cria as informações adicionais vazias. Retorna NULL se faltar memória. */
nfe_infadic *nfe_infadic_new(void);

/* Libera; aceita NULL */
void nfe_infadic_free(nfe_infadic *inf);

/* infAdFisco: de interesse do fisco, 1 a 2000 caracteres */
int nfe_infadic_set_infadfisco(nfe_infadic *inf, const char *texto);
/* infCpl: de interesse do contribuinte, 1 a 5000 caracteres */
int nfe_infadic_set_infcpl(nfe_infadic *inf, const char *texto);
/* obsCont / obsFisco: observação com identificação do campo (xCampo, 1 a
 * 20) e texto (xTexto, 1 a 60); até NFE_MAX_OBS de cada */
int nfe_infadic_add_obscont(nfe_infadic *inf, const char *xcampo,
                            const char *xtexto);
int nfe_infadic_add_obsfisco(nfe_infadic *inf, const char *xcampo,
                             const char *xtexto);
/* procRef: processo referenciado (nProc, 1 a 60), origem e, para processos
 * da SEFAZ, o tipo de ato (NFE_ATO_NAO_INFORMADO omite); até
 * NFE_MAX_PROCREF */
int nfe_infadic_add_procref(nfe_infadic *inf, const char *nproc,
                            nfe_origem_processo indproc,
                            nfe_ato_concessorio tpato);
/* Apagam todas as observações / processos */
int nfe_infadic_remove_obs(nfe_infadic *inf);
int nfe_infadic_remove_procref(nfe_infadic *inf);

/* Escreve o elemento <infAdic>. Retorna 0, E_ISNULL ou E_XML. */
int nfe_infadic_write_xml(xmlTextWriterPtr writer, const nfe_infadic *inf);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/infadic.h).
- [Implementação infadic.c](../../../src/libnfe/infadic.c).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
