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

/* gmtime_r (POSIX) */
#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/chave.h>
#include <libnfe/cnpjcpf.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/esquema.h>
#include <libnfe/ide.h>
#include <libnfe/refNF.h>
#include <libnfe/refNFe.h>
#include <libnfe/utils.h>
#include <libnfe/valida.h>

/* Documento referenciado: um item do grupo NFref */
enum tipo_ref_e {
	REF_NFE,  /* refNFe: NF-e ou NFC-e, pela chave de acesso */
	REF_NF,   /* refNF: nota fiscal modelo 1/1A */
	REF_GRUPO /* grupo genérico NFref (refNFP, refECF, refCTe...) */
};

struct ref_s {
	enum tipo_ref_e tipo;
	union {
		struct refNFe_s *nfe;
		struct refNF_s *nf;
		nfe_grupo *g;
	} doc;
	struct ref_s *prox;
};

struct nfe_ide {
	nfe_uf cUF;
	uint32_t cNF;
	char natOp[NFE_TAM_UTF8(NFE_TAM_NATOP)];
	nfe_modelo mod;
	unsigned serie;
	uint32_t nNF;
	time_t dhEmi;
	time_t dhSaiEnt;
	time_t dPrevEntrega; /* só a data é escrita */
	nfe_tipo_operacao tpNF;
	nfe_destino idDest;
	uint32_t cMunFG;
	uint32_t cMunFGIBS; /* 0: não informado */
	nfe_danfe tpImp;
	nfe_emissao tpEmis;
	unsigned cDV;
	nfe_ambiente tpAmb;
	nfe_finalidade finNFe;
	nfe_tipo_debito tpNFDebito;   /* 0: não informado */
	nfe_tipo_credito tpNFCredito; /* 0: não informado */
	nfe_consumidor indFinal;
	nfe_presenca indPres;
	nfe_intermediador indIntermed;
	char cIndOp[NFE_TAM_ASCII(6)]; /* "": não informado */
	nfe_processo_emissao procEmi;
	char verProc[NFE_TAM_UTF8(NFE_TAM_VERPROC)];
	nfe_tzd tzd;      /* fuso em que as datas são escritas */
	int contingencia; /* dhCont e xJust informados */
	time_t dhCont;
	char xJust[NFE_TAM_UTF8(NFE_TAM_XJUST)];
	struct ref_s *refs; /* NFref, na ordem de inclusão */
	struct ref_s *refsFim;
	int nRefs;
	/* gCompraGov: tpEnteGov 0 = grupo não informado */
	nfe_ente_gov tpEnteGov;
	char pRedutor[NFE_TAM_ASCII(8)]; /* até "100.0000" */
	nfe_oper_gov tpOperGov;
	char refDFeAnt[NFE_MAX_REF_RTC][NFE_TAM_ASCII(NFE_TAM_CHAVE)];
	int nRefDFeAnt;
	/* gPagAntecipado: nPagAntecipado 0 = grupo não informado */
	char pagAntecipado[NFE_MAX_REF_RTC][NFE_TAM_ASCII(NFE_TAM_CHAVE)];
	int nPagAntecipado;
};

/* Escreve em dst (tam bytes) o instante t no formato AAAA-MM-DDThh:mm:ssTZD,
 * no fuso tzd. Ex.: 2010-08-19T13:00:15-03:00.
 * Não há horário de verão no Brasil desde 2019.
 * Retorna 0, E_VALOR (fuso inválido) ou E_TAMANHO (buffer pequeno). */
int nfe_data_hora(char *dst, size_t tam, time_t t, nfe_tzd tzd)
{
	struct tm tm;
	time_t local;
	size_t n;
	int r;

	switch (tzd) {
	case NFE_TZD_FERNANDO_NORONHA:
	case NFE_TZD_BRASILIA:
	case NFE_TZD_MANAUS:
	case NFE_TZD_ACRE:
		break;
	default:
		return E_VALOR;
	}

	/* Hora local = UTC deslocado pelo fuso; gmtime_r não depende do fuso
	 * configurado na máquina */
	local = t + (time_t)tzd * 3600;
	if (!gmtime_r(&local, &tm))
		return E_VALOR;

	n = strftime(dst, tam, NFE_FORMATO_DATA_HORA, &tm);
	if (n == 0)
		return E_TAMANHO;

	r = snprintf(dst + n, tam - n, "-%02d:00", -(int)tzd);
	if (r < 0 || (size_t)r >= tam - n)
		return E_TAMANHO;

	return 0;
}

nfe_ide *nfe_ide_new(void)
{
	nfe_ide *ide = (nfe_ide *)calloc(1, sizeof(nfe_ide));
	if (!ide)
		return NULL;
	ide->mod = NFE_MODELO_NFE;
	ide->dhEmi = NFE_SEM_DATA;
	ide->dhSaiEnt = NFE_SEM_DATA;
	ide->dPrevEntrega = NFE_SEM_DATA;
	ide->tpNF = NFE_OPERACAO_SAIDA;
	ide->idDest = NFE_DESTINO_INTERNO;
	ide->tpImp = NFE_DANFE_NORMAL_RETRATO;
	ide->tpEmis = NFE_EMISSAO_NORMAL;
	ide->tpAmb = NFE_AMBIENTE_HOMOLOGACAO;
	ide->finNFe = NFE_FINALIDADE_NORMAL;
	ide->indFinal = NFE_CONSUMIDOR_NORMAL;
	ide->indPres = NFE_PRESENCA_NAO_SE_APLICA;
	ide->indIntermed = NFE_INTERMEDIADOR_NAO_INFORMADO;
	ide->procEmi = NFE_PROCESSO_APP_CONTRIBUINTE;
	ide->tzd = NFE_TZD_BRASILIA;
	return ide;
}

void nfe_ide_free(nfe_ide *ide)
{
	if (!ide)
		return;

	while (ide->refs) {
		struct ref_s *prox = ide->refs->prox;
		if (ide->refs->tipo == REF_NFE)
			RefNFeDel(ide->refs->doc.nfe);
		else if (ide->refs->tipo == REF_NF)
			RefNFDel(ide->refs->doc.nf);
		else
			nfe_grupo_free(ide->refs->doc.g);
		free(ide->refs);
		ide->refs = prox;
	}

	free(ide);
}

/* Setters: validam e só então gravam o campo */
#define EXIGE_IDE(ide)                                                         \
	do {                                                                   \
		if (!(ide))                                                    \
			return E_ISNULL;                                       \
	} while (0)
#define EXIGE(cond)                                                            \
	do {                                                                   \
		if (!(cond))                                                   \
			return E_VALOR;                                        \
	} while (0)

int nfe_ide_set_cuf(nfe_ide *ide, nfe_uf cuf)
{
	EXIGE_IDE(ide);
	switch (cuf) {
	case NFE_UF_RO:
	case NFE_UF_AC:
	case NFE_UF_AM:
	case NFE_UF_RR:
	case NFE_UF_PA:
	case NFE_UF_AP:
	case NFE_UF_TO:
	case NFE_UF_MA:
	case NFE_UF_PI:
	case NFE_UF_CE:
	case NFE_UF_RN:
	case NFE_UF_PB:
	case NFE_UF_PE:
	case NFE_UF_AL:
	case NFE_UF_SE:
	case NFE_UF_BA:
	case NFE_UF_MG:
	case NFE_UF_ES:
	case NFE_UF_RJ:
	case NFE_UF_SP:
	case NFE_UF_PR:
	case NFE_UF_SC:
	case NFE_UF_RS:
	case NFE_UF_MS:
	case NFE_UF_MT:
	case NFE_UF_GO:
	case NFE_UF_DF:
		ide->cUF = cuf;
		return 0;
	}
	return E_VALOR;
}

int nfe_ide_set_cnf(nfe_ide *ide, uint32_t cnf)
{
	EXIGE_IDE(ide);
	EXIGE(cnf <= 99999999u);
	ide->cNF = cnf;
	return 0;
}

int nfe_ide_set_natop(nfe_ide *ide, const char *natop)
{
	EXIGE_IDE(ide);
	return nfe_copia_texto_validado(ide->natOp, sizeof ide->natOp, natop, 1,
	                                NFE_TAM_NATOP);
}

int nfe_ide_set_mod(nfe_ide *ide, nfe_modelo mod)
{
	EXIGE_IDE(ide);
	EXIGE(mod == NFE_MODELO_NFE || mod == NFE_MODELO_NFCE);
	ide->mod = mod;
	return 0;
}

int nfe_ide_set_serie(nfe_ide *ide, unsigned serie)
{
	EXIGE_IDE(ide);
	EXIGE(serie <= 999u);
	ide->serie = serie;
	return 0;
}

int nfe_ide_set_nnf(nfe_ide *ide, uint32_t nnf)
{
	EXIGE_IDE(ide);
	EXIGE(nnf >= 1u && nnf <= 999999999u);
	ide->nNF = nnf;
	return 0;
}

int nfe_ide_set_dhemi(nfe_ide *ide, time_t dhemi)
{
	EXIGE_IDE(ide);
	EXIGE(dhemi != NFE_SEM_DATA);
	ide->dhEmi = dhemi;
	return 0;
}

int nfe_ide_set_dhsaient(nfe_ide *ide, time_t dhsaient)
{
	EXIGE_IDE(ide);
	ide->dhSaiEnt = dhsaient;
	return 0;
}

int nfe_ide_set_tpnf(nfe_ide *ide, nfe_tipo_operacao tpnf)
{
	EXIGE_IDE(ide);
	EXIGE(tpnf == NFE_OPERACAO_ENTRADA || tpnf == NFE_OPERACAO_SAIDA);
	ide->tpNF = tpnf;
	return 0;
}

int nfe_ide_set_iddest(nfe_ide *ide, nfe_destino iddest)
{
	EXIGE_IDE(ide);
	EXIGE(iddest >= NFE_DESTINO_INTERNO && iddest <= NFE_DESTINO_EXTERIOR);
	ide->idDest = iddest;
	return 0;
}

int nfe_ide_set_cmunfg(nfe_ide *ide, uint32_t cmunfg)
{
	EXIGE_IDE(ide);
	EXIGE(cmunfg >= 1000000u && cmunfg <= 9999999u);
	ide->cMunFG = cmunfg;
	return 0;
}

int nfe_ide_set_tpimp(nfe_ide *ide, nfe_danfe tpimp)
{
	EXIGE_IDE(ide);
	EXIGE(tpimp >= NFE_DANFE_SEM_GERAR &&
	      tpimp <= NFE_DANFE_SIMPLIFICADA_TIPO2);
	ide->tpImp = tpimp;
	return 0;
}

int nfe_ide_set_tpemis(nfe_ide *ide, nfe_emissao tpemis)
{
	EXIGE_IDE(ide);
	EXIGE((tpemis >= NFE_EMISSAO_NORMAL &&
	       tpemis <= NFE_EMISSAO_CONTINGENCIA_SVC_RS) ||
	      tpemis == NFE_EMISSAO_CONTINGENCIA_OFFLINE_NFCE);
	ide->tpEmis = tpemis;
	return 0;
}

int nfe_ide_set_cdv(nfe_ide *ide, unsigned cdv)
{
	EXIGE_IDE(ide);
	EXIGE(cdv <= 9u);
	ide->cDV = cdv;
	return 0;
}

int nfe_ide_set_tpamb(nfe_ide *ide, nfe_ambiente tpamb)
{
	EXIGE_IDE(ide);
	EXIGE(tpamb == NFE_AMBIENTE_PRODUCAO ||
	      tpamb == NFE_AMBIENTE_HOMOLOGACAO);
	ide->tpAmb = tpamb;
	return 0;
}

int nfe_ide_set_finnfe(nfe_ide *ide, nfe_finalidade finnfe)
{
	EXIGE_IDE(ide);
	EXIGE(finnfe >= NFE_FINALIDADE_NORMAL &&
	      finnfe <= NFE_FINALIDADE_DEBITO);
	ide->finNFe = finnfe;
	return 0;
}

int nfe_ide_set_indfinal(nfe_ide *ide, nfe_consumidor indfinal)
{
	EXIGE_IDE(ide);
	EXIGE(indfinal == NFE_CONSUMIDOR_NORMAL ||
	      indfinal == NFE_CONSUMIDOR_FINAL);
	ide->indFinal = indfinal;
	return 0;
}

int nfe_ide_set_indpres(nfe_ide *ide, nfe_presenca indpres)
{
	EXIGE_IDE(ide);
	EXIGE((indpres >= NFE_PRESENCA_NAO_SE_APLICA &&
	       indpres <= NFE_PRESENCA_PRESENCIAL_FORA) ||
	      indpres == NFE_PRESENCA_OUTROS);
	ide->indPres = indpres;
	return 0;
}

int nfe_ide_set_procemi(nfe_ide *ide, nfe_processo_emissao procemi)
{
	EXIGE_IDE(ide);
	EXIGE(procemi >= NFE_PROCESSO_APP_CONTRIBUINTE &&
	      procemi <= NFE_PROCESSO_PAA);
	ide->procEmi = procemi;
	return 0;
}

int nfe_ide_set_verproc(nfe_ide *ide, const char *verproc)
{
	EXIGE_IDE(ide);
	return nfe_copia_texto_validado(ide->verProc, sizeof ide->verProc,
	                                verproc, 1, NFE_TAM_VERPROC);
}

int nfe_ide_set_dpreventrega(nfe_ide *ide, time_t dpreventrega)
{
	EXIGE_IDE(ide);
	ide->dPrevEntrega = dpreventrega;
	return 0;
}

int nfe_ide_set_cmunfgibs(nfe_ide *ide, uint32_t cmunfgibs)
{
	EXIGE_IDE(ide);
	EXIGE(cmunfgibs == 0 ||
	      (cmunfgibs >= 1000000u && cmunfgibs <= 9999999u));
	ide->cMunFGIBS = cmunfgibs;
	return 0;
}

int nfe_ide_set_tpnfdebito(nfe_ide *ide, nfe_tipo_debito tpnfdebito)
{
	EXIGE_IDE(ide);
	EXIGE(tpnfdebito >= NFE_DEBITO_NAO_INFORMADO &&
	      tpnfdebito <= NFE_DEBITO_DESENQUADRAMENTO_SN);
	ide->tpNFDebito = tpnfdebito;
	return 0;
}

int nfe_ide_set_tpnfcredito(nfe_ide *ide, nfe_tipo_credito tpnfcredito)
{
	EXIGE_IDE(ide);
	EXIGE(tpnfcredito >= NFE_CREDITO_NAO_INFORMADO &&
	      tpnfcredito <= NFE_CREDITO_RETORNO_RECUSA_PARCIAL);
	ide->tpNFCredito = tpnfcredito;
	return 0;
}

int nfe_ide_set_indintermed(nfe_ide *ide, nfe_intermediador indintermed)
{
	EXIGE_IDE(ide);
	EXIGE(indintermed >= NFE_INTERMEDIADOR_NAO_INFORMADO &&
	      indintermed <= NFE_INTERMEDIADOR_TERCEIROS);
	ide->indIntermed = indintermed;
	return 0;
}

int nfe_ide_set_cindop(nfe_ide *ide, const char *cindop)
{
	EXIGE_IDE(ide);
	if (!cindop) {
		ide->cIndOp[0] = '\0';
		return 0;
	}
	return nfe_copia_padrao(ide->cIndOp, sizeof ide->cIndOp, cindop,
	                        "[0-9]{6}");
}

int nfe_ide_set_tzd(nfe_ide *ide, nfe_tzd tzd)
{
	char teste[NFE_TAM_ASCII(NFE_TAM_DATA_HORA)];

	EXIGE_IDE(ide);
	/* nfe_data_hora recusa fusos desconhecidos */
	EXIGE(nfe_data_hora(teste, sizeof teste, 0, tzd) == 0);
	ide->tzd = tzd;
	return 0;
}

int nfe_ide_set_contingencia(nfe_ide *ide, time_t dhcont, const char *xjust)
{
	int rc;

	EXIGE_IDE(ide);
	EXIGE(dhcont != NFE_SEM_DATA);
	rc = nfe_copia_texto_validado(ide->xJust, sizeof ide->xJust, xjust, 15,
	                              NFE_TAM_XJUST);
	if (rc != 0)
		return rc;
	ide->dhCont = dhcont;
	ide->contingencia = 1;
	return 0;
}

int nfe_ide_gerar_chave(nfe_ide *ide, const char *cnpjcpf, char *chave,
                        size_t tam)
{
	char doc[NFE_TAM_ASCII(NFE_TAM_CNPJ)];
	struct tm tm;
	time_t local;
	size_t n;
	int dv, rc;

	if (!ide || !cnpjcpf || !chave)
		return E_ISNULL;
	if (tam < NFE_TAM_ASCII(NFE_TAM_CHAVE))
		return E_TAMANHO;
	if (ide->cUF == 0 || ide->nNF == 0 || ide->dhEmi == NFE_SEM_DATA)
		return E_VALOR;

	/* CNPJ (numérico ou alfanumérico) com 14 posições; CPF com 11 dígitos,
	 * completado com zeros à esquerda */
	n = strlen(cnpjcpf);
	if (n == NFE_TAM_CNPJ)
		rc = nfe_cnpj_validar(cnpjcpf);
	else if (n == NFE_TAM_CPF)
		rc = nfe_cpf_validar(cnpjcpf);
	else
		return E_TAMANHO;
	if (rc != 0)
		return rc;
	memset(doc, '0', NFE_TAM_CNPJ);
	memcpy(doc + NFE_TAM_CNPJ - n, cnpjcpf, n);
	doc[NFE_TAM_CNPJ] = '\0';

	/* AAMM: ano e mês da emissão, no fuso do ide */
	local = ide->dhEmi + (time_t)ide->tzd * 3600;
	if (!gmtime_r(&local, &tm))
		return E_VALOR;

	/* 43 dígitos; o 44º é o verificador */
	snprintf(chave, tam, "%02d%02d%02d%s%02d%03u%09" PRIu32 "%d%08" PRIu32,
	         (int)ide->cUF, tm.tm_year % 100, tm.tm_mon + 1, doc,
	         (int)ide->mod, ide->serie, ide->nNF, (int)ide->tpEmis,
	         ide->cNF);
	dv = nfe_chave_dv(chave);
	if (dv < 0)
		return dv;
	chave[NFE_TAM_CHAVE - 1] = (char)('0' + dv);
	chave[NFE_TAM_CHAVE] = '\0';
	ide->cDV = (unsigned)dv;
	return 0;
}

/* Acrescenta uma referência ao fim da lista */
static int nfe_ide_add_ref(nfe_ide *ide, enum tipo_ref_e tipo, void *doc)
{
	struct ref_s *novo;

	if (!ide || !doc)
		return E_ISNULL;
	if (ide->nRefs >= NFE_MAX_NFREF)
		return E_VALOR;
	novo = (struct ref_s *)malloc(sizeof(struct ref_s));
	if (!novo)
		return E_MALLOC;
	novo->tipo = tipo;
	if (tipo == REF_NFE)
		novo->doc.nfe = (struct refNFe_s *)doc;
	else if (tipo == REF_NF)
		novo->doc.nf = (struct refNF_s *)doc;
	else
		novo->doc.g = (nfe_grupo *)doc;
	novo->prox = NULL;
	if (ide->refsFim)
		ide->refsFim->prox = novo;
	else
		ide->refs = novo;
	ide->refsFim = novo;
	ide->nRefs++;
	return 0;
}

int nfe_ide_add_refnfe(nfe_ide *ide, struct refNFe_s *ref)
{
	return nfe_ide_add_ref(ide, REF_NFE, ref);
}

int nfe_ide_add_refnf(nfe_ide *ide, struct refNF_s *ref)
{
	return nfe_ide_add_ref(ide, REF_NF, ref);
}

int nfe_ide_add_nfref(nfe_ide *ide, nfe_grupo **nfref)
{
	nfe_grupo *g;
	int rc;

	if (!ide || !nfref)
		return E_ISNULL;
	g = nfe_grupo_new(&esq_NFref);
	if (!g)
		return E_MALLOC;
	rc = nfe_ide_add_ref(ide, REF_GRUPO, g);
	if (rc != 0) {
		nfe_grupo_free(g);
		return rc;
	}
	*nfref = g;
	return 0;
}

int nfe_ide_set_compragov(nfe_ide *ide, nfe_ente_gov tpentegov,
                          const char *predutor, nfe_oper_gov tpopergov)
{
	char red[sizeof ide->pRedutor];
	int rc;

	EXIGE_IDE(ide);
	if (tpentegov == NFE_ENTE_GOV_NAO_INFORMADO) {
		ide->tpEnteGov = NFE_ENTE_GOV_NAO_INFORMADO;
		ide->nRefDFeAnt = 0;
		return 0;
	}
	EXIGE(tpentegov >= NFE_ENTE_GOV_UNIAO &&
	      tpentegov <= NFE_ENTE_GOV_COMITE_GESTOR_IBS);
	EXIGE(tpopergov >= NFE_OPER_GOV_FORNECIMENTO_PAGAMENTO_POSTERIOR &&
	      tpopergov <= NFE_OPER_GOV_PAGAMENTO_FORNECIMENTO_POSTERIOR);
	/* o padrão limita o valor a 8 caracteres, que sempre cabem em red */
	rc = nfe_copia_padrao(red, sizeof red, predutor,
	                      NFE_PADRAO_TDec_0302_04RTC);
	if (rc != 0)
		return rc;
	memcpy(ide->pRedutor, red, sizeof red);
	ide->tpEnteGov = tpentegov;
	ide->tpOperGov = tpopergov;
	return 0;
}

/* Valida a chave e a copia para a próxima posição livre de lista */
static int add_chave(char lista[][NFE_TAM_ASCII(NFE_TAM_CHAVE)], int *n,
                     const char *chave)
{
	int rc;

	if (!chave)
		return E_ISNULL;
	if (*n >= NFE_MAX_REF_RTC)
		return E_VALOR;
	rc = nfe_chave_validar(chave);
	if (rc != 0)
		return rc;
	memcpy(lista[*n], chave, NFE_TAM_ASCII(NFE_TAM_CHAVE));
	(*n)++;
	return 0;
}

int nfe_ide_add_compragov_refdfeant(nfe_ide *ide, const char *chave)
{
	EXIGE_IDE(ide);
	EXIGE(ide->tpEnteGov != NFE_ENTE_GOV_NAO_INFORMADO);
	return add_chave(ide->refDFeAnt, &ide->nRefDFeAnt, chave);
}

int nfe_ide_add_pagantecipado(nfe_ide *ide, const char *refnfe)
{
	EXIGE_IDE(ide);
	return add_chave(ide->pagAntecipado, &ide->nPagAntecipado, refnfe);
}

int nfe_ide_remove_pagantecipado(nfe_ide *ide)
{
	EXIGE_IDE(ide);
	ide->nPagAntecipado = 0;
	return 0;
}

/* Escreve uma data no fuso do ide */
static int escreve_data(xmlTextWriterPtr writer, const char *tag, time_t t,
                        nfe_tzd tzd)
{
	char dh[NFE_TAM_ASCII(NFE_TAM_DATA_HORA)];
	int rc = nfe_data_hora(dh, sizeof dh, t, tzd);

	if (rc != 0)
		return rc;
	return nfe_escreve(writer, tag, "%s", dh);
}

/* Escreve só a data (AAAA-MM-DD) de t, no fuso do ide */
static int escreve_dia(xmlTextWriterPtr writer, const char *tag, time_t t,
                       nfe_tzd tzd)
{
	char dia[NFE_TAM_ASCII(10)];
	struct tm tm;
	time_t local = t + (time_t)tzd * 3600;

	if (!gmtime_r(&local, &tm))
		return E_VALOR;
	if (strftime(dia, sizeof dia, "%Y-%m-%d", &tm) == 0)
		return E_TAMANHO;
	return nfe_escreve(writer, tag, "%s", dia);
}

#define ESCREVE_DATA(tag, t)                                                   \
	do {                                                                   \
		rc = escreve_data(writer, (tag), (t), ide->tzd);               \
		if (rc != 0)                                                   \
			return rc;                                             \
	} while (0)

int nfe_ide_write_xml(xmlTextWriterPtr writer, const nfe_ide *ide)
{
	const struct ref_s *ref;
	int i, rc;

	if (!writer || !ide)
		return E_ISNULL;

	/* Campos obrigatórios sem valor padrão */
	if (ide->cUF == 0 || ide->natOp[0] == '\0' || ide->nNF == 0 ||
	    ide->dhEmi == NFE_SEM_DATA || ide->cMunFG == 0 ||
	    ide->verProc[0] == '\0')
		return E_VALOR;

	/* Chaves anteriores de gCompraGov: exatamente uma no tpOperGov 2, uma
	 * ou mais no 3 e nenhuma no 1 e no 4 */
	if (ide->tpEnteGov != NFE_ENTE_GOV_NAO_INFORMADO) {
		switch (ide->tpOperGov) {
		case NFE_OPER_GOV_PAGAMENTO_FORNECIMENTO_REALIZADO:
			EXIGE(ide->nRefDFeAnt == 1);
			break;
		case NFE_OPER_GOV_FORNECIMENTO_PAGAMENTO_REALIZADO:
			EXIGE(ide->nRefDFeAnt >= 1);
			break;
		default:
			EXIGE(ide->nRefDFeAnt == 0);
		}
	}

	if (xmlTextWriterStartElement(writer, BAD_CAST "ide") < 0)
		return E_XML;

	NFE_ESCREVE("cUF", "%02d", (int)ide->cUF);
	NFE_ESCREVE("cNF", "%08" PRIu32, ide->cNF);
	NFE_ESCREVE("natOp", "%s", ide->natOp);
	NFE_ESCREVE("mod", "%02d", (int)ide->mod);
	NFE_ESCREVE("serie", "%u", ide->serie);
	NFE_ESCREVE("nNF", "%" PRIu32, ide->nNF);
	ESCREVE_DATA("dhEmi", ide->dhEmi);
	if (ide->dhSaiEnt != NFE_SEM_DATA) /* opcional */
		ESCREVE_DATA("dhSaiEnt", ide->dhSaiEnt);
	if (ide->dPrevEntrega != NFE_SEM_DATA) {
		rc = escreve_dia(writer, "dPrevEntrega", ide->dPrevEntrega,
		                 ide->tzd);
		if (rc != 0)
			return rc;
	}
	NFE_ESCREVE("tpNF", "%d", (int)ide->tpNF);
	NFE_ESCREVE("idDest", "%d", (int)ide->idDest);
	NFE_ESCREVE("cMunFG", "%07" PRIu32, ide->cMunFG);
	if (ide->cMunFGIBS != 0)
		NFE_ESCREVE("cMunFGIBS", "%07" PRIu32, ide->cMunFGIBS);
	NFE_ESCREVE("tpImp", "%d", (int)ide->tpImp);
	NFE_ESCREVE("tpEmis", "%d", (int)ide->tpEmis);
	NFE_ESCREVE("cDV", "%u", ide->cDV);
	NFE_ESCREVE("tpAmb", "%d", (int)ide->tpAmb);
	NFE_ESCREVE("finNFe", "%d", (int)ide->finNFe);
	if (ide->tpNFDebito != NFE_DEBITO_NAO_INFORMADO)
		NFE_ESCREVE("tpNFDebito", "%02d", (int)ide->tpNFDebito);
	if (ide->tpNFCredito != NFE_CREDITO_NAO_INFORMADO)
		NFE_ESCREVE("tpNFCredito", "%02d", (int)ide->tpNFCredito);
	NFE_ESCREVE("indFinal", "%d", (int)ide->indFinal);
	NFE_ESCREVE("indPres", "%d", (int)ide->indPres);
	if (ide->indIntermed != NFE_INTERMEDIADOR_NAO_INFORMADO)
		NFE_ESCREVE("indIntermed", "%d", (int)ide->indIntermed);
	if (ide->cIndOp[0] != '\0')
		NFE_ESCREVE("cIndOp", "%s", ide->cIndOp);
	NFE_ESCREVE("procEmi", "%d", (int)ide->procEmi);
	NFE_ESCREVE("verProc", "%s", ide->verProc);

	/* dhCont e xJust são filhos diretos de <ide>, logo após verProc */
	if (ide->contingencia) {
		ESCREVE_DATA("dhCont", ide->dhCont);
		NFE_ESCREVE("xJust", "%s", ide->xJust);
	}

	/* NFref: um grupo para cada documento referenciado */
	for (ref = ide->refs; ref; ref = ref->prox) {
		if (ref->tipo == REF_GRUPO) {
			rc = nfe_grupo_write_xml(writer, ref->doc.g);
			if (rc != 0)
				return rc;
			continue;
		}
		if (xmlTextWriterStartElement(writer, BAD_CAST "NFref") < 0)
			return E_XML;
		if (ref->tipo == REF_NFE)
			rc = xmlGenRefNFeNode(writer, ref->doc.nfe);
		else
			rc = xmlGenRefNFNode(writer, ref->doc.nf);
		if (rc < 0)
			return rc;
		if (xmlTextWriterEndElement(writer) < 0)
			return E_XML;
	}

	if (ide->tpEnteGov != NFE_ENTE_GOV_NAO_INFORMADO) {
		if (xmlTextWriterStartElement(writer, BAD_CAST "gCompraGov") <
		    0)
			return E_XML;
		NFE_ESCREVE("tpEnteGov", "%d", (int)ide->tpEnteGov);
		NFE_ESCREVE("pRedutor", "%s", ide->pRedutor);
		NFE_ESCREVE("tpOperGov", "%d", (int)ide->tpOperGov);
		for (i = 0; i < ide->nRefDFeAnt; i++)
			NFE_ESCREVE("refDFeAnt", "%s", ide->refDFeAnt[i]);
		if (xmlTextWriterEndElement(writer) < 0)
			return E_XML;
	}

	if (ide->nPagAntecipado > 0) {
		if (xmlTextWriterStartElement(writer,
		                              BAD_CAST "gPagAntecipado") < 0)
			return E_XML;
		for (i = 0; i < ide->nPagAntecipado; i++)
			NFE_ESCREVE("refNFe", "%s", ide->pagAntecipado[i]);
		if (xmlTextWriterEndElement(writer) < 0)
			return E_XML;
	}

	if (xmlTextWriterEndElement(writer) < 0)
		return E_XML;

	return 0;
}
