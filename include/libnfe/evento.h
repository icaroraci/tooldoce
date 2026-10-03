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

#ifndef LIBNFE_EVENTO_H
#define LIBNFE_EVENTO_H

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
 * que foi substituída pela NFC-e chave_subst (emitida em contingência
 * para a mesma venda). veraplic identifica o programa emissor (1 a 20
 * caracteres). */
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

#endif
