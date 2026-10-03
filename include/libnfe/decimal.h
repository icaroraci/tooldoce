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

#ifndef LIBNFE_DECIMAL_H
#define LIBNFE_DECIMAL_H

#include <stddef.h>

#include <libnfe/utils.h>

/* Funções internas para somar valores monetários (até 2 casas decimais)
 * guardados como texto, sem erro de arredondamento: o valor é convertido
 * para centavos (inteiro). */

/* Converte valor ("15", "15.5", "15.50"; "" vale zero) para centavos.
 * Retorna 0, E_ISNULL ou E_VALOR (formato inválido, mais de 2 casas ou
 * valor grande demais). */
NFE_INTERNO int nfe_dec2_ler(const char *valor, long long *centavos);

/* Escreve centavos (>= 0) como texto com 2 casas ("15.50") em dst.
 * Retorna 0, E_VALOR (negativo) ou E_TAMANHO (dst pequeno). */
NFE_INTERNO int nfe_dec2_escreve(long long centavos, char *dst, size_t tam);

#endif
