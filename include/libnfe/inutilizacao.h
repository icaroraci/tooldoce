/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 **
 ** This file is part of tooldoce.
 **
 ** tooldoce is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published
 ** by the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** tooldoce is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with tooldoce.  If not, see <https://www.gnu.org/licenses/>.
 ** */

#ifndef LIBNFE_INUTILIZACAO_H
#define LIBNFE_INUTILIZACAO_H

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

#endif
