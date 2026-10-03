#!/usr/bin/env python3
"""Gera nfe/ide_v4.00.xsd a partir do leiauteNFe_v4.00.xsd oficial.

No leiaute, <ide> é um elemento local dentro do tipo TNFe e não pode ser
validado sozinho. Este script copia a definição de <ide> e os tipos simples
de nível superior do leiaute para um schema próprio, que os testes usam para
validar o XML gerado pela biblioteca enquanto a nota completa não existe.

Rode de novo sempre que os schemas em nfe/ forem atualizados:
    python3 tests/schemas/gerar_ide_xsd.py
"""
import os
import re

DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "nfe")
ORIGEM = os.path.join(DIR, "leiauteNFe_v4.00.xsd")
DESTINO = os.path.join(DIR, "ide_v4.00.xsd")

with open(ORIGEM, encoding="utf-8") as f:
    texto = f.read()

linhas = texto.split("\n")

# Cabeçalho: declaração XML, <xs:schema> e includes/imports
cabecalho = [l for l in linhas[:40]
             if l.startswith("<?xml") or "<xs:schema" in l
             or "<xs:include" in l or "<xs:import" in l]

# Definição do elemento <ide> (do início da tag até o fechamento no mesmo nível)
inicio = re.search(r'\n(\t+)<xs:element name="ide">', texto)
if not inicio:
    raise SystemExit('elemento "ide" não encontrado')
recuo = inicio.group(1)
fim = texto.index("\n" + recuo + "</xs:element>", inicio.end())
ide = texto[inicio.start() + 1:fim + len(recuo) + len("</xs:element>") + 1]

# Tipos simples de nível superior (um tab de recuo) usados por <ide>
simples = re.findall(r'\n\t<xs:simpleType name="[^"]+".*?\n\t</xs:simpleType>',
                     texto, re.S)

with open(DESTINO, "w", encoding="utf-8") as f:
    f.write(cabecalho[0] + "\n")
    f.write("<!-- GERADO por tests/schemas/gerar_ide_xsd.py a partir de "
            "leiauteNFe_v4.00.xsd. Não editar. -->\n")
    f.write("\n".join(cabecalho[1:]) + "\n")
    f.write(ide + "\n")
    f.write("".join(simples) + "\n")
    f.write("</xs:schema>\n")

print("gerado:", DESTINO)
