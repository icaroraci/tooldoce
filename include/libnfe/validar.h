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

#ifndef LIBNFE_VALIDAR_H
#define LIBNFE_VALIDAR_H

#include <stddef.h>

/*
 * Validação do XML da NF-e contra os schemas oficiais (XSD), antes de
 * assinar e transmitir.
 *
 * O validador carrega os schemas uma vez (o que é demorado) e pode ser
 * reaproveitado em muitas validações:
 *   nfe_validador *v = nfe_validador_new(NULL);   (schemas instalados)
 *   nfe_erros *erros = nfe_erros_new();
 *   if (nfe_validar_xml(v, xml, strlen(xml), erros) != 0)
 *       for (i = 0; i < nfe_erros_qtd(erros); i++)
 *           ... nfe_erros_msg(erros, i), nfe_erros_campo(erros, i) ...
 *   nfe_erros_free(erros);
 *   nfe_validador_free(v);
 *
 * O documento pode estar assinado ou não: sem a assinatura (ds:Signature),
 * a nota é validada como se estivesse assinada, ou seja, todo o resto do
 * documento é conferido, inclusive a ordem e o formato de cada campo.
 *
 * Se o schema aceitar o documento, são conferidas também algumas regras de
 * validação da SEFAZ que o schema não cobre (entre parênteses, o código de
 * rejeição da SEFAZ, devolvido por nfe_erros_codigo; 0 quando a regra não
 * tem um código único):
 *   - chave de acesso (Id) com dígito verificador correto (236) e
 *     correspondente a cUF, dhEmi, CNPJ/CPF do emitente, mod, serie, nNF,
 *     tpEmis, cNF e cDV (502);
 *   - totais de ICMSTot iguais à soma dos itens: vProd dos itens com
 *     indTot=1 (564), vBC (531), vICMS (532), vBCST (533) e vST (534);
 *   - vNF = vProd - vDesc + vST + vFCPST + vFrete + vSeg + vOutro + vII +
 *     vIPI + vIPIDevol + vServ, com ou sem a dedução de vICMSDeson (610);
 *   - UF e município do emitente e município do fato gerador (cMunFG) na
 *     UF de cUF (0);
 *   - forma de emissão: dhCont e xJust proibidos na emissão normal (556)
 *     e obrigatórios nas contingências tpEmis 2, 4, 5 e 9 (557); SCAN
 *     (tpEmis=3) extinta (570); NF-e sem contingência off-line (711);
 *     NFC-e sem formulário de segurança (tpEmis 2 ou 5) (714) e sem SVC
 *     (tpEmis 6 ou 7) (783);
 *   - série: 890 a 919 só na emissão pelo Fisco, procEmi 1 ou 2 (451); o
 *     contribuinte usa 0 a 889 e 920 a 969 (244);
 *   - indIntermed obrigatório com indPres 2, 3, 4 ou 9 (434) e proibido
 *     nos demais (435);
 *   - NF-e (mod 55): sem DANFE NFC-e, tpImp 4 ou 5 (710), e sem entrega a
 *     domicílio, indPres=4 (794);
 *   - NFC-e (mod 65): saída (tpNF=1) (706), operação interna (idDest=1)
 *     (707), sem NFref (708), DANFE NFC-e (tpImp 4 ou 5) (709), finalidade
 *     normal (finNFe=1) (715), consumidor final (indFinal=1) (716) e
 *     presencial ou entrega a domicílio (indPres 1 ou 4) (717).
 * As demais regras (datas, campos obrigatórios por condição, cadastros da
 * SEFAZ etc.) ficam para a SEFAZ, que devolve a rejeição na transmissão.
 */

typedef struct nfe_erros nfe_erros;
typedef struct nfe_validador nfe_validador;

/* Lista de erros encontrados. Retorna NULL se faltar memória. */
nfe_erros *nfe_erros_new(void);
void nfe_erros_free(nfe_erros *erros);
/* Apaga os erros (nfe_validar_xml já começa apagando) */
void nfe_erros_limpa(nfe_erros *erros);
/* Quantidade de erros */
int nfe_erros_qtd(const nfe_erros *erros);
/* Erro i (a partir de 0): mensagem (em inglês, da libxml2, para erros de
 * schema; em português, para as regras da SEFAZ), nome do campo
 * (elemento) em que ocorreu, se conhecido, e linha no documento (0 se
 * desconhecida). Texto pertencente à lista; NULL/0 se i não existir. */
const char *nfe_erros_msg(const nfe_erros *erros, int i);
const char *nfe_erros_campo(const nfe_erros *erros, int i);
int nfe_erros_linha(const nfe_erros *erros, int i);
/* Código de rejeição da SEFAZ correspondente ao erro i, ou 0 (erro de
 * schema, regra sem código único ou i inexistente) */
int nfe_erros_codigo(const nfe_erros *erros, int i);

/* Diagnóstico estrutural automático. Os textos pertencem à lista e ficam
 * válidos até limpa/free ou a próxima validação. NULL indica informação
 * indisponível; valor "" é um conteúdo vazio, diferente de NULL.
 * Caminho absoluto para exibição, com prefixos do XML e índices quando
 * houver irmãos do mesmo nome/namespace; atributos usam /@nome. Não é
 * uma expressão XPath independente dos namespaces do documento.
 * Valor é o conteúdo textual do nó (após o parser XML); quando o nó
 * escalar não é fornecido ou o pai pode representar um atributo, usa
 * o valor da faceta, que pode estar normalizado pelo XSD.
 * Grupos não têm valor escalar. A libxml2 pode indicar apenas o pai de
 * um atributo: nesse caso campo/caminho indicam o pai, e a mensagem
 * identifica o atributo.
 * restricao é o nome da faceta XSD violada (pattern, enumeration,
 * minLength...), esperado é o padrão/lista/limite fornecido pela libxml2.
 * Não há catálogo de tags nem tradução de regex para máscara. Erros
 * estruturais, tipos compostos e casos sem faceta usam a mensagem original
 * como diagnóstico; restricao/esperado podem ser NULL. */
const char *nfe_erros_caminho(const nfe_erros *erros, int i);
const char *nfe_erros_valor(const nfe_erros *erros, int i);
const char *nfe_erros_restricao(const nfe_erros *erros, int i);
const char *nfe_erros_esperado(const nfe_erros *erros, int i);
/* Domínio e código originais da libxml2 (xmlErrorDomain/xmlParserErrors),
 * distintos de cStat e dos retornos E_*. 0 se regra local ou inexistente. */
int nfe_erros_dominio_xml(const nfe_erros *erros, int i);
int nfe_erros_codigo_xml(const nfe_erros *erros, int i);

/* Diretório padrão dos schemas, onde make install os coloca */
const char *nfe_dir_schemas(void);

/* Carrega os schemas de dir_schemas (NULL: nfe_dir_schemas()), que deve
 * ter nfe_v4.00.xsd e os arquivos que ele inclui. Retorna NULL se não for
 * possível carregá-los ou faltar memória. */
nfe_validador *nfe_validador_new(const char *dir_schemas);
/* Validador de outro leiaute (MDF-e, CT-e...) ou de outra mensagem: carrega
 * o schema caminho_xsd (com os arquivos que ele inclui, na mesma pasta).
 * Use com nfe_validar_xsd. Retorna NULL se caminho_xsd for NULL, se não
 * for possível carregá-lo ou se faltar memória. */
nfe_validador *nfe_validador_xsd(const char *caminho_xsd);
void nfe_validador_free(nfe_validador *v);

/* Valida o documento xml (tam bytes; não precisa terminar em '\0') e
 * acrescenta os problemas em erros (que pode ser NULL). Retorna 0 (válido),
 * E_VALOR (inválido contra o schema ou contra as regras acima), E_XML
 * (documento malformado ou que não é uma NF-e), E_ISNULL ou E_MALLOC. */
int nfe_validar_xml(nfe_validador *v, const char *xml, size_t tam,
                    nfe_erros *erros);

/* Valida o documento xml só contra o schema de v, sem as regras da NF-e
 * nem exigir a raiz <NFe>. Com completar_assinatura diferente de 0, um
 * documento sem <Signature> filha da raiz é validado como se a tivesse ao
 * fim da raiz (o padrão da NF-e, do MDF-e e do CT-e); use 0 para
 * mensagens que não são assinadas. Retorna 0 (válido), E_VALOR (inválido),
 * E_XML (malformado), E_ISNULL ou E_MALLOC. */
int nfe_validar_xsd(nfe_validador *v, const char *xml, size_t tam,
                    int completar_assinatura, nfe_erros *erros);

#endif
