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

#ifndef LIBNFE_GRUPO_H
#define LIBNFE_GRUPO_H

/*
 * Grupo genérico do leiaute.
 *
 * Grupos com muitos campos (os tributos do item, os subgrupos do produto,
 * cana, exportação...) são preenchidos campo a campo pelo caminho: os nomes
 * dos elementos do leiaute, separados por "/", a partir do grupo. O caminho
 * só precisa ter os nomes suficientes para identificar o campo, na ordem:
 *   nfe_grupo_set(g, "ICMS10/vBC", "100.00");
 * Atributos de um elemento são gravados como campos do elemento.
 *
 * Ao gravar um campo de um ramo de uma escolha do leiaute, os campos dos
 * outros ramos são apagados. Campos com um único valor possível são
 * preenchidos sozinhos. Os campos obrigatórios são conferidos ao gerar o
 * XML.
 *
 * Elementos que se repetem (listas, como DI ou rastro no produto) não são
 * gravados pelo caminho: nfe_grupo_add acrescenta um item e devolve o grupo
 * do item, cujos campos são gravados a partir dele (inclusive os atributos
 * do item, como "dia" em forDia, e o próprio valor quando o item é um campo
 * simples, como NVE: nfe_grupo_set(item, "NVE", "AA0001")).
 *
 * Os grupos pertencem ao objeto que os contém (nfe_imposto, nfe_prod...),
 * que os libera.
 *
 * Retornos: 0, E_ISNULL, E_TAMANHO, E_VALOR (campo inexistente, valor fora
 * do domínio ou do formato, ou lista cheia) ou E_MALLOC. Em caso de erro o
 * grupo não é alterado.
 */

typedef struct nfe_grupo nfe_grupo;

/* Grava o campo; NULL apaga */
int nfe_grupo_set(nfe_grupo *g, const char *caminho, const char *valor);
/* Valor do campo, ou NULL (texto pertencente ao grupo) */
const char *nfe_grupo_get(const nfe_grupo *g, const char *caminho);
/* Apaga todos os campos e itens de listas dentro dos elementos do
 * caminho */
int nfe_grupo_remove(nfe_grupo *g, const char *caminho);
/* Acrescenta um item à lista do caminho e devolve o seu grupo em *item */
int nfe_grupo_add(nfe_grupo *g, const char *caminho, nfe_grupo **item);
/* Quantidade de itens da lista do caminho, ou código de erro (< 0) */
int nfe_grupo_quantidade(const nfe_grupo *g, const char *caminho);
/* Item i (a partir de 0) da lista do caminho, ou NULL */
nfe_grupo *nfe_grupo_item(const nfe_grupo *g, const char *caminho, int i);

#endif
