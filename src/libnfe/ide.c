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
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/ide.h>
#include <libnfe/refNF.h>
#include <libnfe/refNFe.h>
#include <libnfe/utils.h>

/* Documento referenciado: um item do grupo NFref */
enum tipo_ref_e {
  REF_NFE,  /* refNFe: NF-e ou NFC-e, pela chave de acesso */
  REF_NF    /* refNF: nota fiscal modelo 1/1A */
};

struct ref_s {
  enum tipo_ref_e tipo;
  union {
    struct refNFe_s *nfe;
    struct refNF_s *nf;
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
  nfe_tipo_operacao tpNF;
  nfe_destino idDest;
  uint32_t cMunFG;
  nfe_danfe tpImp;
  nfe_emissao tpEmis;
  unsigned cDV;
  nfe_ambiente tpAmb;
  nfe_finalidade finNFe;
  nfe_consumidor indFinal;
  nfe_presenca indPres;
  nfe_processo_emissao procEmi;
  char verProc[NFE_TAM_UTF8(NFE_TAM_VERPROC)];
  nfe_tzd tzd;                       /* fuso em que as datas são escritas */
  int contingencia;                  /* dhCont e xJust informados */
  time_t dhCont;
  char xJust[NFE_TAM_UTF8(NFE_TAM_XJUST)];
  struct ref_s *refs;                /* NFref, na ordem de inclusão */
  struct ref_s *refsFim;
  int nRefs;
};

/* Escreve em dst (tam bytes) o instante t no formato AAAA-MM-DDThh:mm:ssTZD,
 * no fuso tzd. Ex.: 2010-08-19T13:00:15-03:00.
 * Não há horário de verão no Brasil desde 2019.
 * Retorna 0, E_VALOR (fuso inválido) ou E_TAMANHO (buffer pequeno). */
static int DHSet(char *dst, size_t tam, time_t t, nfe_tzd tzd)
{
  struct tm tm;
  time_t local;
  size_t n;
  int r;

  switch (tzd){
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
  ide->tpNF = NFE_OPERACAO_SAIDA;
  ide->idDest = NFE_DESTINO_INTERNO;
  ide->tpImp = NFE_DANFE_NORMAL_RETRATO;
  ide->tpEmis = NFE_EMISSAO_NORMAL;
  ide->tpAmb = NFE_AMBIENTE_HOMOLOGACAO;
  ide->finNFe = NFE_FINALIDADE_NORMAL;
  ide->indFinal = NFE_CONSUMIDOR_NORMAL;
  ide->indPres = NFE_PRESENCA_NAO_SE_APLICA;
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
    else
      RefNFDel(ide->refs->doc.nf);
    free(ide->refs);
    ide->refs = prox;
  }

  free(ide);
}

/* Setters: validam e só então gravam o campo */
#define EXIGE_IDE(ide) do { if (!(ide)) return E_ISNULL; } while (0)
#define EXIGE(cond)    do { if (!(cond)) return E_VALOR; } while (0)

int nfe_ide_set_cuf(nfe_ide *ide, nfe_uf cuf)
{
  EXIGE_IDE(ide);
  switch (cuf) {
    case NFE_UF_RO: case NFE_UF_AC: case NFE_UF_AM: case NFE_UF_RR:
    case NFE_UF_PA: case NFE_UF_AP: case NFE_UF_TO: case NFE_UF_MA:
    case NFE_UF_PI: case NFE_UF_CE: case NFE_UF_RN: case NFE_UF_PB:
    case NFE_UF_PE: case NFE_UF_AL: case NFE_UF_SE: case NFE_UF_BA:
    case NFE_UF_MG: case NFE_UF_ES: case NFE_UF_RJ: case NFE_UF_SP:
    case NFE_UF_PR: case NFE_UF_SC: case NFE_UF_RS: case NFE_UF_MS:
    case NFE_UF_MT: case NFE_UF_GO: case NFE_UF_DF:
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
  return nfe_copia_texto(ide->natOp, sizeof ide->natOp, natop,
                         1, NFE_TAM_NATOP);
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
  EXIGE(tpimp >= NFE_DANFE_SEM_GERAR && tpimp <= NFE_DANFE_NFCE_MSG_ELETRONICA);
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
  EXIGE(tpamb == NFE_AMBIENTE_PRODUCAO || tpamb == NFE_AMBIENTE_HOMOLOGACAO);
  ide->tpAmb = tpamb;
  return 0;
}

int nfe_ide_set_finnfe(nfe_ide *ide, nfe_finalidade finnfe)
{
  EXIGE_IDE(ide);
  EXIGE(finnfe >= NFE_FINALIDADE_NORMAL && finnfe <= NFE_FINALIDADE_DEVOLUCAO);
  ide->finNFe = finnfe;
  return 0;
}

int nfe_ide_set_indfinal(nfe_ide *ide, nfe_consumidor indfinal)
{
  EXIGE_IDE(ide);
  EXIGE(indfinal == NFE_CONSUMIDOR_NORMAL || indfinal == NFE_CONSUMIDOR_FINAL);
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
        procemi <= NFE_PROCESSO_APP_FISCO);
  ide->procEmi = procemi;
  return 0;
}

int nfe_ide_set_verproc(nfe_ide *ide, const char *verproc)
{
  EXIGE_IDE(ide);
  return nfe_copia_texto(ide->verProc, sizeof ide->verProc, verproc,
                         1, NFE_TAM_VERPROC);
}

int nfe_ide_set_tzd(nfe_ide *ide, nfe_tzd tzd)
{
  char teste[NFE_TAM_ASCII(NFE_TAM_DATA_HORA)];

  EXIGE_IDE(ide);
  /* DHSet recusa fusos desconhecidos */
  EXIGE(DHSet(teste, sizeof teste, 0, tzd) == 0);
  ide->tzd = tzd;
  return 0;
}

int nfe_ide_set_contingencia(nfe_ide *ide, time_t dhcont, const char *xjust)
{
  int rc;

  EXIGE_IDE(ide);
  EXIGE(dhcont != NFE_SEM_DATA);
  rc = nfe_copia_texto(ide->xJust, sizeof ide->xJust, xjust,
                       15, NFE_TAM_XJUST);
  if (rc != 0)
    return rc;
  ide->dhCont = dhcont;
  ide->contingencia = 1;
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
  else
    novo->doc.nf = (struct refNF_s *)doc;
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

/* Escreve <tag>valor</tag>; retorna 0 ou E_XML */
static int escreve(xmlTextWriterPtr writer, const char *tag,
                   const char *formato, ...)
{
  va_list ap;
  int rc;

  va_start(ap, formato);
  rc = xmlTextWriterWriteVFormatElement(writer, BAD_CAST tag, formato, ap);
  va_end(ap);
  return rc < 0 ? E_XML : 0;
}

/* Escreve uma data no fuso do ide */
static int escreve_data(xmlTextWriterPtr writer, const char *tag,
                        time_t t, nfe_tzd tzd)
{
  char dh[NFE_TAM_ASCII(NFE_TAM_DATA_HORA)];
  int rc = DHSet(dh, sizeof dh, t, tzd);

  if (rc != 0)
    return rc;
  return escreve(writer, tag, "%s", dh);
}

#define ESCREVE(...) do { \
  rc = escreve(writer, __VA_ARGS__); \
  if (rc != 0) \
    return rc; \
} while (0)

#define ESCREVE_DATA(tag, t) do { \
  rc = escreve_data(writer, (tag), (t), ide->tzd); \
  if (rc != 0) \
    return rc; \
} while (0)

int nfe_ide_write_xml(xmlTextWriterPtr writer, const nfe_ide *ide)
{
  const struct ref_s *ref;
  int rc;

  if (!writer || !ide)
    return E_ISNULL;

  /* Campos obrigatórios sem valor padrão */
  if (ide->cUF == 0 || ide->natOp[0] == '\0' || ide->nNF == 0 ||
      ide->dhEmi == NFE_SEM_DATA || ide->cMunFG == 0 ||
      ide->verProc[0] == '\0')
    return E_VALOR;

  if (xmlTextWriterStartElement(writer, BAD_CAST "ide") < 0)
    return E_XML;

  ESCREVE("cUF", "%02d", (int)ide->cUF);
  ESCREVE("cNF", "%08" PRIu32, ide->cNF);
  ESCREVE("natOp", "%s", ide->natOp);
  ESCREVE("mod", "%02d", (int)ide->mod);
  ESCREVE("serie", "%u", ide->serie);
  ESCREVE("nNF", "%" PRIu32, ide->nNF);
  ESCREVE_DATA("dhEmi", ide->dhEmi);
  if (ide->dhSaiEnt != NFE_SEM_DATA)          /* opcional */
    ESCREVE_DATA("dhSaiEnt", ide->dhSaiEnt);
  ESCREVE("tpNF", "%d", (int)ide->tpNF);
  ESCREVE("idDest", "%d", (int)ide->idDest);
  ESCREVE("cMunFG", "%07" PRIu32, ide->cMunFG);
  ESCREVE("tpImp", "%d", (int)ide->tpImp);
  ESCREVE("tpEmis", "%d", (int)ide->tpEmis);
  ESCREVE("cDV", "%u", ide->cDV);
  ESCREVE("tpAmb", "%d", (int)ide->tpAmb);
  ESCREVE("finNFe", "%d", (int)ide->finNFe);
  ESCREVE("indFinal", "%d", (int)ide->indFinal);
  ESCREVE("indPres", "%d", (int)ide->indPres);
  ESCREVE("procEmi", "%d", (int)ide->procEmi);
  ESCREVE("verProc", "%s", ide->verProc);

  /* dhCont e xJust são filhos diretos de <ide>, logo após verProc */
  if (ide->contingencia) {
    ESCREVE_DATA("dhCont", ide->dhCont);
    ESCREVE("xJust", "%s", ide->xJust);
  }

  /* NFref: um grupo para cada documento referenciado */
  for (ref = ide->refs; ref; ref = ref->prox) {
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

  if (xmlTextWriterEndElement(writer) < 0)
    return E_XML;

  return 0;
}
