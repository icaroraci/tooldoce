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

#ifndef LIBNFE_JSON_H
#define LIBNFE_JSON_H

#include <stddef.h>

#include <libnfe/erros.h>

/* Leitura mínima de JSON (RFC 8259), para as respostas de serviços REST
 * como a NFS-e nacional. O texto inteiro vira uma árvore somente de
 * leitura; os nós pertencem à raiz e valem até nfe_json_free:
 *   nfe_json *j;
 *   if (nfe_json_ler(resposta, tam, &j) == 0) {
 *       const char *chave = nfe_json_texto(nfe_json_campo(j, "chaveAcesso"));
 *       const nfe_json *erros = nfe_json_campo(j, "erros");
 *       size_t i;
 *       for (i = 0; i < nfe_json_qtd(erros); i++)
 *           ... nfe_json_texto(nfe_json_campo(nfe_json_item(erros, i),
 *                                             "Codigo")) ...
 *       nfe_json_free(j);
 *   }
 * As funções de consulta aceitam NULL (devolvem NULL ou 0), para encadear
 * campos que podem faltar. */

typedef struct nfe_json nfe_json;

typedef enum nfe_json_tipo {
	NFE_JSON_NULO,
	NFE_JSON_LOGICO, /* true ou false */
	NFE_JSON_NUMERO,
	NFE_JSON_TEXTO,
	NFE_JSON_LISTA,
	NFE_JSON_OBJETO
} nfe_json_tipo;

/* Maior profundidade de listas e objetos aceita por nfe_json_ler */
#define NFE_JSON_PROFUNDIDADE 64

/* Lê tam bytes de texto JSON (UTF-8; espaços em volta são aceitos) em
 * *raiz, a liberar com nfe_json_free. Textos com \u0000 são recusados,
 * para que nfe_json_texto devolva textos C completos. Em caso de erro
 * *raiz não é alterado.
 * Retorna 0, E_ISNULL (texto ou raiz NULL), E_VALOR (JSON inválido, ou
 * aninhado além de NFE_JSON_PROFUNDIDADE) ou E_MALLOC. */
int nfe_json_ler(const char *texto, size_t tam, nfe_json **raiz);
void nfe_json_free(nfe_json *raiz);

/* Tipo do nó; NFE_JSON_NULO também para j NULL */
nfe_json_tipo nfe_json_tipo_de(const nfe_json *j);

/* Valor do nó como texto: o texto já sem escapes (UTF-8), o número como
 * escrito no JSON, "true" ou "false"; NULL para null, lista, objeto ou j
 * NULL. O texto pertence à árvore. */
const char *nfe_json_texto(const nfe_json *j);

/* Quantidade de itens de uma lista ou de campos de um objeto; 0 para
 * outros nós ou j NULL */
size_t nfe_json_qtd(const nfe_json *j);

/* Item i (a partir de 0) de uma lista ou valor do campo i de um objeto;
 * NULL se i estiver fora da faixa ou j não for lista nem objeto */
const nfe_json *nfe_json_item(const nfe_json *j, size_t i);

/* Nome do campo i de um objeto; NULL se j não for objeto ou i estiver fora
 * da faixa */
const char *nfe_json_nome(const nfe_json *j, size_t i);

/* Valor do campo nome de um objeto (o primeiro, se repetido), com
 * diferença entre maiúsculas e minúsculas; NULL se faltar ou se j não for
 * objeto */
const nfe_json *nfe_json_campo(const nfe_json *j, const char *nome);

#endif /* LIBNFE_JSON_H */
