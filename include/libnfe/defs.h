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
#define NFE_TAM_CHAVE     44 /* chave de acesso */
#define NFE_TAM_CUF       2  /* cUF: código IBGE da UF */
#define NFE_TAM_AAMM      4  /* AAMM: ano e mês de emissão */
#define NFE_TAM_CNPJ      14 /* CNPJ */
#define NFE_TAM_CPF       11 /* CPF */
#define NFE_TAM_IE        14 /* IE: inscrição estadual (dígitos ou "ISENTO") */
#define NFE_TAM_MOD       2 /* mod: modelo do documento fiscal */
#define NFE_TAM_SERIE     3 /* serie */
#define NFE_TAM_NNF       9 /* nNF: número do documento fiscal */
#define NFE_TAM_NECF      3 /* nECF: número de ordem do ECF */
#define NFE_TAM_NCOO      6 /* nCOO: número do contador de ordem de operação */
#define NFE_TAM_DATA_HORA 25 /* AAAA-MM-DDThh:mm:ss-03:00 */
#define NFE_TAM_CEP       8  /* CEP */
#define NFE_TAM_FONE      14 /* fone: DDD e número (6 a 14 dígitos) */
#define NFE_TAM_CNAE      7  /* CNAE fiscal */
#define NFE_TAM_ISUF      9  /* inscrição na SUFRAMA (8 ou 9 dígitos) */
#define NFE_TAM_GTIN      14 /* cEAN/cEANTrib: GTIN (8, 12, 13 ou 14 dígitos) */
#define NFE_TAM_NCM       8  /* NCM */
#define NFE_TAM_NVE       6  /* NVE: 2 letras e 4 dígitos */
#define NFE_TAM_CEST      7  /* CEST */
#define NFE_TAM_EXTIPI    3  /* EXTIPI (2 ou 3 dígitos) */
#define NFE_TAM_NITEMPED  6  /* nItemPed */
#define NFE_TAM_GUID      36 /* nFCI: GUID com hífens */
#define NFE_TAM_DEC       24 /* valor decimal como texto (TDec_*) */

/* Campos de texto livre (UTF-8) */
#define NFE_TAM_XNOME     60  /* xNome: razão social ou nome (2 a 60) */
#define NFE_TAM_XFANT     60  /* xFant: nome fantasia (1 a 60) */
#define NFE_TAM_NATOP     60  /* natOp: natureza da operação (1 a 60) */
#define NFE_TAM_VERPROC   20  /* verProc: versão do emissor (1 a 20) */
#define NFE_TAM_XJUST     256 /* xJust: justificativa (15 a 256) */
#define NFE_TAM_XLGR      60  /* xLgr: logradouro (2 a 60) */
#define NFE_TAM_NRO       60  /* nro: número (1 a 60) */
#define NFE_TAM_XCPL      60  /* xCpl: complemento (1 a 60) */
#define NFE_TAM_XBAIRRO   60  /* xBairro: bairro (2 a 60) */
#define NFE_TAM_XMUN      60  /* xMun: nome do município (2 a 60) */
#define NFE_TAM_XPAIS     60  /* xPais: nome do país (2 a 60) */
#define NFE_TAM_IM        15  /* IM: inscrição municipal (1 a 15) */
#define NFE_TAM_EMAIL     60  /* email (1 a 60) */
#define NFE_TAM_CPROD     60  /* cProd: código do produto (1 a 60) */
#define NFE_TAM_CBARRA    30 /* cBarra/cBarraTrib: código de barras (3 a 30) */
#define NFE_TAM_XPROD     120 /* xProd: descrição do produto (1 a 120) */
#define NFE_TAM_CBENEF    10  /* cBenef: benefício fiscal (8 ou 10) */
#define NFE_TAM_UNIDADE   6   /* uCom/uTrib: unidade (1 a 6) */
#define NFE_TAM_XPED      15  /* xPed: número do pedido (1 a 15) */
#define NFE_TAM_INFADPROD 500 /* infAdProd: informações adicionais do item */

#endif
