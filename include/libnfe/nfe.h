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

enum COD_UF_E {
	/* Regiao Norte*/
	UF_RONDONIA = 11,
	UF_ACRE = 12,
	UF_AMAZONAS = 13,
	UF_RORAIMA = 14,
	UF_PARA = 15,
	UF_AMAPA = 16,
	UF_TOCANTIS = 17,
	/*Regiao Nordeste*/
	UF_MARANHAO = 21,
	UF_PIAUI = 22,
	UF_CEARA = 23,
	UF_RIO_GRANDE_N = 24,
	UF_PARAIBA = 25,
	UF_PERNAMBUCO = 26,
	UF_ALAGOAS = 27,
	UF_SERGIPE = 28,
	UF_BAHIA = 29,
	/*Regiao Sudeste*/
	UF_MINAS_GERAIS = 31,
	UF_ESPIRITO_SANTO = 32,
	UF_RIO_JANEIRO = 33,
	UF_SAO_PAULO = 35,
	/*Regiao Sul*/
	UF_PARANA = 41,
	UF_SANTA_CATARINA = 42,
	UF_RIO_GRANDE_S = 43,
	/*Regiao Centro Oeste*/
	UF_MATO_GROSSO_S = 50,
	UF_MATO_GROSSO = 51,
	UF_GOIAS = 52,
	UF_DISTRITO_FEDERAL = 53
};

enum FORMA_PAGAMENTO_E {
	PAGAMENTO_AVISTA = 0,
	PAGAMENTO_PRAZO = 1,
	PAGAMENTO_OUTROS = 2
};

/*TODO:CABE REGRA*/
enum DOCUMENTO_MODELO_E {
	MODELO_NFE = 55,
	MODELO_NFCE = 65
};

enum TIPO_OPERACAO_E {
	OPERACAO_ENTRADA = 0,
	OPERACAO_SAIDA = 1
};

/*TODO:CABE REGRA*/
enum OPERACAO_DESTINO_E {
	DESTINO_INTERNO = 1,
	DESTINO_INTERESTADUAL = 2,
	DESTINO_EXTERIOR = 3
};

/*TODO:CABE REGRA*/
enum FORMATO_DANFE_E{
	DANFE_SEM_GERAR = 0,
	DANFE_NORMAL_RETRATO = 1,
	DANFE_NORMAL_PAISAGEM = 2,
	DANFE_SIMPLIFICADA = 3,
	DANFE_NFCE = 4,
	DANFE_NFCE_ELETRONICA = 5
};


/*TODO:CABE REGRA*/
enum TIPO_EMISSAO_E{
	/*Nao ha contigencia*/
	EMISSAO_NORMAL = 1,
	/*Ha contigencia*/
	EMISSAO_CONTIGENCIA_FSIA = 2,
	EMISSAO_CONTIGENCIA_SCAN = 3,
	EMISSAO_CONTIGENCIA_DPEC = 4,
	EMISSAO_CONTIGENCIA_FSDA = 5,
	EMISSAO_CONTIGENCIA_SVC_AN = 6,
	EMISSAO_CONTIGENCIA_SVC_RS = 7,
	EMISSAO_CONTIGENCIA_OFFLINE_NFCE = 9
};

enum EMISSAO_AMBIENTE_E{
	AMBIENTE_PRODUCAO = 1,
	AMBIENTE_HOMOLOGACAO = 2
};

enum FINALIDADE_NFE_E{
	NFE_NORMAL = 1,
	NFE_COMPLEMENTAR = 2,
	NFE_AJUSTE = 3,
	NFE_DEVOLUCAO = 4
};

enum OPERACAO_CONSUMIDOR_E{
	CONSUMIDOR_NOMAL = 0,
	CONSUMIDOR_FINAL = 1
};

enum PRESENCA_COMPRADOR_E{
	OPERACAO_NAOAPLICA = 0,
	OPERACAO_PRESENCIAL = 1,
	OPERACAO_NPRESENCIAL_INTERNET = 2,
	OPERACAO_NPRESENCIAL_TELE = 3,
	OPERACAO_NFCE_ENTREGADOMICILIAR = 4,
	OPERACAO_NPRESENCIAL_OUTROS = 9
};
enum PROCESSO_EMISSAO_E{
	EMISSAO_CONTRIBUINTE_APPPROPRIO = 0,
	EMISSAO_AVULSA_FISCO = 1,
	EMISSAO_AVULSA_SITEFISCO = 2,
	EMISSAO_CONTRIBUINTE_APPFISCO = 3
};

		







