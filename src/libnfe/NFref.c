/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/libnfe/defs.h"
#include "NFref.h"


struct NFref_s{
  char refNFe[TAM_CHAVE];
};

struct NFref_s *NFrefNew()
{
  struct NFref_s *nf = (struct NFref_s *)malloc(sizeof(struct NFref_s));
  return nf;
}

void NFrefDel(struct NFref_s *nf)
{
  free(nf);
}

void NFrefSetrefNFe(struct NFref_s *nf, char *ref)
{
  strcpy(nf->refNFe, ref);
}

char *NFrefGetrefNFe(struct NFref_s *nf)
{
  return nf->refNFe;
}

int xmlGenNFrefNode(xmlTextWriterPtr writer, struct NFref_s *nf)
{
  int rc;
  rc = xmlTextWriterStartElement(writer, BAD_CAST "NFref");
    if (rc < 0) {
        printf
            ("NFref: Erro em xmlTextWriterStartElement\n");
        return -1;
    }
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "refNFe","%s", 
                                               nf->refNFe);
    if (rc < 0) {
        printf
            ("NFref: Erro em xmlTextWriterWriteFormatElement\n");
        return -1;
    }
  rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf
            ("NFref: Erro em xmlTextWriterEndElement\n");
        return -1;
    }
  return 0;
}

