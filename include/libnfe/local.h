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

#ifndef LIBNFE_LOCAL_H
#define LIBNFE_LOCAL_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/endereco.h>

/*
 * Local de retirada ou de entrega da mercadoria (grupos retirada e entrega,
 * tipo TLocal), quando diferente do endereço do emitente ou do
 * destinatário.
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado. NULL remove os opcionais.
 */

typedef struct nfe_local nfe_local;

/* Qual grupo nfe_local_write_xml escreve */
typedef enum nfe_local_tipo {
	NFE_LOCAL_RETIRADA, /* <retirada> */
	NFE_LOCAL_ENTREGA   /* <entrega> */
} nfe_local_tipo;

/* Cria um local vazio. Retorna NULL se faltar memória. */
nfe_local *nfe_local_new(void);

/* Libera o local e o endereço que ele possui; aceita NULL */
void nfe_local_free(nfe_local *local);

/* CNPJ ou CPF de quem entrega/recebe (obrigatório; um substitui o outro) */
int nfe_local_set_cnpj(nfe_local *local, const char *cnpj);
int nfe_local_set_cpf(nfe_local *local, const char *cpf);
int nfe_local_set_xnome(nfe_local *local, const char *xnome); /* 2 a 60 */
/* Endereço (obrigatório). Em caso de sucesso o local passa a ser dono de
 * end. */
int nfe_local_set_endereco(nfe_local *local, nfe_endereco *end);
int nfe_local_set_email(nfe_local *local, const char *email); /* 1 a 60 */
/* IE: 2 a 14 dígitos ou "ISENTO" */
int nfe_local_set_ie(nfe_local *local, const char *ie);

/* Escreve <retirada> ou <entrega>. Retorna 0, E_ISNULL, E_VALOR (falta
 * CNPJ/CPF ou endereço, ou endereço incompleto) ou E_XML. */
int nfe_local_write_xml(xmlTextWriterPtr writer, nfe_local_tipo tipo,
                        const nfe_local *local);

#endif
