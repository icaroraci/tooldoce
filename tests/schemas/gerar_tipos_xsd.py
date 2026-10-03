#!/usr/bin/env python3
"""Gera nfe/tipos_v4.00.xsd a partir do leiauteNFe_v4.00.xsd oficial.

No leiaute, grupos como <ide>, <emit>, <dest>, <det>, <prod>, <imposto>, <total>, <transp>, <pag> e <infNFe> são elementos locais dentro
do tipo TNFe, e tipos como TEnderEmi não têm elemento próprio: nenhum deles
pode ser validado sozinho. Este script gera um schema que inclui o leiaute e
declara esses grupos como elementos globais, para que os testes validem o
XML gerado pela biblioteca enquanto a nota completa não existe.

Rode de novo sempre que os schemas em nfe/ forem atualizados:
    python3 tests/schemas/gerar_tipos_xsd.py
"""
import os
import re

DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "nfe")
ORIGEM = os.path.join(DIR, "leiauteNFe_v4.00.xsd")
DESTINO = os.path.join(DIR, "tipos_v4.00.xsd")

# Elementos locais de TNFe/infNFe (e de det) copiados como globais
LOCAIS = ("ide", "emit", "dest", "det", "prod", "imposto", "total", "transp", "pag", "infNFe", "infNFeSupl")

# Tipos complexos do leiaute declarados como elementos globais
TIPOS = (("enderEmit", "TEnderEmi"), ("enderDest", "TEndereco"),
         # Mensagens aos webservices cujo elemento não vem no pacote
         ("enviNFe", "TEnviNFe"), ("retEnviNFe", "TRetEnviNFe"),
         ("consReciNFe", "TConsReciNFe"), ("protNFe", "TProtNFe"),
         ("nfeProc", "TNfeProc"))

CABECALHO = """\
<?xml version="1.0" encoding="UTF-8"?>
<!-- GERADO por tests/schemas/gerar_tipos_xsd.py a partir de leiauteNFe_v4.00.xsd. Não editar. -->
<xs:schema xmlns="http://www.portalfiscal.inf.br/nfe" \
xmlns:ds="http://www.w3.org/2000/09/xmldsig#" \
xmlns:xs="http://www.w3.org/2001/XMLSchema" \
targetNamespace="http://www.portalfiscal.inf.br/nfe" \
elementFormDefault="qualified" attributeFormDefault="unqualified">
\t<xs:include schemaLocation="leiauteNFe_v4.00.xsd"/>
"""


def local(texto, nome):
    """Definição do elemento local nome, sem minOccurs/maxOccurs (que não
    valem em elementos globais) e com o recuo de nível superior."""
    inicio = re.search(r'\n(\t+)<xs:element name="%s"[^>]*>' % nome, texto)
    if not inicio:
        raise SystemExit('elemento "%s" não encontrado' % nome)
    recuo = inicio.group(1)
    fim = texto.index("\n" + recuo + "</xs:element>", inicio.end())
    bloco = texto[inicio.start() + 1:fim + len(recuo) + len("</xs:element>") + 1]
    # As restrições de unicidade (xs:unique) têm nome global; na cópia elas
    # repetiriam os nomes do leiaute incluído
    bloco = re.sub(r"\n\t*<xs:unique .*?</xs:unique>", "", bloco, flags=re.S)
    primeira, resto = bloco.split("\n", 1)
    primeira = re.sub(r'\s+(minOccurs|maxOccurs)="[^"]*"', "", primeira)
    linhas = [primeira] + resto.split("\n")
    tira = len(recuo) - 1
    return "\n".join(l[tira:] if l.startswith(recuo) else l for l in linhas)


def main():
    with open(ORIGEM, encoding="utf-8") as f:
        texto = f.read()
    partes = [CABECALHO]
    for nome in LOCAIS:
        partes.append(local(texto, nome) + "\n")
    for nome, tipo in TIPOS:
        partes.append('\t<xs:element name="%s" type="%s"/>\n' % (nome, tipo))
    partes.append("</xs:schema>\n")
    with open(DESTINO, "w", encoding="utf-8") as f:
        f.write("".join(partes))
    print("gerado:", DESTINO)


if __name__ == "__main__":
    main()
