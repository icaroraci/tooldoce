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
#include <libnfe/refNFe.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/utils.h>


struct refNFe_s{
  char refNFe[NFE_TAM_ASCII(NFE_TAM_CHAVE)];
};

struct refNFe_s *RefNFeNew(void)
{
  /* calloc: campos começam vazios; NULL se faltar memória */
  struct refNFe_s *nf = (struct refNFe_s *)calloc(1, sizeof(struct refNFe_s));
  return nf;
}

void RefNFeDel(struct refNFe_s *nf)
{
  free(nf);
}

int RefNFeSetrefNFe(struct refNFe_s *nf, const char *ref)
{
  if (!nf)
    return E_ISNULL;
  return nfe_copia_texto(nf->refNFe, sizeof nf->refNFe, ref,
                         NFE_TAM_CHAVE, NFE_TAM_CHAVE);
}

char *RefNFeGetrefNFe(struct refNFe_s *nf)
{
  return nf->refNFe;
}

int xmlGenRefNFeNode(xmlTextWriterPtr writer, struct refNFe_s *nf)
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

