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

#ifndef LIBNFE_NFE_NFE_H
#define LIBNFE_NFE_NFE_H

#include <stddef.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/dest.h>
#include <libnfe/det.h>
#include <libnfe/emit.h>
#include <libnfe/ide.h>
#include <libnfe/pag.h>
#include <libnfe/total.h>
#include <libnfe/transp.h>

/*
 * Nota fiscal completa (NF-e modelo 55 ou NFC-e modelo 65): junta os grupos
 * de infNFe e gera o documento XML.
 *
 * Grupos implementados: ide, emit, dest, det, total, transp e pag. Os
 * opcionais avulsa, retirada, entrega, autXML, cobr, infIntermed, infAdic,
 * exporta, compra, cana, infRespTec, infSolicNFF, agropecuario e infPAA
 * ainda não.
 *
 * O documento gerado ainda não tem a assinatura digital (ds:Signature),
 * exigida pela SEFAZ e pelo schema nfe_v4.00.xsd.
 *
 * Uso típico:
 *   nfe_nfe *nota = nfe_nfe_new();
 *   nfe_nfe_set_ide(nota, ide);        (a nota passa a ser dona de cada
 *   nfe_nfe_set_emit(nota, emit);       grupo informado)
 *   nfe_nfe_add_det(nota, det);
 *   nfe_nfe_set_total(nota, total);
 *   nfe_nfe_set_transp(nota, transp);
 *   nfe_nfe_set_pag(nota, pag);
 *   nfe_nfe_xml(nota, &xml, &tam);     (libere xml com free())
 *   nfe_nfe_free(nota);
 */

typedef struct nfe_nfe nfe_nfe;

/* Cria uma nota vazia. Retorna NULL se faltar memória. */
nfe_nfe *nfe_nfe_new(void);

/* Libera a nota e todos os grupos que ela possui; aceita NULL */
void nfe_nfe_free(nfe_nfe *nfe);

/* Em caso de sucesso a nota passa a ser dona do grupo (e libera o
 * anterior). Retornam 0 ou E_ISNULL. */
int nfe_nfe_set_ide(nfe_nfe *nfe, nfe_ide *ide);
int nfe_nfe_set_emit(nfe_nfe *nfe, nfe_emit *emit);
/* dest é opcional na NFC-e: NULL remove */
int nfe_nfe_set_dest(nfe_nfe *nfe, nfe_dest *dest);
int nfe_nfe_set_total(nfe_nfe *nfe, nfe_total *total);
int nfe_nfe_set_transp(nfe_nfe *nfe, nfe_transp *transp);
int nfe_nfe_set_pag(nfe_nfe *nfe, nfe_pag *pag);

/* Acrescenta um item (até NFE_MAX_ITENS). O número do item (nItem) passa a
 * ser a sua posição, a partir de 1. Em caso de sucesso a nota passa a ser
 * dona de det. Retorna 0, E_ISNULL ou E_VALOR (limite atingido). */
int nfe_nfe_add_det(nfe_nfe *nfe, nfe_det *det);

/* Gera a chave de acesso (44 caracteres) em chave (tam >= 45), a partir do
 * ide e do CNPJ/CPF do emitente, e grava o dígito verificador em cDV (ver
 * nfe_ide_gerar_chave). Retorna 0, E_ISNULL, E_TAMANHO ou E_VALOR (falta
 * ide, emitente ou algum campo da chave). */
int nfe_nfe_chave(nfe_nfe *nfe, char *chave, size_t tam);

/* Escreve o elemento <NFe xmlns="http://www.portalfiscal.inf.br/nfe"> com
 * <infNFe versao="4.00" Id="NFe{chave}">. Gera a chave como
 * nfe_nfe_chave. Retorna 0, E_ISNULL, E_VALOR (falta ide, emit, item,
 * total, transp ou pag, ou algum grupo está incompleto) ou E_XML. */
int nfe_nfe_write_xml(xmlTextWriterPtr writer, nfe_nfe *nfe);

/* Gera o documento XML completo (UTF-8, sem espaços entre as tags) em um
 * buffer alocado, terminado em '\0', que deve ser liberado com free(); o
 * tamanho (sem o terminador) vai em *tam, se tam não for NULL. Retorna os
 * mesmos códigos de nfe_nfe_write_xml, ou E_MALLOC. */
int nfe_nfe_xml(nfe_nfe *nfe, char **xml, size_t *tam);

/* Grava o documento XML completo no arquivo caminho (substitui o
 * existente). Retorna os mesmos códigos de nfe_nfe_xml, ou E_ARQUIVO. */
int nfe_nfe_salvar(nfe_nfe *nfe, const char *caminho);

#endif
