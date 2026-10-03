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

#ifndef LIBNFE_NFE_H
#define LIBNFE_NFE_H

/* Data e hora (strftime) no formato AAAA-MM-DDThh:mm:ss; o fuso (TZD, ex.:
 * -03:00) é acrescentado depois, conforme nfe_tzd. */
#define NFE_FORMATO_DATA_HORA "%Y-%m-%dT%H:%M:%S"

/* cUF: código IBGE da Unidade Federativa */
typedef enum nfe_uf {
	/* Região Norte */
	NFE_UF_RO = 11, /* Rondônia */
	NFE_UF_AC = 12, /* Acre */
	NFE_UF_AM = 13, /* Amazonas */
	NFE_UF_RR = 14, /* Roraima */
	NFE_UF_PA = 15, /* Pará */
	NFE_UF_AP = 16, /* Amapá */
	NFE_UF_TO = 17, /* Tocantins */
	/* Região Nordeste */
	NFE_UF_MA = 21, /* Maranhão */
	NFE_UF_PI = 22, /* Piauí */
	NFE_UF_CE = 23, /* Ceará */
	NFE_UF_RN = 24, /* Rio Grande do Norte */
	NFE_UF_PB = 25, /* Paraíba */
	NFE_UF_PE = 26, /* Pernambuco */
	NFE_UF_AL = 27, /* Alagoas */
	NFE_UF_SE = 28, /* Sergipe */
	NFE_UF_BA = 29, /* Bahia */
	/* Região Sudeste */
	NFE_UF_MG = 31, /* Minas Gerais */
	NFE_UF_ES = 32, /* Espírito Santo */
	NFE_UF_RJ = 33, /* Rio de Janeiro */
	NFE_UF_SP = 35, /* São Paulo */
	/* Região Sul */
	NFE_UF_PR = 41, /* Paraná */
	NFE_UF_SC = 42, /* Santa Catarina */
	NFE_UF_RS = 43, /* Rio Grande do Sul */
	/* Região Centro-Oeste */
	NFE_UF_MS = 50, /* Mato Grosso do Sul */
	NFE_UF_MT = 51, /* Mato Grosso */
	NFE_UF_GO = 52, /* Goiás */
	NFE_UF_DF = 53  /* Distrito Federal */
} nfe_uf;

/* Mês, usado no campo AAMM das notas referenciadas */
typedef enum nfe_mes {
	NFE_MES_JANEIRO = 1,
	NFE_MES_FEVEREIRO = 2,
	NFE_MES_MARCO = 3,
	NFE_MES_ABRIL = 4,
	NFE_MES_MAIO = 5,
	NFE_MES_JUNHO = 6,
	NFE_MES_JULHO = 7,
	NFE_MES_AGOSTO = 8,
	NFE_MES_SETEMBRO = 9,
	NFE_MES_OUTUBRO = 10,
	NFE_MES_NOVEMBRO = 11,
	NFE_MES_DEZEMBRO = 12
} nfe_mes;

/* indPag: forma de pagamento (no leiaute 4.00 passa para o grupo pag, ver #43) */
typedef enum nfe_forma_pagamento {
	NFE_PAGAMENTO_AVISTA = 0,
	NFE_PAGAMENTO_PRAZO = 1,
	NFE_PAGAMENTO_OUTROS = 2
} nfe_forma_pagamento;

/* mod: modelo do documento fiscal */
typedef enum nfe_modelo {
	NFE_MODELO_NFE = 55,
	NFE_MODELO_NFCE = 65
} nfe_modelo;

/* tpNF: tipo de operação */
typedef enum nfe_tipo_operacao {
	NFE_OPERACAO_ENTRADA = 0,
	NFE_OPERACAO_SAIDA = 1
} nfe_tipo_operacao;

/* idDest: destino da operação */
typedef enum nfe_destino {
	NFE_DESTINO_INTERNO = 1,
	NFE_DESTINO_INTERESTADUAL = 2,
	NFE_DESTINO_EXTERIOR = 3
} nfe_destino;

/* tpImp: formato de impressão da DANFE */
typedef enum nfe_danfe {
	NFE_DANFE_SEM_GERAR = 0,
	NFE_DANFE_NORMAL_RETRATO = 1,
	NFE_DANFE_NORMAL_PAISAGEM = 2,
	NFE_DANFE_SIMPLIFICADA = 3,
	NFE_DANFE_NFCE = 4,
	NFE_DANFE_NFCE_MSG_ELETRONICA = 5 /* DANFE NFC-e em mensagem eletrônica */
} nfe_danfe;

/* tpEmis: tipo de emissão */
typedef enum nfe_emissao {
	/* Sem contingência */
	NFE_EMISSAO_NORMAL = 1,
	/* Contingência */
	NFE_EMISSAO_CONTINGENCIA_FSIA = 2,
	NFE_EMISSAO_CONTINGENCIA_SCAN = 3,
	NFE_EMISSAO_CONTINGENCIA_DPEC = 4,
	NFE_EMISSAO_CONTINGENCIA_FSDA = 5,
	NFE_EMISSAO_CONTINGENCIA_SVC_AN = 6,
	NFE_EMISSAO_CONTINGENCIA_SVC_RS = 7,
	NFE_EMISSAO_CONTINGENCIA_OFFLINE_NFCE = 9
} nfe_emissao;

/* tpAmb: ambiente de emissão */
typedef enum nfe_ambiente {
	NFE_AMBIENTE_PRODUCAO = 1,
	NFE_AMBIENTE_HOMOLOGACAO = 2
} nfe_ambiente;

/* finNFe: finalidade da emissão */
typedef enum nfe_finalidade {
	NFE_FINALIDADE_NORMAL = 1,
	NFE_FINALIDADE_COMPLEMENTAR = 2,
	NFE_FINALIDADE_AJUSTE = 3,
	NFE_FINALIDADE_DEVOLUCAO = 4
} nfe_finalidade;

/* indFinal: operação com consumidor final */
typedef enum nfe_consumidor {
	NFE_CONSUMIDOR_NORMAL = 0,
	NFE_CONSUMIDOR_FINAL = 1
} nfe_consumidor;

/* indPres: presença do comprador no momento da operação */
typedef enum nfe_presenca {
	NFE_PRESENCA_NAO_SE_APLICA = 0,
	NFE_PRESENCA_PRESENCIAL = 1,
	NFE_PRESENCA_INTERNET = 2,
	NFE_PRESENCA_TELEATENDIMENTO = 3,
	NFE_PRESENCA_ENTREGA_DOMICILIO = 4, /* NFC-e */
	NFE_PRESENCA_OUTROS = 9
} nfe_presenca;

/* procEmis: processo de emissão */
typedef enum nfe_processo_emissao {
	NFE_PROCESSO_APP_CONTRIBUINTE = 0,
	NFE_PROCESSO_AVULSA_FISCO = 1,
	NFE_PROCESSO_AVULSA_SITE_FISCO = 2,
	NFE_PROCESSO_APP_FISCO = 3
} nfe_processo_emissao;

/**
 * nfe_tzd:
 * @NFE_TZD_FERNANDO_NORONHA: horário de Fernando de Noronha (UTC-02:00)
 * @NFE_TZD_BRASILIA: horário oficial de Brasília (UTC-03:00)
 * @NFE_TZD_MANAUS: horário do Amazonas e demais estados em UTC-04:00
 * @NFE_TZD_ACRE: horário do Acre e do extremo oeste do Amazonas (UTC-05:00)
 *
 * Fuso horário (TZD) das datas no formato AAAA-MM-DDThh:mm:ssTZD.
 * Ex.: 2010-08-19T13:00:15-03:00.
 *
 * O horário de verão foi extinto no Brasil em 2019 (Decreto 9.772/2019).
 */
typedef enum nfe_tzd {
	NFE_TZD_FERNANDO_NORONHA = -2,
	NFE_TZD_BRASILIA = -3,
	NFE_TZD_MANAUS = -4,
	NFE_TZD_ACRE = -5
} nfe_tzd;

#endif
