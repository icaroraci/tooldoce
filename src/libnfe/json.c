/* Copyright (c) 2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with tooldoce.  If not, see <https://www.gnu.org/licenses/>.
 * */

#include <stdlib.h>
#include <string.h>

#include <libnfe/erros.h>
#include <libnfe/json.h>

struct nfe_json {
	nfe_json_tipo tipo;
	char *texto;      /* texto, número ou lógico */
	size_t n, cap;    /* itens ou campos */
	nfe_json **itens; /* valores */
	char **nomes;     /* nomes dos campos (só objeto) */
};

/* Texto em leitura */
struct leitor {
	const char *p, *fim;
	int prof;
};

static void pula_espacos(struct leitor *l)
{
	while (l->p < l->fim && (*l->p == ' ' || *l->p == '\t' ||
	                         *l->p == '\n' || *l->p == '\r'))
		l->p++;
}

static nfe_json *novo(nfe_json_tipo tipo)
{
	nfe_json *j = (nfe_json *)calloc(1, sizeof(nfe_json));

	if (j)
		j->tipo = tipo;
	return j;
}

/* Cópia alocada de t, ou NULL */
static char *copia(const char *t)
{
	char *c = (char *)malloc(strlen(t) + 1);

	if (c)
		strcpy(c, t);
	return c;
}

void nfe_json_free(nfe_json *j)
{
	size_t i;

	if (!j)
		return;
	for (i = 0; i < j->n; i++) {
		nfe_json_free(j->itens[i]);
		if (j->nomes)
			free(j->nomes[i]);
	}
	free(j->itens);
	free(j->nomes);
	free(j->texto);
	free(j);
}

/* Acrescenta v (e o nome, num objeto) a j; em caso de erro, não toma
 * posse. Retorna 0 ou E_MALLOC. */
static int acrescenta(nfe_json *j, char *nome, nfe_json *v)
{
	if (j->n == j->cap) {
		size_t cap = j->cap ? j->cap * 2 : 4;
		nfe_json **itens;

		itens = (nfe_json **)realloc(j->itens, cap * sizeof *itens);
		if (!itens)
			return E_MALLOC;
		j->itens = itens;
		if (j->tipo == NFE_JSON_OBJETO) {
			char **nomes;

			nomes = (char **)realloc(j->nomes, cap * sizeof *nomes);
			if (!nomes)
				return E_MALLOC;
			j->nomes = nomes;
		}
		j->cap = cap;
	}
	j->itens[j->n] = v;
	if (j->tipo == NFE_JSON_OBJETO)
		j->nomes[j->n] = nome;
	j->n++;
	return 0;
}

/* Valor de 4 dígitos hexadecimais em s; -1 se inválido */
static long hex4(const char *s)
{
	long v = 0;
	int i;

	for (i = 0; i < 4; i++) {
		char c = s[i];

		v <<= 4;
		if (c >= '0' && c <= '9')
			v |= c - '0';
		else if (c >= 'a' && c <= 'f')
			v |= c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			v |= c - 'A' + 10;
		else
			return -1;
	}
	return v;
}

/* Escreve o código u em UTF-8 em d; devolve os bytes escritos */
static size_t utf8(unsigned long u, char *d)
{
	if (u < 0x80) {
		d[0] = (char)u;
		return 1;
	}
	if (u < 0x800) {
		d[0] = (char)(0xc0 | u >> 6);
		d[1] = (char)(0x80 | (u & 0x3f));
		return 2;
	}
	if (u < 0x10000) {
		d[0] = (char)(0xe0 | u >> 12);
		d[1] = (char)(0x80 | (u >> 6 & 0x3f));
		d[2] = (char)(0x80 | (u & 0x3f));
		return 3;
	}
	d[0] = (char)(0xf0 | u >> 18);
	d[1] = (char)(0x80 | (u >> 12 & 0x3f));
	d[2] = (char)(0x80 | (u >> 6 & 0x3f));
	d[3] = (char)(0x80 | (u & 0x3f));
	return 4;
}

/* Caractere de um escape simples (\n, \" ...); 0 se não for um */
static char escape(char c)
{
	switch (c) {
	case '"':
	case '\\':
	case '/':
		return c;
	case 'b':
		return '\b';
	case 'f':
		return '\f';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 't':
		return '\t';
	}
	return 0;
}

/* Lê um texto entre aspas (l->p na aspa de abertura) em *dst, alocado.
 * Retorna 0, E_VALOR ou E_MALLOC. */
static int le_texto(struct leitor *l, char **dst)
{
	const char *ini = ++l->p, *q;
	char *t, *d;

	/* Acha a aspa de fechamento para dimensionar o texto; os escapes
	 * nunca ocupam mais que o original em UTF-8 */
	for (q = ini; q < l->fim && *q != '"'; q++)
		if (*q == '\\' && ++q == l->fim)
			return E_VALOR;
	if (q >= l->fim)
		return E_VALOR;
	t = (char *)malloc((size_t)(q - ini) + 1);
	if (!t)
		return E_MALLOC;
	d = t;
	while (l->p < q) {
		unsigned char c = (unsigned char)*l->p++;
		long u, u2;

		if (c < 0x20)
			goto invalido;
		if (c != '\\') {
			*d++ = (char)c;
			continue;
		}
		c = (unsigned char)*l->p++;
		if (c != 'u') {
			if (!escape((char)c))
				goto invalido;
			*d++ = escape((char)c);
			continue;
		}
		if (q - l->p < 4 || (u = hex4(l->p)) < 0)
			goto invalido;
		l->p += 4;
		if (u >= 0xd800 && u <= 0xdbff) {
			/* par substituto: \uD8xx\uDCxx */
			if (q - l->p < 6 || l->p[0] != '\\' || l->p[1] != 'u' ||
			    (u2 = hex4(l->p + 2)) < 0xdc00 || u2 > 0xdfff)
				goto invalido;
			l->p += 6;
			u = 0x10000 + ((u - 0xd800) << 10) + (u2 - 0xdc00);
		} else if ((u >= 0xdc00 && u <= 0xdfff) || u == 0) {
			goto invalido;
		}
		d += utf8((unsigned long)u, d);
	}
	*d = '\0';
	l->p = q + 1;
	*dst = t;
	return 0;
invalido:
	free(t);
	return E_VALOR;
}

static int digito(struct leitor *l, const char *p)
{
	return p < l->fim && *p >= '0' && *p <= '9';
}

/* Número JSON: -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)? */
static int le_numero(struct leitor *l, char **dst)
{
	const char *ini = l->p, *p = l->p;
	size_t n;

	if (p < l->fim && *p == '-')
		p++;
	if (!digito(l, p))
		return E_VALOR;
	if (*p == '0')
		p++;
	else
		while (digito(l, p))
			p++;
	if (p < l->fim && *p == '.') {
		if (!digito(l, ++p))
			return E_VALOR;
		while (digito(l, p))
			p++;
	}
	if (p < l->fim && (*p == 'e' || *p == 'E')) {
		p++;
		if (p < l->fim && (*p == '+' || *p == '-'))
			p++;
		if (!digito(l, p))
			return E_VALOR;
		while (digito(l, p))
			p++;
	}
	n = (size_t)(p - ini);
	*dst = (char *)malloc(n + 1);
	if (!*dst)
		return E_MALLOC;
	memcpy(*dst, ini, n);
	(*dst)[n] = '\0';
	l->p = p;
	return 0;
}

/* Palavra literal (true, false, null) em l->p? Avança sobre ela. */
static int palavra(struct leitor *l, const char *w)
{
	size_t n = strlen(w);

	if ((size_t)(l->fim - l->p) < n || memcmp(l->p, w, n) != 0)
		return 0;
	l->p += n;
	return 1;
}

static int le_valor(struct leitor *l, nfe_json **dst);

/* Itens de uma lista ou campos de um objeto (l->p em '[' ou '{') */
static int le_composto(struct leitor *l, nfe_json *j)
{
	char fecha = j->tipo == NFE_JSON_LISTA ? ']' : '}';
	int rc;

	if (++l->prof > NFE_JSON_PROFUNDIDADE)
		return E_VALOR;
	l->p++;
	pula_espacos(l);
	if (l->p < l->fim && *l->p == fecha) {
		l->p++;
		l->prof--;
		return 0;
	}
	for (;;) {
		char *nome = NULL;
		nfe_json *v = NULL;

		if (j->tipo == NFE_JSON_OBJETO) {
			if (l->p >= l->fim || *l->p != '"')
				return E_VALOR;
			rc = le_texto(l, &nome);
			if (rc)
				return rc;
			pula_espacos(l);
			if (l->p >= l->fim || *l->p != ':') {
				free(nome);
				return E_VALOR;
			}
			l->p++;
		}
		rc = le_valor(l, &v);
		if (rc == 0)
			rc = acrescenta(j, nome, v);
		if (rc) {
			free(nome);
			nfe_json_free(v);
			return rc;
		}
		pula_espacos(l);
		if (l->p < l->fim && *l->p == ',') {
			l->p++;
			pula_espacos(l);
			continue;
		}
		if (l->p < l->fim && *l->p == fecha) {
			l->p++;
			l->prof--;
			return 0;
		}
		return E_VALOR;
	}
}

static int le_valor(struct leitor *l, nfe_json **dst)
{
	nfe_json *j;
	int rc = 0;

	pula_espacos(l);
	if (l->p >= l->fim)
		return E_VALOR;
	switch (*l->p) {
	case '{':
	case '[':
		j = novo(*l->p == '{' ? NFE_JSON_OBJETO : NFE_JSON_LISTA);
		if (!j)
			return E_MALLOC;
		rc = le_composto(l, j);
		break;
	case '"':
		j = novo(NFE_JSON_TEXTO);
		if (!j)
			return E_MALLOC;
		rc = le_texto(l, &j->texto);
		break;
	case 't':
	case 'f':
		j = novo(NFE_JSON_LOGICO);
		if (!j)
			return E_MALLOC;
		if (palavra(l, "true"))
			j->texto = copia("true");
		else if (palavra(l, "false"))
			j->texto = copia("false");
		else
			rc = E_VALOR;
		if (rc == 0 && !j->texto)
			rc = E_MALLOC;
		break;
	case 'n':
		j = novo(NFE_JSON_NULO);
		if (!j)
			return E_MALLOC;
		if (!palavra(l, "null"))
			rc = E_VALOR;
		break;
	default:
		j = novo(NFE_JSON_NUMERO);
		if (!j)
			return E_MALLOC;
		rc = le_numero(l, &j->texto);
	}
	if (rc) {
		nfe_json_free(j);
		return rc;
	}
	*dst = j;
	return 0;
}

int nfe_json_ler(const char *texto, size_t tam, nfe_json **raiz)
{
	struct leitor l;
	nfe_json *j = NULL;
	int rc;

	if (!texto || !raiz)
		return E_ISNULL;
	l.p = texto;
	l.fim = texto + tam;
	l.prof = 0;
	rc = le_valor(&l, &j);
	if (rc)
		return rc;
	pula_espacos(&l);
	if (l.p != l.fim) {
		nfe_json_free(j);
		return E_VALOR;
	}
	*raiz = j;
	return 0;
}

nfe_json_tipo nfe_json_tipo_de(const nfe_json *j)
{
	return j ? j->tipo : NFE_JSON_NULO;
}

const char *nfe_json_texto(const nfe_json *j)
{
	return j ? j->texto : NULL;
}

size_t nfe_json_qtd(const nfe_json *j)
{
	return j ? j->n : 0;
}

const nfe_json *nfe_json_item(const nfe_json *j, size_t i)
{
	return j && i < j->n ? j->itens[i] : NULL;
}

const char *nfe_json_nome(const nfe_json *j, size_t i)
{
	return j && j->nomes && i < j->n ? j->nomes[i] : NULL;
}

const nfe_json *nfe_json_campo(const nfe_json *j, const char *nome)
{
	size_t i;

	if (!j || !nome || j->tipo != NFE_JSON_OBJETO)
		return NULL;
	for (i = 0; i < j->n; i++)
		if (strcmp(j->nomes[i], nome) == 0)
			return j->itens[i];
	return NULL;
}
