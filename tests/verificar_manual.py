#!/usr/bin/env python3
"""Confere exemplos publicados, links e obsolescência (requer xmllint)."""
import copy
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET

RAIZ = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(RAIZ / "tools"))
from manual import atualizar, base_xsd  # noqa: E402

NS = "{http://www.portalfiscal.inf.br/nfe}"
ET.register_namespace("", NS[1:-1])


def arvore(e):
    return e.tag, e.attrib, (e.text or "").strip(), [arvore(f) for f in e]


def valida(e, esperado=True):
    p = subprocess.run(["xmllint", "--noout", "--schema",
        str(RAIZ / "tests/schemas/nfe/tipos_v4.00.xsd"), "-"],
        input=ET.tostring(e), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    assert (p.returncode == 0) == esperado, p.stderr.decode()


def exemplos():
    ide = None
    isolado = None
    for argumentos, fixture in [([], "ide-referencias.xml"),
            (["--generico"], "ide-referencias.xml"),
            (["--isolado"], "nfref-isolado.xml")]:
        saida = subprocess.check_output([str(RAIZ / "obj/referenciar_nf"), *argumentos])
        esperado = (RAIZ / "docs/manual/exemplos" / fixture).read_bytes()
        assert saida == esperado, f"saída diferente do exemplo: {argumentos}"
        e = ET.fromstring(saida)
        if not argumentos:
            ide = e
            valida(ide)
        if argumentos == ["--isolado"]:
            isolado = e
    contexto = copy.deepcopy(ide)
    for ref in contexto.findall(NS + "NFref"):
        contexto.remove(ref)
    contexto.append(isolado)
    valida(contexto)
    # O contexto realmente rejeita padrões/domínios e a escolha incorreta.
    for tag, valor in [("cUF", "18"), ("mod", "55"), ("serie", "01"),
                       ("nNF", "0"), ("AAMM", "2613")]:
        ruim = copy.deepcopy(contexto)
        ruim.find(f"{NS}NFref/{NS}refNF/{NS}{tag}").text = valor
        valida(ruim, False)
    ruim = copy.deepcopy(contexto)
    ruim.find(NS + "NFref").append(copy.deepcopy(ide.findall(NS + "NFref")[1][0]))
    valida(ruim, False)
    for p in (RAIZ / "docs/manual").rglob("*.md"):
        for bloco in re.findall(r"```xml\n(.*?)\n```", p.read_text(encoding="utf-8"), re.S):
            refs = list(ET.fromstring("<trecho>" + bloco + "</trecho>"))
            esperado = [isolado] if p.name == "refNF.md" else ide.findall(NS + "NFref")
            assert [arvore(e) for e in refs] == [arvore(e) for e in esperado], p


def links():
    for p in (RAIZ / "docs/manual").rglob("*.md"):
        for link in re.findall(r"\]\(([^)]+)\)", p.read_text(encoding="utf-8")):
            if "://" not in link:
                destino = link.split("#", 1)[0]
                assert not destino or (p.parent / destino).exists(), (p, link)


def obsolescencia():
    with tempfile.TemporaryDirectory() as pasta:
        base = Path(pasta)
        shutil.copytree(RAIZ / "docs/manual", base / "docs/manual")
        shutil.copytree(RAIZ / "tests/schemas/nfe", base / "schemas")
        schema = base / "schemas/nfe_v4.00.xsd"
        manifest = base / "docs/manual/schema-manifest.json"
        original = json.loads(manifest.read_text(encoding="utf-8"))
        caminhos = {p["caminho_xml"] for p in original["paginas"]}
        assinatura, _ = base_xsd(schema)
        tipos = base / "schemas/tiposBasico_v4.00.xsd"
        t = tipos.read_text(encoding="utf-8")
        tipos.write_text(t.replace("<xs:schema", "<!-- comentário -->\n<xs:schema", 1), encoding="utf-8")
        assert base_xsd(schema)[0] == assinatura
        tipos.write_text(t.replace("xs:", "xsd:").replace("xmlns:xs=", "xmlns:xsd="), encoding="utf-8")
        assert base_xsd(schema)[0] == assinatura
        # Uma restrição de tipo compartilhado deve invalidar a base revisada.
        tipos.write_text(t.replace('[0-9A-Z]{12}[0-9]{2}', '[0-9]{12}[0-9]{2}'), encoding="utf-8")
        assert base_xsd(schema)[0] != assinatura
        atualizar(base, caminhos, schema)
        atualizar(base, caminhos, schema)
        novo = json.loads(manifest.read_text(encoding="utf-8"))
        assert all(p["estado"] == "obsoleto" for p in novo["paginas"])
        assert all(p["assinatura_revisada"] == assinatura for p in novo["paginas"])
        atualizar(base, set(), schema)
        for p in original["paginas"]:
            texto = (base / "docs/manual" / p["arquivo"]).read_text(encoding="utf-8")
            assert "nó removido" in texto and "![" not in texto
            assert "## API" in texto  # explicações preservadas


if __name__ == "__main__":
    exemplos()
    links()
    obsolescencia()
    print("Manual: saídas C, XML/XSD, links e obsolescência conferidos.")
