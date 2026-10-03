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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/xmlwriter.h>

#include <libnfe/chave.h>
#include <libnfe/cnpjcpf.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/evento.h>
#include <libnfe/padroes.h>
#include <libnfe/valida.h>

#define NS_NFE "http://www.portalfiscal.inf.br/nfe"

/* Campos específicos de cada evento, escritos dentro de <detEvento> */
struct detalhe {
	int tipo;              /* tpEvento */
	const char *desc;      /* descEvento */
	const char *nprot;     /* protocolo da nota */
	const char *xjust;     /* justificativa */
	const char *subst;     /* chNFeRef (substituição) ou NULL */
	const char *veraplic;  /* verAplic (substituição) */
	int nseq;              /* nSeqEvento */
	const char *xcorrecao; /* correção (CC-e) ou NULL */
};

/* Condições de uso da CC-e: texto fixo do leiaute (e110110_v1.00.xsd),
 * na versão sem acentos que o schema também aceita */
static const char COND_USO[] =
        "A Carta de Correcao e disciplinada pelo paragrafo 1o-A do art. 7o "
        "do Convenio S/N, de 15 de dezembro de 1970 e pode ser utilizada "
        "para regularizacao de erro ocorrido na emissao de documento "
        "fiscal, desde que o erro nao esteja relacionado com: I - as "
        "variaveis que determinam o valor do imposto tais como: base de "
        "calculo, aliquota, diferenca de preco, quantidade, valor da "
        "operacao ou da prestacao; II - a correcao de dados cadastrais que "
        "implique mudanca do remetente ou do destinatario; III - a data de "
        "emissao ou de saida.";

static int chave_valida(const char *chave)
{
	return chave && nfe_valida_padrao(chave, NFE_PADRAO_TChNFe) == 0 &&
	                       nfe_chave_validar(chave) == 0
	               ? 0
	               : E_VALOR;
}

/* Confere os dados comuns; devolve em *cnpj se o autor é CNPJ (1) ou
 * CPF (0) e em dh a data formatada */
static int confere_info(const nfe_evento_info *info, int *cnpj, char *dh,
                        size_t tam_dh)
{
	if (!info->chave || !info->cnpjcpf)
		return E_ISNULL;
	if (info->amb != NFE_AMBIENTE_PRODUCAO &&
	    info->amb != NFE_AMBIENTE_HOMOLOGACAO)
		return E_VALOR;
	if (chave_valida(info->chave) != 0)
		return E_VALOR;
	if (strlen(info->cnpjcpf) == 14 && nfe_cnpj_validar(info->cnpjcpf) == 0)
		*cnpj = 1;
	else if (strlen(info->cnpjcpf) == 11 &&
	         nfe_cpf_validar(info->cnpjcpf) == 0)
		*cnpj = 0;
	else
		return E_VALOR;
	return nfe_data_hora(dh, tam_dh, info->dh, info->tzd) == 0 ? 0
	                                                           : E_VALOR;
}

static int escreve_evento(xmlTextWriterPtr writer, const nfe_evento_info *info,
                          int cnpj, const char *dh, const struct detalhe *d)
{
	char id[64];
	int rc;

	/* Id: "ID" + tpEvento + chave + nSeqEvento (2 dígitos) */
	snprintf(id, sizeof id, "ID%06d%s%02d", d->tipo, info->chave, d->nseq);
	if (xmlTextWriterStartElement(writer, BAD_CAST "evento") < 0 ||
	    xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
	                                BAD_CAST NS_NFE) < 0 ||
	    xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
	                                BAD_CAST "1.00") < 0 ||
	    xmlTextWriterStartElement(writer, BAD_CAST "infEvento") < 0 ||
	    xmlTextWriterWriteAttribute(writer, BAD_CAST "Id", BAD_CAST id) < 0)
		return E_XML;
	/* cOrgao: UF da nota (dois primeiros dígitos da chave) */
	NFE_ESCREVE("cOrgao", "%.2s", info->chave);
	NFE_ESCREVE("tpAmb", "%d", (int)info->amb);
	NFE_ESCREVE(cnpj ? "CNPJ" : "CPF", "%s", info->cnpjcpf);
	NFE_ESCREVE("chNFe", "%s", info->chave);
	NFE_ESCREVE("dhEvento", "%s", dh);
	NFE_ESCREVE("tpEvento", "%06d", d->tipo);
	NFE_ESCREVE("nSeqEvento", "%d", d->nseq);
	NFE_ESCREVE("verEvento", "1.00");
	if (xmlTextWriterStartElement(writer, BAD_CAST "detEvento") < 0 ||
	    xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
	                                BAD_CAST "1.00") < 0)
		return E_XML;
	NFE_ESCREVE("descEvento", "%s", d->desc);
	if (d->xcorrecao) {
		NFE_ESCREVE("xCorrecao", "%s", d->xcorrecao);
		NFE_ESCREVE("xCondUso", "%s", COND_USO);
		goto fim;
	}
	if (d->subst) {
		NFE_ESCREVE("cOrgaoAutor", "%.2s", info->chave);
		NFE_ESCREVE("tpAutor", "1");
		NFE_ESCREVE("verAplic", "%s", d->veraplic);
	}
	NFE_ESCREVE("nProt", "%s", d->nprot);
	NFE_ESCREVE("xJust", "%s", d->xjust);
	if (d->subst)
		NFE_ESCREVE("chNFeRef", "%s", d->subst);
fim:
	if (xmlTextWriterEndElement(writer) < 0 || /* detEvento */
	    xmlTextWriterEndElement(writer) < 0 || /* infEvento */
	    xmlTextWriterEndElement(writer) < 0)   /* evento */
		return E_XML;
	return 0;
}

/* Monta o documento do evento em *xml */
static int gera(const nfe_evento_info *info, const struct detalhe *d,
                char **xml, size_t *tam)
{
	char dh[32];
	xmlBufferPtr buf;
	xmlTextWriterPtr writer;
	const char *conteudo, *fim;
	size_t decl, n;
	int rc, cnpj = 0;

	rc = confere_info(info, &cnpj, dh, sizeof dh);
	if (rc != 0)
		return rc;
	buf = xmlBufferCreate();
	writer = buf ? xmlNewTextWriterMemory(buf, 0) : NULL;
	if (!writer) {
		xmlBufferFree(buf);
		return E_MALLOC;
	}
	rc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0 ? E_XML
	                                                                 : 0;
	if (rc == 0)
		rc = escreve_evento(writer, info, cnpj, dh, d);
	if (rc == 0 && xmlTextWriterEndDocument(writer) < 0)
		rc = E_XML;
	xmlFreeTextWriter(writer);
	if (rc == 0) {
		/* Sem as quebras de linha após a declaração e no fim */
		conteudo = (const char *)xmlBufferContent(buf);
		fim = strstr(conteudo, "?>");
		decl = fim ? (size_t)(fim + 2 - conteudo) : 0;
		fim = conteudo + decl;
		while (*fim == '\n')
			fim++;
		n = strlen(fim);
		while (n > 0 && fim[n - 1] == '\n')
			n--;
		*xml = (char *)malloc(decl + n + 1);
		if (!*xml) {
			rc = E_MALLOC;
		} else {
			memcpy(*xml, conteudo, decl);
			memcpy(*xml + decl, fim, n);
			(*xml)[decl + n] = '\0';
			if (tam)
				*tam = decl + n;
		}
	}
	xmlBufferFree(buf);
	return rc;
}

/* Confere protocolo e justificativa */
static int confere_cancelamento(const char *nprot, const char *xjust)
{
	if (nfe_valida_padrao(nprot, NFE_PADRAO_TProt) != 0)
		return E_VALOR;
	return nfe_valida_texto(xjust, 15, 255);
}

int nfe_evento_cancelamento(const nfe_evento_info *info, const char *nprot,
                            const char *xjust, char **xml, size_t *tam)
{
	struct detalhe d = { NFE_EVENTO_CANCELAMENTO,
		             "Cancelamento",
		             nprot,
		             xjust,
		             NULL,
		             NULL,
		             1,
		             NULL };
	int rc;

	if (!info || !nprot || !xjust || !xml)
		return E_ISNULL;
	rc = confere_cancelamento(nprot, xjust);
	if (rc != 0)
		return rc;
	return gera(info, &d, xml, tam);
}

int nfe_evento_cancelamento_subst(const nfe_evento_info *info,
                                  const char *nprot, const char *xjust,
                                  const char *chave_subst, const char *veraplic,
                                  char **xml, size_t *tam)
{
	struct detalhe d = { NFE_EVENTO_CANCELAMENTO_SUBST,
		             "Cancelamento por substituicao",
		             nprot,
		             xjust,
		             chave_subst,
		             veraplic,
		             1,
		             NULL };
	int rc;

	if (!info || !nprot || !xjust || !chave_subst || !veraplic || !xml ||
	    !info->chave)
		return E_ISNULL;
	rc = confere_cancelamento(nprot, xjust);
	if (rc != 0)
		return rc;
	rc = nfe_valida_texto(veraplic, 1, 20);
	if (rc != 0)
		return rc;
	/* Só NFC-e (modelo 65, posições 21 e 22 da chave), substituída por
	 * outra NFC-e */
	if (chave_valida(chave_subst) != 0 ||
	    strcmp(chave_subst, info->chave) == 0 ||
	    strlen(info->chave) != 44 ||
	    memcmp(info->chave + 20, "65", 2) != 0 ||
	    memcmp(chave_subst + 20, "65", 2) != 0)
		return E_VALOR;
	return gera(info, &d, xml, tam);
}

int nfe_evento_cce(const nfe_evento_info *info, int nseq, const char *xcorrecao,
                   char **xml, size_t *tam)
{
	struct detalhe d = { NFE_EVENTO_CCE, "Carta de Correcao",
		             NULL,           NULL,
		             NULL,           NULL,
		             nseq,           xcorrecao };
	int rc;

	if (!info || !xcorrecao || !xml)
		return E_ISNULL;
	if (nseq < 1 || nseq > 20)
		return E_VALOR;
	rc = nfe_valida_texto(xcorrecao, 15, 1000);
	if (rc != 0)
		return rc;
	return gera(info, &d, xml, tam);
}
