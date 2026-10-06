# tooldoce
[![CI](https://github.com/icaroraci/tooldoce/actions/workflows/ci.yml/badge.svg)](https://github.com/icaroraci/tooldoce/actions/workflows/ci.yml)
[![Versão](https://img.shields.io/github/v/release/icaroraci/tooldoce?include_prereleases&label=vers%C3%A3o)](https://github.com/icaroraci/tooldoce/releases/latest)
[![Licença: LGPL v3+](https://img.shields.io/badge/licen%C3%A7a-LGPL%20v3%2B-blue.svg)](COPYING.LESSER)
[![C99](https://img.shields.io/badge/C-99-informational.svg)](docs/CONVENCOES.md)
[![NF-e homologada](https://img.shields.io/badge/NF--e%2055-homologada%20na%20SEFAZ-brightgreen.svg)](docs/HOMOLOGACAO.md)

Biblioteca livre em C para emissão de documentos fiscais eletrônicos brasileiros (NF-e, NFS-e, CT-e, MDF-e...), feita para ser usada por ERPs e outros sistemas. A plataforma nativa é Linux.

## Situação

**Versão 1.0.0-rc5**, candidata à 1.0 (ver o [histórico de mudanças](CHANGELOG.md)): a emissão da NF-e modelo 55 está completa, da montagem do XML à autorização, aos eventos e à inutilização na SEFAZ. A partir da 1.0, a API segue o [versionamento semântico](https://semver.org/lang/pt-BR/): mudanças incompatíveis só numa nova versão maior, que também troca o `SONAME` (`libnfe.so.1`). A versão fica em `<libnfe/versao.h>` (`NFE_VERSAO`) e, em tempo de execução, em `nfe_versao()`.

A rc5 acrescenta [diagnósticos automáticos do XSD](docs/manual/DIAGNOSTICOS.md), com tag, caminho, linha, valor recebido e padrão/lista/limite esperado, quando disponíveis. Inclui o [manual das 171 estruturas XML e da API](docs/manual/README.md) e um [exemplo C de exibição dos erros](examples/diagnosticar_xml.c).

O que existe hoje:

| Parte | Situação |
|---|---|
| Todos os grupos da NF-e/NFC-e do leiaute 4.00 (PL_010f, com a Reforma Tributária) | Prontos; o XML gerado valida contra o XSD oficial |
| Chave de acesso, totais automáticos (`nfe_nfe_calcular_totais`) | Prontos |
| Validação contra o schema e regras da SEFAZ (`validar.h`) | Pronto |
| Assinatura digital com certificado A1 (`assinatura.h`) | Pronto |
| Comunicação com a SEFAZ: status, envio do lote, consultas e nfeProc (`sefaz.h`) | Pronto; testado na homologação da SEFAZ (ver [resultados](docs/HOMOLOGACAO.md)) |
| Eventos: cancelamento, cancelamento por substituição e carta de correção (`evento.h`) | Prontos; validados contra os schemas oficiais; cancelamento e carta de correção testados na homologação |
| Tabela de endereços por UF e ambiente (NF-e 55, emissão normal e SVC) | Pronta; ver [endereços e atualização](docs/WEBSERVICES.md) |
| Inutilização de numeração (`inutilizacao.h`) | Pronta; validada contra o schema oficial e testada na homologação |
| Certificado A3 (token/cartão) | Fora do roteiro; a assinatura usa certificado A1 |
| NFS-e, CT-e | Planejados (ver [visão do projeto](docs/VISAO.md)) |
| NFC-e (modelo 65) | Projeto à parte, sobre a libnfe: [libnfc](https://github.com/icaroraci/libnfc) |
| MDF-e (modelo 58) | Projeto à parte, sobre a libnfe: [libmdf](https://github.com/icaroraci/libmdf) (em início) |

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

`sefaz.h` conversa com os webservices da SEFAZ (SOAP sobre HTTPS, com o certificado A1): monta as mensagens (status do serviço, lote de notas, consulta do recibo e da nota), envia, lê o retorno (cStat, protocolo) e junta a nota autorizada ao protocolo (nfeProc). Para consultar os endereços da NF-e modelo 55 por UF, ambiente e tipo de emissão, use `nfe_sefaz_endereco` (ver [fontes, exemplo e atualização](docs/WEBSERVICES.md)). A URL explícita continua disponível. Para um primeiro teste com o seu certificado, em homologação:

    ./obj/status_sefaz empresa.pfx senha <endereço do NFeStatusServico4 da UF> 35

A resposta esperada é `cStat 107: Servico em Operacao`. Se a conexão falhar com "unable to get local issuer certificate", falta a autoridade certificadora da ICP-Brasil usada pelo servidor: veja [`docs/TLS.md`](docs/TLS.md).

[`examples/nfe_ibscbs.c`](examples/nfe_ibscbs.c) monta uma NF-e de regime normal com os tributos da Reforma Tributária (IBS e CBS) no item e nos totais. A exigência depende das condições e cronogramas das Notas Técnicas vigentes; consulte [bases e transições do manual](docs/manual/BASES.md). `nfe_nfe_calcular_totais` soma os itens só no ICMSTot; o IBSCBSTot é preenchido por quem usa a biblioteca, como no exemplo.

Para conferir a nota antes de assinar e transmitir, `validar.h` valida o XML contra os schemas oficiais, que `make install` instala em `$(PREFIX)/share/tooldoce/schemas`, e confere também regras da SEFAZ que o schema não cobre (chave de acesso coerente com os campos, totais iguais à soma dos itens, regras da NFC-e), devolvendo a lista de erros com o campo, a linha e, quando houver, o código de rejeição da SEFAZ.

Para compilar um programa seu com a biblioteca instalada:

    $ cc meu_programa.c $(pkg-config --cflags --libs libnfe)


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

Instala a biblioteca em `/usr/local/lib` e os headers em `/usr/local/include/libnfe`. O destino pode ser alterado com `PREFIX` (ex.: `make install PREFIX=/usr`) e `DESTDIR` (útil para empacotamento). O `make install` também instala o `libnfe.pc` em `$(LIBDIR)/pkgconfig`, usado pelo `pkg-config`; se o prefixo não estiver no caminho de busca dele, exporte `PKG_CONFIG_PATH` (ex.: `export PKG_CONFIG_PATH=/opt/libnfe/lib/pkgconfig`). Para remover, use `make uninstall` com os mesmos parâmetros.

### Testes

    $ make test

Compila e executa os testes de `tests/` com AddressSanitizer e UBSan (desative com `make test SANITIZE=`). O XML gerado é validado contra os schemas oficiais da NF-e em `tests/schemas/`. Com clang, é necessário o runtime dos sanitizers (no Debian/Ubuntu, `libclang-rt-dev`). Em kernels com `vm.mmap_rnd_bits` acima de 28 (comum na WSL2), o AddressSanitizer de compiladores mais antigos, como o GCC 12 do Debian 12, entra em laço (`AddressSanitizer:DEADLYSIGNAL`); nesse caso o Makefile compila os testes sem PIE (`-fno-pie -no-pie`), o que resolve. Depois de trocar `SANITIZE`, recompile com `make -B test`.

## Documentação e contribuição

* [Manual da libnfe](docs/manual/README.md): 171 estruturas XML, referência da API C e guias de instalação, emissão, validação, assinatura, serviços, eventos e atualização.

* [Como documentar os nós XML e a API da libnfe](docs/COMO_DOCUMENTAR.md): padrão de páginas, exemplos, referências oficiais e revisão após atualizações dos XSD.

* [Visão e requisitos do projeto](docs/VISAO.md)
* [Diagramas das estruturas da NF-e](docs/diagramas/README.md), gerados do schema oficial, e a [lista do que falta implementar](TODO.md)
* [Convenções de código](docs/CONVENCOES.md) — antes de enviar uma alteração, rode `make formatar` e `make test`
* Veja como [contribuir](https://github.com/icaroraci/tooldoce/blob/master/CONTRIBUTING.md) e como manter um [fork](https://github.com/icaroraci/tooldoce/blob/master/CONTRIBUTING.md#fluxo-com-fork)
* Wiki: [Code Style](https://github.com/icaroraci/tooldoce/wiki/Code-style) e [Como documentar](https://github.com/icaroraci/tooldoce/wiki/Como-documentar) (em caso de divergência, vale `docs/CONVENCOES.md`)

## Licença

GNU LGPL versão 3 ou posterior (ver [COPYING.LESSER](COPYING.LESSER), que complementa a GPLv3 em [COPYING](COPYING)). A biblioteca pode ser usada em programas de qualquer licença, inclusive proprietários; alterações na própria biblioteca devem ser distribuídas sob a mesma licença.
