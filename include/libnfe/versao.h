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

#ifndef LIBNFE_VERSAO_H
#define LIBNFE_VERSAO_H

/* Versão da biblioteca, no formato MAIOR.MENOR.REVISÃO[-PRÉ] (versionamento
 * semântico): a versão maior muda quando a API ou a ABI deixam de ser
 * compatíveis. NFE_VERSAO_PRE marca uma pré-versão (ex.: "rc1") e fica
 * vazio numa versão final. O Makefile lê estas macros para nomear
 * libnfe.so. */
#define NFE_VERSAO_MAIOR   1
#define NFE_VERSAO_MENOR   0
#define NFE_VERSAO_REVISAO 0
#define NFE_VERSAO_PRE     "rc5"
#define NFE_VERSAO         "1.0.0-rc5"

/* Versão da biblioteca carregada em tempo de execução (ex.: "1.0.0-rc1"), que
 * pode diferir de NFE_VERSAO, a dos headers usados na compilação. */
const char *nfe_versao(void);

#endif /* LIBNFE_VERSAO_H */
