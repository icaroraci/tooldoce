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

#ifndef LIBNFE_VALIDAR_H
#define LIBNFE_VALIDAR_H

#include <stddef.h>

/*
 * Validação do XML da NF-e contra os schemas oficiais (XSD), antes de
 * assinar e transmitir.
 *
 * O validador carrega os schemas uma vez (o que é demorado) e pode ser
 * reaproveitado em muitas validações:
 *   nfe_validador *v = nfe_validador_new(NULL);   (schemas instalados)
 *   nfe_erros *erros = nfe_erros_new();
 *   if (nfe_validar_xml(v, xml, strlen(xml), erros) != 0)
 *       for (i = 0; i < nfe_erros_qtd(erros); i++)
 *           ... nfe_erros_msg(erros, i), nfe_erros_campo(erros, i) ...
 *   nfe_erros_free(erros);
 *   nfe_validador_free(v);
 *
 * O documento pode estar assinado ou não: sem a assinatura (ds:Signature),
 * a nota é validada como se estivesse assinada, ou seja, todo o resto do
 * documento é conferido, inclusive a ordem e o formato de cada campo.
 *
 * As regras de validação da SEFAZ que o schema não cobre (somas, datas,
 * campos obrigatórios por condição) ainda não são conferidas.
 */

typedef struct nfe_erros nfe_erros;
typedef struct nfe_validador nfe_validador;

/* Lista de erros encontrados. Retorna NULL se faltar memória. */
nfe_erros *nfe_erros_new(void);
void nfe_erros_free(nfe_erros *erros);
/* Apaga os erros (nfe_validar_xml já começa apagando) */
void nfe_erros_limpa(nfe_erros *erros);
/* Quantidade de erros */
int nfe_erros_qtd(const nfe_erros *erros);
/* Erro i (a partir de 0): mensagem (em inglês, da libxml2), nome do campo
 * (elemento) em que ocorreu, se conhecido, e linha no documento (0 se
 * desconhecida). Texto pertencente à lista; NULL/0 se i não existir. */
const char *nfe_erros_msg(const nfe_erros *erros, int i);
const char *nfe_erros_campo(const nfe_erros *erros, int i);
int nfe_erros_linha(const nfe_erros *erros, int i);

/* Diretório padrão dos schemas, onde make install os coloca */
const char *nfe_dir_schemas(void);

/* Carrega os schemas de dir_schemas (NULL: nfe_dir_schemas()), que deve
 * ter nfe_v4.00.xsd e os arquivos que ele inclui. Retorna NULL se não for
 * possível carregá-los ou faltar memória. */
nfe_validador *nfe_validador_new(const char *dir_schemas);
void nfe_validador_free(nfe_validador *v);

/* Valida o documento xml (tam bytes; não precisa terminar em '\0') e
 * acrescenta os problemas em erros (que pode ser NULL). Retorna 0 (válido),
 * E_VALOR (inválido contra o schema), E_XML (documento malformado ou que
 * não é uma NF-e), E_ISNULL ou E_MALLOC. */
int nfe_validar_xml(nfe_validador *v, const char *xml, size_t tam,
                    nfe_erros *erros);

#endif
