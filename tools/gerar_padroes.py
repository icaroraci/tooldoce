#!/usr/bin/env python3
"""Gera include/libnfe/padroes.h a partir dos tipos simples nomeados do XSD.

Para cada xs:simpleType de tiposBasico_v4.00.xsd, DFeTiposBasicos_v1.00.xsd
e leiauteNFe_v4.00.xsd são gerados, conforme as facetas do tipo:

  NFE_PADRAO_<tipo>    o xs:pattern, como string C (sintaxe do XML Schema,
                       usada por nfe_valida_padrao)
  NFE_TAM_MIN_<tipo>   xs:minLength / xs:length
  NFE_TAM_MAX_<tipo>   xs:maxLength / xs:length
  NFE_VALORES_<tipo>   os xs:enumeration, separados por vírgula, para montar
                       uma lista: { NFE_VALORES_TUf, NULL }

Uso: python3 tools/gerar_padroes.py [--verificar]
  --verificar: não grava; sai com código 1 se o arquivo estiver desatualizado
"""

import os
import sys
import xml.etree.ElementTree as ET

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCHEMAS = os.path.join(RAIZ, "tests", "schemas", "nfe")
ARQUIVOS = ("tiposBasico_v4.00.xsd", "DFeTiposBasicos_v1.00.xsd",
            "leiauteNFe_v4.00.xsd")
SAIDA = os.path.join(RAIZ, "include", "libnfe", "padroes.h")
XS = "{http://www.w3.org/2001/XMLSchema}"

# Tipos cuja base é um tipo primitivo do XML Schema sem padrão equivalente
# nos arquivos: padrão escrito à mão, com a mesma semântica
PRIMITIVOS = {
    "xs:gYearMonth": "[0-9]{4}-(0[1-9]|1[0-2])",
}

CABECALHO = """\
/* Gerado por tools/gerar_padroes.py a partir de tests/schemas/nfe.
 * Não edite à mão: rode `python3 tools/gerar_padroes.py`.
 *
 * Padrões (NFE_PADRAO_*) na sintaxe de expressões regulares do XML Schema,
 * já ancorados ao valor inteiro; use com nfe_valida_padrao() (valida.h). */

#ifndef LIBNFE_PADROES_H
#define LIBNFE_PADROES_H

/* clang-format off */
"""

RODAPE = """\
/* clang-format on */

#endif
"""


def literal_c(texto):
    """String C com o texto em UTF-8; bytes não ASCII viram escapes \\x. Se
    o caractere seguinte a um escape for um dígito hexadecimal, a string é
    partida para que ele não seja lido como parte do escape."""
    partes = [""]
    escape = False
    for b in texto.encode("utf-8"):
        c = chr(b)
        if b >= 0x80:
            partes[-1] += "\\x%02X" % b
            escape = True
            continue
        if escape and c in "0123456789abcdefABCDEF":
            partes.append("")
        escape = False
        partes[-1] += "\\" + c if c in "\\\"" else c
    return " ".join('"%s"' % p for p in partes)


def tipos():
    """[(nome, base, facetas)] na ordem dos arquivos; nomes repetidos com
    definição diferente são erro."""
    vistos = {}
    saida = []
    for arquivo in ARQUIVOS:
        raiz = ET.parse(os.path.join(SCHEMAS, arquivo)).getroot()
        for t in raiz.findall(XS + "simpleType"):
            nome = t.get("name")
            r = t.find(XS + "restriction")
            facetas = {"enumeration": []}
            for f in r:
                chave = f.tag.replace(XS, "")
                if chave == "enumeration":
                    facetas["enumeration"].append(f.get("value"))
                elif chave == "pattern":
                    if "pattern" in facetas:
                        sys.exit(f"{nome}: mais de um xs:pattern")
                    facetas["pattern"] = f.get("value")
                elif chave in ("minLength", "maxLength", "length"):
                    facetas[chave] = f.get("value")
            item = (nome, r.get("base"), facetas)
            if nome in vistos:
                if vistos[nome] != item:
                    sys.exit(f"{nome}: definido de formas diferentes")
                continue
            vistos[nome] = item
            saida.append(item)
    return saida


def gerar():
    linhas = [CABECALHO]
    nomes = {t[0] for t in tipos()}
    com_padrao = []
    for nome, base, f in tipos():
        defs = []
        base_local = base.split(":")[-1]
        if "pattern" in f:
            defs.append(("NFE_PADRAO_" + nome, literal_c(f["pattern"])))
        elif base in PRIMITIVOS:
            defs.append(("NFE_PADRAO_" + nome, literal_c(PRIMITIVOS[base])))
        elif base_local in nomes:
            # tipo derivado sem padrão próprio: herda o da base
            defs.append(("NFE_PADRAO_" + nome, "NFE_PADRAO_" + base_local))
        minimo = f.get("length", f.get("minLength"))
        maximo = f.get("length", f.get("maxLength"))
        if minimo is not None:
            defs.append(("NFE_TAM_MIN_" + nome, minimo))
        if maximo is not None:
            defs.append(("NFE_TAM_MAX_" + nome, maximo))
        if f["enumeration"]:
            defs.append(("NFE_VALORES_" + nome,
                         ", ".join(literal_c(v) for v in f["enumeration"])))
        if defs and defs[0][0].startswith("NFE_PADRAO_"):
            com_padrao.append(nome)
        if not defs:
            continue
        linhas.append(f"/* {nome} (base {base}) */\n")
        for macro, valor in defs:
            linhas.append(f"#define {macro} {valor}\n")
        linhas.append("\n")
    # lista X-macro de todos os tipos com padrão (usada nos testes)
    linhas.append("/* Todos os tipos com NFE_PADRAO_*: NFE_TIPOS_COM_PADRAO(X) chama\n"
                  " * X(tipo) para cada um */\n")
    linhas.append("#define NFE_TIPOS_COM_PADRAO(X) \\\n")
    linhas.append(" \\\n".join(f"\tX({n})" for n in com_padrao))
    linhas.append("\n\n")
    linhas.append(RODAPE)
    return "".join(linhas)


def main():
    conteudo = gerar()
    if "--verificar" in sys.argv[1:]:
        atual = open(SAIDA, encoding="utf-8").read() \
            if os.path.exists(SAIDA) else ""
        if atual != conteudo:
            print(f"{SAIDA} desatualizado: rode python3 tools/gerar_padroes.py")
            return 1
        return 0
    with open(SAIDA, "w", encoding="utf-8") as f:
        f.write(conteudo)
    print(f"gerado {SAIDA}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
