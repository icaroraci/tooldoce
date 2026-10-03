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

#include <stdlib.h>
#include <string.h>

#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/esquema.h>
#include <libnfe/valida.h>

struct nfe_grupo {
	const struct nfe_esq *esq;
	char **valor; /* um por folha; NULL: não informado */
};

nfe_grupo *nfe_grupo_new(const struct nfe_esq *esq)
{
	nfe_grupo *g = (nfe_grupo *)calloc(1, sizeof(nfe_grupo));

	if (!g)
		return NULL;
	g->esq = esq;
	g->valor = (char **)calloc((size_t)esq->nfolhas, sizeof(char *));
	if (!g->valor) {
		free(g);
		return NULL;
	}
	return g;
}

/* Apaga as folhas [ini, fim) */
static void apaga(nfe_grupo *g, int ini, int fim)
{
	int i;

	for (i = ini; i < fim; i++) {
		free(g->valor[i]);
		g->valor[i] = NULL;
	}
}

void nfe_grupo_free(nfe_grupo *g)
{
	if (!g)
		return;
	apaga(g, 0, g->esq->nfolhas);
	free(g->valor);
	free(g);
}

/* Algum campo informado nas folhas do nó */
static int presente(const nfe_grupo *g, const struct nfe_esq_no *n)
{
	int i;

	for (i = n->ini; i < n->fim; i++)
		if (g->valor[i])
			return 1;
	return 0;
}

/* O nó casa com o caminho: o último segmento é o nome do nó e os demais
 * aparecem, em ordem, entre os elementos ancestrais (sem a raiz) */
static int casa(const struct nfe_esq *esq, int i, const char *caminho)
{
	const char *fim = caminho + strlen(caminho), *seg;
	size_t n;

	if (!esq->nos[i].nome)
		return 0;
	/* último segmento */
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
		for (; i > 0; i = esq->nos[i].pai)
			if (esq->nos[i].nome && strlen(esq->nos[i].nome) == n &&
			    strncmp(esq->nos[i].nome, seg, n) == 0)
				break;
		if (i <= 0) /* a raiz (0) não entra no caminho */
			return 0;
		i = esq->nos[i].pai;
		fim = seg > caminho ? seg - 1 : caminho;
	}
	return 1;
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

/* Procura o nó do caminho. Com folha, só folhas. Entre vários candidatos,
 * fica com o de ramo já presente; sem nenhum presente, com o primeiro.
 * Retorna o índice ou -1. */
static int procura(const nfe_grupo *g, const char *caminho, int folha)
{
	const struct nfe_esq *esq = g->esq;
	int i, achado = -1;

	if (!caminho || !caminho[0])
		return -1;
	for (i = 1; i < esq->nnos; i++) {
		if (folha && esq->nos[i].folha < 0)
			continue;
		if (!casa(esq, i, caminho))
			continue;
		if (achado < 0)
			achado = i;
		else if (ramo_presente(g, i) && !ramo_presente(g, achado))
			achado = i;
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
	if (n->padrao) {
		rc = nfe_valida_padrao(valor, n->padrao);
		if (rc != 0)
			return rc;
	}
	if (n->tstring)
		return nfe_valida_texto(valor, n->tmin ? n->tmin : 1, n->tmax);
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
	i = procura(g, caminho, 1);
	if (i < 0)
		return E_VALOR;
	return valida_no(&g->esq->nos[i], valor);
}

int nfe_grupo_set(nfe_grupo *g, const char *caminho, const char *valor)
{
	const struct nfe_esq_no *nos;
	char *copia;
	int i, a, filho, rc;

	if (!g || !caminho)
		return E_ISNULL;
	nos = g->esq->nos;
	i = procura(g, caminho, 1);
	if (i < 0)
		return E_VALOR;
	if (!valor) {
		apaga(g, nos[i].folha, nos[i].folha + 1);
		return 0;
	}
	rc = valida_no(&nos[i], valor);
	if (rc != 0)
		return rc;
	copia = (char *)malloc(strlen(valor) + 1);
	if (!copia)
		return E_MALLOC;
	strcpy(copia, valor);

	/* Apaga os outros ramos das escolhas acima do campo */
	for (filho = i, a = nos[i].pai; a >= 0; filho = a, a = nos[a].pai) {
		if (nos[a].tipo != ESQ_CHOICE)
			continue;
		apaga(g, nos[a].ini, nos[filho].ini);
		apaga(g, nos[filho].fim, nos[a].fim);
	}
	free(g->valor[nos[i].folha]);
	g->valor[nos[i].folha] = copia;
	return 0;
}

const char *nfe_grupo_get(const nfe_grupo *g, const char *caminho)
{
	const struct nfe_esq *esq;
	int i;

	if (!g || !caminho)
		return NULL;
	esq = g->esq;
	for (i = 1; i < esq->nnos; i++)
		if (esq->nos[i].folha >= 0 && g->valor[esq->nos[i].folha] &&
		    casa(esq, i, caminho))
			return g->valor[esq->nos[i].folha];
	return NULL;
}

int nfe_grupo_limpa(nfe_grupo *g, const char *caminho)
{
	const struct nfe_esq *esq;
	int i, achou = 0;

	if (!g || !caminho)
		return E_ISNULL;
	esq = g->esq;
	for (i = 1; i < esq->nnos; i++) {
		if (casa(esq, i, caminho)) {
			apaga(g, esq->nos[i].ini, esq->nos[i].fim);
			achou = 1;
		}
	}
	return achou ? 0 : E_VALOR;
}

static int escreve_no(xmlTextWriterPtr writer, const nfe_grupo *g, int i,
                      int forcado);

/* Escreve os filhos de i */
static int escreve_filhos(xmlTextWriterPtr writer, const nfe_grupo *g, int i)
{
	int f, rc;

	for (f = g->esq->nos[i].filho; f >= 0; f = g->esq->nos[f].irmao) {
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
		const char *v = g->valor[n->folha];

		/* Valor único e obrigatório (ex.: CST de um grupo) */
		if (!v && n->min && n->valores && n->valores[0] &&
		    !n->valores[1])
			v = n->valores[0];
		if (!v)
			return n->min ? E_VALOR : 0;
		return nfe_escreve(writer, n->nome, "%s", v);
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
			return n->min ? E_VALOR : 0;
		return escreve_no(writer, g, escolhido, 1);
	}
}

int nfe_grupo_write_xml(xmlTextWriterPtr writer, const nfe_grupo *g)
{
	if (!writer || !g)
		return E_ISNULL;
	return escreve_no(writer, g, 0, 1);
}
