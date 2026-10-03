#!/usr/bin/env python3
"""Gera a tabela de NF-e 4.00 a partir das capturas oficiais em texto."""
import argparse
import json
from pathlib import Path
from urllib.parse import urlsplit

RAIZ = Path(__file__).resolve().parents[1]
UFS = dict(zip(
    'RO AC AM RR PA AP TO MA PI CE RN PB PE AL SE BA MG ES RJ SP PR SC RS MS MT GO DF'.split(),
    (11, 12, 13, 14, 15, 16, 17, 21, 22, 23, 24, 25, 26, 27, 28, 29,
     31, 32, 33, 35, 41, 42, 43, 50, 51, 52, 53)))
SERVICOS = {'NFeAutorizacao': 'AUTORIZACAO',
            'NFeRetAutorizacao': 'RET_AUTORIZACAO',
            'NfeConsultaProtocolo': 'CONSULTA',
            'NfeStatusServico': 'STATUS', 'RecepcaoEvento': 'EVENTO',
            'NfeInutilizacao': 'INUTILIZACAO'}
AUTORES = 'AM BA GO MG MS MT PE PR RS SP SVAN SVRS SVC-AN SVC-RS'.split()


def constante(autor):
    return 'AUTOR_' + autor.replace('-', '_')


def ler(ambiente):
    caminho = RAIZ / 'docs/webservices' / (ambiente + '.txt')
    mapas, tabelas, secao = {}, {}, None
    for linha in caminho.read_text(encoding='utf-8').splitlines():
        if linha.startswith(('Usuarios', 'UsuarioSVRS')):
            chave, valores = linha.split(': ', 1)
            if chave in mapas:
                raise ValueError('Mapeamento duplicado: ' + chave)
            mapas[chave] = valores.split(', ')
        elif linha.startswith('['):
            secao = linha[1:-1]
            if secao in tabelas:
                raise ValueError('Autorizador duplicado: ' + secao)
            tabelas[secao] = {}
        elif '\t' in linha:
            servico, versao, url = linha.split('\t')
            if secao is None or servico in tabelas[secao]:
                raise ValueError('Serviço duplicado ou sem autorizador')
            if urlsplit(url).scheme != 'https' or not urlsplit(url).netloc:
                raise ValueError('URL inválida: ' + url)
            if servico in SERVICOS and versao != '4.00':
                raise ValueError('Versão de serviço não suportada')
            # O portal publica alguns links para o WSDL. SOAP usa o endpoint.
            tabelas[secao][servico] = url.removesuffix('?wsdl')
    if not set(AUTORES).issubset(tabelas):
        raise ValueError('Captura incompleta')
    normais = {uf: uf for uf in UFS if uf in AUTORES}
    for chave, autor in [('UsuariosSVAN', 'SVAN'),
                         ('UsuarioSVRS_DemServ', 'SVRS')]:
        for uf in mapas[chave]:
            if uf not in UFS or uf in normais:
                raise ValueError('UF normal inválida ou duplicada: ' + uf)
            normais[uf] = autor
    contingencia = {}
    for chave, autor in [('UsuariosSVCAN', 'SVC-AN'),
                         ('UsuariosSVCRS', 'SVC-RS')]:
        for uf in mapas[chave]:
            if uf not in UFS or uf in contingencia:
                raise ValueError('UF de contingência inválida ou duplicada: ' + uf)
            contingencia[uf] = autor
    if set(normais) != set(UFS) or set(contingencia) != set(UFS):
        raise ValueError('Todas as 27 UFs devem ter autorizador')
    for autor in AUTORES:
        esperados = set(SERVICOS)
        if autor == 'SVC-RS':
            esperados.remove('NfeInutilizacao')
        if not esperados.issubset(tabelas[autor]):
            raise ValueError('Serviços incompletos: ' + autor)
    return normais, contingencia, tabelas


def gerar():
    linhas = ['/* Gerado por tools/gerar_enderecos.py; não edite. */',
              '/* Fonte e captura: arquivos .txt em docs/webservices. */',
              'enum { ' + ', '.join(map(constante, AUTORES)) + ', AUTOR_TOTAL };',
              'static const struct {', '\tnfe_uf uf;',
              '\tint normal[2], contingencia[2];', '} autorizadores[] = {']
    ambientes = [ler('producao'), ler('homologacao')]
    for uf in UFS:
        normal = ', '.join(constante(a[0][uf]) for a in ambientes)
        svc = ', '.join(constante(a[1][uf]) for a in ambientes)
        linhas.append(f'\t{{ NFE_UF_{uf}, {{ {normal} }}, {{ {svc} }} }},')
    linhas += ['};', '', 'static const char *const enderecos[2][AUTOR_TOTAL][NFE_SERVICO_INUTILIZACAO + 1] = {']
    for indice, (_, _, tabelas) in enumerate(ambientes):
        linhas.append(f'\t[{indice}] = {{')
        for autor in AUTORES:
            linhas.append(f'\t\t[{constante(autor)}] = {{')
            for servico, enum in SERVICOS.items():
                url = tabelas[autor].get(servico)
                if url:
                    linhas.append(f'\t\t\t[NFE_SERVICO_{enum}] =')
                    # Divide as strings sem modificar nenhum caractere da URL.
                    partes = [url[n:n+60] for n in range(0, len(url), 60)]
                    for n, parte in enumerate(partes):
                        linhas.append('\t\t\t\t' + json.dumps(parte) +
                                      (',' if n == len(partes)-1 else ''))
            linhas.append('\t\t},')
        linhas.append('\t},')
    linhas += ['};', '']
    return '\n'.join(linhas)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--verificar', action='store_true')
    args = parser.parse_args()
    destino = RAIZ / 'src/libnfe/enderecos_dados.h'
    conteudo = gerar()
    if args.verificar:
        if not destino.exists() or destino.read_text(encoding='utf-8') != conteudo:
            parser.exit(1, 'Tabela desatualizada: execute tools/gerar_enderecos.py\n')
    else:
        destino.write_text(conteudo, encoding='utf-8')


if __name__ == '__main__':
    main()
