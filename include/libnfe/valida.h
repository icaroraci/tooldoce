/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef LIBNFE_VALIDA_H
#define LIBNFE_VALIDA_H

#include <stddef.h>

#include <libnfe/padroes.h>
#include <libnfe/utils.h>

/* Validação de valores textuais conforme as facetas do XSD. Os padrões
 * seguem a sintaxe de expressões regulares do XML Schema (a mesma do
 * leiaute) e casam com o valor inteiro; os de cada tipo nomeado estão em
 * padroes.h (NFE_PADRAO_*), gerados a partir do XSD oficial. */

/* Confere se valor casa com o padrão.
 * Retorna 0, E_ISNULL (valor ou padrao NULL), E_VALOR (não casa ou valor
 * não é UTF-8 válido) ou E_MALLOC (falha ao compilar o padrão). */
NFE_INTERNO int nfe_valida_padrao(const char *valor, const char *padrao);

/* Confere se valor é um dos itens de lista (terminada em NULL), por exemplo
 * { NFE_VALORES_TUf, NULL }.
 * Retorna 0, E_ISNULL ou E_VALOR. */
NFE_INTERNO int nfe_valida_lista(const char *valor, const char *const *lista);

/* Confere um texto do tipo TString: entre min e max caracteres UTF-8
 * (max == 0: sem limite) e casando com NFE_PADRAO_TString (caracteres de
 * U+0020 a U+00FF, sem espaço no início nem no fim).
 * Retorna 0, E_ISNULL, E_TAMANHO, E_VALOR ou E_MALLOC. */
NFE_INTERNO int nfe_valida_texto(const char *valor, size_t min, size_t max);

/* Valida valor com nfe_valida_padrao e, se válido, copia para dst (tam
 * bytes, incluindo o terminador). Em caso de erro dst não é alterado.
 * Retorna 0, E_ISNULL, E_VALOR, E_TAMANHO (não cabe em dst) ou E_MALLOC. */
NFE_INTERNO int nfe_copia_padrao(char *dst, size_t tam, const char *valor,
                                 const char *padrao);

/* Valida com nfe_valida_texto e, se válido, copia para dst.
 * Retorna 0, E_ISNULL, E_TAMANHO, E_VALOR ou E_MALLOC. */
NFE_INTERNO int nfe_copia_texto_validado(char *dst, size_t tam,
                                         const char *valor, size_t min,
                                         size_t max);
#endif
