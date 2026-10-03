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

#ifndef LIBNFE_IDE_H
#define LIBNFE_IDE_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/nfe.h>

/*
 * Grupo ide: identificação da NF-e.
 *
 * Uso:
 *   nfe_ide *ide = nfe_ide_new();
 *   nfe_ide_set_cuf(ide, NFE_UF_SP);
 *   nfe_ide_set_natop(ide, "VENDA");
 *   ...
 *   nfe_ide_write_xml(writer, ide);
 *   nfe_ide_free(ide);
 *
 * Os setters validam o valor contra o leiaute 4.00 e retornam 0, E_ISNULL,
 * E_VALOR (valor fora do domínio) ou E_TAMANHO (texto fora dos limites);
 * em caso de erro o campo não é alterado.
 */

typedef struct nfe_ide nfe_ide;

struct refNFe_s;
struct refNF_s;

/* Data/hora opcional não informada (ex.: dhSaiEnt) */
#define NFE_SEM_DATA ((time_t) - 1)

/* Número máximo de documentos referenciados (grupo NFref: maxOccurs="999"
 * no leiauteNFe_v4.00.xsd) */
#define NFE_MAX_NFREF 999

/* Cria um ide com os valores padrão: mod 55, tpNF saída, idDest interna,
 * tpImp DANFE retrato, tpEmis normal, tpAmb HOMOLOGAÇÃO, finNFe normal,
 * indFinal normal, indPres não se aplica, procEmi aplicativo do
 * contribuinte, fuso de Brasília. Retorna NULL se faltar memória. */
nfe_ide *nfe_ide_new(void);

/* Libera o ide e as referências que ele possui; aceita NULL */
void nfe_ide_free(nfe_ide *ide);

int nfe_ide_set_cuf(nfe_ide *ide, nfe_uf cuf);
int nfe_ide_set_cnf(nfe_ide *ide, uint32_t cnf); /* 0 a 99999999 */
/* Textos (natOp, verProc, xJust): limites em caracteres UTF-8, só
 * caracteres de U+0020 a U+00FF e sem espaço no início ou no fim (tipo
 * TString do leiaute); fora disso, E_TAMANHO ou E_VALOR */
int nfe_ide_set_natop(nfe_ide *ide, const char *natop); /* 1 a 60 caracteres */
int nfe_ide_set_mod(nfe_ide *ide, nfe_modelo mod);
int nfe_ide_set_serie(nfe_ide *ide, unsigned serie); /* 0 a 999 */
int nfe_ide_set_nnf(nfe_ide *ide, uint32_t nnf);     /* 1 a 999999999 */
int nfe_ide_set_dhemi(nfe_ide *ide, time_t dhemi);
int nfe_ide_set_dhsaient(nfe_ide *ide,
                         time_t dhsaient); /* aceita NFE_SEM_DATA */
int nfe_ide_set_tpnf(nfe_ide *ide, nfe_tipo_operacao tpnf);
int nfe_ide_set_iddest(nfe_ide *ide, nfe_destino iddest);
int nfe_ide_set_cmunfg(nfe_ide *ide, uint32_t cmunfg); /* 7 dígitos */
int nfe_ide_set_tpimp(nfe_ide *ide, nfe_danfe tpimp);
int nfe_ide_set_tpemis(nfe_ide *ide, nfe_emissao tpemis);
int nfe_ide_set_cdv(nfe_ide *ide, unsigned cdv); /* 0 a 9 */
int nfe_ide_set_tpamb(nfe_ide *ide, nfe_ambiente tpamb);
int nfe_ide_set_finnfe(nfe_ide *ide, nfe_finalidade finnfe);
int nfe_ide_set_indfinal(nfe_ide *ide, nfe_consumidor indfinal);
int nfe_ide_set_indpres(nfe_ide *ide, nfe_presenca indpres);
int nfe_ide_set_procemi(nfe_ide *ide, nfe_processo_emissao procemi);
int nfe_ide_set_verproc(nfe_ide *ide,
                        const char *verproc); /* 1 a 20 caracteres */

/* Campos opcionais incluídos pela Reforma Tributária (PL_010f) e pela NT
 * 2020.006. Cada um só é escrito no XML se tiver sido informado. */

/* dPrevEntrega: data prevista de entrega (só a data, no fuso do ide);
 * NFE_SEM_DATA remove */
int nfe_ide_set_dpreventrega(nfe_ide *ide, time_t dpreventrega);
/* cMunFGIBS: município do fato gerador do IBS/CBS (7 dígitos); 0 remove */
int nfe_ide_set_cmunfgibs(nfe_ide *ide, uint32_t cmunfgibs);
/* tpNFDebito / tpNFCredito: tipo da nota de débito ou de crédito;
 * NFE_DEBITO_NAO_INFORMADO / NFE_CREDITO_NAO_INFORMADO removem */
int nfe_ide_set_tpnfdebito(nfe_ide *ide, nfe_tipo_debito tpnfdebito);
int nfe_ide_set_tpnfcredito(nfe_ide *ide, nfe_tipo_credito tpnfcredito);
/* indIntermed: intermediador; NFE_INTERMEDIADOR_NAO_INFORMADO remove */
int nfe_ide_set_indintermed(nfe_ide *ide, nfe_intermediador indintermed);
/* cIndOp: código indicador do local da operação (6 dígitos); NULL remove */
int nfe_ide_set_cindop(nfe_ide *ide, const char *cindop);

/* Fuso horário em que as datas (dhEmi, dhSaiEnt, dhCont) são escritas */
int nfe_ide_set_tzd(nfe_ide *ide, nfe_tzd tzd);

/* Entrada em contingência: instante e justificativa (15 a 256 caracteres).
 * Gera dhCont e xJust no XML. */
int nfe_ide_set_contingencia(nfe_ide *ide, time_t dhcont, const char *xjust);

/* Gera a chave de acesso da nota em chave (tam >= 45 bytes) a partir de
 * cUF, dhEmi (ano e mês no fuso do ide), mod, serie, nNF, tpEmis e cNF do
 * ide, e do CNPJ (14 posições, numérico ou alfanumérico) ou CPF (11
 * dígitos) do emitente. Calcula o dígito verificador e o grava em cDV.
 * Defina todos esses campos antes: alterá-los depois invalida a chave.
 * Retorna 0, E_ISNULL, E_TAMANHO (buffer pequeno ou CNPJ/CPF com tamanho
 * errado) ou E_VALOR (cUF, nNF ou dhEmi não informados, ou CNPJ/CPF
 * inválido; ver cnpjcpf.h). */
int nfe_ide_gerar_chave(nfe_ide *ide, const char *cnpjcpf, char *chave,
                        size_t tam);

/* Documentos fiscais referenciados (grupo NFref, até NFE_MAX_NFREF), gerados
 * na ordem em que foram adicionados. Em caso de sucesso, o ide passa a ser
 * dono da referência e a libera em nfe_ide_free. Retornam 0, E_ISNULL,
 * E_VALOR (limite atingido) ou E_MALLOC. */
int nfe_ide_add_refnfe(nfe_ide *ide, struct refNFe_s *ref);
int nfe_ide_add_refnf(nfe_ide *ide, struct refNF_s *ref);

/* Escreve o elemento <ide>. Retorna 0, E_ISNULL, E_VALOR (campo obrigatório
 * sem valor padrão não informado: cUF, natOp, nNF, dhEmi, cMunFG ou verProc)
 * ou E_XML. */
int nfe_ide_write_xml(xmlTextWriterPtr writer, const nfe_ide *ide);

#endif
