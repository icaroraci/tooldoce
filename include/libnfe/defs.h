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


#ifndef LIBNFE_DEFS_H
#define LIBNFE_DEFS_H

/*
 * Tamanhos dos campos do leiaute.
 *
 * NFE_TAM_<CAMPO> é o limite do campo em CARACTERES, como no leiaute.
 * Para declarar o buffer, use:
 *   NFE_TAM_ASCII(n) - campos só com dígitos ou ASCII (chave, CNPJ...):
 *                      1 byte por caractere + terminador;
 *   NFE_TAM_UTF8(n)  - campos de texto livre (UTF-8): até 4 bytes por
 *                      caractere + terminador.
 * Ex.: char natOp[NFE_TAM_UTF8(NFE_TAM_NATOP)];
 */
#define NFE_TAM_ASCII(n) ((n) + 1)
#define NFE_TAM_UTF8(n)  ((n) * 4 + 1)

/* Campos numéricos / ASCII */
#define NFE_TAM_CHAVE    44  /* chave de acesso */
#define NFE_TAM_CUF       2  /* cUF: código IBGE da UF */
#define NFE_TAM_AAMM      4  /* AAMM: ano e mês de emissão */
#define NFE_TAM_CNPJ     14  /* CNPJ */
#define NFE_TAM_CPF      11  /* CPF */
#define NFE_TAM_IE       14  /* IE: inscrição estadual (dígitos ou "ISENTO") */
#define NFE_TAM_MOD       2  /* mod: modelo do documento fiscal */
#define NFE_TAM_SERIE     3  /* serie */
#define NFE_TAM_NNF       9  /* nNF: número do documento fiscal */
#define NFE_TAM_NECF      3  /* nECF: número de ordem do ECF */
#define NFE_TAM_NCOO      6  /* nCOO: número do contador de ordem de operação */
#define NFE_TAM_DATA_HORA 25 /* data e hora com fuso: AAAA-MM-DDThh:mm:ss-03:00 */

/* Campos de texto livre (UTF-8) */
#define NFE_TAM_XNOME    60  /* xNome: razão social ou nome (2 a 60) */
#define NFE_TAM_XFANT    60  /* xFant: nome fantasia (1 a 60) */
#define NFE_TAM_NATOP    60  /* natOp: natureza da operação (1 a 60) */
#define NFE_TAM_VERPROC  20  /* verProc: versão do aplicativo emissor (1 a 20) */
#define NFE_TAM_XJUST   256  /* xJust: justificativa da contingência (15 a 256) */
#define NFE_TAM_XLGR     60  /* xLgr: logradouro (2 a 60) */
#define NFE_TAM_NRO      60  /* nro: número (1 a 60) */
#define NFE_TAM_XCPL     60  /* xCpl: complemento (1 a 60) */
#define NFE_TAM_XBAIRRO  60  /* xBairro: bairro (2 a 60) */

#endif
