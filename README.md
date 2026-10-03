# tooldoce
[![CI](https://github.com/icaroraci/tooldoce/actions/workflows/ci.yml/badge.svg)](https://github.com/icaroraci/tooldoce/actions/workflows/ci.yml)

Biblioteca livre em C para emissão de documentos fiscais eletrônicos brasileiros (NF-e, NFC-e, NFS-e, CT-e, MDF-e...), feita para ser usada por ERPs e outros sistemas. A plataforma nativa é Linux.

* [Visão e requisitos do projeto](docs/VISAO.md)
* Veja como [contribuir](https://github.com/icaroraci/tooldoce/blob/master/CONTRIBUTING.md)
* Como manter um [fork](https://github.com/icaroraci/tooldoce/blob/master/CONTRIBUTING.md#mantendo-um-fork)
* [Code Style](https://github.com/icaroraci/tooldoce/wiki/Code-style)
* [Como documentar](https://github.com/icaroraci/tooldoce/wiki/Como-documentar)

## Como compilar

### Dependências
* Compilador C99 (gcc ou clang) e GNU make
* [libxml2](http://xmlsoft.org/) com os arquivos de desenvolvimento (fornece o `xml2-config`)

| Distribuição | Comando |
|---|---|
| Debian / Ubuntu | `sudo apt install build-essential libxml2-dev` |
| Fedora / RHEL | `sudo dnf install gcc make libxml2-devel` |
| Arch Linux | `sudo pacman -S base-devel libxml2` |
| macOS (Homebrew) | `brew install libxml2` |

### Compilação

    $ make

A biblioteca é gerada em `lib/` e os objetos intermediários em `obj/`. Para limpar, use `make clean`.

Se o `xml2-config` estiver fora do `PATH`, informe o caminho: `make XML2_CONFIG=/caminho/para/xml2-config`.

### Instalação

    $ sudo make install

Instala a biblioteca em `/usr/local/lib` e os headers em `/usr/local/include/libnfe`. O destino pode ser alterado com `PREFIX` (ex.: `make install PREFIX=/usr`) e `DESTDIR` (útil para empacotamento). Para remover, use `make uninstall` com os mesmos parâmetros.
