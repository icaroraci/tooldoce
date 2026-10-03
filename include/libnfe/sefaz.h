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

#ifndef LIBNFE_SEFAZ_H
#define LIBNFE_SEFAZ_H

#include <stddef.h>

#include <libnfe/assinatura.h>
#include <libnfe/nfe.h>

/*
 * Comunicação com os webservices da SEFAZ (SOAP 1.2 sobre HTTPS, com
 * autenticação mútua pelo certificado A1 do emitente).
 *
 * A conexão guarda o certificado e as opções; cada chamada a
 * nfe_sefaz_enviar envia uma mensagem (montada pelas funções
 * nfe_sefaz_msg_*) ao endereço do webservice e devolve o elemento de
 * retorno da SEFAZ:
 *   nfe_sefaz *s = nfe_sefaz_new(cert);
 *   char *msg, *ret;
 *   size_t tam;
 *   int cstat;
 *   char motivo[256];
 *   nfe_sefaz_msg_status(NFE_AMBIENTE_HOMOLOGACAO, NFE_UF_SP, &msg);
 *   rc = nfe_sefaz_enviar(s, url, NFE_SERVICO_STATUS, msg, &ret, &tam);
 *   if (rc == 0)
 *       nfe_sefaz_cstat(ret, tam, &cstat, motivo, sizeof motivo);
 *   else
 *       ... nfe_sefaz_erro(s) ...
 *   free(msg); free(ret);
 *   nfe_sefaz_free(s);
 *
 * Os endereços (url) de cada webservice, por UF e ambiente, são publicados
 * no Portal Nacional da NF-e (Relação de Serviços Web). Para NF-e modelo
 * 55, nfe_sefaz_endereco consulta a tabela local por UF, ambiente e emissão.
 * Também é possível informar uma URL manualmente.
 *
 * Fluxo da emissão: monte a nota (nfe_nfe.h), assine (assinatura.h), envie
 * o lote com nfe_sefaz_msg_lote ao serviço de autorização e, se a SEFAZ
 * autorizar (cStat 100 no protNFe), junte a nota e o protocolo com
 * nfe_sefaz_proc: o resultado (nfeProc) é o XML a guardar e enviar ao
 * destinatário. No envio assíncrono, a SEFAZ devolve um recibo (nRec), e o
 * resultado é consultado depois com nfe_sefaz_msg_recibo.
 *
 * As funções retornam 0 ou um código de erro (erros.h); E_REDE indica
 * falha na comunicação (endereço inválido, servidor fora do ar,
 * certificado recusado, resposta HTTP de erro ou SOAP Fault), com a
 * descrição em nfe_sefaz_erro.
 */

typedef struct nfe_sefaz nfe_sefaz;

/* Webservices da NF-e 4.00 */
typedef enum nfe_servico {
	NFE_SERVICO_AUTORIZACAO,     /* NFeAutorizacao4: envio do lote */
	NFE_SERVICO_RET_AUTORIZACAO, /* NFeRetAutorizacao4: consulta recibo */
	NFE_SERVICO_CONSULTA,        /* NFeConsultaProtocolo4: pela chave */
	NFE_SERVICO_STATUS,          /* NFeStatusServico4 */
	NFE_SERVICO_EVENTO,          /* NFeRecepcaoEvento4 */
	NFE_SERVICO_INUTILIZACAO     /* NFeInutilizacao4 */
} nfe_servico;

/* Endereço de um serviço da NF-e modelo 55, sem acesso à rede. Aceita
 * emissão normal ou contingência SVC-AN/SVC-RS compatível com a UF no
 * ambiente escolhido. Não seleciona contingência automaticamente, nem
 * verifica se ela está ativada. Não usar para NFC-e (modelo 65).
 * Em sucesso, *url recebe texto estático (não libere nem altere).
 * Retorna 0, E_ISNULL (url nulo) ou E_VALOR (parâmetro inválido,
 * contingência incompatível ou serviço ausente na tabela). Em falha,
 * *url permanece inalterado. Fontes e atualização: docs/WEBSERVICES.md. */
int nfe_sefaz_endereco(nfe_uf uf, nfe_ambiente amb, nfe_emissao emissao,
                       nfe_servico servico, const char **url);

/* Cria a conexão com o certificado do emitente (que deve continuar
 * existindo enquanto a conexão for usada). Retorna NULL se cert for NULL
 * ou faltar memória. */
nfe_sefaz *nfe_sefaz_new(const nfe_certificado *cert);
void nfe_sefaz_free(nfe_sefaz *s);

/* Arquivo PEM com as autoridades certificadoras em que confiar para
 * conferir o certificado do servidor da SEFAZ (por exemplo, a cadeia
 * ICP-Brasil); NULL volta às autoridades do sistema. Retorna 0, E_ISNULL
 * ou E_MALLOC. */
int nfe_sefaz_set_ca(nfe_sefaz *s, const char *arquivo_pem);

/* Tempo máximo de cada envio, em segundos (padrão: 60). Retorna 0,
 * E_ISNULL ou E_VALOR (menor que 1). */
int nfe_sefaz_set_timeout(nfe_sefaz *s, long segundos);

/* Envia msg (ex.: <consStatServ>, sem o envelope SOAP) ao webservice url
 * (https) do serviço e devolve em *resposta o elemento de retorno da SEFAZ
 * (ex.: <retConsStatServ>), alocado e terminado em '\0' (libere com
 * free()); o tamanho vai em *tam, se não for NULL. Retorna 0, E_ISNULL,
 * E_VALOR (serviço inválido), E_REDE (ver nfe_sefaz_erro), E_XML
 * (resposta sem o elemento de retorno) ou E_MALLOC. */
int nfe_sefaz_enviar(nfe_sefaz *s, const char *url, nfe_servico servico,
                     const char *msg, char **resposta, size_t *tam);

/* Descrição da última falha de nfe_sefaz_enviar ("" se não houve);
 * texto pertencente à conexão */
const char *nfe_sefaz_erro(const nfe_sefaz *s);

/* ---- Mensagens (alocadas, terminadas em '\0'; libere com free()) ----
 * Retornam 0, E_ISNULL, E_VALOR (valor inválido) ou E_MALLOC. */

/* consStatServ: status do serviço da UF */
int nfe_sefaz_msg_status(nfe_ambiente amb, nfe_uf uf, char **msg);

/* enviNFe: lote com n (1 a 50) notas assinadas (documentos <NFe>, com ou
 * sem a declaração <?xml?>), identificado por id_lote (1 a 15 dígitos).
 * sincrono: 1 pede o resultado na própria resposta (só lotes de uma nota),
 * 0 pede um recibo para consulta posterior. */
int nfe_sefaz_msg_lote(const char *id_lote, int sincrono,
                       const char *const *nfes, int n, char **msg);

/* consReciNFe: resultado do lote pelo recibo (nRec, 15 dígitos) */
int nfe_sefaz_msg_recibo(nfe_ambiente amb, const char *nrec, char **msg);

/* consSitNFe: situação da nota pela chave de acesso */
int nfe_sefaz_msg_consulta(nfe_ambiente amb, const char *chave, char **msg);

/* envEvento: lote com n (1 a 20) eventos assinados (documentos <evento>,
 * ver evento.h), identificado por id_lote (1 a 15 dígitos) */
int nfe_sefaz_msg_evento(const char *id_lote, const char *const *eventos, int n,
                         char **msg);

/* ---- Leitura do retorno ---- */

/* cStat e xMotivo do elemento de retorno: os do lote ou do serviço (no
 * retorno completo) ou os da nota ou do evento (num <protNFe> ou
 * <retEvento>). xmotivo (pode ser NULL) recebe o texto truncado em
 * tam_xmotivo. Retorna 0, E_ISNULL ou E_XML (retorno malformado ou sem
 * cStat). */
int nfe_sefaz_cstat(const char *ret, size_t tam, int *cstat, char *xmotivo,
                    size_t tam_xmotivo);

/* Protocolo (<protNFe>) da nota com a chave (NULL: o primeiro) dentro do
 * retorno, alocado em *prot (libere com free()); o tamanho vai em
 * *tam_prot, se não for NULL. Retorna 0, E_ISNULL, E_XML (retorno
 * malformado), E_VALOR (protocolo não encontrado) ou E_MALLOC. */
int nfe_sefaz_protocolo(const char *ret, size_t tam, const char *chave,
                        char **prot, size_t *tam_prot);

/* nfeProc: a nota assinada (<NFe>) com o seu protocolo (<protNFe>),
 * alocado em *proc (libere com free()). A nota é copiada sem alterações,
 * preservando a assinatura. Retorna 0, E_ISNULL, E_XML (documentos
 * malformados), E_VALOR (o protocolo é de outra nota) ou E_MALLOC. */
int nfe_sefaz_proc(const char *nfe, size_t tam_nfe, const char *prot,
                   size_t tam_prot, char **proc, size_t *tam_proc);

/* procEventoNFe: o evento assinado (<evento>) com o seu registro
 * (<retEvento>, procurado no retorno ret pela chave, tipo e sequência do
 * evento), alocado em *proc (libere com free()). O evento é copiado sem
 * alterações. Retorna 0, E_ISNULL, E_XML (documentos malformados), E_VALOR
 * (retorno sem o registro do evento) ou E_MALLOC. */
int nfe_sefaz_proc_evento(const char *evento, size_t tam_evento,
                          const char *ret, size_t tam_ret, char **proc,
                          size_t *tam_proc);

#endif
