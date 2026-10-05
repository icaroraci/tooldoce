#!/usr/bin/env python3
"""Gera diagramas SVG das estruturas de um documento a partir dos schemas.

Na NF-e (padrão), lê tests/schemas/nfe/leiauteNFe_v4.00.xsd (e os tipos que
ele inclui) e produz, em docs/diagramas/:

  - um SVG por estrutura (elemento com filhos), em pastas que seguem a
    hierarquia da nota: NFe/infNFe/det/prod/arma.svg;
  - README.md: índice navegável de todas as estruturas.

Com --todo, regenera também o TODO.md (lista de estruturas a implementar),
mantendo marcados ([x]) os itens que já estavam marcados.

Uso (na raiz do projeto):
    python3 tools/gerar_diagramas.py           # diagramas e índice
    python3 tools/gerar_diagramas.py --todo    # também o TODO.md

Outro documento (MDF-e, CT-e...) é descrito num arquivo de configuração
(ver tools/documento.py), passado com --config.

Só usa a biblioteca padrão do Python 3.
"""
import argparse
import html
import os
import re
import shutil
import sys
import textwrap
import xml.etree.ElementTree as ET

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from documento import Documento, argumento  # noqa: E402
from manual import atualizar as atualizar_manual  # noqa: E402

# Documento em uso (configurar()); a NF-e, se nada for configurado
DOC = None
SAIDA = TODO = None


def configurar(config=None):
    """Lê a configuração do documento (None: a NF-e)"""
    global DOC, SAIDA, TODO
    DOC = Documento(config)
    SAIDA = DOC.caminho(DOC.secao("diagramas", "saida"))
    TODO = DOC.caminho(DOC.secao("diagramas", "todo"))
    return DOC


def doc_atual():
    return DOC or configurar()


def nome_doc():
    """Ex.: NF-e (leiaute 4.00)"""
    return f"{doc_atual()['documento']} (leiaute {doc_atual()['versao']})"


def da():
    """Artigo antes do nome do documento (da NF-e, do CT-e)"""
    return doc_atual().get("artigo", "da")


def link_todo():
    """Pasta dos diagramas em relação ao TODO.md (ex.: docs/diagramas)"""
    return os.path.relpath(SAIDA, os.path.dirname(TODO) or ".").replace(
        os.sep, "/")

XS = "{http://www.w3.org/2001/XMLSchema}"

# ---------------------------------------------------------------- modelo


class Elemento:
    """Elemento do leiaute: simples (folha) ou estrutura (com filhos)."""

    def __init__(self, nome, minimo, maximo, doc):
        self.nome = nome
        self.minimo = minimo
        self.maximo = maximo
        self.doc = doc
        self.tipo = ""          # nome do tipo ou base da restrição
        self.facetas = {}       # pattern, minLength, maxLength, enumeration...
        self.conteudo = None    # Grupo, se for estrutura
        self.atributos = []     # lista de Elemento (atributos XML)
        self.caminho = ""       # ex.: NFe/infNFe/ide

    @property
    def estrutura(self):
        return self.conteudo is not None


class Grupo:
    """Compositor xs:sequence ou xs:choice."""

    def __init__(self, tipo, minimo, maximo):
        self.tipo = tipo        # "sequence" ou "choice"
        self.minimo = minimo
        self.maximo = maximo
        self.itens = []         # Elemento ou Grupo


def texto_doc(no):
    """Primeira documentação do nó, em uma linha."""
    if no is None:
        return ""
    d = no.find(XS + "annotation/" + XS + "documentation")
    if d is None or not d.text:
        return ""
    return re.sub(r"\s+", " ", d.text).strip()


class Leiaute:
    def __init__(self):
        self.simples = {}
        self.complexos = {}
        for arquivo in doc_atual().arquivos:
            raiz = ET.parse(doc_atual().xsd(arquivo)).getroot()
            for t in raiz.findall(XS + "simpleType"):
                self.simples[t.get("name")] = t
            for t in raiz.findall(XS + "complexType"):
                self.complexos[t.get("name")] = t

    def facetas(self, restricao, facetas):
        """Acumula as facetas de uma xs:restriction (e da base, se nomeada)."""
        base = restricao.get("base", "")
        for f in restricao:
            nome = f.tag.replace(XS, "")
            if nome == "enumeration":
                facetas.setdefault("enumeration", []).append(f.get("value"))
            elif nome in ("pattern", "minLength", "maxLength", "length",
                          "totalDigits", "fractionDigits", "whiteSpace"):
                facetas.setdefault(nome, f.get("value"))
        base_local = base.split(":")[-1]
        if base_local in self.simples:
            self.facetas_tipo(base_local, facetas)
        return base

    def facetas_tipo(self, nome, facetas):
        t = self.simples.get(nome)
        if t is None:
            return
        r = t.find(XS + "restriction")
        if r is not None:
            self.facetas(r, facetas)

    def elemento(self, no, caminho_pai):
        nome = no.get("name") or no.get("ref", "")
        e = Elemento(nome, no.get("minOccurs", "1"),
                     no.get("maxOccurs", "1"), texto_doc(no))
        e.caminho = f"{caminho_pai}/{nome}" if caminho_pai else nome
        if no.get("ref"):
            e.tipo = "assinatura digital (XMLDSig)" \
                if nome.endswith("Signature") else nome
            return e

        tipo = no.get("type", "")
        tipo_local = tipo.split(":")[-1]
        complexo = no.find(XS + "complexType")
        if complexo is None and tipo_local in self.complexos:
            complexo = self.complexos[tipo_local]
            e.tipo = tipo_local
            if not e.doc:
                e.doc = texto_doc(complexo)
        if complexo is not None:
            self.complexo(complexo, e)
            return e

        simples = no.find(XS + "simpleType")
        if simples is not None:
            r = simples.find(XS + "restriction")
            if r is not None:
                e.tipo = self.facetas(r, e.facetas).split(":")[-1]
        else:
            e.tipo = tipo_local
            self.facetas_tipo(tipo_local, e.facetas)
        return e

    def complexo(self, complexo, e):
        for a in complexo.findall(XS + "attribute"):
            e.atributos.append(self.atributo(a))
        # conteúdo simples com atributos (ex.: valor com atributo)
        sc = complexo.find(XS + "simpleContent")
        if sc is not None:
            ext = sc[0]
            e.tipo = ext.get("base", "").split(":")[-1]
            for a in ext.findall(XS + "attribute"):
                e.atributos.append(self.atributo(a))
            return
        for filho in complexo:
            nome = filho.tag.replace(XS, "")
            if nome in ("sequence", "choice"):
                e.conteudo = self.grupo(filho, e.caminho)
                return
        e.conteudo = Grupo("sequence", "1", "1")  # estrutura vazia

    def atributo(self, a):
        at = Elemento("@" + a.get("name"),
                      "1" if a.get("use") == "required" else "0", "1",
                      texto_doc(a))
        tipo = a.get("type", "")
        if tipo:
            at.tipo = tipo.split(":")[-1]
            self.facetas_tipo(at.tipo, at.facetas)
        else:
            r = a.find(XS + "simpleType/" + XS + "restriction")
            if r is not None:
                at.tipo = self.facetas(r, at.facetas).split(":")[-1]
        return at

    def grupo(self, no, caminho):
        g = Grupo(no.tag.replace(XS, ""), no.get("minOccurs", "1"),
                  no.get("maxOccurs", "1"))
        for filho in no:
            nome = filho.tag.replace(XS, "")
            if nome == "element":
                g.itens.append(self.elemento(filho, caminho))
            elif nome in ("sequence", "choice"):
                g.itens.append(self.grupo(filho, caminho))
        return g

    def raiz(self):
        """Elemento raiz do documento (na NF-e, <NFe>, do tipo TNFe)."""
        r = doc_atual()["raiz"]
        no = ET.Element(XS + "element", {"name": r["elemento"],
                                         "type": r["tipo"]})
        return self.elemento(no, "")


def estruturas(e, vistos=None):
    """Percorre as estruturas em profundidade (o próprio e primeiro).

    Um mesmo caminho pode aparecer em mais de um ramo de uma escolha (ex.: o
    grupo IPI, do tipo TIpi, nos dois ramos de imposto); só a primeira
    ocorrência é devolvida."""
    vistos = set() if vistos is None else vistos
    if not e.estrutura or e.caminho in vistos:
        return
    vistos.add(e.caminho)
    yield e
    for filho in filhos(e.conteudo):
        yield from estruturas(filho, vistos)


def filhos(grupo):
    for item in grupo.itens:
        if isinstance(item, Grupo):
            yield from filhos(item)
        else:
            yield item


def ocorrencia(minimo, maximo):
    maximo = "∞" if maximo == "unbounded" else maximo
    return "" if (minimo, maximo) == ("1", "1") else f"{minimo}..{maximo}"


# ------------------------------------------------------------------- SVG

LARG_RAIZ = 230
LARG_CAIXA = 330
LARG_GRUPO = 56
ESP_X = 34          # espaço horizontal entre colunas
ESP_Y = 14          # espaço vertical entre caixas
LINHA = 15          # altura de linha de texto
FONTE = "font-family='Segoe UI, Helvetica, Arial, sans-serif'"
CAR_POR_PX = 6.0    # largura média de um caractere da documentação

COR_BORDA = "#1f4e79"
COR_CABECALHO = "#dce9f5"
COR_ESTRUTURA = "#fff2cc"
COR_TEXTO = "#1a1a1a"
COR_DOC = "#5f6b76"
COR_LINHA = "#7a8a99"


def esc(t):
    return html.escape(t, quote=True)


def curto(t, n):
    return t if len(t) <= n else t[: n - 1] + "…"


def linhas_detalhe(e):
    """Linhas de propriedades exibidas numa caixa de elemento."""
    d = []
    if e.estrutura:
        d.append(("estrutura", (f"{e.tipo} — " if e.tipo else "")
                  + "ver diagrama próprio"))
        return d
    if e.tipo:
        d.append(("tipo", e.tipo))
    f = e.facetas
    if "length" in f:
        d.append(("tamanho", f["length"]))
    elif "minLength" in f or "maxLength" in f:
        d.append(("tamanho", f"{f.get('minLength', '0')} a "
                             f"{f.get('maxLength', '-')}"))
    if "enumeration" in f:
        d.append(("valores", ", ".join(f["enumeration"])))
    if "pattern" in f:
        d.append(("padrão", f["pattern"]))
    return d


def quebra_doc(doc, largura):
    """Documentação quebrada para caber na caixa (até 4 linhas)."""
    n = int((largura - 16) / CAR_POR_PX)
    linhas = textwrap.wrap(doc, n)
    if len(linhas) > 4:
        linhas = linhas[:4]
        linhas[-1] = curto(linhas[-1] + " …", n)
    return linhas


def altura_caixa(e, largura):
    det = len(linhas_detalhe(e))
    doc = len(quebra_doc(e.doc, largura)) if e.doc else 0
    return 24 + det * LINHA + 6 + doc * (LINHA - 2) + (6 if doc else 0)


class Desenho:
    def __init__(self):
        self.partes = []
        self.largura = 0
        self.altura = 0

    def add(self, s):
        self.partes.append(s)

    def limite(self, x, y):
        self.largura = max(self.largura, x)
        self.altura = max(self.altura, y)

    def caixa(self, e, x, y, largura, link=None):
        """Desenha a caixa de um elemento; retorna a altura."""
        h = altura_caixa(e, largura)
        opcional = e.minimo == "0"
        traco = " stroke-dasharray='5,3'" if opcional else ""
        fundo = COR_ESTRUTURA if e.estrutura else "#ffffff"
        abre = f"<a href='{esc(link)}'>" if link else ""
        fecha = "</a>" if link else ""
        self.add(abre)
        self.add(f"<rect x='{x}' y='{y}' width='{largura}' height='{h}' "
                 f"rx='3' fill='{fundo}' stroke='{COR_BORDA}' "
                 f"stroke-width='1.3'{traco}/>")
        self.add(f"<rect x='{x}' y='{y}' width='{largura}' height='22' "
                 f"rx='3' fill='{COR_CABECALHO}' stroke='{COR_BORDA}' "
                 f"stroke-width='1.3'{traco}/>")
        marca = " ⊞" if e.estrutura else ""
        self.add(f"<text x='{x + 8}' y='{y + 16}' {FONTE} font-size='13' "
                 f"font-weight='bold' fill='{COR_TEXTO}'>{esc(e.nome)}"
                 f"{marca}</text>")
        oc = ocorrencia(e.minimo, e.maximo)
        if oc:
            self.add(f"<text x='{x + largura - 8}' y='{y + 16}' {FONTE} "
                     f"font-size='11' text-anchor='end' fill='{COR_BORDA}'>"
                     f"{esc(oc)}</text>")
        yy = y + 22
        for rotulo, valor in linhas_detalhe(e):
            yy += LINHA
            self.add(f"<text x='{x + 8}' y='{yy - 3}' {FONTE} font-size='11'"
                     f" fill='{COR_DOC}'>{esc(rotulo)}</text>")
            self.add(f"<text x='{x + 74}' y='{yy - 3}' {FONTE} "
                     f"font-size='11' fill='{COR_TEXTO}'>"
                     f"<title>{esc(valor)}</title>{esc(curto(valor, 42))}"
                     f"</text>")
        if e.doc:
            yy += 6
            for linha in quebra_doc(e.doc, largura):
                yy += LINHA - 2
                self.add(f"<text x='{x + 8}' y='{yy - 3}' {FONTE} "
                         f"font-size='10.5' font-style='italic' "
                         f"fill='{COR_DOC}'>{esc(linha)}</text>")
        self.add(fecha)
        self.limite(x + largura, y + h)
        return h

    def simbolo_grupo(self, g, x, y):
        """Símbolo do compositor (sequência ou escolha); altura 26."""
        rotulo = "seq." if g.tipo == "sequence" else "escolha"
        traco = " stroke-dasharray='4,3'" if g.minimo == "0" else ""
        self.add(f"<rect x='{x}' y='{y}' width='{LARG_GRUPO}' height='26' "
                 f"rx='13' fill='#f3f6f9' stroke='{COR_LINHA}'{traco}/>")
        self.add(f"<text x='{x + LARG_GRUPO / 2}' y='{y + 17}' {FONTE} "
                 f"font-size='10.5' text-anchor='middle' fill='{COR_TEXTO}'>"
                 f"{rotulo}</text>")
        oc = ocorrencia(g.minimo, g.maximo)
        if oc:
            self.add(f"<text x='{x + LARG_GRUPO / 2}' y='{y + 38}' {FONTE} "
                     f"font-size='10' text-anchor='middle' "
                     f"fill='{COR_BORDA}'>{esc(oc)}</text>")
        self.limite(x + LARG_GRUPO, y + 40)

    def linha(self, x1, y1, x2, y2):
        self.add(f"<path d='M{x1},{y1} H{(x1 + x2) / 2} V{y2} H{x2}' "
                 f"fill='none' stroke='{COR_LINHA}' stroke-width='1.2'/>")

    def grupo(self, g, x, y, links):
        """Desenha o compositor e seus itens; retorna (altura, y_meio)."""
        xi = x + LARG_GRUPO + ESP_X
        yy = y
        ancoras = []
        for item in g.itens:
            if isinstance(item, Grupo):
                h, meio = self.grupo(item, xi, yy, links)
            else:
                h = self.caixa(item, xi, yy, LARG_CAIXA,
                               links.get(item.caminho))
                meio = yy + 11
            ancoras.append(meio)
            yy += h + ESP_Y
        altura = max(yy - y - ESP_Y, 40)
        meio_grupo = (ancoras[0] + ancoras[-1]) / 2 if ancoras else y + 13
        yg = meio_grupo - 13
        self.simbolo_grupo(g, x, yg)
        for a in ancoras:
            self.linha(x + LARG_GRUPO, meio_grupo, xi, a)
        return altura, meio_grupo


def diagrama(e, links, titulo):
    """SVG da estrutura e: caixa da raiz + conteúdo."""
    d = Desenho()
    topo = 52
    x_grupo = 20 + LARG_RAIZ + ESP_X
    altura_conteudo, meio = d.grupo(e.conteudo, x_grupo, topo, links)

    # caixa da raiz (com atributos listados), centrada no compositor
    raiz = Elemento(e.nome, e.minimo, e.maximo, e.doc)
    hr = altura_caixa(raiz, LARG_RAIZ) + len(e.atributos) * LINHA
    yr = max(topo, meio - 11)
    d.caixa(raiz, 20, yr, LARG_RAIZ)
    ya = yr + altura_caixa(raiz, LARG_RAIZ)
    for at in e.atributos:
        oc = "obrigatório" if at.minimo == "1" else "opcional"
        info = at.tipo or ""
        if "pattern" in at.facetas:
            info += " " + at.facetas["pattern"]
        if "enumeration" in at.facetas:
            info += " (" + ", ".join(at.facetas["enumeration"]) + ")"
        d.add(f"<text x='28' y='{ya + 11}' {FONTE} font-size='11' "
              f"fill='{COR_TEXTO}'><title>{esc(info.strip())}</title>"
              f"{esc(at.nome)} <tspan fill='{COR_DOC}'>{oc}"
              f"</tspan></text>")
        ya += LINHA
    d.limite(20 + LARG_RAIZ, yr + hr)
    d.linha(20 + LARG_RAIZ, yr + 11, x_grupo, meio)

    largura = d.largura + 20
    altura = max(d.altura, topo + altura_conteudo) + 20
    cab = (f"<text x='20' y='26' {FONTE} font-size='15' font-weight='bold' "
           f"fill='{COR_TEXTO}'>{esc(titulo)}</text>"
           f"<text x='20' y='42' {FONTE} font-size='11' fill='{COR_DOC}'>"
           f"Gerado de {doc_atual()['leiaute']} por tools/gerar_diagramas.py — "
           f"caixa tracejada: opcional; ⊞: estrutura com diagrama próprio"
           f"</text>")
    return ("<?xml version='1.0' encoding='UTF-8'?>\n"
            f"<svg xmlns='http://www.w3.org/2000/svg' width='{largura}' "
            f"height='{altura}' viewBox='0 0 {largura} {altura}'>\n"
            f"<rect width='100%' height='100%' fill='#ffffff'/>\n"
            + cab + "\n" + "\n".join(d.partes) + "\n</svg>\n")


# ----------------------------------------------------------- índice/TODO


def linhas_arvore(e, nivel=0, vistos=None):
    """(nível, elemento) de cada estrutura, em ordem do leiaute, sem repetir
    caminhos (ver estruturas())."""
    vistos = set() if vistos is None else vistos
    vistos.add(e.caminho)
    out = [(nivel, e)]
    for filho in filhos(e.conteudo):
        if filho.estrutura and filho.caminho not in vistos:
            out.extend(linhas_arvore(filho, nivel + 1, vistos))
    return out


def indice(raiz):
    partes = [
        f"# Diagramas das estruturas {da()} {nome_doc()}",
        "",
        "Gerados automaticamente a partir do schema oficial "
        f"(`{doc_atual()['schemas']}/{doc_atual()['leiaute']}`) por "
        "`tools/gerar_diagramas.py`. **Não edite os SVGs**: atualize o "
        f"schema e rode `{doc_atual().comando('gerar_diagramas.py')}`.",
        "",
        "Em cada diagrama: caixa tracejada = opcional; `0..1`, `1..∞` = "
        "ocorrências; **seq.** = os filhos aparecem nessa ordem; "
        "**escolha** = apenas um dos filhos; caixas amarelas (⊞) são "
        "estruturas com diagrama próprio, listadas abaixo.",
        "",
    ]
    for nivel, e in linhas_arvore(raiz):
        oc = ocorrencia(e.minimo, e.maximo)
        oc = f" `{oc}`" if oc else ""
        doc = f" — {e.doc}" if e.doc else ""
        partes.append(f"{'  ' * nivel}- [{e.nome}]({e.caminho}.svg)"
                      f"{oc}{curto(doc, 110)}")
        if doc_atual().padrao:
            pagina = os.path.join(doc_atual().base, "docs", "manual", e.caminho + ".md")
            if os.path.isfile(pagina):
                link = os.path.relpath(pagina, SAIDA).replace(os.sep, "/")
                partes[-1] += f" — [manual]({link})"
    return "\n".join(partes) + "\n"


def marcados_todo():
    """Caminhos de estruturas já marcadas como feitas no TODO.md atual."""
    feitos = set()
    if os.path.exists(TODO):
        with open(TODO, encoding="utf-8") as f:
            for linha in f:
                m = re.search(r"- \[x\].*\(%s/(.+?)\.svg\)"
                              % re.escape(link_todo()), linha)
                if m:
                    feitos.add(m.group(1))
    return feitos


def todo(raiz, extras):
    feitos = marcados_todo()
    partes = [
        "# TODO",
        "",
        f"Estruturas {da()} {nome_doc()} a implementar, na ordem do "
        "schema oficial. Cada item leva ao diagrama da estrutura.",
        "",
        "Marque `[x]` quando a estrutura tiver: criação/liberação, setters "
        "com validação, geração do XML e testes validando contra o XSD. A "
        "lista é gerada por "
        f"`{doc_atual().comando('gerar_diagramas.py', ' --todo')}`, que "
        "preserva os itens marcados.",
        "",
        f"## Estruturas {da()} {doc_atual()['documento']}",
        "",
    ]
    for nivel, e in linhas_arvore(raiz):
        x = "x" if e.caminho in feitos else " "
        oc = ocorrencia(e.minimo, e.maximo)
        oc = f" `{oc}`" if oc else ""
        opc = " _(opcional)_" if e.minimo == "0" else ""
        partes.append(f"{'  ' * nivel}- [{x}] [**{e.nome}**]"
                      f"({link_todo()}/{e.caminho}.svg){oc}{opc}")
    partes.append("")
    partes.extend(extras)
    return "\n".join(partes) + "\n"


EXTRAS_PADRAO = [
    "## Além do leiaute",
    "",
    "- [x] Chave de acesso e dígito verificador (#54)",
    "- [ ] CNPJ alfanumérico (#66)",
    "- [ ] Assinatura digital XMLDSig (#56)",
    "- [ ] Transmissão aos webservices da SEFAZ e tratamento do retorno "
    "(#57)",
    "- [ ] Troca da licença para LGPL (#59)",
]


def extras_padrao():
    if doc_atual().padrao:
        return EXTRAS_PADRAO
    return ["## Além do leiaute", "",
            "- [ ] Assinatura, transmissão e eventos"]


def extras_todo():
    """Mantém a seção 'Além do leiaute' do TODO.md atual, se existir."""
    if not os.path.exists(TODO):
        return extras_padrao()
    with open(TODO, encoding="utf-8") as f:
        texto = f.read()
    i = texto.find("## Além do leiaute")
    if i < 0:
        return extras_padrao()
    return texto[i:].rstrip("\n").split("\n")


# ------------------------------------------------------------------ main


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    argumento(ap)
    ap.add_argument("--todo", action="store_true",
                    help="regenera também o TODO.md")
    args = ap.parse_args()
    configurar(args.config)

    leiaute = Leiaute()
    raiz = leiaute.raiz()
    todas = list(estruturas(raiz))
    links_de = {}
    for e in todas:
        for filho in filhos(e.conteudo):
            if filho.estrutura:
                rel = os.path.relpath(filho.caminho + ".svg",
                                      os.path.dirname(e.caminho) or ".")
                links_de.setdefault(e.caminho, {})[filho.caminho] = rel

    destino_svg = os.path.join(SAIDA, raiz.nome)
    if os.path.isdir(destino_svg):
        shutil.rmtree(destino_svg)  # remove diagramas de estruturas extintas
    for e in todas:
        arquivo = os.path.join(SAIDA, e.caminho + ".svg")
        os.makedirs(os.path.dirname(arquivo), exist_ok=True)
        titulo = e.caminho.replace("/", " › ")
        with open(arquivo, "w", encoding="utf-8") as f:
            f.write(diagrama(e, links_de.get(e.caminho, {}), titulo))

    with open(os.path.join(SAIDA, "README.md"), "w", encoding="utf-8") as f:
        f.write(indice(raiz))

    if args.todo:
        conteudo = todo(raiz, extras_todo())
        with open(TODO, "w", encoding="utf-8") as f:
            f.write(conteudo)

    if doc_atual().padrao:
        atualizar_manual(doc_atual().base, {e.caminho for e in todas},
                         doc_atual().caminho(os.path.join(doc_atual()["schemas"], "nfe_v4.00.xsd")))

    print(f"{len(todas)} diagramas em {os.path.relpath(SAIDA)}/"
          + (f" e {os.path.relpath(TODO)} atualizado" if args.todo else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
