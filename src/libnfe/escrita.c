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

#include <stdarg.h>

#include <libnfe/erros.h>
#include <libnfe/escrita.h>

int nfe_escreve(xmlTextWriterPtr writer, const char *tag, const char *formato,
                ...)
{
	va_list ap;
	int rc;

	va_start(ap, formato);
	rc = xmlTextWriterWriteVFormatElement(writer, BAD_CAST tag, formato,
	                                      ap);
	va_end(ap);
	return rc < 0 ? E_XML : 0;
}

int nfe_abre(xmlTextWriterPtr writer, const char *tag)
{
	return xmlTextWriterStartElement(writer, BAD_CAST tag) < 0 ? E_XML : 0;
}

int nfe_fecha(xmlTextWriterPtr writer)
{
	return xmlTextWriterEndElement(writer) < 0 ? E_XML : 0;
}
