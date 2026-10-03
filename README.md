# tooldoce
[![CI](https://github.com/icaroraci/tooldoce/actions/workflows/ci.yml/badge.svg)](https://github.com/icaroraci/tooldoce/actions/workflows/ci.yml)

Biblioteca livre em C para emissão de documentos fiscais eletrônicos brasileiros (NF-e, NFC-e, NFS-e, CT-e, MDF-e...), feita para ser usada por ERPs e outros sistemas. A plataforma nativa é Linux.

## Situação

**Em desenvolvimento.** O que existe hoje:

| Parte | Situação |
|---|---|
| Todos os grupos da NF-e/NFC-e do leiaute 4.00 (PL_010f, com a Reforma Tributária) | Prontos; o XML gerado valida contra o XSD oficial |
| Chave de acesso, totais automáticos (`nfe_nfe_calcular_totais`) | Prontos |
| Validação contra o schema e regras da SEFAZ (`validar.h`) | Pronto |
| Assinatura digital com certificado A1 (`assinatura.h`) | Pronto |
| Comunicação com a SEFAZ: status, envio do lote, consultas e nfeProc (`sefaz.h`) | Pronto, testado com um servidor falso; falta testar na homologação da SEFAZ |
| Eventos (cancelamento, carta de correção), inutilização, tabela de endereços por UF | A fazer |
| Certificado A3 (token/cartão) | A fazer |
| NFS-e, CT-e, MDF-e | Planejados (ver [visão do projeto](docs/VISAO.md)) |

O roteiro detalhado está nas [issues](https://github.com/icaroraci/tooldoce/issues).

## Exemplo

```c
#include <time.h>
#include <libxml/xmlwriter.h>
#include <libnfe/erros.h>
#include <libnfe/ide.h>

nfe_ide *ide = nfe_ide_new();          /* ambiente padrão: homologação */
nfe_ide_set_cuf(ide, NFE_UF_SP);
nfe_ide_set_natop(ide, "VENDA DE MERCADORIA");
nfe_ide_set_nnf(ide, 1);
nfe_ide_set_dhemi(ide, time(NULL));
nfe_ide_set_cmunfg(ide, 3550308);
nfe_ide_set_verproc(ide, "meu ERP 1.0");

int rc = nfe_ide_write_xml(writer, ide); /* writer: xmlTextWriterPtr */
if (rc != 0)
        fprintf(stderr, "erro: %s\n", nfe_strerror(rc));
nfe_ide_free(ide);
```

Os setters validam cada valor contra o leiaute e retornam um código de erro (`erros.h`) quando ele é inválido; a biblioteca não imprime nada. O programa completo está em [`examples/gerar_ide.c`](examples/gerar_ide.c), e [`examples/gerar_nfe.c`](examples/gerar_nfe.c) monta uma NFC-e completa:

    $ make exemplos
    $ ./obj/gerar_ide
    $ ./obj/gerar_nfe
    $ ./obj/assinar_nfe tests/certificados/teste.pfx teste

`assinar_nfe` monta a mesma NFC-e e a assina com o certificado A1 indicado (arquivo .pfx e senha). O de `tests/certificados` é só de teste; para conferir com o seu certificado, rode o exemplo na sua máquina e valide a nota num validador de assinatura de NF-e. Nunca coloque um certificado real no repositório.

`sefaz.h` conversa com os webservices da SEFAZ (SOAP sobre HTTPS, com o certificado A1): monta as mensagens (status do serviço, lote de notas, consulta do recibo e da nota), envia, lê o retorno (cStat, protocolo) e junta a nota autorizada ao protocolo (nfeProc). Para um primeiro teste com o seu certificado, em homologação:

    ./obj/status_sefaz empresa.pfx senha <endereço do NFeStatusServico4 da UF> 35

A resposta esperada é `cStat 107: Servico em Operacao`.

Para conferir a nota antes de assinar e transmitir, `validar.h` valida o XML contra os schemas oficiais, que `make install` instala em `$(PREFIX)/share/tooldoce/schemas`, e confere também regras da SEFAZ que o schema não cobre (chave de acesso coerente com os campos, totais iguais à soma dos itens, regras da NFC-e), devolvendo a lista de erros com o campo, a linha e, quando houver, o código de rejeição da SEFAZ.

Para compilar um programa seu com a biblioteca instalada:

    $ cc meu_programa.c $(xml2-config --cflags) -lnfe $(xml2-config --libs)


## Como compilar

### Dependências
* Compilador C99 (gcc ou clang) e GNU make
* [libxml2](http://xmlsoft.org/) com os arquivos de desenvolvimento (fornece o `xml2-config`)
* [xmlsec1](https://www.aleksey.com/xmlsec/) com OpenSSL, para a assinatura digital (licenças MIT e Apache 2.0)
* [libcurl](https://curl.se/libcurl/) com OpenSSL, para a comunicação com a SEFAZ (licença curl, no estilo MIT)

| Distribuição | Comando |
|---|---|
| Debian / Ubuntu | `sudo apt install build-essential libxml2-dev libxmlsec1-dev libcurl4-openssl-dev` |
| Fedora / RHEL | `sudo dnf install gcc make libxml2-devel xmlsec1-devel xmlsec1-openssl-devel libcurl-devel` |
| Arch Linux | `sudo pacman -S base-devel libxml2` |
| macOS (Homebrew) | `brew install libxml2` |

### Compilação

    $ make

A biblioteca é gerada em `lib/` e os objetos intermediários em `obj/`. Para limpar, use `make clean`.

Se o `xml2-config` estiver fora do `PATH`, informe o caminho: `make XML2_CONFIG=/caminho/para/xml2-config`.

### Instalação

    $ sudo make install

Instala a biblioteca em `/usr/local/lib` e os headers em `/usr/local/include/libnfe`. O destino pode ser alterado com `PREFIX` (ex.: `make install PREFIX=/usr`) e `DESTDIR` (útil para empacotamento). Para remover, use `make uninstall` com os mesmos parâmetros.

### Testes

    $ make test

Compila e executa os testes de `tests/` com AddressSanitizer e UBSan (desative com `make test SANITIZE=`). O XML gerado é validado contra os schemas oficiais da NF-e em `tests/schemas/`. Com clang, é necessário o runtime dos sanitizers (no Debian/Ubuntu, `libclang-rt-dev`).

## Documentação e contribuição

* [Visão e requisitos do projeto](docs/VISAO.md)
* [Diagramas das estruturas da NF-e](docs/diagramas/README.md), gerados do schema oficial, e a [lista do que falta implementar](TODO.md)
* [Convenções de código](docs/CONVENCOES.md) — antes de enviar uma alteração, rode `make formatar` e `make test`
* Veja como [contribuir](https://github.com/icaroraci/tooldoce/blob/master/CONTRIBUTING.md) e como manter um [fork](https://github.com/icaroraci/tooldoce/blob/master/CONTRIBUTING.md#mantendo-um-fork)
* Wiki: [Code Style](https://github.com/icaroraci/tooldoce/wiki/Code-style) e [Como documentar](https://github.com/icaroraci/tooldoce/wiki/Como-documentar) (em caso de divergência, vale `docs/CONVENCOES.md`)

## Licença

GPLv3 (ver [LICENSE](LICENSE)). Está em andamento a troca para LGPL, para permitir o uso em programas de qualquer licença — ver a issue [#59](https://github.com/icaroraci/tooldoce/issues/59).
