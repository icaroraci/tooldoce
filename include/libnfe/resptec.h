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

#ifndef LIBNFE_RESPTEC_H
#define LIBNFE_RESPTEC_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

/*
 * Responsável técnico pelo sistema emissor (grupo infRespTec).
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado.
 */

typedef struct nfe_resptec nfe_resptec;

/* Cria vazio. Retorna NULL se faltar memória. */
nfe_resptec *nfe_resptec_new(void);

/* Libera; aceita NULL */
void nfe_resptec_free(nfe_resptec *rt);

/* Dados obrigatórios: CNPJ da empresa, nome do contato (2 a 60), e-mail
 * (6 a 60) e telefone (6 a 14 dígitos). Só grava se todos forem válidos. */
int nfe_resptec_set(nfe_resptec *rt, const char *cnpj, const char *xcontato,
                    const char *email, const char *fone);
/* CSRT (código de segurança do responsável técnico): identificador (0 a
 * 99) e hash já calculado (SHA-1 do CSRT concatenado com a chave de
 * acesso, em base64: 28 caracteres). hashcsrt NULL remove. */
int nfe_resptec_set_csrt(nfe_resptec *rt, unsigned idcsrt,
                         const char *hashcsrt);

/* Escreve <infRespTec>. Retorna 0, E_ISNULL, E_VALOR (dados obrigatórios
 * não informados) ou E_XML. */
int nfe_resptec_write_xml(xmlTextWriterPtr writer, const nfe_resptec *rt);

#endif
