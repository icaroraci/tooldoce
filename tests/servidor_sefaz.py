#!/usr/bin/env python3
"""Servidor HTTPS falso da SEFAZ para tests/test_sefaz.c.

Exige o certificado de cliente de tests/certificados/teste.pfx (autenticação
mútua, como a SEFAZ) e responde mensagens fixas. Escreve a porta na saída e
atende até ser encerrado.

Uso: servidor_sefaz.py <diretório tests>
"""

import http.server
import os
import re
import ssl
import sys

NS = "http://www.portalfiscal.inf.br/nfe"
WSDL = NS + "/wsdl/"


def envelope(corpo):
    return ('<?xml version="1.0" encoding="utf-8"?><soap:Envelope '
            'xmlns:soap="http://www.w3.org/2003/05/soap-envelope">'
            '<soap:Body>' + corpo + '</soap:Body></soap:Envelope>')


def fault(motivo):
    return envelope('<soap:Fault><soap:Code><soap:Value>soap:Receiver'
                    '</soap:Value></soap:Code><soap:Reason>'
                    '<soap:Text xml:lang="pt">' + motivo +
                    '</soap:Text></soap:Reason></soap:Fault>')


def status():
    return envelope(
        '<nfeResultMsg xmlns="' + WSDL + 'NFeStatusServico4">'
        '<retConsStatServ xmlns="' + NS + '" versao="4.00"><tpAmb>2</tpAmb>'
        '<verAplic>TESTE</verAplic><cStat>107</cStat>'
        '<xMotivo>Servico em Operacao</xMotivo><cUF>35</cUF>'
        '<dhRecbto>2026-10-03T10:00:00-03:00</dhRecbto>'
        '</retConsStatServ></nfeResultMsg>')


def autorizacao(chave):
    # Namespace declarado com prefixo no elemento de fora, para conferir que
    # o cliente reconstrói as declarações herdadas
    return envelope(
        '<nfeResultMsg xmlns="' + WSDL + 'NFeAutorizacao4" xmlns:n="' + NS +
        '"><n:retEnviNFe versao="4.00"><n:tpAmb>2</n:tpAmb>'
        '<n:verAplic>TESTE</n:verAplic><n:cStat>104</n:cStat>'
        '<n:xMotivo>Lote processado</n:xMotivo><n:cUF>35</n:cUF>'
        '<n:dhRecbto>2026-10-03T10:00:00-03:00</n:dhRecbto>'
        '<n:protNFe versao="4.00"><n:infProt><n:tpAmb>2</n:tpAmb>'
        '<n:verAplic>TESTE</n:verAplic><n:chNFe>' + chave + '</n:chNFe>'
        '<n:dhRecbto>2026-10-03T10:00:00-03:00</n:dhRecbto>'
        '<n:nProt>135260000000001</n:nProt>'
        '<n:digVal>AAAA</n:digVal><n:cStat>100</n:cStat>'
        '<n:xMotivo>Autorizado o uso da NF-e</n:xMotivo></n:infProt>'
        '</n:protNFe></n:retEnviNFe></nfeResultMsg>')


class Tratador(http.server.BaseHTTPRequestHandler):
    def responde(self, codigo, corpo):
        dados = corpo.encode("utf-8")
        self.send_response(codigo)
        self.send_header("Content-Type", "application/soap+xml; charset=utf-8")
        self.send_header("Content-Length", str(len(dados)))
        self.end_headers()
        self.wfile.write(dados)

    def do_POST(self):
        n = int(self.headers.get("Content-Length", "0"))
        corpo = self.rfile.read(n).decode("utf-8")
        tipo = self.headers.get("Content-Type", "")
        m = re.search(r'action="([^"]*)"', tipo)
        acao = m.group(1) if m else ""
        if not self.connection.getpeercert():
            self.responde(403, fault("sem certificado"))
        elif self.path == "/fault":
            self.responde(500, fault("Erro de teste"))
        elif self.path == "/http404":
            self.responde(404, "nada aqui")
        elif self.path == "/vazio":
            self.responde(200, envelope("<nfeResultMsg/>"))
        elif (acao == WSDL + "NFeStatusServico4/nfeStatusServicoNF" and
              '<nfeDadosMsg xmlns="' + WSDL + 'NFeStatusServico4">'
              '<consStatServ' in corpo):
            self.responde(200, status())
        elif acao == WSDL + "NFeAutorizacao4/nfeAutorizacaoLote":
            m = re.search(r'Id="NFe([0-9]{44})"', corpo)
            if m and "<enviNFe" in corpo:
                self.responde(200, autorizacao(m.group(1)))
            else:
                self.responde(500, fault("lote invalido"))
        else:
            self.responde(500, fault("acao desconhecida: " + acao))

    def log_message(self, *args):
        pass


def main():
    certs = os.path.join(sys.argv[1], "certificados")
    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ctx.load_cert_chain(os.path.join(certs, "servidor.pem"),
                        os.path.join(certs, "servidor_chave.pem"))
    ctx.verify_mode = ssl.CERT_REQUIRED
    ctx.load_verify_locations(os.path.join(certs, "teste_cert.pem"))
    srv = http.server.HTTPServer(("127.0.0.1", 0), Tratador)
    srv.socket = ctx.wrap_socket(srv.socket, server_side=True)
    print(srv.server_address[1], flush=True)
    srv.serve_forever()


if __name__ == "__main__":
    main()
