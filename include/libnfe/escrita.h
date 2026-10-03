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

#ifndef LIBNFE_ESCRITA_H
#define LIBNFE_ESCRITA_H

#include <libxml/xmlwriter.h>

#include <libnfe/utils.h>

/* Funções internas de escrita do XML */

/* Escreve <tag>valor</tag>, com valor formatado como em printf.
 * Retorna 0 ou E_XML. */
NFE_INTERNO int nfe_escreve(xmlTextWriterPtr writer, const char *tag,
                            const char *formato, ...)
#if defined(__GNUC__)
        __attribute__((format(printf, 3, 4)))
#endif
        ;

/* Abre e fecha um elemento; retornam 0 ou E_XML */
NFE_INTERNO int nfe_abre(xmlTextWriterPtr writer, const char *tag);
NFE_INTERNO int nfe_fecha(xmlTextWriterPtr writer);

/* Chama nfe_escreve e retorna da função em caso de erro; requer uma
 * variável int rc e um xmlTextWriterPtr writer no escopo */
#define NFE_ESCREVE(...)                                                       \
	do {                                                                   \
		rc = nfe_escreve(writer, __VA_ARGS__);                         \
		if (rc != 0)                                                   \
			return rc;                                             \
	} while (0)

#endif
