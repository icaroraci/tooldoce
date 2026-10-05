# Geradores, novos XSDs e manutenção do manual

[Manual](README.md) · [Como documentar](../COMO_DOCUMENTAR.md) · [Motor para outros documentos](../ESQUEMAS.md)

Os XSDs oficiais são mantidos sem edição. Tabelas do motor, padrões e diagramas são derivados; as páginas explicativas ficam em docs/manual, separadas dos SVGs. O gerador pode apagar e recriar diagramas mantendo nome e caminho; o texto editorial continua no mesmo endereço.

## Ferramentas

| Ferramenta | Entrada / resultado |
|---|---|
| [gerar_padroes.py](../../tools/gerar_padroes.py) | Tipos dos XSDs → padrões, comprimentos e enumerações C |
| [gerar_esquemas.py](../../tools/gerar_esquemas.py) | Estruturas dos XSDs → tabelas C do motor |
| [gerar_diagramas.py](../../tools/gerar_diagramas.py) | Leiaute → SVGs, índice, TODO e avisos das páginas |
| [manual.py](../../tools/manual.py) | Base normalizada, preservação da revisão e relatório de estado |
| [gerar_issues.py](../../tools/gerar_issues.py) | Textos de issues por estrutura; a geração não publica no GitHub |
| [gerar_tipos_xsd.py](../../tests/schemas/gerar_tipos_xsd.py) | Wrapper para conferir grupos isolados e mensagens |
| [verificar_manual.py](../../tests/verificar_manual.py) | Cobertura, C/XML, links, API e obsolescência |

Sem --config, os geradores usam [nfe.json](../../tools/documentos/nfe.json). Para outro documento, informe seu JSON com diretórios, raiz, tipos, prefixos e saídas. Os caminhos são relativos à raiz de execução. [documento.py](../../tools/documento.py) descreve as chaves.

## Atualizar o pacote

1. Confira pacote, NT e tabelas atuais no Portal; registre origem, versão, publicação e vigência.
2. Substitua os XSDs oficiais da família correta, preservando nomes e dependências. Não alterar XSD para fazer um exemplo passar.
3. Regenere padrões, tabelas, wrapper e diagramas. Confira alterações necessárias na API e nos cálculos.
4. Leia o relatório; revise campos, escolhas, contexto, regras, exemplos, navegação e contratos.
5. Compile/execute os exemplos, valide XML no contexto correto e rode as verificações. Atualize catálogos se o código de origem mudar.
6. Após a revisão, atualize assinatura_revisada e fontes_revisadas das páginas com a base atual; registre biblioteca/pacote/data e regenere o relatório.

```sh
python3 tools/gerar_padroes.py
python3 tools/gerar_esquemas.py
python3 tests/schemas/gerar_tipos_xsd.py
python3 tools/gerar_diagramas.py --todo
make exemplos
make test
python3 tests/verificar_manual.py
```

Os geradores de padrões/esquemas aceitam --verificar. O [CI](../../.github/workflows/ci.yml) confere artefatos e exemplos. Gere num ambiente compatível com os caminhos do projeto, como Linux/WSL, e revise o diff.

## Base atual e base revisada

O [manifesto](schema-manifest.json) associa caminho XML, Markdown e base revisada; [ESTADO.md](ESTADO.md) mostra a base atual. A detecção é **conservadora pelo conjunto de dependências transitivas**: uma mudança relevante marca todas as páginas desse conjunto como obsoletas, mesmo se o campo alterado estiver em outro grupo.

Comentários, indentação e prefixos equivalentes não alteram a assinatura. Tipos, padrões, anotações, atributos e contexto entram na base. O relatório identifica arquivos alterados, sem localizar automaticamente um campo. Nós removidos preservam sua página explicativa, com aviso e sem imagem atual. Nós novos exigem página, exemplo e entrada no manifesto/catálogo.

A geração atualiza a base atual, sem promover automaticamente a revisão. Hashes iguais mostram **Base atual e revisada** uma vez. Hashes distintos mostram **Base revisada** e **Base atual** em linhas próprias, mantendo cada rótulo junto de seu hash, sem quebra entre eles.

NT, MOC, tabelas e API podem mudar sem alteração de XSD e exigem revisão editorial própria. O hash estrutural não detecta toda mudança normativa.

## Outra biblioteca

make install instala os geradores; pkg-config --variable=ferramentas libnfe informa a pasta. Use configuração explícita nas ferramentas instaladas. Schemas/tabelas do outro documento pertencem à sua biblioteca; não acessar as tabelas internas esq_* da NF-e.

[esquema.h](api/esquema.md) e NFE_ESQ_VERSAO definem a ABI do motor. [ESQUEMAS.md](../ESQUEMAS.md) descreve limitações de conteúdo misto, simpleContent, repetição de sequência/escolha e assinatura por ref. [Novo subprojeto](../NOVO-SUBPROJETO.md) e [test_outro_documento.c](../../tests/test_outro_documento.c) mostram configuração e integração.

Anterior: [Exemplos](EXEMPLOS.md) | Próximo: [Índice XML](INDICE.md)
