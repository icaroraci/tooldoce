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
 *   - NFC-e (mod 65): consumidor final (indFinal=1), operação interna
 *     (idDest=1) e DANFE NFC-e (tpImp 4 ou 5) (0).
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
