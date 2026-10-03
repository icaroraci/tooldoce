/* Copyright (c) 2017-2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
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
 */

/*
 * Elemento refNFe (dentro de NFref): NF-e ou NFC-e referenciada pela chave
 * de acesso.
 */

#ifndef LIBNFE_REFNFE_H
#define LIBNFE_REFNFE_H

#include <libxml/xmlwriter.h>

struct refNFe_s;

/* Cria um refNFe vazio; NULL se faltar memória */
struct refNFe_s *RefNFeNew(void);
/* Libera o refNFe (aceita NULL) */
void RefNFeDel(struct refNFe_s *nf);

/* Chave de acesso: 44 caracteres com dígito verificador válido.
 * Retorna 0, E_ISNULL, E_TAMANHO ou E_VALOR; se recusada, a chave anterior
 * é mantida. */
int RefNFeSetrefNFe(struct refNFe_s *nf, const char *ref);
/* "" enquanto a chave não for informada; NULL se nf for NULL */
const char *RefNFeGetrefNFe(const struct refNFe_s *nf);

/* Escreve <refNFe>; quem chama abre e fecha <NFref> */
int xmlGenRefNFeNode(xmlTextWriterPtr writer, const struct refNFe_s *nf);

#endif
