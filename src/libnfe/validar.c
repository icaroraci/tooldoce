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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

#include <libnfe/chave.h>
#include <libnfe/decimal.h>
#include <libnfe/erros.h>
#include <libnfe/validar.h>

#ifndef NFE_DIR_SCHEMAS
#define NFE_DIR_SCHEMAS "/usr/local/share/tooldoce/schemas"
#endif

#define NS_NFE "http://www.portalfiscal.inf.br/nfe"
#define NS_DS  "http://www.w3.org/2000/09/xmldsig#"

/* Assinatura de mentira, com a estrutura exigida pelo schema, usada só na
 * cópia do documento que é validada quando ele ainda não está assinado */
static const char ASSINATURA[] =
        "<Signature xmlns=\"" NS_DS "\"><SignedInfo>"
        "<CanonicalizationMethod Algorithm=\"http://www.w3.org/TR/2001/"
        "REC-xml-c14n-20010315\"/><SignatureMethod Algorithm=\"" NS_DS
        "rsa-sha1\"/><Reference URI=\"#NFe\"><Transforms><Transform "
        "Algorithm=\"" NS_DS "enveloped-signature\"/><Transform Algorithm="
        "\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315\"/></Transforms>"
        "<DigestMethod Algorithm=\"" NS_DS "sha1\"/><DigestValue>AAAA"
        "</DigestValue></Reference></SignedInfo><SignatureValue>AAAA"
        "</SignatureValue><KeyInfo><X509Data><X509Certificate>AAAA"
        "</X509Certificate></X509Data></KeyInfo></Signature>";

struct erro_s {
	char *msg;
	char *campo;
	char *caminho, *valor, *restricao, *esperado;
	int linha;
	int codigo;
	int dominio_xml, codigo_xml;
};

struct nfe_erros {
	struct erro_s *e;
	int n, cap;
	int sem_memoria;
};

struct nfe_validador {
	xmlSchemaPtr schema;
};

nfe_erros *nfe_erros_new(void)
{
	return (nfe_erros *)calloc(1, sizeof(nfe_erros));
}

void nfe_erros_limpa(nfe_erros *erros)
{
	int i;

	if (!erros)
		return;
	for (i = 0; i < erros->n; i++) {
		free(erros->e[i].msg);
		free(erros->e[i].campo);
		free(erros->e[i].caminho);
		free(erros->e[i].valor);
		free(erros->e[i].restricao);
		free(erros->e[i].esperado);
	}
	erros->n = 0;
	erros->sem_memoria = 0;
}

void nfe_erros_free(nfe_erros *erros)
{
	if (!erros)
		return;
	nfe_erros_limpa(erros);
	free(erros->e);
	free(erros);
}

int nfe_erros_qtd(const nfe_erros *erros)
{
	return erros ? erros->n : 0;
}

const char *nfe_erros_msg(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].msg : NULL;
}

const char *nfe_erros_campo(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].campo : NULL;
}

int nfe_erros_linha(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].linha : 0;
}

int nfe_erros_codigo(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].codigo : 0;
}

const char *nfe_erros_caminho(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].caminho : NULL;
}

const char *nfe_erros_valor(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].valor : NULL;
}

const char *nfe_erros_restricao(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].restricao : NULL;
}

const char *nfe_erros_esperado(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].esperado : NULL;
}

int nfe_erros_dominio_xml(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].dominio_xml : 0;
}

int nfe_erros_codigo_xml(const nfe_erros *erros, int i)
{
	return erros && i >= 0 && i < erros->n ? erros->e[i].codigo_xml : 0;
}

/* Cópia exata: valores e regex podem terminar com uma quebra de linha. */
static char *duplica(const char *s)
{
	char *c;
	size_t n;

	if (!s)
		return NULL;
	n = strlen(s) + 1;
	c = malloc(n);
	if (c)
		memcpy(c, s, n);
	return c;
}

/* Cópia de s (NULL vira NULL); tira a quebra de linha do fim */
static char *copia(const char *s)
{
	char *c;
	size_t n;

	if (!s)
		return NULL;
	n = strlen(s);
	while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
		n--;
	c = (char *)malloc(n + 1);
	if (c) {
		memcpy(c, s, n);
		c[n] = '\0';
	}
	return c;
}

static struct erro_s *acrescenta(nfe_erros *erros, const char *msg,
                                 const char *campo, int linha, int codigo)
{
	struct erro_s *e;

	if (!erros)
		return NULL;
	if (erros->n == erros->cap) {
		int cap = erros->cap ? erros->cap * 2 : 8;
		e = (struct erro_s *)realloc(
		        erros->e, (size_t)cap * sizeof(struct erro_s));
		if (!e) {
			erros->sem_memoria = 1;
			return NULL;
		}
		erros->e = e;
		erros->cap = cap;
	}
	e = &erros->e[erros->n];
	memset(e, 0, sizeof *e);
	e->msg = copia(msg ? msg : "erro");
	e->campo = duplica(campo);
	e->linha = linha;
	e->codigo = codigo;
	if (!e->msg || (campo && !e->campo)) {
		free(e->msg);
		free(e->campo);
		erros->sem_memoria = 1;
		return NULL;
	}
	erros->n++;
	return e;
}

static int mesmo_nome(xmlNodePtr a, xmlNodePtr b)
{
	return a->type == b->type && xmlStrEqual(a->name, b->name) &&
	       xmlStrEqual(a->ns ? a->ns->href : NULL,
	                   b->ns ? b->ns->href : NULL);
}

/* Não usa xmlGetNodePath: namespaces padrão podem virar curingas. */
static int caminho_no(xmlBufferPtr b, xmlNodePtr no)
{
	xmlNodePtr irmao;
	int pos = 1, total = 0;
	char indice[32];

	if (!no ||
	    (no->type != XML_ELEMENT_NODE && no->type != XML_ATTRIBUTE_NODE))
		return 0;
	if (caminho_no(b, no->parent) != 0 ||
	    xmlBufferCat(b, BAD_CAST "/") != 0)
		return -1;
	if (no->type == XML_ATTRIBUTE_NODE &&
	    xmlBufferCat(b, BAD_CAST "@") != 0)
		return -1;
	if (no->ns && no->ns->prefix &&
	    (xmlBufferCat(b, no->ns->prefix) != 0 ||
	     xmlBufferCat(b, BAD_CAST ":") != 0))
		return -1;
	if (xmlBufferCat(b, no->name) != 0)
		return -1;
	if (no->type == XML_ELEMENT_NODE && no->parent) {
		for (irmao = no->parent->children; irmao; irmao = irmao->next)
			if (mesmo_nome(no, irmao)) {
				total++;
				if (irmao == no)
					pos = total;
			}
		if (total > 1) {
			snprintf(indice, sizeof indice, "[%d]", pos);
			if (xmlBufferCat(b, BAD_CAST indice) != 0)
				return -1;
		}
	}
	return 0;
}

static void detalhe_no(nfe_erros *erros, struct erro_s *e, xmlNodePtr no)
{
	xmlBufferPtr b;
	xmlNodePtr filho;
	xmlChar *valor;

	if (!no ||
	    (no->type != XML_ELEMENT_NODE && no->type != XML_ATTRIBUTE_NODE))
		return;
	b = xmlBufferCreate();
	if (!b || caminho_no(b, no) != 0) {
		xmlBufferFree(b);
		erros->sem_memoria = 1;
		return;
	}
	e->caminho = duplica((const char *)xmlBufferContent(b));
	xmlBufferFree(b);
	if (!e->caminho)
		erros->sem_memoria = 1;
	if (!e->linha)
		e->linha = (int)xmlGetLineNo(
		        no->type == XML_ATTRIBUTE_NODE ? no->parent : no);
	for (filho = no->children; filho; filho = filho->next)
		if (filho->type == XML_ELEMENT_NODE)
			return;
	valor = xmlNodeGetContent(no);
	e->valor = duplica((const char *)valor);
	xmlFree(valor);
	if (!e->valor)
		erros->sem_memoria = 1;
}

/* Adaptador das facetas estruturadas da libxml2. O próprio validador
 * resolve
 * includes, imports, tipos anônimos e herança. Nunca interpreta
 * mensagem em
 * inglês nem tenta validar novamente ou consultar por tag. */
static const char *faceta(int codigo)
{
	switch (codigo) {
	case XML_SCHEMAV_CVC_PATTERN_VALID:
		return "pattern";
	case XML_SCHEMAV_CVC_ENUMERATION_VALID:
		return "enumeration";
	case XML_SCHEMAV_CVC_LENGTH_VALID:
		return "length";
	case XML_SCHEMAV_CVC_MINLENGTH_VALID:
		return "minLength";
	case XML_SCHEMAV_CVC_MAXLENGTH_VALID:
		return "maxLength";
	case XML_SCHEMAV_CVC_MININCLUSIVE_VALID:
		return "minInclusive";
	case XML_SCHEMAV_CVC_MAXINCLUSIVE_VALID:
		return "maxInclusive";
	case XML_SCHEMAV_CVC_MINEXCLUSIVE_VALID:
		return "minExclusive";
	case XML_SCHEMAV_CVC_MAXEXCLUSIVE_VALID:
		return "maxExclusive";
	case XML_SCHEMAV_CVC_TOTALDIGITS_VALID:
		return "totalDigits";
	case XML_SCHEMAV_CVC_FRACTIONDIGITS_VALID:
		return "fractionDigits";
	default:
		return NULL;
	}
}

/* Tratador de erros da libxml2: guarda o erro na lista */
#if LIBXML_VERSION >= 21200
static void guarda_erro(void *ctx, const xmlError *erro)
#else
static void guarda_erro(void *ctx, xmlErrorPtr erro)
#endif
{
	const char *campo = NULL;
	const char *restricao, *esperado;
	struct erro_s *e;
	nfe_erros *erros = ctx;
	xmlNodePtr no;

	if (!erro || !erros)
		return;
	/* Após falha de parsing, a árvore parcial pode já ter sido liberada. */
	no = erro->domain == XML_FROM_SCHEMASV ? erro->node : NULL;
	if (no &&
	    (no->type == XML_ELEMENT_NODE || no->type == XML_ATTRIBUTE_NODE))
		campo = (const char *)no->name;
	e = acrescenta(erros, erro->message, campo, erro->line, 0);
	if (!e)
		return;
	e->dominio_xml = erro->domain;
	e->codigo_xml = erro->code;
	detalhe_no(erros, e, no);
	if (erro->domain != XML_FROM_SCHEMASV)
		return;
	restricao = faceta(erro->code);
	if (!restricao)
		return;
	esperado = erro->str2;
	/* Comprimento de atributo: str1=valor, str2=tamanho, str3=limite.
	 * Algumas versões da libxml2 substituem o nó atributo pelo pai. */
	if (erro->str3 && (erro->code == XML_SCHEMAV_CVC_LENGTH_VALID ||
	                   erro->code == XML_SCHEMAV_CVC_MINLENGTH_VALID ||
	                   erro->code == XML_SCHEMAV_CVC_MAXLENGTH_VALID)) {
		esperado = erro->str3;
		free(e->valor);
		e->valor = duplica(erro->str1);
		if (erro->str1 && !e->valor)
			erros->sem_memoria = 1;
	} else if (!e->valor && erro->str1 &&
	           erro->code != XML_SCHEMAV_CVC_LENGTH_VALID &&
	           erro->code != XML_SCHEMAV_CVC_MINLENGTH_VALID &&
	           erro->code != XML_SCHEMAV_CVC_MAXLENGTH_VALID) {
		e->valor = duplica(erro->str1);
		if (!e->valor)
			erros->sem_memoria = 1;
	}
	e->restricao = duplica(restricao);
	e->esperado = duplica(esperado);
	if (!e->restricao || (esperado && !e->esperado))
		erros->sem_memoria = 1;
}

const char *nfe_dir_schemas(void)
{
	return NFE_DIR_SCHEMAS;
}

/* Carrega o schema do arquivo caminho; NULL se não der */
static nfe_validador *carrega(const char *caminho)
{
	xmlSchemaParserCtxtPtr pctx;
	nfe_validador *v;

	/* Sem o arquivo, a libxml2 imprimiria um aviso: confere antes */
	{
		FILE *f = fopen(caminho, "rb");

		if (!f)
			return NULL;
		fclose(f);
	}
	v = (nfe_validador *)calloc(1, sizeof(nfe_validador));
	if (!v)
		return NULL;
	pctx = xmlSchemaNewParserCtxt(caminho);
	if (pctx) {
		/* Erros de carga não são impressos */
		xmlSchemaSetParserStructuredErrors(pctx, guarda_erro, NULL);
		v->schema = xmlSchemaParse(pctx);
		xmlSchemaFreeParserCtxt(pctx);
	}
	if (!v->schema) {
		free(v);
		return NULL;
	}
	return v;
}

nfe_validador *nfe_validador_new(const char *dir_schemas)
{
	char caminho[4096];
	int n;

	if (!dir_schemas)
		dir_schemas = nfe_dir_schemas();
	n = snprintf(caminho, sizeof caminho, "%s/nfe_v4.00.xsd", dir_schemas);
	if (n < 0 || (size_t)n >= sizeof caminho)
		return NULL;
	return carrega(caminho);
}

nfe_validador *nfe_validador_xsd(const char *caminho_xsd)
{
	return caminho_xsd ? carrega(caminho_xsd) : NULL;
}

void nfe_validador_free(nfe_validador *v)
{
	if (!v)
		return;
	xmlSchemaFree(v->schema);
	free(v);
}

/* Acrescenta a assinatura de mentira ao fim da raiz, se não houver
 * assinatura. Retorna 0 ou E_MALLOC. */
static int completa_assinatura(xmlNodePtr raiz)
{
	xmlNodePtr filho, assinatura = NULL;
	xmlDocPtr frag;
	xmlNodePtr copia_no;

	for (filho = raiz->children; filho; filho = filho->next)
		if (filho->type == XML_ELEMENT_NODE &&
		    xmlStrEqual(filho->name, BAD_CAST "Signature"))
			return 0;
	frag = xmlReadMemory(ASSINATURA, (int)sizeof ASSINATURA - 1,
	                     "assinatura.xml", NULL,
	                     XML_PARSE_NONET | XML_PARSE_NOERROR);
	if (!frag)
		return E_MALLOC;
	assinatura = xmlDocGetRootElement(frag);
	copia_no = xmlDocCopyNode(assinatura, raiz->doc, 1);
	xmlFreeDoc(frag);
	if (!copia_no)
		return E_MALLOC;
	if (!xmlAddChild(raiz, copia_no)) {
		xmlFreeNode(copia_no);
		return E_MALLOC;
	}
	return 0;
}

/* Acrescenta a assinatura de mentira ao fim de <NFe>, se não houver
 * assinatura. Retorna 0, E_XML ou E_MALLOC. */
static int assina_de_mentira(xmlDocPtr doc)
{
	xmlNodePtr raiz = xmlDocGetRootElement(doc);

	if (!raiz || !xmlStrEqual(raiz->name, BAD_CAST "NFe") || !raiz->ns ||
	    !xmlStrEqual(raiz->ns->href, BAD_CAST NS_NFE))
		return E_XML;
	return completa_assinatura(raiz);
}

/* ---- Regras de validação da SEFAZ que o schema não cobre ---- */

/* Primeiro filho elemento de pai com o nome (NULL: qualquer nome) */
static xmlNodePtr filho(xmlNodePtr pai, const char *nome)
{
	xmlNodePtr f;

	for (f = pai ? pai->children : NULL; f; f = f->next)
		if (f->type == XML_ELEMENT_NODE &&
		    (!nome || xmlStrEqual(f->name, BAD_CAST nome)))
			return f;
	return NULL;
}

/* Elemento pelo caminho ("ide/cUF"; "*" vale qualquer nome) */
static xmlNodePtr no(xmlNodePtr pai, const char *caminho)
{
	char nome[64];
	const char *p = caminho, *fim;
	size_t n;

	while (pai && *p) {
		fim = strchr(p, '/');
		n = fim ? (size_t)(fim - p) : strlen(p);
		if (n >= sizeof nome)
			return NULL;
		memcpy(nome, p, n);
		nome[n] = '\0';
		pai = filho(pai, strcmp(nome, "*") == 0 ? NULL : nome);
		p += n;
		if (*p == '/')
			p++;
	}
	return pai;
}

/* Texto do elemento pelo caminho; "" se não houver */
static const char *texto(xmlNodePtr pai, const char *caminho)
{
	xmlNodePtr e = no(pai, caminho);

	if (e && e->children && e->children->type == XML_TEXT_NODE &&
	    e->children->content)
		return (const char *)e->children->content;
	return "";
}

/* Centavos do campo (0 se ausente) */
static long long valor(xmlNodePtr pai, const char *caminho)
{
	long long c = 0;

	if (nfe_dec2_ler(texto(pai, caminho), &c) != 0)
		return 0;
	return c;
}

static void regra(nfe_erros *erros, int *n, xmlNodePtr e, const char *campo,
                  int codigo, const char *msg)
{
	struct erro_s *erro = acrescenta(erros, msg, campo,
	                                 e ? (int)xmlGetLineNo(e) : 0, codigo);
	if (erro)
		detalhe_no(erros, erro, e);
	(*n)++;
}

/* Código IBGE da UF pela sigla; 0 se desconhecida */
static int codigo_uf(const char *uf)
{
	static const char *const siglas[] = {
		"RO", "AC", "AM", "RR", "PA", "AP", "TO", "MA", "PI",
		"CE", "RN", "PB", "PE", "AL", "SE", "BA", "MG", "ES",
		"RJ", "SP", "PR", "SC", "RS", "MS", "MT", "GO", "DF",
	};
	static const int codigos[] = {
		11, 12, 13, 14, 15, 16, 17, 21, 22, 23, 24, 25, 26, 27,
		28, 29, 31, 32, 33, 35, 41, 42, 43, 50, 51, 52, 53,
	};
	size_t i;

	for (i = 0; i < sizeof codigos / sizeof codigos[0]; i++)
		if (strcmp(uf, siglas[i]) == 0)
			return codigos[i];
	return 0;
}

/* Chave de acesso (Id) coerente com os campos de ide e emit */
static void regra_chave(xmlNodePtr inf, nfe_erros *erros, int *n)
{
	xmlNodePtr ide = filho(inf, "ide"), emit = filho(inf, "emit");
	xmlChar *id = xmlGetProp(inf, BAD_CAST "Id");
	const char *doc = texto(emit, "CNPJ"), *dh = texto(ide, "dhEmi");
	char esperada[64];

	if (!id || strlen((const char *)id) != 3 + 44) {
		xmlFree(id);
		return;
	}
	if (!*doc)
		doc = texto(emit, "CPF");
	if (nfe_chave_validar((const char *)id + 3) != 0)
		regra(erros, n, inf, "infNFe", 236,
		      "chave de acesso (Id) com dígito verificador inválido");
	if (strlen(dh) >= 7)
		snprintf(esperada, sizeof esperada,
		         "%s%.2s%.2s%14s%s%03d%09ld%s%s%s", texto(ide, "cUF"),
		         dh + 2, dh + 5, doc, texto(ide, "mod"),
		         atoi(texto(ide, "serie")), atol(texto(ide, "nNF")),
		         texto(ide, "tpEmis"), texto(ide, "cNF"),
		         texto(ide, "cDV"));
	else
		esperada[0] = '\0';
	/* %14s completa com espaços: troca por zeros (CPF) */
	{
		char *p;

		for (p = esperada; *p; p++)
			if (*p == ' ')
				*p = '0';
	}
	if (strcmp(esperada, (const char *)id + 3) != 0)
		regra(erros, n, inf, "infNFe", 502,
		      "chave de acesso (Id) não corresponde aos campos cUF, "
		      "dhEmi, CNPJ/CPF do emitente, mod, serie, nNF, tpEmis, "
		      "cNF e cDV");
	xmlFree(id);
}

/* Totais (ICMSTot) iguais à soma dos itens, e a fórmula de vNF */
static void regra_totais(xmlNodePtr inf, nfe_erros *erros, int *n)
{
	static const struct {
		const char *item, *total;
		int codigo;
		const char *msg;
	} somas[] = {
		{ "imposto/ICMS/*/vBC", "vBC", 531,
		  "vBC do total difere da soma das bases do ICMS dos itens" },
		{ "imposto/ICMS/*/vICMS", "vICMS", 532,
		  "vICMS do total difere da soma do ICMS dos itens" },
		{ "imposto/ICMS/*/vBCST", "vBCST", 533,
		  "vBCST do total difere da soma das bases do ICMS ST dos "
		  "itens" },
		{ "imposto/ICMS/*/vICMSST", "vST", 534,
		  "vST do total difere da soma do ICMS ST dos itens" },
	};
	enum { NSOMAS = sizeof somas / sizeof somas[0] };
	xmlNodePtr tot = no(inf, "total/ICMSTot"), det;
	long long soma[NSOMAS] = { 0 }, vprod = 0, vnf;
	size_t i;

	if (!tot)
		return;
	for (det = inf->children; det; det = det->next) {
		if (det->type != XML_ELEMENT_NODE ||
		    !xmlStrEqual(det->name, BAD_CAST "det"))
			continue;
		if (strcmp(texto(det, "prod/indTot"), "1") == 0)
			vprod += valor(det, "prod/vProd");
		for (i = 0; i < NSOMAS; i++)
			soma[i] += valor(det, somas[i].item);
	}
	if (vprod != valor(tot, "vProd"))
		regra(erros, n, no(tot, "vProd"), "vProd", 564,
		      "vProd do total difere da soma de vProd dos itens com "
		      "indTot=1");
	for (i = 0; i < NSOMAS; i++)
		if (soma[i] != valor(tot, somas[i].total))
			regra(erros, n, no(tot, somas[i].total), somas[i].total,
			      somas[i].codigo, somas[i].msg);
	vnf = valor(tot, "vProd") - valor(tot, "vDesc") + valor(tot, "vST") +
	      valor(tot, "vFCPST") + valor(tot, "vFrete") + valor(tot, "vSeg") +
	      valor(tot, "vOutro") + valor(tot, "vII") + valor(tot, "vIPI") +
	      valor(tot, "vIPIDevol") + valor(inf, "total/ISSQNtot/vServ");
	/* Desonerado: há casos em que é deduzido do total e casos em que
	 * não; os dois são aceitos */
	if (valor(tot, "vNF") != vnf &&
	    valor(tot, "vNF") != vnf - valor(tot, "vICMSDeson"))
		regra(erros, n, no(tot, "vNF"), "vNF", 610,
		      "vNF difere de vProd - vDesc (- vICMSDeson) + vST + "
		      "vFCPST + vFrete + vSeg + vOutro + vII + vIPI + "
		      "vIPIDevol + vServ");
}

/* UF do emitente e município do fato gerador coerentes com cUF */
static void regra_uf(xmlNodePtr inf, nfe_erros *erros, int *n)
{
	xmlNodePtr ide = filho(inf, "ide");
	int cuf = atoi(texto(ide, "cUF"));

	if (codigo_uf(texto(inf, "emit/enderEmit/UF")) != cuf)
		regra(erros, n, no(inf, "emit/enderEmit/UF"), "UF", 0,
		      "UF do emitente diferente da UF de cUF");
	if (atoi(texto(inf, "emit/enderEmit/cMun")) / 100000 != cuf)
		regra(erros, n, no(inf, "emit/enderEmit/cMun"), "cMun", 0,
		      "município do emitente fora da UF de cUF");
	if (atoi(texto(ide, "cMunFG")) / 100000 != cuf)
		regra(erros, n, no(ide, "cMunFG"), "cMunFG", 0,
		      "município do fato gerador fora da UF de cUF");
}

/* v é um dos códigos de um caractere em opcoes ("" não é nenhum) */
static int um_de(const char *v, const char *opcoes)
{
	return v[0] && !v[1] && strchr(opcoes, v[0]) != NULL;
}

/* Forma de emissão (tpEmis) e entrada em contingência (dhCont e xJust) */
static void regra_contingencia(xmlNodePtr inf, nfe_erros *erros, int *n)
{
	xmlNodePtr ide = filho(inf, "ide"), tpemis = no(ide, "tpEmis");
	xmlNodePtr dhcont = no(ide, "dhCont"), xjust = no(ide, "xJust");
	const char *te = texto(ide, "tpEmis");
	int nfce = strcmp(texto(ide, "mod"), "65") == 0;

	if (strcmp(te, "1") == 0 && (dhcont || xjust))
		regra(erros, n, dhcont ? dhcont : xjust,
		      dhcont ? "dhCont" : "xJust", 556,
		      "dhCont e xJust não devem ser informados na emissão "
		      "normal (tpEmis=1)");
	if (um_de(te, "2459") && (!dhcont || !xjust))
		regra(erros, n, tpemis, "tpEmis", 557,
		      "emissão em contingência sem dhCont e xJust");
	if (strcmp(te, "3") == 0)
		regra(erros, n, tpemis, "tpEmis", 570,
		      "contingência SCAN (tpEmis=3) não existe mais");
	if (!nfce && strcmp(te, "9") == 0)
		regra(erros, n, tpemis, "tpEmis", 711,
		      "NF-e não tem contingência off-line (tpEmis=9)");
	if (nfce && um_de(te, "25"))
		regra(erros, n, tpemis, "tpEmis", 714,
		      "NFC-e não aceita contingência em formulário de "
		      "segurança (tpEmis 2 ou 5)");
	if (nfce && um_de(te, "67"))
		regra(erros, n, tpemis, "tpEmis", 783,
		      "NFC-e não é autorizada pela SVC (tpEmis 6 ou 7)");
}

/* Série compatível com o processo de emissão (contribuinte ou Fisco) */
static void regra_serie(xmlNodePtr inf, nfe_erros *erros, int *n)
{
	xmlNodePtr ide = filho(inf, "ide");
	int serie = atoi(texto(ide, "serie"));

	if (um_de(texto(ide, "procEmi"), "12")) {
		if (serie < 890 || serie > 919)
			regra(erros, n, no(ide, "serie"), "serie", 451,
			      "emissão pelo Fisco deve usar série de 890 a "
			      "919");
	} else if (serie > 969 || (serie >= 890 && serie <= 919)) {
		regra(erros, n, no(ide, "serie"), "serie", 244,
		      "série fora das faixas do contribuinte (0 a 889 e "
		      "920 a 969)");
	}
}

/* Indicativo do intermediador conforme o indicativo de presença */
static void regra_intermediador(xmlNodePtr inf, nfe_erros *erros, int *n)
{
	xmlNodePtr ide = filho(inf, "ide"), ind = no(ide, "indIntermed");

	if (um_de(texto(ide, "indPres"), "2349")) {
		if (!ind)
			regra(erros, n, no(ide, "indPres"), "indIntermed", 434,
			      "indIntermed obrigatório para indPres 2, "
			      "3, 4 ou 9");
	} else if (ind) {
		regra(erros, n, ind, "indIntermed", 435,
		      "indIntermed só é informado para indPres 2, 3, 4 ou 9");
	}
}

/* NF-e: sem o que é próprio da NFC-e (DANFE NFC-e e entrega a domicílio) */
static void regra_nfe(xmlNodePtr ide, nfe_erros *erros, int *n)
{
	if (um_de(texto(ide, "tpImp"), "45"))
		regra(erros, n, no(ide, "tpImp"), "tpImp", 710,
		      "NF-e não usa DANFE NFC-e (tpImp 4 ou 5)");
	if (strcmp(texto(ide, "indPres"), "4") == 0)
		regra(erros, n, no(ide, "indPres"), "indPres", 794,
		      "NF-e não usa entrega a domicílio (indPres=4), que é "
		      "da NFC-e");
}

/* NFC-e: saída para consumidor final, interna, presencial e sem
 * documento referenciado, com DANFE NFC-e */
static void regra_nfce(xmlNodePtr inf, nfe_erros *erros, int *n)
{
	xmlNodePtr ide = filho(inf, "ide");

	if (strcmp(texto(ide, "mod"), "65") != 0) {
		regra_nfe(ide, erros, n);
		return;
	}
	if (strcmp(texto(ide, "tpNF"), "0") == 0)
		regra(erros, n, no(ide, "tpNF"), "tpNF", 706,
		      "NFC-e não pode ser de entrada (tpNF=0)");
	if (strcmp(texto(ide, "idDest"), "1") != 0)
		regra(erros, n, no(ide, "idDest"), "idDest", 707,
		      "NFC-e deve ser de operação interna (idDest=1)");
	if (no(ide, "NFref"))
		regra(erros, n, no(ide, "NFref"), "NFref", 708,
		      "NFC-e não pode referenciar documento fiscal (NFref)");
	if (!um_de(texto(ide, "tpImp"), "45"))
		regra(erros, n, no(ide, "tpImp"), "tpImp", 709,
		      "NFC-e deve ter DANFE NFC-e (tpImp 4 ou 5)");
	if (strcmp(texto(ide, "finNFe"), "1") != 0)
		regra(erros, n, no(ide, "finNFe"), "finNFe", 715,
		      "NFC-e deve ter finalidade normal (finNFe=1)");
	if (strcmp(texto(ide, "indFinal"), "1") != 0)
		regra(erros, n, no(ide, "indFinal"), "indFinal", 716,
		      "NFC-e deve ser para consumidor final (indFinal=1)");
	if (!um_de(texto(ide, "indPres"), "14"))
		regra(erros, n, no(ide, "indPres"), "indPres", 717,
		      "NFC-e deve ser presencial ou entrega a domicílio "
		      "(indPres 1 ou 4)");
}

/* Aplica as regras; retorna a quantidade de problemas */
static int regras(xmlNodePtr raiz, nfe_erros *erros)
{
	xmlNodePtr inf = filho(raiz, "infNFe");
	int n = 0;

	if (!inf)
		return 0;
	regra_chave(inf, erros, &n);
	regra_totais(inf, erros, &n);
	regra_uf(inf, erros, &n);
	regra_contingencia(inf, erros, &n);
	regra_serie(inf, erros, &n);
	regra_intermediador(inf, erros, &n);
	regra_nfce(inf, erros, &n);
	return n;
}

/* Lê o documento, acrescentando o erro de leitura em erros; NULL se
 * malformado ou sem memória (*rc: E_XML ou E_MALLOC) */
static xmlDocPtr le(const char *xml, size_t tam, nfe_erros *erros, int *rc)
{
	xmlParserCtxtPtr pctx;
	xmlDocPtr doc;

	*rc = E_XML;
	if (tam > 0x7fffffff)
		return NULL;
	pctx = xmlNewParserCtxt();
	if (!pctx) {
		*rc = E_MALLOC;
		return NULL;
	}
	/* Sem impressão: o erro de leitura é pego do contexto */
	doc = xmlCtxtReadMemory(pctx, xml, (int)tam, "nfe.xml", NULL,
	                        XML_PARSE_NONET | XML_PARSE_NOERROR |
	                                XML_PARSE_NOWARNING);
	if (!doc) {
		const xmlError *e = xmlCtxtGetLastError(pctx);

		if (e)
			guarda_erro(erros, (xmlErrorPtr)e);
		else
			acrescenta(erros, "XML malformado", NULL, 0, 0);
		if (erros && erros->sem_memoria)
			*rc = E_MALLOC;
	}
	xmlFreeParserCtxt(pctx);
	return doc;
}

/* Valida doc contra o schema de v; 0, 1 (inválido) ou -1 (erro interno) */
static int valida_schema(nfe_validador *v, xmlDocPtr doc, nfe_erros *erros)
{
	xmlSchemaValidCtxtPtr ctx = xmlSchemaNewValidCtxt(v->schema);
	int rc;

	if (!ctx)
		return -1;
	xmlSchemaSetValidStructuredErrors(ctx, guarda_erro, erros);
	rc = xmlSchemaValidateDoc(ctx, doc);
	xmlSchemaFreeValidCtxt(ctx);
	return rc < 0 || (erros && erros->sem_memoria) ? -1 : rc != 0;
}

int nfe_validar_xml(nfe_validador *v, const char *xml, size_t tam,
                    nfe_erros *erros)
{
	xmlDocPtr doc;
	int rc;

	if (!v || !xml)
		return E_ISNULL;
	nfe_erros_limpa(erros);
	doc = le(xml, tam, erros, &rc);
	if (!doc)
		return rc;
	rc = assina_de_mentira(doc);
	if (rc == E_XML)
		acrescenta(erros, "o elemento raiz não é <NFe> da NF-e", NULL,
		           0, 0);
	if (rc != 0) {
		xmlFreeDoc(doc);
		return erros && erros->sem_memoria ? E_MALLOC : rc;
	}
	rc = valida_schema(v, doc, erros);
	/* As regras supõem a estrutura do schema: só depois dele */
	if (rc == 0 && regras(xmlDocGetRootElement(doc), erros) > 0)
		rc = 1;
	xmlFreeDoc(doc);
	if (rc < 0 || (erros && erros->sem_memoria))
		return E_MALLOC;
	return rc == 0 ? 0 : E_VALOR;
}

int nfe_validar_xsd(nfe_validador *v, const char *xml, size_t tam,
                    int completar_assinatura, nfe_erros *erros)
{
	xmlDocPtr doc;
	int rc;

	if (!v || !xml)
		return E_ISNULL;
	nfe_erros_limpa(erros);
	doc = le(xml, tam, erros, &rc);
	if (!doc)
		return rc;
	if (completar_assinatura) {
		rc = completa_assinatura(xmlDocGetRootElement(doc));
		if (rc != 0) {
			xmlFreeDoc(doc);
			return rc;
		}
	}
	rc = valida_schema(v, doc, erros);
	xmlFreeDoc(doc);
	if (rc < 0)
		return E_MALLOC;
	return rc == 0 ? 0 : E_VALOR;
}
