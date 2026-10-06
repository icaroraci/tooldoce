# inutilizacao.h — Montagem do pedido de inutilização para série e faixa numérica. Aceitação local do pedido não significa homologação da faixa pela SEFAZ.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/inutilizacao.h>`. Montagem do pedido de inutilização para série e faixa numérica. Aceitação local do pedido não significa homologação da faixa pela SEFAZ.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <stddef.h>

#include <libnfe/nfe.h>

/*
 * Inutilização de numeração: avisa a SEFAZ que uma faixa de números de
 * uma série não será usada (por exemplo, números pulados por falha do
 * sistema), antes do dia 10 do mês seguinte.
 *
 * Fluxo:
 *   nfe_inutilizacao(NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP, 26,
 *                    "12345678000195", NFE_MODELO_NFE, 1, 10, 12,
 *                    "NUMEROS PULADOS POR FALHA DO SISTEMA", &inut, &tam);
 *   nfe_assinar_xml(cert, inut, tam, &assinado, &tam_assinado);
 *   nfe_sefaz_enviar(s, url, NFE_SERVICO_INUTILIZACAO, assinado, &ret,
 *                    &tam_ret);
 *   nfe_sefaz_proc_inutilizacao(assinado, tam_assinado, ret, tam_ret,
 *                               &proc, NULL);                (sefaz.h)
 * Se a SEFAZ homologar a inutilização (cStat 102 no retorno), o
 * ProcInutNFe é o XML a guardar.
 */

/* Monta o documento <inutNFe>, ainda sem a assinatura: ano é o ano da
 * inutilização com 2 dígitos (0 a 99), cnpj o do emitente, serie de 0 a
 * 999, a faixa de nini a nfin (1 a 999999999, nini <= nfin) e xjust a
 * justificativa (15 a 255 caracteres). Devolve o documento em *xml,
 * alocado e terminado em '\0' (libere com free()), com o tamanho em *tam
 * se tam não for NULL. Retorna 0, E_ISNULL, E_TAMANHO (justificativa fora
 * dos limites), E_VALOR ou E_MALLOC. */
int nfe_inutilizacao(nfe_ambiente amb, nfe_uf uf, int ano, const char *cnpj,
                     nfe_modelo mod, int serie, long nini, long nfin,
                     const char *xjust, char **xml, size_t *tam);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/inutilizacao.h).
- [Implementação inutilizacao.c](../../../src/libnfe/inutilizacao.c).
- [Programa de testes compilável](../../../tests/test_inutilizacao.c): `make obj/test_inutilizacao` e `./obj/test_inutilizacao tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
