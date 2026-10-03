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

#include <stdio.h>
#include <stdlib.h>

#include <libnfe/cnpjcpf.h>
#include <libnfe/erros.h>
#include <libnfe/esquema.h>
#include <libnfe/transp.h>

/* Os campos ficam em um grupo genérico (esquema.h) com a estrutura de
 * <transp> do leiaute; os setters específicos são atalhos para ele. */
struct nfe_transp {
	nfe_grupo *g;
};

nfe_transp *nfe_transp_new(void)
{
	nfe_transp *tr = (nfe_transp *)calloc(1, sizeof(nfe_transp));

	if (!tr)
		return NULL;
	tr->g = nfe_grupo_new(&esq_transp);
	if (!tr->g || nfe_grupo_set(tr->g, "modFrete", "9") != 0) {
		nfe_transp_free(tr);
		return NULL;
	}
	return tr;
}

void nfe_transp_free(nfe_transp *tr)
{
	if (!tr)
		return;
	nfe_grupo_free(tr->g);
	free(tr);
}

nfe_grupo *nfe_transp_grupo(nfe_transp *tr)
{
	return tr ? tr->g : NULL;
}

int nfe_transp_set_modfrete(nfe_transp *tr, nfe_mod_frete modfrete)
{
	char v[8];

	if (!tr)
		return E_ISNULL;
	if ((int)modfrete < 0 || (int)modfrete > 9)
		return E_VALOR;
	snprintf(v, sizeof v, "%d", (int)modfrete);
	return nfe_grupo_set(tr->g, "modFrete", v);
}

/* Campo opcional do transportador: NULL remove */
static int transporta(nfe_transp *tr, const char *campo, const char *v)
{
	char caminho[32];

	if (!tr)
		return E_ISNULL;
	snprintf(caminho, sizeof caminho, "transporta/%s", campo);
	return nfe_grupo_set(tr->g, caminho, v);
}

/* CNPJ/CPF do transportador: NULL remove o documento */
static int doc(nfe_transp *tr, const char *campo, const char *v,
               int (*valida)(const char *))
{
	int rc;

	if (!tr)
		return E_ISNULL;
	if (!v) {
		nfe_grupo_set(tr->g, "transporta/CNPJ", NULL);
		return nfe_grupo_set(tr->g, "transporta/CPF", NULL);
	}
	rc = valida(v);
	if (rc != 0)
		return rc;
	return transporta(tr, campo, v);
}

int nfe_transp_set_transporta_cnpj(nfe_transp *tr, const char *cnpj)
{
	return doc(tr, "CNPJ", cnpj, nfe_cnpj_validar);
}

int nfe_transp_set_transporta_cpf(nfe_transp *tr, const char *cpf)
{
	return doc(tr, "CPF", cpf, nfe_cpf_validar);
}

int nfe_transp_set_transporta_xnome(nfe_transp *tr, const char *xnome)
{
	return transporta(tr, "xNome", xnome);
}

int nfe_transp_set_transporta_ie(nfe_transp *tr, const char *ie)
{
	return transporta(tr, "IE", ie);
}

int nfe_transp_set_transporta_xender(nfe_transp *tr, const char *xender)
{
	return transporta(tr, "xEnder", xender);
}

int nfe_transp_set_transporta_xmun(nfe_transp *tr, const char *xmun)
{
	return transporta(tr, "xMun", xmun);
}

int nfe_transp_set_transporta_uf(nfe_transp *tr, const char *uf)
{
	return transporta(tr, "UF", uf);
}

int nfe_transp_add_vol(nfe_transp *tr, const char *qvol, const char *esp,
                       const char *marca, const char *nvol, const char *pesol,
                       const char *pesob)
{
	const char *const campos[] = { "qVol", "esp",   "marca",
		                       "nVol", "pesoL", "pesoB" };
	const char *valores[6];
	nfe_grupo *vol;
	int i, rc;

	if (!tr)
		return E_ISNULL;
	valores[0] = qvol;
	valores[1] = esp;
	valores[2] = marca;
	valores[3] = nvol;
	valores[4] = pesol;
	valores[5] = pesob;
	rc = nfe_grupo_add(tr->g, "vol", &vol);
	if (rc != 0)
		return rc;
	for (i = 0; rc == 0 && i < 6; i++)
		if (valores[i])
			rc = nfe_grupo_set(vol, campos[i], valores[i]);
	if (rc != 0)
		nfe_grupo_remove_ultimo(tr->g, "vol");
	return rc;
}

int nfe_transp_remove_vol(nfe_transp *tr)
{
	if (!tr)
		return E_ISNULL;
	return nfe_grupo_remove(tr->g, "vol");
}

int nfe_transp_write_xml(xmlTextWriterPtr writer, const nfe_transp *tr)
{
	if (!writer || !tr)
		return E_ISNULL;
	return nfe_grupo_write_xml(writer, tr->g);
}
