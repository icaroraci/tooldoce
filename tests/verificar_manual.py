#!/usr/bin/env python3
"""Confere exemplos publicados, links e obsolescência (requer xmllint)."""
import copy
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET

RAIZ = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(RAIZ / "tools"))
from manual import atualizar, base_xsd  # noqa: E402
import gerar_diagramas as diagramas  # noqa: E402

NS = "{http://www.portalfiscal.inf.br/nfe}"
ET.register_namespace("", NS[1:-1])


def arvore(e):
    return e.tag, sorted(e.attrib.items()), (e.text or "").strip(), [arvore(f) for f in e]


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
    for p in [RAIZ / "docs/manual/NFe/infNFe/ide/NFref.md",
              RAIZ / "docs/manual/NFe/infNFe/ide/NFref/refNF.md"]:
        for bloco in re.findall(r"```xml\n(.*?)\n```", p.read_text(encoding="utf-8"), re.S):
            refs = list(ET.fromstring("<trecho>" + bloco + "</trecho>"))
            esperado = [isolado] if p.name == "refNF.md" else ide.findall(NS + "NFref")
            assert [arvore(e) for e in refs] == [arvore(e) for e in esperado], p


def elemento(raiz, caminho):
    tags = caminho.split("/")
    inicio = tags.index(raiz.tag.split("}")[-1])
    nos = [raiz]
    for nome in tags[inicio + 1:]:
        nos = [f for no in nos for f in no.findall(NS + nome)]
    assert nos, (raiz.tag, caminho)
    return nos[0]


def identidade(raiz):
    # Representação independente de prefixos, indentação e ordem de atributos.
    return repr(arvore(raiz))


def capturar_testes(programas):
    """Reexecuta cenários existentes e observa suas validações positivas."""
    with tempfile.TemporaryDirectory() as pasta:
        base = Path(pasta)
        modulo = base / "captura.so"
        cc = shlex.split(os.environ.get("CC", "cc"))
        flags = shlex.split(subprocess.check_output(
            ["xml2-config", "--cflags", "--libs"], text=True))
        subprocess.run([*cc, "-shared", "-fPIC", "-std=c99", "-Wall", "-Wextra",
                        "-Werror", str(RAIZ / "tests/manual_captura.c"),
                        "-o", str(modulo), *flags, "-ldl"], check=True)
        vistos = set()
        for programa in sorted(programas):
            executavel = RAIZ / "obj" / programa
            assert executavel.exists(), f"compile {executavel} com make test"
            deps = subprocess.check_output(["ldd", str(executavel)], text=True)
            # ASan exige seu runtime em primeiro lugar na lista de preload.
            runtime = re.search(r"libasan\.so\S*\s+=>\s+(\S+)", deps)
            bibliotecas = ([runtime[1]] if runtime else []) + [str(modulo)]
            env = dict(os.environ, MANUAL_CAPTURA=str(base),
                       LD_PRELOAD=":".join(bibliotecas))
            p = subprocess.run([str(executavel), "tests"], cwd=RAIZ, env=env,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            assert p.returncode == 0, (programa, p.stdout.decode(), p.stderr.decode())
        for p in base.glob("*.xml"):
            vistos.add(identidade(ET.parse(p).getroot()))
        return vistos


def catalogo():
    manual = RAIZ / "docs/manual"
    cat = json.loads((manual / "exemplos/catalogo.json").read_text(encoding="utf-8"))
    manifest = json.loads((manual / "schema-manifest.json").read_text(encoding="utf-8"))
    diagramas.configurar()
    caminhos = {e.caminho for e in diagramas.estruturas(diagramas.Leiaute().raiz())}
    assert set(cat) == caminhos == {p["caminho_xml"] for p in manifest["paginas"]}
    assert {p.relative_to(manual).as_posix()[:-3] for p in (manual / "NFe").rglob("*.md")} | {"NFe"} == caminhos
    vistos = capturar_testes({e["programa"] for e in cat.values() if e["tipo"] == "teste"})
    validados = set()
    for caminho, e in cat.items():
        fonte = RAIZ / e["fonte"]
        texto = fonte.read_bytes().replace(b"\r\n", b"\n")
        assert hashlib.sha256(texto).hexdigest() == e["fonte_sha256"], fonte
        assert e["funcao"] in texto.decode().splitlines()[e["linha"] - 1], (caminho, e["linha"])
        fixture = manual / e["arquivo"]
        raiz = ET.parse(fixture).getroot()
        no = elemento(raiz, caminho)
        if e["tipo"] == "programa":
            saida = subprocess.check_output([str(RAIZ / "obj" / e["programa"]), *e["argumentos"]])
            assert identidade(ET.fromstring(saida)) == identidade(raiz), caminho
            contexto = manual / e["contexto"]
            assert identidade(elemento(ET.parse(contexto).getroot(), caminho)) == identidade(no), caminho
            arquivo = contexto
        else:
            assert identidade(raiz) in vistos, f"fixture não reproduzida pelo C: {fixture}"
            arquivo = fixture
        par = (arquivo, e["schema"])
        if par not in validados:
            p = subprocess.run(["xmllint", "--noout", "--schema", str(RAIZ / e["schema"]), str(arquivo)],
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            assert p.returncode == 0, p.stderr.decode()
            validados.add(par)
        if caminho not in {"NFe/infNFe/ide/NFref", "NFe/infNFe/ide/NFref/refNF"}:
            pagina = manual / (caminho + ".md")
            t = pagina.read_text(encoding="utf-8")
            blocos_c = re.findall(r"```c\n(.*?)\n```", t, re.S)
            assert len(blocos_c) == 1 and blocos_c[0] in texto.decode(), pagina
            blocos = re.findall(r"```xml\n(.*?)\n```", t, re.S)
            assert len(blocos) == 1, pagina
            assert identidade(ET.fromstring(blocos[0])) == identidade(no), pagina
            for secao in ["Finalidade", "Estrutura", "Campos", "API C", "Exemplo C", "XML correspondente", "Validação", "Referências"]:
                assert "## " + secao in t, (pagina, secao)
    print(f"Estruturas: {len(caminhos)}; C genérico: {sum(e['tipo'] == 'programa' for e in cat.values())}; fixtures de testes reproduzidas: {sum(e['tipo'] == 'teste' for e in cat.values())}.")


def mensagens():
    raizes = {}
    for nome in ["status", "recibo", "consulta", "cancelamento", "cce", "inutilizacao"]:
        saida = subprocess.check_output([str(RAIZ / "obj/manual_servicos"), nome])
        fixture = RAIZ / "docs/manual/exemplos/servicos" / (nome + ".xml")
        assert saida == fixture.read_bytes(), nome
        raizes[nome] = ET.fromstring(saida)
    for guia, nomes in [("SERVICOS.md", ["status", "recibo", "consulta"]),
                        ("EVENTOS.md", ["cancelamento", "cce"]),
                        ("INUTILIZACAO.md", ["inutilizacao"])]:
        texto = (RAIZ / "docs/manual" / guia).read_text(encoding="utf-8")
        blocos = re.findall(r"```xml\n(.*?)\n```", texto, re.S)
        assert [arvore(ET.fromstring(b)) for b in blocos] == [arvore(raizes[n]) for n in nomes], guia
    # Não há consStatServ no pacote adotado: confere a mensagem pelo contrato.
    assert raizes["status"].tag == NS + "consStatServ"
    assert raizes["status"].attrib == {"versao": "4.00"}
    assert [(f.tag, f.text) for f in raizes["status"]] == [
        (NS + "tpAmb", "2"), (NS + "cUF", "35"), (NS + "xServ", "STATUS")]
    # Assinatura sintética de apoio: confere estrutura, não criptografia.
    nota = ET.parse(RAIZ / "docs/manual/exemplos/contextos/caso_000.xml").getroot()
    assinatura = nota.find("{http://www.w3.org/2000/09/xmldsig#}Signature")
    assert assinatura is not None
    with tempfile.TemporaryDirectory() as pasta:
        wrapper = Path(pasta) / "inut.xsd"
        wrapper.write_text(
            '<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" '
            'xmlns:nfe="http://www.portalfiscal.inf.br/nfe" '
            'targetNamespace="http://www.portalfiscal.inf.br/nfe" elementFormDefault="qualified">'
            '<xs:include schemaLocation="' + (RAIZ / "tests/schemas/nfe/leiauteInutNFe_v4.00.xsd").as_uri() + '"/>'
            '<xs:element name="inutNFe" type="nfe:TInutNFe"/></xs:schema>', encoding="utf-8")
        schemas = {
            "consulta": RAIZ / "tests/schemas/nfe/consSitNFe_v4.00.xsd",
            "recibo": RAIZ / "tests/schemas/nfe/tipos_v4.00.xsd",
            "cancelamento": RAIZ / "tests/schemas/evento_canc/eventoCancNFe_v1.00.xsd",
            "cce": RAIZ / "tests/schemas/evento_cce/CCe_v1.00.xsd",
            "inutilizacao": wrapper,
        }
        for nome, schema in schemas.items():
            raiz = copy.deepcopy(raizes[nome])
            if nome in {"cancelamento", "cce", "inutilizacao"}:
                raiz.append(copy.deepcopy(assinatura))
            p = subprocess.run(["xmllint", "--noout", "--schema", str(schema), "-"],
                               input=ET.tostring(raiz), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            assert p.returncode == 0, (nome, p.stderr.decode())
    print("Mensagens: seis saídas C/XML; cinco XSD/contextos e contrato de status conferidos.")


def diagnosticos():
    programa = str(RAIZ / "obj/diagnosticar_xml")
    schema = str(RAIZ / "tests/schemas/nfe/tipos_v4.00.xsd")
    fixture = RAIZ / "docs/manual/exemplos/ide-refnf-invalido.xml"
    p = subprocess.run([programa, schema, str(fixture)], capture_output=True,
                       text=True, encoding="utf-8")
    assert p.returncode == 1, (p.stdout, p.stderr)
    assert "Retorno local: -3 · 4 problema(s)" in p.stdout
    for tag, valor in [("AAMM", "202610"), ("mod", "55"),
                       ("serie", "01"), ("nNF", "0")]:
        assert f'Tag: "{tag}"' in p.stdout
        assert f'Recebido: "{valor}"' in p.stdout
        assert f'/ide/NFref[1]/refNF/{tag}' in p.stdout
    assert "'01', '02'" in p.stdout
    assert p.stdout.count("Diagnóstico original:") == 4
    tree = ET.parse(fixture)
    ref = tree.getroot().find(NS + "NFref/" + NS + "refNF")
    for tag, valor in [("AAMM", "2610"), ("mod", "01"),
                       ("serie", "1"), ("nNF", "10")]:
        ref.find(NS + tag).text = valor
    with tempfile.TemporaryDirectory() as pasta:
        corrigido = Path(pasta) / "corrigido.xml"
        tree.write(corrigido, encoding="utf-8", xml_declaration=True)
        p = subprocess.run([programa, schema, str(corrigido)],
                           capture_output=True, text=True, encoding="utf-8")
        assert p.returncode == 0, (p.stdout, p.stderr)
        assert "Retorno local: 0 · 0 problema(s)" in p.stdout
    print("Diagnósticos: quatro erros XSD e correção da fixture conferidos.")


def funcoes_header(p):
    t = re.sub(r"/\*.*?\*/", "", p.read_text(encoding="utf-8"), flags=re.S)
    linhas = []
    continuacao = False
    for linha in t.splitlines():
        if continuacao or linha.lstrip().startswith("#"):
            continuacao = linha.rstrip().endswith("\\")
            continue
        linhas.append(linha)
    t = "\n".join(linhas)
    t = re.sub(r"\bNFE_INTERNO\b[^;]*;", "", t, flags=re.S)
    return re.findall(r"\b((?:nfe_|RefNF|xmlGen)[A-Za-z0-9_]*)\s*\(", t)


def api():
    inv = json.loads((RAIZ / "docs/manual/api/inventario.json").read_text(encoding="utf-8"))
    headers = list((RAIZ / "include/libnfe").glob("*.h"))
    assert set(inv) == {p.stem for p in headers}
    nomes = set()
    inline = set()
    for p in headers:
        funcoes = funcoes_header(p)
        assert inv[p.stem] == funcoes, p
        pagina = RAIZ / "docs/manual/api" / (p.stem + ".md")
        texto = pagina.read_text(encoding="utf-8")
        assert all(nome in texto for nome in funcoes), pagina
        nomes.update(funcoes)
        inline.update(re.findall(r"static\s+inline\s+\w+\s+(\w+)\s*\(", p.read_text(encoding="utf-8")))
    saida = subprocess.check_output(["nm", "-D", "--defined-only", str(RAIZ / "lib/libnfe.so")], text=True)
    exportados = {linha.split()[-1] for linha in saida.splitlines()}
    assert nomes - inline == exportados, ("declaração/exportação divergente", nomes - inline - exportados, exportados - nomes)
    print(f"API: {len(headers)} headers e {len(exportados)} funções exportadas conferidos.")


def links():
    for p in (RAIZ / "docs/manual").rglob("*.md"):
        texto = re.sub(r"```.*?```", "", p.read_text(encoding="utf-8"), flags=re.S)
        texto = re.sub(r"`[^`\n]*`", "", texto)
        for link in re.findall(r"\]\(([^)]+)\)", texto):
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
        atualizar(base, caminhos, schema)
        for p in original["paginas"]:
            texto = (base / "docs/manual" / p["arquivo"]).read_text(encoding="utf-8")
            assert texto.count(assinatura) == 1
            assert f"`Base\u00a0atual\u00a0e\u00a0revisada:\u00a0{assinatura}`" in texto
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
        atual = base_xsd(schema)[0]
        for p in original["paginas"]:
            texto = (base / "docs/manual" / p["arquivo"]).read_text(encoding="utf-8")
            assert f"`Base\u00a0revisada:\u00a0{assinatura}`  \n> `Base\u00a0atual:\u00a0{atual}`" in texto
        atualizar(base, set(), schema)
        for p in original["paginas"]:
            texto = (base / "docs/manual" / p["arquivo"]).read_text(encoding="utf-8")
            assert "nó removido" in texto and "![" not in texto
            assert "## API" in texto  # explicações preservadas


if __name__ == "__main__":
    exemplos()
    catalogo()
    mensagens()
    diagnosticos()
    api()
    links()
    obsolescencia()
    print("Manual: saídas C, XML/XSD, links e obsolescência conferidos.")
