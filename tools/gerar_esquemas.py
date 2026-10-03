#!/usr/bin/env python3
"""Gera src/libnfe/esquemas.c: a estrutura de grupos do leiaute, como
tabelas C, para o motor genérico de grupos (src/libnfe/grupo.c).

Para cada raiz (por exemplo, <imposto> de det) a tabela descreve, em
profundidade, os nós do XSD: elementos, sequências e escolhas, com
minOccurs, o padrão (xs:pattern), os valores (xs:enumeration) e os limites
de tamanho de cada campo, e se o campo é texto livre (tipo TString). As
folhas de uma raiz recebem índices consecutivos, de modo que as folhas de
qualquer nó formam um intervalo.

Elementos repetidos (maxOccurs > 1) ganham tabela própria e aparecem na
tabela do pai como listas (ESQ_LISTA); atributos são folhas (ESQ_ATTR).
Conteúdo misto, simpleContent e sequências repetidas não são suportados (o
gerador falha se encontrar algum).

Uso: python3 tools/gerar_esquemas.py [--verificar]
"""

import os
import sys
import xml.etree.ElementTree as ET

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gerar_padroes import literal_c, PRIMITIVOS  # noqa: E402

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCHEMAS = os.path.join(RAIZ, "tests", "schemas", "nfe")
ARQUIVOS = ("tiposBasico_v4.00.xsd", "DFeTiposBasicos_v1.00.xsd",
            "leiauteNFe_v4.00.xsd")
SAIDA = os.path.join(RAIZ, "src", "libnfe", "esquemas.c")
XS = "{http://www.w3.org/2001/XMLSchema}"

# (nome C, nome do elemento no leiaute) das raízes descritas
RAIZES = (
    ("imposto", "imposto"),
    ("prod", "prod"),
    ("transp", "transp"),
    ("total", "total"),
    ("NFref", "NFref"),
    ("impostoDevol", "impostoDevol"),
    ("obsItem", "obsItem"),
    ("DFeReferenciado", "DFeReferenciado"),
    ("avulsa", "avulsa"),
    ("exporta", "exporta"),
    ("compra", "compra"),
    ("cana", "cana"),
    ("infSolicNFF", "infSolicNFF"),
    ("agropecuario", "agropecuario"),
    ("infPAA", "infPAA"),
    ("infNFeSupl", "infNFeSupl"),
)

CABECALHO = """\
/* Gerado por tools/gerar_esquemas.py a partir de tests/schemas/nfe.
 * Não edite à mão: rode `python3 tools/gerar_esquemas.py`. */

#include <stddef.h>

#include <libnfe/esquema.h>

/* clang-format off */
"""

RODAPE = "/* clang-format on */\n"


class Tipos:
    def __init__(self):
        self.simples = {}
        self.complexos = {}
        self.leiaute = None
        for arquivo in ARQUIVOS:
            raiz = ET.parse(os.path.join(SCHEMAS, arquivo)).getroot()
            if arquivo.startswith("leiaute"):
                self.leiaute = raiz
            for t in raiz.findall(XS + "simpleType"):
                self.simples.setdefault(t.get("name"), t)
            for t in raiz.findall(XS + "complexType"):
                self.complexos.setdefault(t.get("name"), t)

    def facetas(self, restricao):
        """dict com pattern, enumeration, minLength, maxLength e tstring,
        juntando as facetas da base (tipos nomeados) com as próprias."""
        base = restricao.get("base")
        f = {"enumeration": [], "tstring": False}
        nome = base.split(":")[-1]
        if nome in self.simples:
            f = dict(self.facetas(self.simples[nome].find(XS + "restriction")))
            f["enumeration"] = list(f["enumeration"])
            if nome == "TString":
                f["tstring"] = True
        elif base in PRIMITIVOS:
            f["pattern"] = PRIMITIVOS[base]
        elif base == "xs:base64Binary":
            f["base64"] = True
        proprias = []
        padroes = []
        for c in restricao:
            chave = c.tag.replace(XS, "")
            if chave == "enumeration":
                proprias.append(c.get("value"))
            elif chave == "pattern":
                padroes.append(c.get("value"))
            elif chave in ("minLength", "maxLength"):
                f[chave] = int(c.get("value"))
            elif chave == "length":
                f["minLength"] = f["maxLength"] = int(c.get("value"))
        if proprias:
            f["enumeration"] = proprias
        # Vários xs:pattern no mesmo passo valem como alternativas
        if len(padroes) == 1:
            f["pattern"] = padroes[0]
        elif padroes:
            f["pattern"] = "|".join("(%s)" % p for p in padroes)
        if f.get("base64"):
            # xs:base64Binary com length n (em bytes)
            n = f.pop("minLength", None)
            f.pop("maxLength", None)
            if n is None:
                f["pattern"] = "[A-Za-z0-9+/=]+"
            else:
                completos, resto = divmod(n, 3)
                p = "[A-Za-z0-9+/]{%d}" % (completos * 4)
                if resto == 1:
                    p += "[A-Za-z0-9+/]{2}=="
                elif resto == 2:
                    p += "[A-Za-z0-9+/]{3}="
                f["pattern"] = p
        return f

    def facetas_elemento(self, el):
        tipo = el.get("type")
        if tipo is not None:
            nome = tipo.split(":")[-1]
            if nome in self.simples:
                return self.facetas(self.simples[nome].find(XS + "restriction"))
            if tipo.startswith("xs:"):
                return self.facetas(ET.Element(XS + "restriction",
                                               base=tipo))
            return None
        st = el.find(XS + "simpleType")
        if st is not None:
            return self.facetas(st.find(XS + "restriction"))
        return None

    def conteudo_complexo(self, el):
        """(sequence/choice, atributos) do elemento complexo; (None, [])
        se for simples"""
        tipo = el.get("type")
        ct = None
        if tipo is not None and tipo.split(":")[-1] in self.complexos:
            ct = self.complexos[tipo.split(":")[-1]]
        elif el.find(XS + "complexType") is not None:
            ct = el.find(XS + "complexType")
        if ct is None:
            return None, []
        atributos = ct.findall(XS + "attribute")
        conteudo = None
        for c in ct:
            if c.tag in (XS + "sequence", XS + "choice"):
                conteudo = c
            elif c.tag not in (XS + "attribute", XS + "annotation"):
                sys.exit("%s: conteúdo complexo não suportado"
                         % el.get("name"))
        return conteudo, atributos


class Gerador:
    """Gera a tabela de uma raiz. Elementos repetidos (maxOccurs > 1) viram
    nós ESQ_LISTA que apontam para a tabela própria do elemento, gerada
    antes (em tabelas)."""

    def __init__(self, tipos, nome_c, tabelas, listas):
        self.tipos = tipos
        self.nome_c = nome_c
        self.tabelas = tabelas
        self.listas = listas
        self.nos = []
        self.folhas = 0
        self.nlistas = 0

    def lista(self, valores):
        chave = tuple(valores)
        if chave not in self.listas:
            self.listas[chave] = "valores_%d" % len(self.listas)
        return self.listas[chave]

    def no(self, pai, tipo, nome=None, minimo=1, f=None):
        i = len(self.nos)
        self.nos.append({"pai": pai, "tipo": tipo, "nome": nome,
                         "min": minimo, "max": 1, "f": f, "filho": -1,
                         "irmao": -1, "folha": -1, "lista": -1,
                         "ini": self.folhas, "lini": self.nlistas,
                         "sub": None})
        if pai >= 0:
            p = self.nos[pai]
            if p["filho"] < 0:
                p["filho"] = i
            else:
                j = p["filho"]
                while self.nos[j]["irmao"] >= 0:
                    j = self.nos[j]["irmao"]
                self.nos[j]["irmao"] = i
        return i

    def fecha(self, i):
        self.nos[i]["fim"] = self.folhas
        self.nos[i]["lfim"] = self.nlistas

    def folha(self, pai, tipo, nome, minimo, f):
        i = self.no(pai, tipo, nome, minimo, f)
        self.nos[i]["folha"] = self.folhas
        self.folhas += 1
        self.fecha(i)
        return i

    def elemento(self, el, pai, raiz=False):
        maximo = el.get("maxOccurs")
        if not raiz and maximo not in (None, "1"):
            sub = gera_raiz(self.tipos, "%s_%s" % (self.nome_c,
                                                   el.get("name")),
                            el, self.tabelas, self.listas)
            i = self.no(pai, "ESQ_LISTA", el.get("name"),
                        0 if el.get("minOccurs") == "0" else 1)
            self.nos[i]["max"] = 0 if maximo == "unbounded" else int(maximo)
            self.nos[i]["sub"] = sub
            self.nos[i]["lista"] = self.nlistas
            self.nlistas += 1
            self.fecha(i)
            return i
        minimo = 1 if raiz or el.get("minOccurs") != "0" else 0
        conteudo, atributos = self.tipos.conteudo_complexo(el)
        if conteudo is None and not atributos:
            f = self.tipos.facetas_elemento(el)
            if f is None:
                sys.exit("%s: tipo desconhecido" % el.get("name"))
            return self.folha(pai, "ESQ_ELEM", el.get("name"), minimo, f)
        i = self.no(pai, "ESQ_ELEM", el.get("name"), minimo)
        for a in atributos:
            f = self.tipos.facetas_elemento(a)
            if f is None:
                sys.exit("%s/@%s: tipo desconhecido" % (el.get("name"),
                                                        a.get("name")))
            self.folha(i, "ESQ_ATTR", a.get("name"),
                       1 if a.get("use") == "required" else 0, f)
        if conteudo is not None:
            self.grupo(conteudo, i)
        self.fecha(i)
        return i

    def grupo(self, g, pai):
        tipo = "ESQ_SEQ" if g.tag == XS + "sequence" else "ESQ_CHOICE"
        if g.get("maxOccurs") not in (None, "1"):
            sys.exit("sequence/choice com maxOccurs não suportado")
        i = self.no(pai, tipo, None, 0 if g.get("minOccurs") == "0" else 1)
        for c in g:
            if c.tag == XS + "element":
                self.elemento(c, i)
            elif c.tag in (XS + "sequence", XS + "choice"):
                self.grupo(c, i)
            elif c.tag != XS + "annotation":
                sys.exit("nó %s não suportado" % c.tag)
        self.fecha(i)
        return i


def gera_raiz(tipos, nome_c, el, tabelas, listas):
    """Gera a tabela do elemento el (e, antes, as das suas listas) e a
    acrescenta a tabelas; retorna o nome C da estrutura"""
    g = Gerador(tipos, nome_c, tabelas, listas)
    g.elemento(el, -1, raiz=True)
    tabelas.append((nome_c, el.get("name"), g))
    return "esq_" + nome_c


def procura(raiz, nome):
    for e in raiz.iter(XS + "element"):
        if e.get("name") == nome:
            return e
    sys.exit("elemento %s não encontrado" % nome)


def c_texto(s):
    return "NULL" if s is None else literal_c(s)


def gerar():
    tipos = Tipos()
    partes = [CABECALHO]
    tabelas = []
    listas = {}
    publicas = set()
    for nome_c, nome in RAIZES:
        gera_raiz(tipos, nome_c, procura(tipos.leiaute, nome), tabelas,
                  listas)
        publicas.add(nome_c)
    corpo = []
    for nome_c, nome, g in tabelas:
        corpo.append("static const struct nfe_esq_no nos_%s[] = {\n"
                     % nome_c)
        for n in g.nos:
            f = n["f"] or {}
            valores = (g.lista(f["enumeration"]) if f.get("enumeration")
                       else "NULL")
            corpo.append(
                "\t{ %s, %s, %d, %d, %s, %s, %d, %d, %d, %d, %d, %d, %d,"
                " %d, %d, %d, %d, %d, %s },\n"
                % (c_texto(n["nome"]), n["tipo"], n["min"], n["max"],
                   c_texto(f.get("pattern")), valores,
                   f.get("minLength", 0), f.get("maxLength", 0),
                   1 if f.get("tstring") else 0, n["pai"], n["filho"],
                   n["irmao"], n["folha"], n["ini"], n["fim"], n["lista"],
                   n["lini"], n["lfim"],
                   "&" + n["sub"] if n["sub"] else "NULL"))
        corpo.append("};\n\n")
        corpo.append("%sconst struct nfe_esq esq_%s = { %s, nos_%s, %d,"
                     " %d, %d };\n\n"
                     % ("" if nome_c in publicas else "static ", nome_c,
                        literal_c(nome), nome_c, len(g.nos), g.folhas,
                        g.nlistas))
    for valores, nome in sorted(listas.items(),
                                key=lambda x: int(x[1].split("_")[1])):
        partes.append("static const char *const %s[] = { %s, NULL };\n"
                      % (nome, ", ".join(literal_c(v) for v in valores)))
    partes.append("\n")
    partes.extend(corpo)
    partes.append(RODAPE)
    return "".join(partes)


def main():
    conteudo = gerar()
    if "--verificar" in sys.argv[1:]:
        atual = open(SAIDA, encoding="utf-8").read() \
            if os.path.exists(SAIDA) else ""
        if atual != conteudo:
            print(f"{SAIDA} desatualizado: rode python3 tools/gerar_esquemas.py")
            return 1
        return 0
    with open(SAIDA, "w", encoding="utf-8") as f:
        f.write(conteudo)
    print(f"gerado {SAIDA}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
