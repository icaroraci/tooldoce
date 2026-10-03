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

#ifndef LIBNFE_TRANSP_H
#define LIBNFE_TRANSP_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/grupo.h>
#include <libnfe/nfe.h>

/*
 * Transporte (grupo transp): modalidade do frete, transportador e volumes.
 *
 * Os campos mais usados têm setters próprios (abaixo). Os demais (retTransp,
 * veicTransp, reboque, vagao, balsa e os lacres de cada volume) são
 * preenchidos pelo grupo genérico (grupo.h) devolvido por
 * nfe_transp_grupo; os volumes são itens da lista "vol":
 *   nfe_grupo *vol = nfe_grupo_item(nfe_transp_grupo(tr), "vol", 0), *lac;
 *   nfe_grupo_add(vol, "lacres", &lac);
 *   nfe_grupo_set(lac, "nLacre", "123");
 *
 * Uso típico:
 *   nfe_transp *tr = nfe_transp_new();   (sem ocorrência de transporte)
 *   nfe_transp_set_modfrete(tr, NFE_FRETE_REMETENTE);
 *   nfe_transp_set_transporta_cnpj(tr, "12345678000195");
 *   nfe_transp_add_vol(tr, "2", "CAIXA", NULL, NULL, "10.500", "11.000");
 *   nfe_transp_write_xml(writer, tr);
 *   nfe_transp_free(tr);
 *
 * Os setters retornam 0, E_ISNULL, E_TAMANHO ou E_VALOR; em caso de erro o
 * objeto não é alterado. Textos seguem o tipo TString; campos opcionais são
 * removidos com NULL.
 */

typedef struct nfe_transp nfe_transp;

/* Número máximo de volumes */
#define NFE_MAX_VOL 5000

/* Cria o transporte com modFrete NFE_FRETE_SEM_TRANSPORTE. Retorna NULL se
 * faltar memória. */
nfe_transp *nfe_transp_new(void);

/* Libera o transporte; aceita NULL */
void nfe_transp_free(nfe_transp *tr);

/* Grupo genérico com todos os campos de <transp> (pertence ao transporte) */
nfe_grupo *nfe_transp_grupo(nfe_transp *tr);

int nfe_transp_set_modfrete(nfe_transp *tr, nfe_mod_frete modfrete);

/* Transportador (grupo transporta): só é escrito se algum campo for
 * informado. CNPJ e CPF se substituem; NULL remove o documento. */
int nfe_transp_set_transporta_cnpj(nfe_transp *tr, const char *cnpj);
int nfe_transp_set_transporta_cpf(nfe_transp *tr, const char *cpf);
int nfe_transp_set_transporta_xnome(nfe_transp *tr,
                                    const char *xnome); /* 2 a 60 */
/* IE: 2 a 14 dígitos ou "ISENTO" */
int nfe_transp_set_transporta_ie(nfe_transp *tr, const char *ie);
int nfe_transp_set_transporta_xender(nfe_transp *tr,
                                     const char *xender); /* 1 a 60 */
int nfe_transp_set_transporta_xmun(nfe_transp *tr,
                                   const char *xmun); /* 1 a 60 */
int nfe_transp_set_transporta_uf(nfe_transp *tr, const char *uf);

/* Acrescenta um volume (até NFE_MAX_VOL). Todos os campos são opcionais
 * (NULL omite): qvol (quantidade, 1 a 15 dígitos), esp (espécie), marca,
 * nvol (numeração), com 1 a 60 caracteres, e pesol / pesob (pesos líquido e
 * bruto em kg, com 3 casas: "10.500"). Retorna 0, E_ISNULL, E_TAMANHO,
 * E_VALOR (valor inválido ou limite atingido) ou E_MALLOC. */
int nfe_transp_add_vol(nfe_transp *tr, const char *qvol, const char *esp,
                       const char *marca, const char *nvol, const char *pesol,
                       const char *pesob);
/* Apaga todos os volumes */
int nfe_transp_remove_vol(nfe_transp *tr);

/* Escreve o elemento <transp>. Retorna 0, E_ISNULL, E_VALOR (subgrupo
 * incompleto) ou E_XML. */
int nfe_transp_write_xml(xmlTextWriterPtr writer, const nfe_transp *tr);

#endif
