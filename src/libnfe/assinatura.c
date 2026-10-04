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

/* pthread_once (POSIX) */
#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/valid.h>

#include <openssl/asn1.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include <xmlsec/base64.h>
#include <xmlsec/crypto.h>
#include <xmlsec/errors.h>
#include <xmlsec/openssl/evp.h>
#include <xmlsec/openssl/x509.h>
#include <xmlsec/templates.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>

#include <libnfe/assinatura.h>
#include <libnfe/erros.h>

#define NS_NFE "http://www.portalfiscal.inf.br/nfe"

/* Maior arquivo .pfx aceito */
#define TAM_MAX_PFX (1024 * 1024)

struct nfe_certificado {
	xmlSecKeyPtr chave; /* chave privada e certificado */
	char titular[256];
	time_t validade;
};

/* ---- inicialização da xmlsec (uma vez por processo) ---- */

static pthread_once_t inicio = PTHREAD_ONCE_INIT;
static int iniciada; /* 1: ok; -1: falhou */

/* A xmlsec imprimiria os erros: são descartados (o erro volta pelo código
 * de retorno) */
static void silencia(const char *file, int line, const char *func,
                     const char *errorObject, const char *errorSubject,
                     int reason, const char *msg)
{
	(void)file;
	(void)line;
	(void)func;
	(void)errorObject;
	(void)errorSubject;
	(void)reason;
	(void)msg;
}

static void inicia(void)
{
	xmlSecErrorsSetCallback(silencia);
	if (xmlSecInit() < 0 || xmlSecCryptoAppInit(NULL) < 0 ||
	    xmlSecCryptoInit() < 0) {
		iniciada = -1;
		return;
	}
	/* A inicialização volta ao tratador padrão, que imprime */
	xmlSecErrorsSetCallback(silencia);
	/* Base64 sem quebras de linha (a xmlsec exige um tamanho de linha:
	 * um muito grande não quebra nunca) */
	xmlSecBase64SetDefaultLineSize(1 << 24);
	iniciada = 1;
}

static int prepara(void)
{
	if (pthread_once(&inicio, inicia) != 0)
		return E_MALLOC;
	return iniciada == 1 ? 0 : E_MALLOC;
}

/* ---- certificado ---- */

/* Dias desde 1970-01-01 da data civil (algoritmo de Howard Hinnant) */
static long dias_civis(long a, unsigned m, unsigned d)
{
	long era;
	unsigned ano_era, dia_ano, dia_era;

	a -= m <= 2;
	era = (a >= 0 ? a : a - 399) / 400;
	ano_era = (unsigned)(a - era * 400);
	dia_ano = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
	dia_era = ano_era * 365 + ano_era / 4 - ano_era / 100 + dia_ano;
	return era * 146097 + (long)dia_era - 719468;
}

/* Guarda titular e validade do certificado da chave */
static void le_certificado(nfe_certificado *cert)
{
	xmlSecKeyDataPtr dados;
	X509 *x;
	struct tm tm;

	cert->titular[0] = '\0';
	cert->validade = (time_t)-1;
	dados = xmlSecKeyGetData(cert->chave, xmlSecOpenSSLKeyDataX509Id);
	x = dados ? xmlSecOpenSSLKeyDataX509GetKeyCert(dados) : NULL;
	if (!x)
		return;
	if (X509_NAME_get_text_by_NID(X509_get_subject_name(x), NID_commonName,
	                              cert->titular,
	                              (int)sizeof cert->titular) < 0)
		cert->titular[0] = '\0';
	memset(&tm, 0, sizeof tm);
	if (ASN1_TIME_to_tm(X509_get0_notAfter(x), &tm) == 1)
		cert->validade = (time_t)(dias_civis(tm.tm_year + 1900,
		                                     (unsigned)tm.tm_mon + 1,
		                                     (unsigned)tm.tm_mday) *
		                                  86400L +
		                          tm.tm_hour * 3600L + tm.tm_min * 60L +
		                          tm.tm_sec);
}

nfe_certificado *nfe_certificado_pfx_memoria(const void *dados, size_t tam,
                                             const char *senha, int *rc)
{
	nfe_certificado *cert;
	int erro;

	if (!rc)
		rc = &erro;
	if (!dados || !senha) {
		*rc = E_ISNULL;
		return NULL;
	}
	if (tam == 0 || tam > TAM_MAX_PFX) {
		*rc = E_VALOR;
		return NULL;
	}
	*rc = prepara();
	if (*rc != 0)
		return NULL;
	cert = (nfe_certificado *)calloc(1, sizeof(nfe_certificado));
	if (!cert) {
		*rc = E_MALLOC;
		return NULL;
	}
	cert->chave = xmlSecCryptoAppKeyLoadMemory(
	        (const xmlSecByte *)dados, (xmlSecSize)tam,
	        xmlSecKeyDataFormatPkcs12, senha, NULL, NULL);
	if (!cert->chave) {
		free(cert);
		*rc = E_VALOR;
		return NULL;
	}
	le_certificado(cert);
	*rc = 0;
	return cert;
}

nfe_certificado *nfe_certificado_pfx(const char *caminho, const char *senha,
                                     int *rc)
{
	unsigned char *dados;
	nfe_certificado *cert;
	size_t tam;
	FILE *f;
	int erro;

	if (!rc)
		rc = &erro;
	if (!caminho || !senha) {
		*rc = E_ISNULL;
		return NULL;
	}
	f = fopen(caminho, "rb");
	if (!f) {
		*rc = E_ARQUIVO;
		return NULL;
	}
	dados = (unsigned char *)malloc(TAM_MAX_PFX + 1);
	if (!dados) {
		fclose(f);
		*rc = E_MALLOC;
		return NULL;
	}
	tam = fread(dados, 1, TAM_MAX_PFX + 1, f);
	fclose(f);
	cert = nfe_certificado_pfx_memoria(dados, tam, senha, rc);
	/* Não deixa a chave na memória liberada */
	memset(dados, 0, tam);
	free(dados);
	return cert;
}

void nfe_certificado_free(nfe_certificado *cert)
{
	if (!cert)
		return;
	xmlSecKeyDestroy(cert->chave);
	free(cert);
}

const char *nfe_certificado_titular(const nfe_certificado *cert)
{
	return cert && cert->titular[0] ? cert->titular : NULL;
}

time_t nfe_certificado_validade(const nfe_certificado *cert)
{
	return cert ? cert->validade : (time_t)-1;
}

/* ---- documento ---- */

/* Lê o documento sem imprimir erros */
static xmlDocPtr le_documento(const char *xml, size_t tam)
{
	if (tam > 0x7fffffff)
		return NULL;
	return xmlReadMemory(xml, (int)tam, "nfe.xml", NULL,
	                     XML_PARSE_NONET | XML_PARSE_NOERROR |
	                             XML_PARSE_NOWARNING);
}

/* Documentos assinados: raiz e elemento referenciado pela assinatura */
static const struct {
	const char *raiz, *info;
} assinaveis[] = {
	{ "NFe", "infNFe" },       /* nota */
	{ "evento", "infEvento" }, /* evento (cancelamento, CC-e...) */
	{ "inutNFe", "infInut" },  /* inutilização de numeração */
};

/* Elemento assinado (<infNFe> da raiz <NFe>, <infEvento> de <evento>...),
 * com o seu Id registrado como ID (para a referência "#Id" da assinatura);
 * NULL se o documento não for de um dos tipos assinados */
static xmlNodePtr infnfe(xmlDocPtr doc, xmlChar **id)
{
	xmlNodePtr raiz = xmlDocGetRootElement(doc), n = NULL;
	xmlAttrPtr atr;
	size_t i;

	if (!raiz || !raiz->ns || !xmlStrEqual(raiz->ns->href, BAD_CAST NS_NFE))
		return NULL;
	for (i = 0; i < sizeof assinaveis / sizeof assinaveis[0]; i++)
		if (xmlStrEqual(raiz->name, BAD_CAST assinaveis[i].raiz))
			break;
	if (i == sizeof assinaveis / sizeof assinaveis[0])
		return NULL;
	for (n = raiz->children; n; n = n->next)
		if (n->type == XML_ELEMENT_NODE &&
		    xmlStrEqual(n->name, BAD_CAST assinaveis[i].info))
			break;
	if (!n)
		return NULL;
	atr = xmlHasProp(n, BAD_CAST "Id");
	*id = atr ? xmlNodeGetContent((xmlNodePtr)atr) : NULL;
	if (!*id || !(*id)[0]) {
		xmlFree(*id);
		*id = NULL;
		return NULL;
	}
	if (!xmlGetID(doc, *id))
		xmlAddID(NULL, doc, *id, atr);
	return n;
}

/* <Signature> filha da raiz, ou NULL */
static xmlNodePtr assinatura(xmlDocPtr doc)
{
	xmlNodePtr n;

	for (n = xmlDocGetRootElement(doc)->children; n; n = n->next)
		if (n->type == XML_ELEMENT_NODE &&
		    xmlStrEqual(n->name, xmlSecNodeSignature) && n->ns &&
		    xmlStrEqual(n->ns->href, xmlSecDSigNs))
			return n;
	return NULL;
}

/* Remove os nós de texto só com espaços e quebras de linha dentro de n
 * (a xmlsec indenta o modelo da assinatura) */
static void tira_espacos(xmlNodePtr n)
{
	xmlNodePtr f = n->children, prox;

	for (; f; f = prox) {
		prox = f->next;
		if (f->type == XML_TEXT_NODE && xmlIsBlankNode(f)) {
			xmlUnlinkNode(f);
			xmlFreeNode(f);
		} else if (f->type == XML_ELEMENT_NODE) {
			tira_espacos(f);
		}
	}
}

/* Tira os espaços e quebras de linha do texto base64 de n e dos seus
 * descendentes (SignatureValue e X509Certificate, fora do SignedInfo) */
static void compacta_base64(xmlNodePtr n, const xmlChar *nome)
{
	xmlNodePtr f;

	for (f = n->children; f; f = f->next) {
		if (f->type != XML_ELEMENT_NODE)
			continue;
		if (xmlStrEqual(f->name, nome)) {
			xmlChar *v = xmlNodeGetContent(f), *d = v, *o = v;

			if (!v)
				continue;
			for (; *o; o++)
				if (*o != '\n' && *o != '\r' && *o != ' ' &&
				    *o != '\t')
					*d++ = *o;
			*d = '\0';
			xmlNodeSetContent(f, v);
			xmlFree(v);
		} else {
			compacta_base64(f, nome);
		}
	}
}

/* Serializa o documento sem quebras de linha após a declaração e no fim */
static int serializa(xmlDocPtr doc, char **saida, size_t *tam)
{
	xmlChar *buf = NULL;
	int n = 0;
	const char *fim_decl, *corpo;
	size_t decl, resto;

	xmlDocDumpMemoryEnc(doc, &buf, &n, "UTF-8");
	if (!buf)
		return E_MALLOC;
	fim_decl = strstr((const char *)buf, "?>");
	decl = fim_decl ? (size_t)(fim_decl + 2 - (const char *)buf) : 0;
	corpo = (const char *)buf + decl;
	while (*corpo == '\n')
		corpo++;
	resto = strlen(corpo);
	while (resto > 0 && corpo[resto - 1] == '\n')
		resto--;
	*saida = (char *)malloc(decl + resto + 1);
	if (!*saida) {
		xmlFree(buf);
		return E_MALLOC;
	}
	memcpy(*saida, buf, decl);
	memcpy(*saida + decl, corpo, resto);
	(*saida)[decl + resto] = '\0';
	if (tam)
		*tam = decl + resto;
	xmlFree(buf);
	return 0;
}

int nfe_assinar_xml(const nfe_certificado *cert, const char *xml, size_t tam,
                    char **assinado, size_t *tam_assinado)
{
	xmlNodePtr assin, ref, info, x509;
	xmlSecDSigCtxPtr ctx = NULL;
	xmlChar *id = NULL;
	char uri[128];
	xmlDocPtr doc;
	int rc;

	if (!cert || !xml || !assinado)
		return E_ISNULL;
	rc = prepara();
	if (rc != 0)
		return rc;
	doc = le_documento(xml, tam);
	if (!doc)
		return E_XML;
	if (!infnfe(doc, &id) || assinatura(doc) ||
	    (size_t)snprintf(uri, sizeof uri, "#%s", (const char *)id) >=
	            sizeof uri) {
		xmlFree(id);
		xmlFreeDoc(doc);
		return E_XML;
	}
	xmlFree(id);

	/* Modelo da assinatura, com os algoritmos exigidos pelo leiaute */
	rc = E_MALLOC;
	assin = xmlSecTmplSignatureCreate(doc, xmlSecTransformInclC14NId,
	                                  xmlSecTransformRsaSha1Id, NULL);
	if (!assin)
		goto fim;
	xmlAddChild(xmlDocGetRootElement(doc), assin);
	ref = xmlSecTmplSignatureAddReference(assin, xmlSecTransformSha1Id,
	                                      NULL, BAD_CAST uri, NULL);
	if (!ref ||
	    !xmlSecTmplReferenceAddTransform(ref, xmlSecTransformEnvelopedId) ||
	    !xmlSecTmplReferenceAddTransform(ref, xmlSecTransformInclC14NId))
		goto fim;
	info = xmlSecTmplSignatureEnsureKeyInfo(assin, NULL);
	x509 = info ? xmlSecTmplKeyInfoAddX509Data(info) : NULL;
	if (!x509 || !xmlSecTmplX509DataAddCertificate(x509))
		goto fim;

	tira_espacos(assin);

	ctx = xmlSecDSigCtxCreate(NULL);
	if (!ctx)
		goto fim;
	ctx->signKey = xmlSecKeyDuplicate(cert->chave);
	if (!ctx->signKey)
		goto fim;
	if (xmlSecDSigCtxSign(ctx, assin) < 0) {
		rc = E_VALOR;
		goto fim;
	}
	compacta_base64(assin, BAD_CAST "SignatureValue");
	compacta_base64(assin, BAD_CAST "X509Certificate");
	rc = serializa(doc, assinado, tam_assinado);
fim:
	if (ctx)
		xmlSecDSigCtxDestroy(ctx);
	xmlFreeDoc(doc);
	return rc;
}

int nfe_verificar_assinatura(const char *xml, size_t tam)
{
	xmlSecKeysMngrPtr mngr = NULL;
	xmlSecDSigCtxPtr ctx = NULL;
	xmlNodePtr assin, ref;
	xmlChar *id = NULL, *uri = NULL;
	xmlDocPtr doc;
	int rc;

	if (!xml)
		return E_ISNULL;
	rc = prepara();
	if (rc != 0)
		return rc;
	doc = le_documento(xml, tam);
	if (!doc)
		return E_XML;
	rc = E_XML;
	if (!infnfe(doc, &id) || !(assin = assinatura(doc)))
		goto fim;
	/* A assinatura tem de cobrir o infNFe da própria nota */
	ref = xmlSecFindNode(assin, xmlSecNodeReference, xmlSecDSigNs);
	uri = ref ? xmlGetProp(ref, BAD_CAST "URI") : NULL;
	if (!uri || uri[0] != '#' || !xmlStrEqual(uri + 1, id)) {
		rc = E_VALOR;
		goto fim;
	}

	rc = E_MALLOC;
	mngr = xmlSecKeysMngrCreate();
	if (!mngr || xmlSecCryptoAppDefaultKeysMngrInit(mngr) < 0)
		goto fim;
	ctx = xmlSecDSigCtxCreate(mngr);
	if (!ctx)
		goto fim;
	/* Confia no certificado do próprio documento (sem cadeia ICP) */
	ctx->keyInfoReadCtx.flags |=
	        XMLSEC_KEYINFO_FLAGS_X509DATA_DONT_VERIFY_CERTS;
	if (xmlSecDSigCtxVerify(ctx, assin) < 0 ||
	    ctx->status != xmlSecDSigStatusSucceeded)
		rc = E_VALOR;
	else
		rc = 0;
fim:
	xmlFree(uri);
	xmlFree(id);
	if (ctx)
		xmlSecDSigCtxDestroy(ctx);
	if (mngr)
		xmlSecKeysMngrDestroy(mngr);
	xmlFreeDoc(doc);
	return rc;
}

int nfe_certificado_assinar(const nfe_certificado *cert, const void *dados,
                            size_t tam, unsigned char **assinatura,
                            size_t *tam_assinatura)
{
	EVP_PKEY *pk;
	EVP_MD_CTX *ctx;
	unsigned char *sig;
	size_t tam_sig = 0;
	int ok;

	if (!cert || (!dados && tam > 0) || !assinatura || !tam_assinatura)
		return E_ISNULL;
	pk = xmlSecOpenSSLEvpKeyDataGetEvp(xmlSecKeyGetValue(cert->chave));
	if (!pk || EVP_PKEY_base_id(pk) != EVP_PKEY_RSA)
		return E_VALOR;
	ctx = EVP_MD_CTX_new();
	if (!ctx)
		return E_MALLOC;
	/* PKCS#1 v1.5 é o preenchimento padrão para RSA */
	ok = EVP_DigestSignInit(ctx, NULL, EVP_sha1(), NULL, pk) == 1 &&
	     EVP_DigestSign(ctx, NULL, &tam_sig, dados, tam) == 1;
	sig = ok ? malloc(tam_sig) : NULL;
	if (ok && !sig) {
		EVP_MD_CTX_free(ctx);
		return E_MALLOC;
	}
	ok = ok && EVP_DigestSign(ctx, sig, &tam_sig, dados, tam) == 1;
	EVP_MD_CTX_free(ctx);
	ERR_clear_error();
	if (!ok) {
		free(sig);
		return E_VALOR;
	}
	*assinatura = sig;
	*tam_assinatura = tam_sig;
	return 0;
}

int nfe_certificado_ssl_ctx(const nfe_certificado *cert, void *ssl_ctx)
{
	SSL_CTX *ctx = (SSL_CTX *)ssl_ctx;
	xmlSecKeyDataPtr dados;
	EVP_PKEY *pk;
	X509 *x, *outro;
	xmlSecSize i, n;

	if (!cert || !ctx)
		return E_VALOR;
	pk = xmlSecOpenSSLEvpKeyDataGetEvp(xmlSecKeyGetValue(cert->chave));
	dados = xmlSecKeyGetData(cert->chave, xmlSecOpenSSLKeyDataX509Id);
	x = dados ? xmlSecOpenSSLKeyDataX509GetKeyCert(dados) : NULL;
	if (!pk || !x || SSL_CTX_use_certificate(ctx, x) != 1 ||
	    SSL_CTX_use_PrivateKey(ctx, pk) != 1) {
		ERR_clear_error();
		return E_VALOR;
	}
	/* Demais certificados do .pfx: cadeia (autoridades intermediárias) */
	n = xmlSecOpenSSLKeyDataX509GetCertsSize(dados);
	for (i = 0; i < n; i++) {
		outro = xmlSecOpenSSLKeyDataX509GetCert(dados, i);
		if (outro && X509_cmp(outro, x) != 0)
			SSL_CTX_add1_chain_cert(ctx, outro);
	}
	ERR_clear_error();
	return 0;
}
