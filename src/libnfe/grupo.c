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

#include <stdlib.h>
#include <string.h>

#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/esquema.h>
#include <libnfe/valida.h>

/* Itens de uma lista */
struct lista_s {
	nfe_grupo **item;
	int n, cap;
};

struct nfe_grupo {
	const struct nfe_esq *esq;
	char **valor;           /* um por folha; NULL: não informado */
	struct lista_s *listas; /* uma por lista do esquema */
};

nfe_grupo *nfe_grupo_new(const struct nfe_esq *esq)
{
	nfe_grupo *g;

	if (!esq)
		return NULL;
	g = (nfe_grupo *)calloc(1, sizeof(nfe_grupo));
	if (!g)
		return NULL;
	g->esq = esq;
	g->valor = (char **)calloc((size_t)esq->nfolhas + 1, sizeof(char *));
	g->listas = (struct lista_s *)calloc((size_t)esq->nlistas + 1,
	                                     sizeof(struct lista_s));
	if (!g->valor || !g->listas) {
		free(g->valor);
		free(g->listas);
		free(g);
		return NULL;
	}
	return g;
}

/* Apaga as folhas [ini, fim) e os itens das listas [lini, lfim) */
static void apaga(nfe_grupo *g, int ini, int fim, int lini, int lfim)
{
	int i, j;

	for (i = ini; i < fim; i++) {
		free(g->valor[i]);
		g->valor[i] = NULL;
	}
	for (i = lini; i < lfim; i++) {
		for (j = 0; j < g->listas[i].n; j++)
			nfe_grupo_free(g->listas[i].item[j]);
		free(g->listas[i].item);
		memset(&g->listas[i], 0, sizeof g->listas[i]);
	}
}

static void apaga_no(nfe_grupo *g, const struct nfe_esq_no *n)
{
	apaga(g, n->ini, n->fim, n->lini, n->lfim);
}

void nfe_grupo_free(nfe_grupo *g)
{
	if (!g)
		return;
	apaga(g, 0, g->esq->nfolhas, 0, g->esq->nlistas);
	free(g->valor);
	free(g->listas);
	free(g);
}

/* Algum campo informado ou item de lista no nó */
static int presente(const nfe_grupo *g, const struct nfe_esq_no *n)
{
	int i;

	for (i = n->ini; i < n->fim; i++)
		if (g->valor[i])
			return 1;
	for (i = n->lini; i < n->lfim; i++)
		if (g->listas[i].n > 0)
			return 1;
	return 0;
}

int nfe_grupo_vazio(const nfe_grupo *g)
{
	return !g || !presente(g, &g->esq->nos[0]);
}

/* O nó casa com o caminho: o último segmento é o nome do nó e os demais
 * aparecem, em ordem, entre os elementos ancestrais. A raiz só entra no
 * caminho quando é o próprio nó (itens de listas de campos simples).
 * Retorna 0 (não casa), CASA_PARCIAL (pulando elementos, como
 * "veicTracao/UF" para veicTracao/prop/UF), CASA_EXATO (os segmentos são
 * elementos consecutivos, sem pular nenhum entre eles, como "UF" para
 * prop/UF) ou CASA_RAIZ (exato e a partir da raiz do grupo, como "UF" para
 * o UF filho direto do grupo). */
enum { CASA_PARCIAL = 1, CASA_EXATO = 2, CASA_RAIZ = 3 };

static int casa(const struct nfe_esq *esq, int i, const char *caminho)
{
	const char *fim = caminho + strlen(caminho), *seg;
	size_t n;
	int pulou = 0;

	if (!esq->nos[i].nome)
		return 0;
	seg = fim;
	while (seg > caminho && seg[-1] != '/')
		seg--;
	n = (size_t)(fim - seg);
	if (strlen(esq->nos[i].nome) != n ||
	    strncmp(esq->nos[i].nome, seg, n) != 0)
		return 0;
	fim = seg > caminho ? seg - 1 : caminho;
	i = esq->nos[i].pai;
	while (fim > caminho) {
		seg = fim;
		while (seg > caminho && seg[-1] != '/')
			seg--;
		n = (size_t)(fim - seg);
		for (; i > 0; i = esq->nos[i].pai) {
			if (!esq->nos[i].nome || esq->nos[i].tipo != ESQ_ELEM)
				continue;
			if (strlen(esq->nos[i].nome) == n &&
			    strncmp(esq->nos[i].nome, seg, n) == 0)
				break;
			pulou = 1;
		}
		if (i <= 0)
			return 0;
		i = esq->nos[i].pai;
		fim = seg > caminho ? seg - 1 : caminho;
	}
	if (pulou)
		return CASA_PARCIAL;
	for (; i > 0; i = esq->nos[i].pai)
		if (esq->nos[i].nome && esq->nos[i].tipo == ESQ_ELEM)
			return CASA_EXATO;
	return CASA_RAIZ;
}

/* Ramo da escolha mais próxima de i já está presente */
static int ramo_presente(const nfe_grupo *g, int i)
{
	const struct nfe_esq_no *nos = g->esq->nos;
	int filho = i;

	for (i = nos[i].pai; i >= 0; filho = i, i = nos[i].pai)
		if (nos[i].tipo == ESQ_CHOICE)
			return presente(g, &nos[filho]);
	return 0;
}

enum procura_e { FOLHA, LISTA, QUALQUER };

/* Procura o nó do caminho do tipo pedido. Entre vários candidatos, fica
 * com o que casa melhor com o caminho (ver casa); entre esses, com o de ramo já
 * presente; sem nenhum presente, com o primeiro. Retorna o índice ou -1. */
static int procura(const nfe_grupo *g, const char *caminho, enum procura_e o)
{
	const struct nfe_esq *esq = g->esq;
	int i, c, achado = -1, melhor = 0;

	if (!caminho || !caminho[0])
		return -1;
	/* A raiz só é candidata quando é um campo (item de lista simples) */
	for (i = esq->nos[0].folha >= 0 ? 0 : 1; i < esq->nnos; i++) {
		if (o == FOLHA && esq->nos[i].folha < 0)
			continue;
		if (o == LISTA && esq->nos[i].lista < 0)
			continue;
		c = casa(esq, i, caminho);
		if (!c || c < melhor)
			continue;
		if (achado < 0 || c > melhor ||
		    (ramo_presente(g, i) && !ramo_presente(g, achado))) {
			achado = i;
			melhor = c;
		}
	}
	return achado;
}

/* Quantidade de caracteres UTF-8 */
static size_t caracteres(const char *s)
{
	size_t n = 0;

	for (; *s; s++)
		if (((unsigned char)*s & 0xC0) != 0x80)
			n++;
	return n;
}

static int valida_no(const struct nfe_esq_no *n, const char *valor)
{
	size_t c;
	int rc;

	if (n->valores) {
		rc = nfe_valida_lista(valor, n->valores);
		if (rc != 0)
			return rc;
	}
	/* TString: tamanho antes do padrão, para "" dar E_TAMANHO */
	if (n->tstring) {
		rc = nfe_valida_texto(valor, n->tmin ? n->tmin : 1, n->tmax);
		if (rc != 0)
			return rc;
	}
	if (n->padrao) {
		rc = nfe_valida_padrao(valor, n->padrao);
		if (rc != 0)
			return rc;
	}
	if (n->tstring)
		return 0;
	c = caracteres(valor);
	if ((n->tmin && c < n->tmin) || (n->tmax && c > n->tmax))
		return E_TAMANHO;
	return 0;
}

int nfe_grupo_valida(const nfe_grupo *g, const char *caminho, const char *valor)
{
	int i;

	if (!g || !caminho || !valor)
		return E_ISNULL;
	i = procura(g, caminho, FOLHA);
	if (i < 0)
		return E_VALOR;
	return valida_no(&g->esq->nos[i], valor);
}

/* Apaga os outros ramos das escolhas acima do nó i */
static void apaga_outros_ramos(nfe_grupo *g, int i)
{
	const struct nfe_esq_no *nos = g->esq->nos;
	int a, filho;

	for (filho = i, a = nos[i].pai; a >= 0; filho = a, a = nos[a].pai) {
		if (nos[a].tipo != ESQ_CHOICE)
			continue;
		apaga(g, nos[a].ini, nos[filho].ini, nos[a].lini,
		      nos[filho].lini);
		apaga(g, nos[filho].fim, nos[a].fim, nos[filho].lfim,
		      nos[a].lfim);
	}
}

int nfe_grupo_set(nfe_grupo *g, const char *caminho, const char *valor)
{
	const struct nfe_esq_no *nos;
	char *copia;
	int i, rc;

	if (!g || !caminho)
		return E_ISNULL;
	nos = g->esq->nos;
	i = procura(g, caminho, FOLHA);
	if (i < 0)
		return E_VALOR;
	if (!valor) {
		apaga(g, nos[i].folha, nos[i].folha + 1, 0, 0);
		return 0;
	}
	rc = valida_no(&nos[i], valor);
	if (rc != 0)
		return rc;
	copia = (char *)malloc(strlen(valor) + 1);
	if (!copia)
		return E_MALLOC;
	strcpy(copia, valor);
	apaga_outros_ramos(g, i);
	free(g->valor[nos[i].folha]);
	g->valor[nos[i].folha] = copia;
	return 0;
}

const char *nfe_grupo_get(const nfe_grupo *g, const char *caminho)
{
	const struct nfe_esq *esq;
	int i, c;

	if (!g || !caminho)
		return NULL;
	esq = g->esq;
	/* Primeiro o campo que casa exatamente com o caminho, a partir da
	 * raiz do grupo */
	for (c = CASA_RAIZ; c >= CASA_PARCIAL; c--)
		for (i = 0; i < esq->nnos; i++)
			if (esq->nos[i].folha >= 0 &&
			    g->valor[esq->nos[i].folha] &&
			    (i > 0 || esq->nos[0].folha >= 0) &&
			    casa(esq, i, caminho) >= c)
				return g->valor[esq->nos[i].folha];
	return NULL;
}

int nfe_grupo_remove(nfe_grupo *g, const char *caminho)
{
	const struct nfe_esq *esq;
	int i, c, achou = 0, minimo = CASA_PARCIAL;

	if (!g || !caminho)
		return E_ISNULL;
	esq = g->esq;
	/* Só os elementos que casam melhor com o caminho (ver casa) */
	for (i = 1; i < esq->nnos; i++) {
		c = casa(esq, i, caminho);
		if (c > minimo)
			minimo = c;
	}
	for (i = 1; i < esq->nnos; i++) {
		if (casa(esq, i, caminho) >= minimo) {
			apaga_no(g, &esq->nos[i]);
			achou = 1;
		}
	}
	return achou ? 0 : E_VALOR;
}

int nfe_grupo_add(nfe_grupo *g, const char *caminho, nfe_grupo **item)
{
	const struct nfe_esq_no *n;
	struct lista_s *l;
	nfe_grupo *novo;
	int i;

	if (!g || !caminho || !item)
		return E_ISNULL;
	i = procura(g, caminho, LISTA);
	if (i < 0)
		return E_VALOR;
	n = &g->esq->nos[i];
	l = &g->listas[n->lista];
	if (n->max && l->n >= n->max)
		return E_VALOR;
	if (l->n == l->cap) {
		int cap = l->cap ? l->cap * 2 : 4;
		nfe_grupo **v = (nfe_grupo **)realloc(
		        l->item, (size_t)cap * sizeof(nfe_grupo *));
		if (!v)
			return E_MALLOC;
		l->item = v;
		l->cap = cap;
	}
	novo = nfe_grupo_new(n->sub);
	if (!novo)
		return E_MALLOC;
	apaga_outros_ramos(g, i);
	l = &g->listas[n->lista];
	l->item[l->n++] = novo;
	*item = novo;
	return 0;
}

void nfe_grupo_remove_ultimo(nfe_grupo *g, const char *caminho)
{
	struct lista_s *l;
	int i = g ? procura(g, caminho, LISTA) : -1;

	if (i < 0)
		return;
	l = &g->listas[g->esq->nos[i].lista];
	if (l->n > 0)
		nfe_grupo_free(l->item[--l->n]);
}

int nfe_grupo_quantidade(const nfe_grupo *g, const char *caminho)
{
	int i;

	if (!g || !caminho)
		return E_ISNULL;
	i = procura(g, caminho, LISTA);
	if (i < 0)
		return E_VALOR;
	return g->listas[g->esq->nos[i].lista].n;
}

nfe_grupo *nfe_grupo_item(const nfe_grupo *g, const char *caminho, int i)
{
	const struct lista_s *l;
	int n;

	if (!g || !caminho)
		return NULL;
	n = procura(g, caminho, LISTA);
	if (n < 0)
		return NULL;
	l = &g->listas[g->esq->nos[n].lista];
	return i >= 0 && i < l->n ? l->item[i] : NULL;
}

/* Valor a escrever na folha: o informado ou, se o campo é obrigatório e
 * tem um único valor possível, esse valor */
static const char *valor_folha(const nfe_grupo *g, const struct nfe_esq_no *n)
{
	const char *v = g->valor[n->folha];

	if (!v && n->min && n->valores && n->valores[0] && !n->valores[1])
		v = n->valores[0];
	return v;
}

static int escreve_no(xmlTextWriterPtr writer, const nfe_grupo *g, int i,
                      int forcado);

/* O nó exige conteúdo: não pode ficar vazio (ex.: escolha cujos ramos são
 * todos opcionais não exige nada) */
static int exige(const struct nfe_esq *esq, int i)
{
	const struct nfe_esq_no *n = &esq->nos[i];
	int f;

	if (!n->min)
		return 0;
	switch (n->tipo) {
	case ESQ_SEQ:
		for (f = n->filho; f >= 0; f = esq->nos[f].irmao)
			if (exige(esq, f))
				return 1;
		return 0;
	case ESQ_CHOICE:
		for (f = n->filho; f >= 0; f = esq->nos[f].irmao)
			if (!exige(esq, f))
				return 0;
		return 1;
	default:
		return 1;
	}
}

/* Escreve os filhos de i: primeiro os atributos, depois o conteúdo */
static int escreve_filhos(xmlTextWriterPtr writer, const nfe_grupo *g, int i)
{
	const struct nfe_esq_no *nos = g->esq->nos;
	int f, rc;

	for (f = nos[i].filho; f >= 0; f = nos[f].irmao) {
		const char *v;

		if (nos[f].tipo != ESQ_ATTR)
			continue;
		v = valor_folha(g, &nos[f]);
		if (!v) {
			if (nos[f].min)
				return E_VALOR;
			continue;
		}
		if (xmlTextWriterWriteAttribute(writer, BAD_CAST nos[f].nome,
		                                BAD_CAST v) < 0)
			return E_XML;
	}
	for (f = nos[i].filho; f >= 0; f = nos[f].irmao) {
		if (nos[f].tipo == ESQ_ATTR)
			continue;
		rc = escreve_no(writer, g, f, 0);
		if (rc != 0)
			return rc;
	}
	return 0;
}

static int escreve_no(xmlTextWriterPtr writer, const nfe_grupo *g, int i,
                      int forcado)
{
	const struct nfe_esq_no *n = &g->esq->nos[i];
	int f, escolhido, rc;

	if (n->tipo == ESQ_ELEM && n->folha >= 0) {
		const char *v = valor_folha(g, n);

		if (!v)
			return n->min ? E_VALOR : 0;
		return nfe_escreve(writer, n->nome, "%s", v);
	}
	if (n->tipo == ESQ_LISTA) {
		const struct lista_s *l = &g->listas[n->lista];

		if (l->n == 0)
			return n->min ? E_VALOR : 0;
		for (f = 0; f < l->n; f++) {
			rc = nfe_grupo_write_xml(writer, l->item[f]);
			if (rc != 0)
				return rc;
		}
		return 0;
	}
	if (!forcado && !presente(g, n) && !n->min)
		return 0;
	switch (n->tipo) {
	case ESQ_ELEM:
		rc = nfe_abre(writer, n->nome);
		if (rc == 0)
			rc = escreve_filhos(writer, g, i);
		if (rc == 0)
			rc = nfe_fecha(writer);
		return rc;
	case ESQ_SEQ:
		return escreve_filhos(writer, g, i);
	default: /* ESQ_CHOICE: exatamente um ramo */
		escolhido = -1;
		for (f = n->filho; f >= 0; f = g->esq->nos[f].irmao) {
			if (!presente(g, &g->esq->nos[f]))
				continue;
			if (escolhido >= 0)
				return E_VALOR;
			escolhido = f;
		}
		if (escolhido < 0)
			return exige(g->esq, i) ? E_VALOR : 0;
		return escreve_no(writer, g, escolhido, 1);
	}
}

int nfe_grupo_write_xml(xmlTextWriterPtr writer, const nfe_grupo *g)
{
	if (!writer || !g)
		return E_ISNULL;
	return escreve_no(writer, g, 0, 1);
}
