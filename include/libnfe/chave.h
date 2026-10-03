/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef LIBNFE_CHAVE_H
#define LIBNFE_CHAVE_H

/*
 * Chave de acesso da NF-e/NFC-e: 44 dígitos.
 *
 *   cUF(2) AAMM(4) CNPJ/CPF(14) mod(2) serie(3) nNF(9) tpEmis(1) cNF(8) cDV(1)
 *
 * O dígito verificador (cDV) é calculado pelo módulo 11 sobre os 43
 * primeiros dígitos, com pesos de 2 a 9 da direita para a esquerda; resto 0
 * ou 1 resulta em 0. Para gerar a chave de uma nota, use
 * nfe_ide_gerar_chave() (ide.h).
 */

/* Calcula o dígito verificador dos 43 primeiros caracteres de chave.
 * Retorna o dígito (0 a 9), E_ISNULL, E_TAMANHO (menos de 43 caracteres) ou
 * E_VALOR (caractere que não é dígito). */
int nfe_chave_dv(const char *chave);

/* Confere uma chave completa: 44 dígitos com o dígito verificador correto.
 * Retorna 0, E_ISNULL, E_TAMANHO ou E_VALOR. */
int nfe_chave_validar(const char *chave);

#endif
