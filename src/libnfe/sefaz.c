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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <libnfe/chave.h>
#include <libnfe/erros.h>
#include <libnfe/padroes.h>
#include <libnfe/sefaz.h>
#include <libnfe/valida.h>

#define NS_NFE  "http://www.portalfiscal.inf.br/nfe"
#define NS_WSDL "http://www.portalfiscal.inf.br/nfe/wsdl/"

/* Maior resposta aceita da SEFAZ */
#define TAM_MAX_RESPOSTA (32u * 1024 * 1024)
/* Máximo de notas por lote (leiaute) */
#define MAX_LOTE 50
/* Máximo de eventos por lote */
#define MAX_EVENTOS 20

struct nfe_sefaz {
	const nfe_certificado *cert;
	char *ca;
	long timeout;
	char erro[CURL_ERROR_SIZE + 512];
};

/* Namespace (sem o prefixo NS_WSDL) e operação de cada serviço */
static const struct {
	const char *ns, *operacao;
} servicos[] = {
	[NFE_SERVICO_AUTORIZACAO] = { "NFeAutorizacao4", "nfeAutorizacaoLote" },
	[NFE_SERVICO_RET_AUTORIZACAO] = { "NFeRetAutorizacao4",
	                                  "nfeRetAutorizacaoLote" },
	[NFE_SERVICO_CONSULTA] = { "NFeConsultaProtocolo4", "nfeConsultaNF" },
	[NFE_SERVICO_STATUS] = { "NFeStatusServico4", "nfeStatusServicoNF" },
	[NFE_SERVICO_EVENTO] = { "NFeRecepcaoEvento4", "nfeRecepcaoEvento" },
	[NFE_SERVICO_INUTILIZACAO] = { "NFeInutilizacao4",
	                               "nfeInutilizacaoNF" },
};

/* ---- texto que cresce ---- */

struct buf {
	char *p;
	size_t n, cap;
	int erro;
};

static void poe_n(struct buf *b, const char *s, size_t n)
{
	if (b->erro)
		return;
	if (b->n + n + 1 > b->cap) {
		size_t cap = b->cap ? b->cap : 256;
		char *p;

		while (cap < b->n + n + 1)
			cap *= 2;
		p = (char *)realloc(b->p, cap);
		if (!p) {
			b->erro = E_MALLOC;
			return;
		}
		b->p = p;
		b->cap = cap;
	}
	memcpy(b->p + b->n, s, n);
	b->n += n;
	b->p[b->n] = '\0';
}

static void poe(struct buf *b, const char *s)
{
	poe_n(b, s, strlen(s));
}

/* Entrega o texto em *dst (e o tamanho em *tam); 0 ou E_MALLOC */
static int entrega(struct buf *b, char **dst, size_t *tam)
{
	if (b->erro || !b->p) {
		free(b->p);
		return E_MALLOC;
	}
	*dst = b->p;
	if (tam)
		*tam = b->n;
	return 0;
}

/* Pula espaços, BOM e a declaração <?xml ...?> do início de s (n bytes);
 * devolve o início do elemento e acerta *n (sem espaços no fim) */
static const char *pula_declaracao(const char *s, size_t *n)
{
	const char *fim = s + *n, *p;

	if (fim - s >= 3 && memcmp(s, "\xEF\xBB\xBF", 3) == 0)
		s += 3;
	while (s < fim && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n'))
		s++;
	if (fim - s >= 5 && memcmp(s, "<?xml", 5) == 0) {
		for (p = s; p + 1 < fim && !(p[0] == '?' && p[1] == '>'); p++)
			;
		s = p + 1 < fim ? p + 2 : fim;
		while (s < fim &&
		       (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n'))
			s++;
	}
	while (fim > s && (fim[-1] == ' ' || fim[-1] == '\t' ||
	                   fim[-1] == '\r' || fim[-1] == '\n'))
		fim--;
	*n = (size_t)(fim - s);
	return s;
}

/* s (n bytes) começa com o elemento nome? */
static int comeca_com(const char *s, size_t n, const char *nome)
{
	size_t k = strlen(nome);

	return n > k + 1 && s[0] == '<' && memcmp(s + 1, nome, k) == 0 &&
	       (s[k + 1] == ' ' || s[k + 1] == '>' || s[k + 1] == '/');
}

static int so_digitos(const char *s, size_t min, size_t max)
{
	size_t n = strlen(s), i;

	if (n < min || n > max)
		return 0;
	for (i = 0; i < n; i++)
		if (s[i] < '0' || s[i] > '9')
			return 0;
	return 1;
}

/* ---- XML ---- */

static xmlDocPtr le_xml(const char *xml, size_t tam)
{
	if (tam > 0x7fffffff)
		return NULL;
	return xmlReadMemory(xml, (int)tam, "sefaz.xml", NULL,
	                     XML_PARSE_NONET | XML_PARSE_NOERROR |
	                             XML_PARSE_NOWARNING | XML_PARSE_NOBLANKS);
}

/* Primeiro filho elemento de pai com o nome local (NULL: qualquer) */
static xmlNodePtr filho(xmlNodePtr pai, const char *nome)
{
	xmlNodePtr f;

	for (f = pai ? pai->children : NULL; f; f = f->next)
		if (f->type == XML_ELEMENT_NODE &&
		    (!nome || xmlStrEqual(f->name, BAD_CAST nome)))
			return f;
	return NULL;
}

/* Texto de um filho; "" se não houver */
static const char *texto(xmlNodePtr pai, const char *nome)
{
	xmlNodePtr e = filho(pai, nome);

	if (e && e->children && e->children->type == XML_TEXT_NODE &&
	    e->children->content)
		return (const char *)e->children->content;
	return "";
}

/* Serializa o elemento (com as declarações de namespace herdadas) em
 * *dst. Retorna 0 ou E_MALLOC. */
static int serializa(xmlNodePtr no, char **dst, size_t *tam)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr copia;
	xmlBufferPtr b;
	struct buf saida = { 0 };
	int rc = E_MALLOC;

	if (!doc)
		return E_MALLOC;
	copia = xmlDocCopyNode(no, doc, 1);
	if (copia) {
		xmlDocSetRootElement(doc, copia);
		xmlReconciliateNs(doc, copia);
		b = xmlBufferCreate();
		if (b) {
			if (xmlNodeDump(b, doc, copia, 0, 0) >= 0) {
				poe_n(&saida, (const char *)xmlBufferContent(b),
				      (size_t)xmlBufferLength(b));
				rc = entrega(&saida, dst, tam);
			}
			xmlBufferFree(b);
		}
	}
	xmlFreeDoc(doc);
	return rc;
}

/* ---- conexão ---- */

static pthread_once_t inicio = PTHREAD_ONCE_INIT;
static CURLcode inicio_rc = CURLE_FAILED_INIT;

static void inicia_curl(void)
{
	inicio_rc = curl_global_init(CURL_GLOBAL_DEFAULT);
}

nfe_sefaz *nfe_sefaz_new(const nfe_certificado *cert)
{
	nfe_sefaz *s;

	if (!cert)
		return NULL;
	s = (nfe_sefaz *)calloc(1, sizeof(nfe_sefaz));
	if (!s)
		return NULL;
	s->cert = cert;
	s->timeout = 60;
	return s;
}

void nfe_sefaz_free(nfe_sefaz *s)
{
	if (!s)
		return;
	free(s->ca);
	free(s);
}

int nfe_sefaz_set_ca(nfe_sefaz *s, const char *arquivo_pem)
{
	char *c = NULL;

	if (!s)
		return E_ISNULL;
	if (arquivo_pem) {
		c = (char *)malloc(strlen(arquivo_pem) + 1);
		if (!c)
			return E_MALLOC;
		strcpy(c, arquivo_pem);
	}
	free(s->ca);
	s->ca = c;
	return 0;
}

int nfe_sefaz_set_timeout(nfe_sefaz *s, long segundos)
{
	if (!s)
		return E_ISNULL;
	if (segundos < 1)
		return E_VALOR;
	s->timeout = segundos;
	return 0;
}

const char *nfe_sefaz_erro(const nfe_sefaz *s)
{
	return s ? s->erro : "";
}

/* Callback da libcurl: põe o certificado do emitente na conexão TLS */
static CURLcode poe_certificado(CURL *c, void *ssl_ctx, void *dados)
{
	(void)c;
	return nfe_certificado_ssl_ctx((const nfe_certificado *)dados,
	                               ssl_ctx) == 0
	               ? CURLE_OK
	               : CURLE_SSL_CERTPROBLEM;
}

/* Callback da libcurl: guarda a resposta */
static size_t recebe(char *dados, size_t tam, size_t n, void *userdata)
{
	struct buf *b = (struct buf *)userdata;

	if (n && tam > TAM_MAX_RESPOSTA / n)
		return 0;
	if (b->n + tam * n > TAM_MAX_RESPOSTA)
		return 0;
	poe_n(b, dados, tam * n);
	return b->erro ? 0 : tam * n;
}

/* Motivo de um SOAP Fault (Reason/Text ou faultstring), ou NULL */
static const char *motivo_fault(xmlNodePtr fault)
{
	xmlNodePtr r = filho(fault, "Reason");
	const char *t;

	if (r) {
		t = texto(r, "Text");
		if (*t)
			return t;
	}
	t = texto(fault, "faultstring");
	return *t ? t : NULL;
}

/* Interpreta a resposta SOAP. Retorna 0, E_REDE (Fault), E_XML ou
 * E_MALLOC. */
static int le_resposta(nfe_sefaz *s, const struct buf *b, long http,
                       char **resposta, size_t *tam)
{
	xmlDocPtr doc = b->p ? le_xml(b->p, b->n) : NULL;
	xmlNodePtr corpo, res, ret;
	int rc;

	corpo = doc ? filho(xmlDocGetRootElement(doc), "Body") : NULL;
	res = filho(corpo, NULL);
	if (res && xmlStrEqual(res->name, BAD_CAST "Fault")) {
		const char *m = motivo_fault(res);

		snprintf(s->erro, sizeof s->erro,
		         "SOAP Fault (HTTP %ld): %.400s", http,
		         m ? m : "sem descrição");
		xmlFreeDoc(doc);
		return E_REDE;
	}
	if (http != 200) {
		snprintf(s->erro, sizeof s->erro, "resposta HTTP %ld", http);
		xmlFreeDoc(doc);
		return E_REDE;
	}
	ret = filho(res, NULL);
	if (!ret) {
		snprintf(s->erro, sizeof s->erro,
		         "resposta sem o elemento de retorno da SEFAZ");
		xmlFreeDoc(doc);
		return E_XML;
	}
	rc = serializa(ret, resposta, tam);
	xmlFreeDoc(doc);
	return rc;
}

/* Texto que pode ir num atributo entre aspas e no cabeçalho HTTP? */
static int texto_seguro(const char *t)
{
	if (!*t)
		return 0;
	for (; *t; t++)
		if (*t == '"' || *t == '<' || *t == '>' || *t == '&' ||
		    (unsigned char)*t < 0x20 || *t == 0x7f)
			return 0;
	return 1;
}

/* Nome de elemento XML simples (letras ASCII, dígitos e _) */
static int nome_seguro(const char *t)
{
	if (!((*t >= 'A' && *t <= 'Z') || (*t >= 'a' && *t <= 'z') ||
	      *t == '_'))
		return 0;
	for (; *t; t++)
		if (!((*t >= 'A' && *t <= 'Z') || (*t >= 'a' && *t <= 'z') ||
		      (*t >= '0' && *t <= '9') || *t == '_'))
			return 0;
	return 1;
}

/* Envio SOAP 1.2: <elemento xmlns="ns">msg</elemento> no corpo e
 * action="ns/operacao"; cabecalho (ou NULL) vai em <soap12:Header> */
static int envia(nfe_sefaz *s, const char *url, const char *ns,
                 const char *operacao, const char *elemento,
                 const char *cabecalho_soap, const char *msg, char **resposta,
                 size_t *tam)
{
	struct buf envelope = { 0 }, cabecalho = { 0 }, recebido = { 0 };
	char errbuf[CURL_ERROR_SIZE];
	struct curl_slist *h = NULL, *h2;
	const curl_version_info_data *ver;
	const char *corpo;
	size_t n;
	long http = 0;
	CURLcode cc;
	CURL *c;
	int rc;

	pthread_once(&inicio, inicia_curl);
	if (inicio_rc != CURLE_OK) {
		snprintf(s->erro, sizeof s->erro, "falha ao iniciar a libcurl");
		return E_REDE;
	}
	ver = curl_version_info(CURLVERSION_NOW);
	if (!ver || !ver->ssl_version || !strstr(ver->ssl_version, "OpenSSL")) {
		snprintf(s->erro, sizeof s->erro,
		         "a libcurl precisa usar a OpenSSL para o certificado");
		return E_REDE;
	}

	n = strlen(msg);
	corpo = pula_declaracao(msg, &n);
	poe(&envelope, "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	               "<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/"
	               "XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/"
	               "2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/"
	               "2003/05/soap-envelope\">");
	if (cabecalho_soap) {
		poe(&envelope, "<soap12:Header>");
		poe(&envelope, cabecalho_soap);
		poe(&envelope, "</soap12:Header>");
	}
	poe(&envelope, "<soap12:Body><");
	poe(&envelope, elemento);
	poe(&envelope, " xmlns=\"");
	poe(&envelope, ns);
	poe(&envelope, "\">");
	poe_n(&envelope, corpo, n);
	poe(&envelope, "</");
	poe(&envelope, elemento);
	poe(&envelope, "></soap12:Body></soap12:Envelope>");
	poe(&cabecalho, "Content-Type: application/soap+xml; charset=utf-8; "
	                "action=\"");
	poe(&cabecalho, ns);
	poe(&cabecalho, "/");
	poe(&cabecalho, operacao);
	poe(&cabecalho, "\"");
	if (envelope.erro || cabecalho.erro) {
		free(envelope.p);
		free(cabecalho.p);
		return E_MALLOC;
	}

	c = curl_easy_init();
	h = curl_slist_append(NULL, cabecalho.p);
	h2 = h ? curl_slist_append(h, "Expect:") : NULL;
	if (!c || !h2) {
		curl_easy_cleanup(c);
		curl_slist_free_all(h);
		free(envelope.p);
		free(cabecalho.p);
		return E_MALLOC;
	}
	h = h2;
	errbuf[0] = '\0';
	curl_easy_setopt(c, CURLOPT_URL, url);
#if LIBCURL_VERSION_NUM >= 0x075500
	curl_easy_setopt(c, CURLOPT_PROTOCOLS_STR, "https");
#else
	curl_easy_setopt(c, CURLOPT_PROTOCOLS, (long)CURLPROTO_HTTPS);
#endif
	curl_easy_setopt(c, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(c, CURLOPT_TIMEOUT, s->timeout);
	curl_easy_setopt(c, CURLOPT_ERRORBUFFER, errbuf);
	curl_easy_setopt(c, CURLOPT_HTTPHEADER, h);
	curl_easy_setopt(c, CURLOPT_POSTFIELDS, envelope.p);
	curl_easy_setopt(c, CURLOPT_POSTFIELDSIZE_LARGE,
	                 (curl_off_t)envelope.n);
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, recebe);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, &recebido);
	curl_easy_setopt(c, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_2);
	curl_easy_setopt(c, CURLOPT_SSL_CTX_FUNCTION, poe_certificado);
	curl_easy_setopt(c, CURLOPT_SSL_CTX_DATA, (void *)s->cert);
	if (s->ca)
		curl_easy_setopt(c, CURLOPT_CAINFO, s->ca);

	cc = curl_easy_perform(c);
	curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http);
	curl_easy_cleanup(c);
	curl_slist_free_all(h);
	free(envelope.p);
	free(cabecalho.p);

	if (cc != CURLE_OK) {
		snprintf(s->erro, sizeof s->erro, "%s",
		         errbuf[0] ? errbuf : curl_easy_strerror(cc));
		free(recebido.p);
		return recebido.erro ? recebido.erro : E_REDE;
	}
	rc = le_resposta(s, &recebido, http, resposta, tam);
	free(recebido.p);
	return rc;
}

int nfe_sefaz_enviar(nfe_sefaz *s, const char *url, nfe_servico servico,
                     const char *msg, char **resposta, size_t *tam)
{
	char ns[128];

	if (!s || !url || !msg || !resposta)
		return E_ISNULL;
	s->erro[0] = '\0';
	if ((int)servico < 0 ||
	    (size_t)servico >= sizeof servicos / sizeof servicos[0])
		return E_VALOR;
	snprintf(ns, sizeof ns, "%s%s", NS_WSDL, servicos[servico].ns);
	return envia(s, url, ns, servicos[servico].operacao, "nfeDadosMsg",
	             NULL, msg, resposta, tam);
}

int nfe_sefaz_enviar_ws(nfe_sefaz *s, const char *url, const char *ns_wsdl,
                        const char *operacao, const char *elemento,
                        const char *cabecalho, const char *msg, char **resposta,
                        size_t *tam)
{
	if (!s || !url || !ns_wsdl || !operacao || !elemento || !msg ||
	    !resposta)
		return E_ISNULL;
	s->erro[0] = '\0';
	if (!texto_seguro(ns_wsdl) || !texto_seguro(operacao) ||
	    !nome_seguro(elemento))
		return E_VALOR;
	return envia(s, url, ns_wsdl, operacao, elemento, cabecalho, msg,
	             resposta, tam);
}

/* ---- mensagens ---- */

/* Chave de acesso (com letras nas posições do CNPJ alfanumérico) e dígito
 * verificador; 0 ou E_VALOR */
static int chave_valida(const char *chave)
{
	return nfe_valida_padrao(chave, NFE_PADRAO_TChNFe) == 0 &&
	                       nfe_chave_validar(chave) == 0
	               ? 0
	               : E_VALOR;
}

static int amb_valido(nfe_ambiente amb)
{
	return amb == NFE_AMBIENTE_PRODUCAO || amb == NFE_AMBIENTE_HOMOLOGACAO;
}

/* Grava a mensagem montada em fmt (com um texto) em *msg */
static int msg_simples(char **msg, const char *fmt, int amb, const char *a)
{
	struct buf b = { 0 };
	char tmp[512];
	int n = snprintf(tmp, sizeof tmp, fmt, amb, a);

	if (n < 0 || (size_t)n >= sizeof tmp)
		return E_VALOR;
	poe(&b, tmp);
	return entrega(&b, msg, NULL);
}

int nfe_sefaz_msg_status(nfe_ambiente amb, nfe_uf uf, char **msg)
{
	char cuf[4];

	if (!msg)
		return E_ISNULL;
	if (!amb_valido(amb) || nfe_uf_valida((int)uf) != 0)
		return E_VALOR;
	snprintf(cuf, sizeof cuf, "%02d", (int)uf);
	return msg_simples(msg,
	                   "<consStatServ xmlns=\"" NS_NFE "\" versao=\"4.00\">"
	                   "<tpAmb>%d</tpAmb><cUF>%s</cUF><xServ>STATUS</xServ>"
	                   "</consStatServ>",
	                   (int)amb, cuf);
}

int nfe_sefaz_msg_recibo(nfe_ambiente amb, const char *nrec, char **msg)
{
	if (!nrec || !msg)
		return E_ISNULL;
	if (!amb_valido(amb) || !so_digitos(nrec, 15, 15))
		return E_VALOR;
	return msg_simples(msg,
	                   "<consReciNFe xmlns=\"" NS_NFE "\" versao=\"4.00\">"
	                   "<tpAmb>%d</tpAmb><nRec>%s</nRec></consReciNFe>",
	                   (int)amb, nrec);
}

int nfe_sefaz_msg_consulta(nfe_ambiente amb, const char *chave, char **msg)
{
	if (!chave || !msg)
		return E_ISNULL;
	if (!amb_valido(amb) || chave_valida(chave) != 0)
		return E_VALOR;
	return msg_simples(msg,
	                   "<consSitNFe xmlns=\"" NS_NFE "\" versao=\"4.00\">"
	                   "<tpAmb>%d</tpAmb><xServ>CONSULTAR</xServ>"
	                   "<chNFe>%s</chNFe></consSitNFe>",
	                   (int)amb, chave);
}

int nfe_sefaz_msg_lote(const char *id_lote, int sincrono,
                       const char *const *nfes, int n, char **msg)
{
	struct buf b = { 0 };
	const char *nota;
	size_t tam;
	int i;

	if (!id_lote || !nfes || !msg)
		return E_ISNULL;
	if (!so_digitos(id_lote, 1, 15) || n < 1 || n > MAX_LOTE ||
	    (sincrono != 0 && sincrono != 1) || (sincrono && n > 1))
		return E_VALOR;
	for (i = 0; i < n; i++) {
		if (!nfes[i])
			return E_ISNULL;
		tam = strlen(nfes[i]);
		nota = pula_declaracao(nfes[i], &tam);
		if (!comeca_com(nota, tam, "NFe"))
			return E_VALOR;
	}
	poe(&b, "<enviNFe xmlns=\"" NS_NFE "\" versao=\"4.00\"><idLote>");
	poe(&b, id_lote);
	poe(&b, sincrono ? "</idLote><indSinc>1</indSinc>"
	                 : "</idLote><indSinc>0</indSinc>");
	for (i = 0; i < n; i++) {
		tam = strlen(nfes[i]);
		nota = pula_declaracao(nfes[i], &tam);
		poe_n(&b, nota, tam);
	}
	poe(&b, "</enviNFe>");
	return entrega(&b, msg, NULL);
}

int nfe_sefaz_msg_evento(const char *id_lote, const char *const *eventos, int n,
                         char **msg)
{
	struct buf b = { 0 };
	const char *ev;
	size_t tam;
	int i;

	if (!id_lote || !eventos || !msg)
		return E_ISNULL;
	if (!so_digitos(id_lote, 1, 15) || n < 1 || n > MAX_EVENTOS)
		return E_VALOR;
	for (i = 0; i < n; i++) {
		if (!eventos[i])
			return E_ISNULL;
		tam = strlen(eventos[i]);
		ev = pula_declaracao(eventos[i], &tam);
		if (!comeca_com(ev, tam, "evento"))
			return E_VALOR;
	}
	poe(&b, "<envEvento xmlns=\"" NS_NFE "\" versao=\"1.00\"><idLote>");
	poe(&b, id_lote);
	poe(&b, "</idLote>");
	for (i = 0; i < n; i++) {
		tam = strlen(eventos[i]);
		ev = pula_declaracao(eventos[i], &tam);
		poe_n(&b, ev, tam);
	}
	poe(&b, "</envEvento>");
	return entrega(&b, msg, NULL);
}

/* ---- retorno ---- */

int nfe_sefaz_cstat(const char *ret, size_t tam, int *cstat, char *xmotivo,
                    size_t tam_xmotivo)
{
	xmlDocPtr doc;
	xmlNodePtr raiz;
	const char *c;

	if (!ret || !cstat)
		return E_ISNULL;
	doc = le_xml(ret, tam);
	raiz = doc ? xmlDocGetRootElement(doc) : NULL;
	c = texto(raiz, "cStat");
	if (!*c) {
		/* Protocolo (protNFe/infProt) ou retorno de evento
		 * (retEvento/infEvento): o cStat fica no primeiro filho */
		xmlNodePtr inf = filho(raiz, NULL);

		if (inf && (xmlStrEqual(inf->name, BAD_CAST "infProt") ||
		            xmlStrEqual(inf->name, BAD_CAST "infEvento") ||
		            xmlStrEqual(inf->name, BAD_CAST "infInut"))) {
			raiz = inf;
			c = texto(raiz, "cStat");
		}
	}
	if (!so_digitos(c, 3, 4)) { /* TStat: 3 ou 4 dígitos */
		xmlFreeDoc(doc);
		return E_XML;
	}
	*cstat = atoi(c);
	if (xmotivo && tam_xmotivo > 0) {
		const char *m = texto(raiz, "xMotivo");
		size_t n = strlen(m);

		/* Truncado sem cortar um caractere UTF-8 ao meio */
		if (n >= tam_xmotivo) {
			n = tam_xmotivo - 1;
			while (n > 0 && ((unsigned char)m[n] & 0xC0) == 0x80)
				n--;
		}
		memcpy(xmotivo, m, n);
		xmotivo[n] = '\0';
	}
	xmlFreeDoc(doc);
	return 0;
}

/* protNFe com a chave (NULL: qualquer) em no ou nos descendentes */
static xmlNodePtr acha_protocolo(xmlNodePtr no, const char *chave)
{
	xmlNodePtr f, achado;

	for (f = no; f; f = f->next) {
		if (f->type != XML_ELEMENT_NODE)
			continue;
		if (xmlStrEqual(f->name, BAD_CAST "protNFe") &&
		    (!chave ||
		     strcmp(texto(filho(f, "infProt"), "chNFe"), chave) == 0))
			return f;
		achado = acha_protocolo(f->children, chave);
		if (achado)
			return achado;
	}
	return NULL;
}

int nfe_sefaz_protocolo(const char *ret, size_t tam, const char *chave,
                        char **prot, size_t *tam_prot)
{
	xmlDocPtr doc;
	xmlNodePtr p;
	int rc;

	if (!ret || !prot)
		return E_ISNULL;
	doc = le_xml(ret, tam);
	if (!doc)
		return E_XML;
	p = acha_protocolo(xmlDocGetRootElement(doc), chave);
	rc = p ? serializa(p, prot, tam_prot) : E_VALOR;
	xmlFreeDoc(doc);
	return rc;
}

/* Chave do Id de infNFe na nota (texto), em chave (45 bytes) */
static int chave_da_nota(const char *nfe, size_t tam, char *chave)
{
	static const char marca[] = "Id=\"NFe";
	size_t k = sizeof marca - 1, i;

	for (i = 0; i + k + 44 <= tam; i++)
		if (memcmp(nfe + i, marca, k) == 0) {
			memcpy(chave, nfe + i + k, 44);
			chave[44] = '\0';
			return chave_valida(chave) == 0 ? 0 : E_XML;
		}
	return E_XML;
}

int nfe_sefaz_proc(const char *nfe, size_t tam_nfe, const char *prot,
                   size_t tam_prot, char **proc, size_t *tam_proc)
{
	char chave[45];
	struct buf b = { 0 };
	const char *nota, *p;
	xmlDocPtr doc;
	xmlNodePtr raiz;
	int igual;

	if (!nfe || !prot || !proc)
		return E_ISNULL;
	nota = pula_declaracao(nfe, &tam_nfe);
	p = pula_declaracao(prot, &tam_prot);
	if (!comeca_com(nota, tam_nfe, "NFe") ||
	    chave_da_nota(nota, tam_nfe, chave) != 0)
		return E_XML;
	/* O protocolo pode vir com prefixo de namespace (<n:protNFe>) */
	doc = le_xml(p, tam_prot);
	raiz = doc ? xmlDocGetRootElement(doc) : NULL;
	if (!raiz || !xmlStrEqual(raiz->name, BAD_CAST "protNFe")) {
		xmlFreeDoc(doc);
		return E_XML;
	}
	igual = strcmp(texto(filho(raiz, "infProt"), "chNFe"), chave) == 0;
	xmlFreeDoc(doc);
	if (!igual)
		return E_VALOR;
	poe(&b,
	    "<?xml version=\"1.0\" encoding=\"UTF-8\"?><nfeProc xmlns=\"" NS_NFE
	    "\" versao=\"4.00\">");
	poe_n(&b, nota, tam_nfe);
	poe_n(&b, p, tam_prot);
	poe(&b, "</nfeProc>");
	return entrega(&b, proc, tam_proc);
}

/* retEvento com a chave, o tipo e a sequência em no ou nos descendentes */
static xmlNodePtr acha_ret_evento(xmlNodePtr no, const char *chave,
                                  const char *tipo, const char *seq)
{
	xmlNodePtr f, inf, achado;

	for (f = no; f; f = f->next) {
		if (f->type != XML_ELEMENT_NODE)
			continue;
		if (xmlStrEqual(f->name, BAD_CAST "retEvento")) {
			inf = filho(f, "infEvento");
			if (strcmp(texto(inf, "chNFe"), chave) == 0 &&
			    strcmp(texto(inf, "tpEvento"), tipo) == 0 &&
			    strcmp(texto(inf, "nSeqEvento"), seq) == 0)
				return f;
			continue;
		}
		achado = acha_ret_evento(f->children, chave, tipo, seq);
		if (achado)
			return achado;
	}
	return NULL;
}

int nfe_sefaz_proc_evento(const char *evento, size_t tam_evento,
                          const char *ret, size_t tam_ret, char **proc,
                          size_t *tam_proc)
{
	struct buf b = { 0 };
	const char *ev;
	xmlDocPtr doc_ev, doc_ret;
	xmlNodePtr inf, r;
	char *ret_ev = NULL;
	size_t tam_ret_ev = 0;
	int rc;

	if (!evento || !ret || !proc)
		return E_ISNULL;
	ev = pula_declaracao(evento, &tam_evento);
	if (!comeca_com(ev, tam_evento, "evento"))
		return E_XML;
	doc_ev = le_xml(ev, tam_evento);
	inf = doc_ev ? filho(xmlDocGetRootElement(doc_ev), "infEvento") : NULL;
	doc_ret = le_xml(ret, tam_ret);
	if (!inf || !doc_ret) {
		xmlFreeDoc(doc_ev);
		xmlFreeDoc(doc_ret);
		return E_XML;
	}
	r = acha_ret_evento(xmlDocGetRootElement(doc_ret), texto(inf, "chNFe"),
	                    texto(inf, "tpEvento"), texto(inf, "nSeqEvento"));
	rc = r ? serializa(r, &ret_ev, &tam_ret_ev) : E_VALOR;
	xmlFreeDoc(doc_ev);
	xmlFreeDoc(doc_ret);
	if (rc != 0)
		return rc;
	poe(&b, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><procEventoNFe "
	        "xmlns=\"" NS_NFE "\" versao=\"1.00\">");
	poe_n(&b, ev, tam_evento);
	poe_n(&b, ret_ev, tam_ret_ev);
	poe(&b, "</procEventoNFe>");
	free(ret_ev);
	return entrega(&b, proc, tam_proc);
}

/* retInutNFe em no ou nos descendentes */
static xmlNodePtr acha_ret_inut(xmlNodePtr no)
{
	xmlNodePtr f, achado;

	for (f = no; f; f = f->next) {
		if (f->type != XML_ELEMENT_NODE)
			continue;
		if (xmlStrEqual(f->name, BAD_CAST "retInutNFe"))
			return f;
		achado = acha_ret_inut(f->children);
		if (achado)
			return achado;
	}
	return NULL;
}

int nfe_sefaz_proc_inutilizacao(const char *inut, size_t tam_inut,
                                const char *ret, size_t tam_ret, char **proc,
                                size_t *tam_proc)
{
	static const char *const campos[] = { "cUF",   "ano",    "CNPJ",  "mod",
		                              "serie", "nNFIni", "nNFFin" };
	struct buf b = { 0 };
	const char *in, *v;
	xmlDocPtr doc_in, doc_ret;
	xmlNodePtr inf, r;
	char *ret_inut = NULL;
	size_t tam_ret_inut = 0, i;
	int rc;

	if (!inut || !ret || !proc)
		return E_ISNULL;
	in = pula_declaracao(inut, &tam_inut);
	if (!comeca_com(in, tam_inut, "inutNFe"))
		return E_XML;
	doc_in = le_xml(in, tam_inut);
	inf = doc_in ? filho(xmlDocGetRootElement(doc_in), "infInut") : NULL;
	doc_ret = le_xml(ret, tam_ret);
	if (!inf || !doc_ret) {
		xmlFreeDoc(doc_in);
		xmlFreeDoc(doc_ret);
		return E_XML;
	}
	r = acha_ret_inut(xmlDocGetRootElement(doc_ret));
	rc = r ? 0 : E_VALOR;
	/* O retorno tem de ser da mesma faixa (os campos que ele trouxer) */
	for (i = 0; rc == 0 && i < sizeof campos / sizeof campos[0]; i++) {
		v = texto(filho(r, "infInut"), campos[i]);
		if (*v && strcmp(v, texto(inf, campos[i])) != 0)
			rc = E_VALOR;
	}
	if (rc == 0)
		rc = serializa(r, &ret_inut, &tam_ret_inut);
	xmlFreeDoc(doc_in);
	xmlFreeDoc(doc_ret);
	if (rc != 0)
		return rc;
	poe(&b, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ProcInutNFe "
	        "xmlns=\"" NS_NFE "\" versao=\"4.00\">");
	poe_n(&b, in, tam_inut);
	poe_n(&b, ret_inut, tam_ret_inut);
	poe(&b, "</ProcInutNFe>");
	free(ret_inut);
	return entrega(&b, proc, tam_proc);
}
