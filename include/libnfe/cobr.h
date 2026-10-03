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

#ifndef LIBNFE_COBR_H
#define LIBNFE_COBR_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

/*
 * Cobrança (grupo cobr): fatura (fat) e duplicatas (dup).
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado. Valores são texto no formato do XML ("100.00");
 * NULL omite os opcionais.
 */

typedef struct nfe_cobr nfe_cobr;

/* Número máximo de duplicatas */
#define NFE_MAX_DUP 120

/* Cria a cobrança vazia. Retorna NULL se faltar memória. */
nfe_cobr *nfe_cobr_new(void);

/* Libera; aceita NULL */
void nfe_cobr_free(nfe_cobr *cobr);

/* fat: número (1 a 60), valor original, desconto e valor líquido, todos
 * opcionais. Com todos NULL, remove a fatura. */
int nfe_cobr_set_fat(nfe_cobr *cobr, const char *nfat, const char *vorig,
                     const char *vdesc, const char *vliq);
/* dup: número (1 a 60) e vencimento ("AAAA-MM-DD") opcionais; valor
 * obrigatório, maior que zero. Até NFE_MAX_DUP. */
int nfe_cobr_add_dup(nfe_cobr *cobr, const char *ndup, const char *dvenc,
                     const char *vdup);
/* Apaga todas as duplicatas */
int nfe_cobr_remove_dup(nfe_cobr *cobr);

/* Escreve o elemento <cobr>. Retorna 0, E_ISNULL ou E_XML. */
int nfe_cobr_write_xml(xmlTextWriterPtr writer, const nfe_cobr *cobr);

#endif
