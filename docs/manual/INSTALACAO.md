# Instalação da libnfe

[Manual](README.md) · [Bases conferidas](BASES.md) · [API](API.md)

Esta revisão usa a **1.0.0-rc4**, candidata à versão 1.0. A plataforma nativa é Linux; os exemplos e testes foram executados em Debian via WSL. O número do leiaute XML 4.00, a versão do pacote PL010f e a versão da biblioteca são identificadores independentes.

## Dependências e compilação

O [Makefile](../../Makefile) exige compilador C99, GNU make, libxml2 de desenvolvimento, xmlsec1 com OpenSSL, libcurl e zlib de desenvolvimento e pkg-config. Em Debian/Ubuntu:

```sh
sudo apt install build-essential pkg-config libxml2-dev libxmlsec1-dev libcurl4-openssl-dev zlib1g-dev
make
make exemplos
```

Para conferir XMLs e regenerar a documentação, acrescente Python 3 e `libxml2-utils`; a verificação de formato usa clang-format. Os geradores do projeto usam a biblioteca padrão do Python. As bibliotecas resultantes ficam em `lib/` e os executáveis em `obj/`. Se `xml2-config` estiver fora do PATH, informe `make XML2_CONFIG=/caminho/xml2-config`.

## Destino da instalação

```sh
sudo make install
sudo ldconfig
pkg-config --modversion libnfe
pkg-config --cflags --libs libnfe
```

`make install` instala os headers em `/usr/local/include/libnfe`, a biblioteca em `/usr/local/lib`, os cinco XSDs da nota em `/usr/local/share/tooldoce/schemas`, os geradores em `/usr/local/share/tooldoce/ferramentas` e **`libnfe.pc` em `/usr/local/lib/pkgconfig`**. Não é necessário copiar o arquivo `.pc` manualmente.

`PREFIX`, `LIBDIR`, `INCLUDEDIR`, `SCHEMADIR`, `FERRAMENTASDIR` e `PKGCONFIGDIR` permitem mudar destinos. Use os mesmos valores na compilação e instalação, pois o caminho padrão dos schemas é gravado na biblioteca. Ao mudar destinos depois de uma compilação, use make clean e recompile com os novos valores; a simples mudança de PREFIX não invalida os objetos já produzidos. `DESTDIR` acrescenta uma raiz temporária para empacotamento; não altera os caminhos finais gravados no `.pc` nem na biblioteca.

Num prefixo próprio, configure a busca do pkg-config e do carregador para os diretórios escolhidos. O [README principal](../../README.md) explica esses ajustes. Se a versão encontrada não for a instalada, confira `pkg-config --variable=libdir libnfe` e evite misturar headers de um prefixo com biblioteca de outro.

## Compilar uma aplicação

```sh
cc meu_programa.c -o meu_programa $(pkg-config --cflags --libs libnfe)
```

Os headers usam `<libnfe/...h>`. `libnfe.pc` declara libxml2 como dependência pública e xmlsec1/libcurl/zlib como dependências privadas. A biblioteca é compartilhada, com SONAME `libnfe.so.1`; a versão completa de pré-lançamento é fornecida pelo `.pc`, por `NFE_VERSAO` e por `nfe_versao()`.

## Conferência após instalar

Execute um [exemplo local](EXEMPLOS.md) e carregue `nfe_validador_new(NULL)`. NULL nessa criação indica falha de leitura dos schemas ou de memória; use `nfe_dir_schemas()` para localizar o diretório esperado. Os schemas de mensagens/eventos dos testes não são todos instalados pelo alvo padrão: para outra mensagem, mantenha seu pacote completo e passe o XSD a `nfe_validador_xsd`.

`make test` roda os testes da biblioteca com sanitizadores. `python3 tests/verificar_manual.py`, após `make exemplos`, confere exemplos e documentação. `make uninstall` remove a instalação dos destinos informados; revise os valores antes de usá-lo em prefixos compartilhados.

Anterior: [Bases](BASES.md) | Próximo: [API e memória](API.md)
