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

/* open, write e close (POSIX); a biblioteca não usa stdio para não
 * depender das funções de impressão */
#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libnfe/cnpjcpf.h>
#include <libnfe/decimal.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/esquema.h>
#include <libnfe/nfe_nfe.h>
#include <libnfe/valida.h>

#define NS_NFE     "http://www.portalfiscal.inf.br/nfe"
#define VERSAO_NFE "4.00"

struct nfe_nfe {
	nfe_ide *ide;
	nfe_emit *emit;
	nfe_dest *dest; /* opcionais: NULL quando não informados */
	nfe_local *retirada;
	nfe_local *entrega;
	char autXML[NFE_MAX_AUTXML][NFE_TAM_ASCII(NFE_TAM_CNPJ)];
	int nAutXML;
	nfe_det *det[NFE_MAX_ITENS];
	int nDet;
	nfe_total *total;
	nfe_transp *transp;
	nfe_cobr *cobr;
	nfe_pag *pag;
	char intermedCNPJ[NFE_TAM_ASCII(NFE_TAM_CNPJ)]; /* "": não informado */
	char idCadIntTran[NFE_TAM_UTF8(NFE_TAM_IDCADINT)];
	nfe_infadic *infadic;
	nfe_resptec *resptec;
	nfe_grupo *grupo[8]; /* na ordem de grupos[] */
};

/* Grupos genéricos da nota */
enum {
	G_AVULSA,
	G_EXPORTA,
	G_COMPRA,
	G_CANA,
	G_SOLICNFF,
	G_AGRO,
	G_PAA,
	G_SUPL,
	G_N
};
static const struct {
	const char *nome;
	const struct nfe_esq *esq;
} grupos[G_N] = {
	{ "avulsa", &esq_avulsa },
	{ "exporta", &esq_exporta },
	{ "compra", &esq_compra },
	{ "cana", &esq_cana },
	{ "infSolicNFF", &esq_infSolicNFF },
	{ "agropecuario", &esq_agropecuario },
	{ "infPAA", &esq_infPAA },
	{ "infNFeSupl", &esq_infNFeSupl },
};

nfe_nfe *nfe_nfe_new(void)
{
	return (nfe_nfe *)calloc(1, sizeof(nfe_nfe));
}

void nfe_nfe_free(nfe_nfe *nfe)
{
	int i;

	if (!nfe)
		return;
	nfe_ide_free(nfe->ide);
	nfe_emit_free(nfe->emit);
	nfe_dest_free(nfe->dest);
	nfe_local_free(nfe->retirada);
	nfe_local_free(nfe->entrega);
	nfe_cobr_free(nfe->cobr);
	nfe_infadic_free(nfe->infadic);
	nfe_resptec_free(nfe->resptec);
	for (i = 0; i < G_N; i++)
		nfe_grupo_free(nfe->grupo[i]);
	for (i = 0; i < nfe->nDet; i++)
		nfe_det_free(nfe->det[i]);
	nfe_total_free(nfe->total);
	nfe_transp_free(nfe->transp);
	nfe_pag_free(nfe->pag);
	free(nfe);
}

/* Troca o grupo campo por novo, liberando o anterior com libera. Com
 * opcional, novo pode ser NULL (remove o grupo). */
#define TROCA(campo, novo, libera, opcional)                                   \
	do {                                                                   \
		if (!nfe || (!(novo) && !(opcional)))                          \
			return E_ISNULL;                                       \
		if (nfe->campo != (novo)) {                                    \
			libera(nfe->campo);                                    \
			nfe->campo = (novo);                                   \
		}                                                              \
		return 0;                                                      \
	} while (0)

int nfe_nfe_set_ide(nfe_nfe *nfe, nfe_ide *ide)
{
	TROCA(ide, ide, nfe_ide_free, 0);
}

int nfe_nfe_set_emit(nfe_nfe *nfe, nfe_emit *emit)
{
	TROCA(emit, emit, nfe_emit_free, 0);
}

int nfe_nfe_set_dest(nfe_nfe *nfe, nfe_dest *dest)
{
	TROCA(dest, dest, nfe_dest_free, 1);
}

int nfe_nfe_set_total(nfe_nfe *nfe, nfe_total *total)
{
	TROCA(total, total, nfe_total_free, 0);
}

int nfe_nfe_set_transp(nfe_nfe *nfe, nfe_transp *transp)
{
	TROCA(transp, transp, nfe_transp_free, 0);
}

int nfe_nfe_set_pag(nfe_nfe *nfe, nfe_pag *pag)
{
	TROCA(pag, pag, nfe_pag_free, 0);
}

nfe_grupo *nfe_nfe_grupo(nfe_nfe *nfe, const char *nome)
{
	int i;

	if (!nfe || !nome)
		return NULL;
	for (i = 0; i < G_N; i++) {
		if (strcmp(nome, grupos[i].nome) != 0)
			continue;
		if (!nfe->grupo[i])
			nfe->grupo[i] = nfe_grupo_new(grupos[i].esq);
		return nfe->grupo[i];
	}
	return NULL;
}

/* Escreve o grupo genérico i, se tiver algum campo */
static int escreve_grupo(xmlTextWriterPtr writer, const nfe_nfe *nfe, int i)
{
	if (nfe_grupo_vazio(nfe->grupo[i]))
		return 0;
	return nfe_grupo_write_xml(writer, nfe->grupo[i]);
}

int nfe_nfe_set_retirada(nfe_nfe *nfe, nfe_local *retirada)
{
	TROCA(retirada, retirada, nfe_local_free, 1);
}

int nfe_nfe_set_entrega(nfe_nfe *nfe, nfe_local *entrega)
{
	TROCA(entrega, entrega, nfe_local_free, 1);
}

int nfe_nfe_set_cobr(nfe_nfe *nfe, nfe_cobr *cobr)
{
	TROCA(cobr, cobr, nfe_cobr_free, 1);
}

int nfe_nfe_set_infadic(nfe_nfe *nfe, nfe_infadic *infadic)
{
	TROCA(infadic, infadic, nfe_infadic_free, 1);
}

int nfe_nfe_set_resptec(nfe_nfe *nfe, nfe_resptec *resptec)
{
	TROCA(resptec, resptec, nfe_resptec_free, 1);
}

int nfe_nfe_add_autxml(nfe_nfe *nfe, const char *cnpjcpf)
{
	size_t n;
	int rc;

	if (!nfe || !cnpjcpf)
		return E_ISNULL;
	if (nfe->nAutXML >= NFE_MAX_AUTXML)
		return E_VALOR;
	n = strlen(cnpjcpf);
	if (n == NFE_TAM_CNPJ)
		rc = nfe_cnpj_validar(cnpjcpf);
	else if (n == NFE_TAM_CPF)
		rc = nfe_cpf_validar(cnpjcpf);
	else
		return E_TAMANHO;
	if (rc != 0)
		return rc;
	memcpy(nfe->autXML[nfe->nAutXML++], cnpjcpf, n + 1);
	return 0;
}

int nfe_nfe_remove_autxml(nfe_nfe *nfe)
{
	if (!nfe)
		return E_ISNULL;
	nfe->nAutXML = 0;
	return 0;
}

int nfe_nfe_set_intermed(nfe_nfe *nfe, const char *cnpj,
                         const char *idcadinttran)
{
	int rc;

	if (!nfe)
		return E_ISNULL;
	if (!cnpj && !idcadinttran) {
		nfe->intermedCNPJ[0] = '\0';
		return 0;
	}
	if (!cnpj || !idcadinttran)
		return E_ISNULL;
	rc = nfe_cnpj_validar(cnpj);
	if (rc == 0)
		rc = nfe_valida_texto(idcadinttran, 2, NFE_TAM_IDCADINT);
	if (rc != 0)
		return rc;
	strcpy(nfe->intermedCNPJ, cnpj);
	strcpy(nfe->idCadIntTran, idcadinttran);
	return 0;
}

int nfe_nfe_add_det(nfe_nfe *nfe, nfe_det *det)
{
	int rc;

	if (!nfe || !det)
		return E_ISNULL;
	if (nfe->nDet >= NFE_MAX_ITENS)
		return E_VALOR;
	rc = nfe_det_set_nitem(det, (unsigned)nfe->nDet + 1);
	if (rc != 0)
		return rc;
	nfe->det[nfe->nDet++] = det;
	return 0;
}

/* Soma valor (centavos) em *total; "" vale zero */
static int soma(long long *total, const char *valor)
{
	long long v;
	int rc = nfe_dec2_ler(valor, &v);

	if (rc == 0)
		*total += v;
	return rc;
}

/* Grava centavos no campo do total */
static int grava(nfe_total *tot, nfe_campo_icmstot campo, long long centavos)
{
	char texto[32];
	int rc = nfe_dec2_escreve(centavos, texto, sizeof texto);

	return rc ? rc : nfe_total_set_icmstot(tot, campo, texto);
}

int nfe_nfe_calcular_totais(nfe_nfe *nfe)
{
	/* Campos somados a partir dos itens */
	enum {
		VBC,
		VICMS,
		VFCP,
		VPROD,
		VFRETE,
		VSEG,
		VDESC,
		VOUTRO,
		VPIS,
		VCOFINS,
		VTOTTRIB,
		VST,
		VFCPST,
		VICMSDESON,
		VII,
		VIPI,
		VBCST,
		N
	};
	static const nfe_campo_icmstot destino[N] = {
		NFE_TOT_VBC,     NFE_TOT_VICMS,      NFE_TOT_VFCP,
		NFE_TOT_VPROD,   NFE_TOT_VFRETE,     NFE_TOT_VSEG,
		NFE_TOT_VDESC,   NFE_TOT_VOUTRO,     NFE_TOT_VPIS,
		NFE_TOT_VCOFINS, NFE_TOT_VTOTTRIB,   NFE_TOT_VST,
		NFE_TOT_VFCPST,  NFE_TOT_VICMSDESON, NFE_TOT_VII,
		NFE_TOT_VIPI,    NFE_TOT_VBCST,
	};
	/* Campo do total que entra no vNF além dos somados */
	static const nfe_campo_icmstot extras[] = { NFE_TOT_VIPIDEVOL };
	long long v[N] = { 0 }, vnf, x;
	int temTotTrib = 0, i, rc = 0;
	nfe_total *tot;

	if (!nfe)
		return E_ISNULL;
	if (nfe->nDet == 0)
		return E_VALOR;
	for (i = 0; i < nfe->nDet; i++) {
		const nfe_prod *p = nfe_det_prod(nfe->det[i]);
		const nfe_imposto *imp = nfe_det_imposto(nfe->det[i]);

		if (!p)
			return E_VALOR;
		if (nfe_prod_indtot(p))
			rc |= soma(&v[VPROD],
			           nfe_prod_valor(p, NFE_PROD_VPROD));
		rc |= soma(&v[VFRETE], nfe_prod_valor(p, NFE_PROD_VFRETE));
		rc |= soma(&v[VSEG], nfe_prod_valor(p, NFE_PROD_VSEG));
		rc |= soma(&v[VDESC], nfe_prod_valor(p, NFE_PROD_VDESC));
		rc |= soma(&v[VOUTRO], nfe_prod_valor(p, NFE_PROD_VOUTRO));
		if (imp) {
			const char *tt =
			        nfe_imposto_valor(imp, NFE_IMP_VTOTTRIB);

			rc |= soma(&v[VBC],
			           nfe_imposto_valor(imp, NFE_IMP_VBC));
			rc |= soma(&v[VICMS],
			           nfe_imposto_valor(imp, NFE_IMP_VICMS));
			rc |= soma(&v[VFCP],
			           nfe_imposto_valor(imp, NFE_IMP_VFCP));
			rc |= soma(&v[VPIS],
			           nfe_imposto_valor(imp, NFE_IMP_VPIS));
			rc |= soma(&v[VCOFINS],
			           nfe_imposto_valor(imp, NFE_IMP_VCOFINS));
			rc |= soma(&v[VTOTTRIB], tt);
			rc |= soma(&v[VST],
			           nfe_imposto_valor(imp, NFE_IMP_VST));
			rc |= soma(&v[VFCPST],
			           nfe_imposto_valor(imp, NFE_IMP_VFCPST));
			rc |= soma(&v[VICMSDESON],
			           nfe_imposto_valor(imp, NFE_IMP_VICMSDESON));
			rc |= soma(&v[VII],
			           nfe_imposto_valor(imp, NFE_IMP_VII));
			rc |= soma(&v[VIPI],
			           nfe_imposto_valor(imp, NFE_IMP_VIPI));
			rc |= soma(&v[VBCST],
			           nfe_imposto_valor(imp, NFE_IMP_VBCST));
			temTotTrib |= tt[0] != '\0';
		}
	}
	if (rc != 0)
		return E_VALOR;

	tot = nfe->total ? nfe->total : nfe_total_new();
	if (!tot)
		return E_MALLOC;

	vnf = v[VPROD] - v[VDESC] + v[VFRETE] + v[VSEG] + v[VOUTRO] + v[VST] +
	      v[VFCPST] + v[VII] + v[VIPI];
	for (i = 0; i < (int)(sizeof extras / sizeof extras[0]); i++) {
		rc = nfe_dec2_ler(nfe_total_valor(tot, extras[i]), &x);
		if (rc != 0)
			break;
		vnf += x;
	}
	if (rc == 0 && vnf < 0)
		rc = E_VALOR;
	for (i = 0; rc == 0 && i < N; i++) {
		if (i == VTOTTRIB && !temTotTrib)
			rc = nfe_total_set_icmstot(tot, destino[i], NULL);
		else
			rc = grava(tot, destino[i], v[i]);
	}
	if (rc == 0)
		rc = grava(tot, NFE_TOT_VNF, vnf);
	if (rc != 0) {
		if (tot != nfe->total)
			nfe_total_free(tot);
		return rc;
	}
	nfe->total = tot;
	return 0;
}

int nfe_nfe_chave(nfe_nfe *nfe, char *chave, size_t tam)
{
	const char *doc;

	if (!nfe || !chave)
		return E_ISNULL;
	doc = nfe_emit_documento(nfe->emit);
	if (!nfe->ide || !doc)
		return E_VALOR;
	return nfe_ide_gerar_chave(nfe->ide, doc, chave, tam);
}

int nfe_nfe_write_xml(xmlTextWriterPtr writer, nfe_nfe *nfe)
{
	char chave[NFE_TAM_ASCII(NFE_TAM_CHAVE)];
	int i, rc;

	if (!writer || !nfe)
		return E_ISNULL;
	if (!nfe->ide || !nfe->emit || nfe->nDet == 0 || !nfe->total ||
	    !nfe->transp || !nfe->pag)
		return E_VALOR;
	rc = nfe_nfe_chave(nfe, chave, sizeof chave);
	if (rc != 0)
		return rc;

	rc = nfe_abre(writer, "NFe");
	if (rc != 0)
		return rc;
	if (xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
	                                BAD_CAST NS_NFE) < 0)
		return E_XML;
	rc = nfe_abre(writer, "infNFe");
	if (rc != 0)
		return rc;
	if (xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
	                                BAD_CAST VERSAO_NFE) < 0 ||
	    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "Id", "NFe%s",
	                                      chave) < 0)
		return E_XML;

	rc = nfe_ide_write_xml(writer, nfe->ide);
	if (rc == 0)
		rc = nfe_emit_write_xml(writer, nfe->emit);
	if (rc == 0)
		rc = escreve_grupo(writer, nfe, G_AVULSA);
	if (rc == 0 && nfe->dest)
		rc = nfe_dest_write_xml(writer, nfe->dest);
	if (rc == 0 && nfe->retirada)
		rc = nfe_local_write_xml(writer, NFE_LOCAL_RETIRADA,
		                         nfe->retirada);
	if (rc == 0 && nfe->entrega)
		rc = nfe_local_write_xml(writer, NFE_LOCAL_ENTREGA,
		                         nfe->entrega);
	for (i = 0; rc == 0 && i < nfe->nAutXML; i++) {
		rc = nfe_abre(writer, "autXML");
		if (rc == 0)
			rc = nfe_escreve(writer,
			                 strlen(nfe->autXML[i]) == NFE_TAM_CNPJ
			                         ? "CNPJ"
			                         : "CPF",
			                 "%s", nfe->autXML[i]);
		if (rc == 0)
			rc = nfe_fecha(writer);
	}
	for (i = 0; rc == 0 && i < nfe->nDet; i++)
		rc = nfe_det_write_xml(writer, nfe->det[i]);
	if (rc == 0)
		rc = nfe_total_write_xml(writer, nfe->total);
	if (rc == 0)
		rc = nfe_transp_write_xml(writer, nfe->transp);
	if (rc == 0 && nfe->cobr)
		rc = nfe_cobr_write_xml(writer, nfe->cobr);
	if (rc == 0)
		rc = nfe_pag_write_xml(writer, nfe->pag);
	if (rc == 0 && nfe->intermedCNPJ[0] != '\0') {
		rc = nfe_abre(writer, "infIntermed");
		if (rc == 0)
			rc = nfe_escreve(writer, "CNPJ", "%s",
			                 nfe->intermedCNPJ);
		if (rc == 0)
			rc = nfe_escreve(writer, "idCadIntTran", "%s",
			                 nfe->idCadIntTran);
		if (rc == 0)
			rc = nfe_fecha(writer);
	}
	if (rc == 0 && nfe->infadic)
		rc = nfe_infadic_write_xml(writer, nfe->infadic);
	for (i = G_EXPORTA; rc == 0 && i <= G_CANA; i++)
		rc = escreve_grupo(writer, nfe, i);
	if (rc == 0 && nfe->resptec)
		rc = nfe_resptec_write_xml(writer, nfe->resptec);
	for (i = G_SOLICNFF; rc == 0 && i <= G_PAA; i++)
		rc = escreve_grupo(writer, nfe, i);
	if (rc != 0)
		return rc;

	rc = nfe_fecha(writer); /* infNFe */
	if (rc == 0)
		rc = escreve_grupo(writer, nfe, G_SUPL);
	if (rc != 0)
		return rc;
	return nfe_fecha(writer); /* NFe */
}

int nfe_nfe_xml(nfe_nfe *nfe, char **xml, size_t *tam)
{
	xmlBufferPtr buf;
	xmlTextWriterPtr writer;
	size_t n;
	int rc;

	if (!nfe || !xml)
		return E_ISNULL;
	buf = xmlBufferCreate();
	if (!buf)
		return E_MALLOC;
	writer = xmlNewTextWriterMemory(buf, 0);
	if (!writer) {
		xmlBufferFree(buf);
		return E_MALLOC;
	}

	rc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0 ? E_XML
	                                                                 : 0;
	if (rc == 0)
		rc = nfe_nfe_write_xml(writer, nfe);
	if (rc == 0 && xmlTextWriterEndDocument(writer) < 0)
		rc = E_XML;
	xmlFreeTextWriter(writer); /* descarrega o conteúdo em buf */

	if (rc == 0) {
		/* Sem a quebra de linha que o libxml2 põe no fim do documento
		 * e após a declaração XML */
		const char *conteudo = (const char *)xmlBufferContent(buf);
		const char *fim_decl = strstr(conteudo, "?>");
		size_t decl = fim_decl ? (size_t)(fim_decl + 2 - conteudo) : 0;
		const char *corpo = conteudo + decl;

		while (*corpo == '\n')
			corpo++;
		n = strlen(corpo);
		while (n > 0 && corpo[n - 1] == '\n')
			n--;
		*xml = (char *)malloc(decl + n + 1);
		if (!*xml) {
			rc = E_MALLOC;
		} else {
			memcpy(*xml, conteudo, decl);
			memcpy(*xml + decl, corpo, n);
			(*xml)[decl + n] = '\0';
			if (tam)
				*tam = decl + n;
		}
	}
	xmlBufferFree(buf);
	return rc;
}

int nfe_nfe_salvar(nfe_nfe *nfe, const char *caminho)
{
	char *xml;
	size_t tam, feito = 0;
	int fd, rc;

	if (!nfe || !caminho)
		return E_ISNULL;
	rc = nfe_nfe_xml(nfe, &xml, &tam);
	if (rc != 0)
		return rc;
	fd = open(caminho, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) {
		free(xml);
		return E_ARQUIVO;
	}
	while (feito < tam) {
		ssize_t n = write(fd, xml + feito, tam - feito);
		if (n <= 0) {
			rc = E_ARQUIVO;
			break;
		}
		feito += (size_t)n;
	}
	if (close(fd) != 0)
		rc = E_ARQUIVO;
	free(xml);
	return rc;
}
