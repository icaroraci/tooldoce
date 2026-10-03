/* Copyright (c) 2017-2026 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
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
 */

#include <stdlib.h>

#include <libnfe/refNFe.h>
#include <libnfe/chave.h>
#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/escrita.h>
#include <libnfe/utils.h>

struct refNFe_s {
	char chave[NFE_TAM_ASCII(NFE_TAM_CHAVE)];
};

struct refNFe_s *RefNFeNew(void)
{
	return calloc(1, sizeof(struct refNFe_s));
}

void RefNFeDel(struct refNFe_s *nf)
{
	free(nf);
}

int RefNFeSetrefNFe(struct refNFe_s *nf, const char *ref)
{
	int erro;

	if (!nf)
		return E_ISNULL;
	/* Valida antes de copiar, para não deixar uma chave inválida gravada */
	erro = nfe_chave_validar(ref);
	if (erro)
		return erro;
	return nfe_copia_texto(nf->chave, sizeof nf->chave, ref, NFE_TAM_CHAVE,
	                       NFE_TAM_CHAVE);
}

const char *RefNFeGetrefNFe(const struct refNFe_s *nf)
{
	return nf ? nf->chave : NULL;
}

int xmlGenRefNFeNode(xmlTextWriterPtr writer, const struct refNFe_s *nf)
{
	if (!writer || !nf)
		return E_ISNULL;
	return nfe_escreve(writer, "refNFe", "%s", nf->chave);
}
