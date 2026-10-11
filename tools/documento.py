"""Configuração de um documento fiscal para os geradores tools/gerar_*.py.

Cada documento (NF-e, MDF-e, CT-e...) descreve num arquivo JSON onde estão
os seus schemas oficiais e onde os geradores gravam o que produzem:

  documento     nome do documento, usado nos textos gerados ("MDF-e")
  artigo        "da" (padrão) ou "do", para os textos ("do CT-e")
  biblioteca    nome da biblioteca, para as issues geradas ("libmdf")
  versao        versão do leiaute ("3.00")
  schemas       pasta com os XSD oficiais, sem alteração
  tipos         XSD com os tipos simples e complexos, em ordem de
                prioridade (um nome repetido vale na primeira ocorrência)
  leiaute       XSD com o leiaute do documento
  raiz          elemento raiz do documento e o seu tipo, para os diagramas:
                {"elemento": "MDFe", "tipo": "TMDFe"}
  repositorio   dono/nome no GitHub e ramo principal ("ramo"), para os
                links das issues geradas
  padroes       {"saida": header, "prefixo": "MDF_"}: tools/gerar_padroes.py
  esquemas      {"saida": .c, "cabecalho": .h, "prefixo": "mdf_esq_",
                 "raizes": ["rodo", ["infDoc_c", "infDoc"], ...]}: tabelas
                do motor de grupos (<libnfe/esquema.h>), por
                tools/gerar_esquemas.py; cada raiz é o nome do elemento ou
                o par [nome C, elemento]
  diagramas     {"saida": pasta, "todo": arquivo}: tools/gerar_diagramas.py

Os caminhos são relativos à pasta de onde o gerador é chamado (a raiz do
projeto do documento). Sem --config vale tools/documentos/nfe.json, com os
caminhos relativos à raiz do tooldoce.

O `make install` do tooldoce instala os geradores numa pasta indicada pela
variável `ferramentas` do libnfe.pc:

  python3 "$(pkg-config --variable=ferramentas libnfe)/gerar_esquemas.py" \
      --config tools/documento.json
"""
import json
import os

RAIZ_TOOLDOCE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PADRAO = os.path.join(RAIZ_TOOLDOCE, "tools", "documentos", "nfe.json")


class Documento:
    def __init__(self, config=None):
        if config is None:
            if not os.path.exists(PADRAO):
                raise SystemExit("informe a configuração do documento com "
                                 "--config (ver documento.py)")
            self.arquivo, self.base = PADRAO, RAIZ_TOOLDOCE
        else:
            self.arquivo, self.base = config, os.getcwd()
        with open(self.arquivo, encoding="utf-8") as f:
            self.c = json.load(f)
        self.padrao = config is None

    def __getitem__(self, chave):
        if chave not in self.c:
            raise SystemExit(f"{self.arquivo}: falta a chave {chave!r}")
        return self.c[chave]

    def get(self, chave, padrao=None):
        return self.c.get(chave, padrao)

    def secao(self, nome, chave):
        s = self[nome]
        if chave not in s:
            raise SystemExit(f"{self.arquivo}: falta {nome}.{chave}")
        return s[chave]

    def caminho(self, relativo):
        return os.path.join(self.base, relativo)

    @property
    def schemas(self):
        return self.caminho(self["schemas"])

    @property
    def arquivos(self):
        """XSD de tipos e o do leiaute, em ordem de prioridade"""
        return list(self["tipos"]) + [self["leiaute"]]

    def xsd(self, arquivo):
        return os.path.join(self.schemas, arquivo)

    def comando(self, script, extra=""):
        """Como rodar o gerador de novo, para os comentários gerados"""
        if self.padrao:
            return f"python3 tools/{script}{extra}"
        return ('python3 "$(pkg-config --variable=ferramentas libnfe)/'
                f'{script}" --config {self.arquivo}{extra}')


def argumento(ap):
    ap.add_argument("--config", metavar="ARQUIVO.json",
                    help="configuração do documento (padrão: a NF-e, em "
                         "tools/documentos/nfe.json)")


def padrao(valor):
    """Valor de um xs:pattern sem o ^ do início e o $ do fim. No XML Schema
    o padrão já casa com o valor inteiro e ^ e $ são caracteres comuns, mas
    alguns schemas oficiais os usam como âncoras (TSSerieDPS da NFS-e:
    "^0{0,4}\\d{1,5}$"); o validador da libnfe os tira do mesmo jeito."""
    if valor is None:
        return None
    if valor.startswith("^"):
        valor = valor[1:]
    if valor.endswith("$") and not valor.endswith("\\$"):
        valor = valor[:-1]
    return valor
