# evento.h — Montagem de cancelamento, cancelamento por substituição e carta de correção. Os documentos produzidos ainda precisam de assinatura, envio e avaliação do retorno.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/evento.h>`. Montagem de cancelamento, cancelamento por substituição e carta de correção. Os documentos produzidos ainda precisam de assinatura, envio e avaliação do retorno.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <stddef.h>
#include <time.h>

#include <libnfe/nfe.h>

/*
 * Eventos da NF-e/NFC-e: documentos <evento> enviados à SEFAZ depois da
 * autorização, ligados à nota pela chave de acesso.
 *
 * Fluxo:
 *   nfe_evento_info info = { NFE_AMBIENTE_HOMOLOGACAO, chave,
 *                            "12345678000195", time(NULL),
 *                            NFE_TZD_BRASILIA };
 *   nfe_evento_cancelamento(&info, nprot, "ERRO NA DIGITACAO DO PEDIDO",
 *                           &evento, &tam);
 *   nfe_assinar_xml(cert, evento, tam, &assinado, &tam_assinado);
 *   nfe_sefaz_msg_evento("1", lote, 1, &msg);           (sefaz.h)
 *   nfe_sefaz_enviar(s, url, NFE_SERVICO_EVENTO, msg, &ret, &tam_ret);
 *   nfe_sefaz_proc_evento(assinado, tam_assinado, ret, tam_ret, &proc,
 *                         NULL);
 * Se a SEFAZ registrar o evento (cStat 135 ou 136 em retEvento), o
 * procEventoNFe é o XML a guardar. Para a carta de correção, use
 * nfe_evento_cce no lugar de nfe_evento_cancelamento.
 *
 * As funções retornam 0, E_ISNULL, E_TAMANHO (texto fora dos limites),
 * E_VALOR (valor inválido) ou E_MALLOC, e devolvem o documento em *xml,
 * alocado e terminado em '\0' (libere com free()), com o tamanho em *tam
 * se tam não for NULL.
 */

/* Códigos dos eventos (tpEvento) */
#define NFE_EVENTO_CCE                110110
#define NFE_EVENTO_CANCELAMENTO       110111
#define NFE_EVENTO_CANCELAMENTO_SUBST 110112

/* Dados comuns a todos os eventos */
typedef struct nfe_evento_info {
	nfe_ambiente amb;    /* tpAmb */
	const char *chave;   /* chave de acesso da nota */
	const char *cnpjcpf; /* CNPJ (14) ou CPF (11) do autor: o emitente */
	time_t dh;           /* instante do evento (dhEvento) */
	nfe_tzd tzd;         /* fuso em que dhEvento é escrito */
} nfe_evento_info;

/* Cancelamento (110111): nprot é o protocolo de autorização da nota (15 ou
 * 17 dígitos) e xjust a justificativa (15 a 255 caracteres). */
int nfe_evento_cancelamento(const nfe_evento_info *info, const char *nprot,
                            const char *xjust, char **xml, size_t *tam);

/* Cancelamento por substituição (110112), só para NFC-e: cancela a nota
 * que foi substituída pela NFC-e chave_subst, emitida em contingência
 * offline (tpEmis 9) para a mesma venda. A nota cancelada não pode ser de
 * contingência offline: é o caso do PDV que não recebeu a resposta da
 * autorização, emitiu offline e depois descobre a nota normal autorizada.
 * Uma NFC-e offline só se desfaz com o cancelamento comum (110111). Outras
 * combinações a SEFAZ rejeita (cStat 920), e aqui dão E_VALOR. veraplic
 * identifica o programa emissor (1 a 20 caracteres). */
int nfe_evento_cancelamento_subst(const nfe_evento_info *info,
                                  const char *nprot, const char *xjust,
                                  const char *chave_subst, const char *veraplic,
                                  char **xml, size_t *tam);

/* Carta de correção (110110): corrige erros da nota que não mudam o valor
 * do imposto, os dados cadastrais do emitente ou do destinatário nem a
 * data (as condições de uso, texto fixo do leiaute, são acrescentadas
 * sozinhas). xcorrecao (15 a 1000 caracteres) substitui as correções
 * anteriores: cada nova carta deve trazer todas as correções, com nseq
 * (1 a 20) uma unidade maior que o da anterior. */
int nfe_evento_cce(const nfe_evento_info *info, int nseq, const char *xcorrecao,
                   char **xml, size_t *tam);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/evento.h).
- [Implementação evento.c](../../../src/libnfe/evento.c).
- [Programa de testes compilável](../../../tests/test_evento.c): `make obj/test_evento` e `./obj/test_evento tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
