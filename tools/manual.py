"""Controle conservador da base XSD das páginas editoriais (Python padrão)."""
import hashlib
import json
from pathlib import Path
import re
import xml.etree.ElementTree as ET

INICIO = "<!-- estado-xsd:inicio -->"
FIM = "<!-- estado-xsd:fim -->"
XS = "{http://www.w3.org/2001/XMLSchema}"


def base_xsd(arquivo):
    """Inclui dependências transitivas; ignora comentários e indentação XML.

    A assinatura é conservadora: qualquer alteração no conjunto de schemas
    marca todas as páginas. Assim também cobre tipos, ancestrais e exemplos.
    Não atribui automaticamente uma nova base à revisão humana.
    """
    fontes = {}

    def visita(p):
        p = Path(p).resolve()
        if p.name in fontes:
            return
        raiz = ET.parse(p).getroot()
        namespaces = dict(dados for _, dados in ET.iterparse(p, events=("start-ns",)))

        def atributo(nome, valor):
            if nome in ("type", "base", "ref", "itemType", "memberTypes"):
                def expandir(qname):
                    prefixo, separador, local = qname.partition(":")
                    if separador and prefixo in namespaces:
                        return "{" + namespaces[prefixo] + "}" + local
                    return qname
                return " ".join(expandir(q) for q in valor.split())
            return valor

        def normaliza(e):
            return [e.tag, sorted((n, atributo(n, v)) for n, v in e.attrib.items()),
                    " ".join((e.text or "").split()),
                    [normaliza(f) for f in e]]

        dados = json.dumps(normaliza(raiz), ensure_ascii=False).encode()
        fontes[p.name] = hashlib.sha256(dados).hexdigest()
        for e in raiz:
            if e.tag in (XS + "include", XS + "import"):
                if e.get("schemaLocation"):
                    visita(p.parent / e.get("schemaLocation"))

    visita(arquivo)
    assinatura = hashlib.sha256(json.dumps(fontes, sort_keys=True).encode()).hexdigest()
    return assinatura, fontes


def atualizar(base, caminhos, leiaute):
    manual = Path(base) / "docs/manual"
    arquivo = manual / "schema-manifest.json"
    if not arquivo.exists():
        return
    inventario = json.loads(arquivo.read_text(encoding="utf-8"))
    assinatura, fontes = base_xsd(leiaute)
    inventario["base_atual"] = {"assinatura": assinatura, "fontes": fontes}
    estados = []
    for pagina in inventario["paginas"]:
        removido = pagina["caminho_xml"] not in caminhos
        antigas = pagina.get("fontes_revisadas", {})
        mudancas = sorted(n for n in set(antigas) | set(fontes)
                          if antigas.get(n) != fontes.get(n))
        estado = ("obsoleto — nó removido" if removido else
                  "em revisão" if not pagina.get("assinatura_revisada") else
                  "obsoleto" if assinatura != pagina["assinatura_revisada"] else
                  "documentado")
        pagina["estado"] = estado
        motivo = ("Nó removido do XSD atual." if removido else
                  "Base revisada ausente; conferência necessária." if estado == "em revisão" else
                  "Mudança conservadoramente detectada em: " + ", ".join(mudancas) + "."
                  if estado == "obsoleto" else
                  "Estrutura conferida contra a base XSD registrada.")
        estados.append(f"- [{pagina['caminho_xml']}]({pagina['arquivo']}) — **{estado}**. {motivo}")
        p = manual / pagina["arquivo"]
        texto = p.read_text(encoding="utf-8")
        bloco = f"{INICIO}\n> **Estado: {estado}.** {motivo} Base revisada: `{pagina.get('assinatura_revisada', 'ausente')}`. Base atual: `{assinatura}`.\n{FIM}"
        if removido:
            # Preserva o texto, mas evita incorporar um SVG que foi removido.
            texto = re.sub(r"!\[([^]]*)\]\(([^)]+\.svg)\)",
                           r"Diagrama atual indisponível: nó removido (\1).", texto)
        texto, quantidade = re.subn(re.escape(INICIO) + r".*?" + re.escape(FIM),
                                   lambda m: bloco, texto, flags=re.S)
        if quantidade != 1:
            raise ValueError(f"bloco de estado ausente ou duplicado: {p}")
        p.write_text(texto, encoding="utf-8", newline="\n")
    (manual / "ESTADO.md").write_text("# Estado da documentação\n\n"
        "Gerado após os diagramas; a assinatura revisada só muda por revisão editorial.\n\n"
        + "\n".join(estados) + "\n", encoding="utf-8", newline="\n")
    arquivo.write_text(json.dumps(inventario, ensure_ascii=False, indent=2) + "\n",
                       encoding="utf-8", newline="\n")
