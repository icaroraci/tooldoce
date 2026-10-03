/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
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

#ifndef LIBNFE_ERROS_H
#define LIBNFE_ERROS_H

/* Códigos de erro devolvidos pelas funções da biblioteca (sempre negativos;
 * 0 indica sucesso). A biblioteca não imprime mensagens: use nfe_strerror()
 * para obter a descrição de um código. */
#define E_ISNULL  -1   /* ponteiro nulo recebido */
#define E_TAMANHO -2   /* texto fora dos limites do campo */
#define E_VALOR   -3   /* valor fora da faixa permitida */
#define E_XML     -4   /* XML malformado ou falha ao gerá-lo (libxml2) */
#define E_ARQUIVO -5   /* falha ao gravar o arquivo */
#define E_REDE    -6   /* falha na comunicação com a SEFAZ */
#define E_MALLOC  -101 /* falta de memória */

/* Descrição do código de erro (texto estático, não deve ser liberado) */
const char *nfe_strerror(int codigo);

#endif
