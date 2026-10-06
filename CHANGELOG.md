# Histórico de mudanças

As mudanças relevantes de cada versão ficam registradas aqui. O formato segue o [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/) e as versões seguem o [versionamento semântico](https://semver.org/lang/pt-BR/): a versão maior muda quando a API ou a ABI deixam de ser compatíveis, e com ela o `SONAME` da biblioteca.

## [Não lançado]

### Adicionado

- Diagnósticos automáticos de validação: `nfe_erros_caminho`, `nfe_erros_valor`, `nfe_erros_restricao`, `nfe_erros_esperado`, `nfe_erros_dominio_xml` e `nfe_erros_codigo_xml` (`validar.h`). Padrões, enumerações e limites vêm dos erros estruturados do XSD carregado, sem catálogo por tag; mensagens nativas e retornos existentes são preservados. Falhas ao copiar diagnósticos retornam `E_MALLOC`.
- `compacta.h`: `nfe_gzip_base64` e `nfe_base64_gunzip`, para o XML que trafega compactado em gzip e base64 (autorização do MDF-e, `dpsXmlGZipB64` e `nfseXmlGZipB64` da NFS-e nacional). A descompactação recusa conteúdo acima de um limite (16 MiB por padrão). A libnfe passa a depender da zlib, declarada em `Requires.private` do `libnfe.pc` (#285).
- Exemplo `diagnosticar_xml` e guia de diagnóstico, com reprodução dos quatro erros de `refNF`, facetas herdadas/importadas, atributos e alternativas para dados indisponíveis.

### Corrigido

- Validador e geradores: um `^` no início ou um `$` no fim de um `xs:pattern` são lidos como âncoras redundantes e ignorados, como fazem os autorizadores. O schema oficial da NFS-e nacional 1.01 usa `^0{0,4}\d{1,5}$` na série da DPS, e a libxml2, que segue o XML Schema e trata `^` e `$` como caracteres comuns, recusava qualquer série real (#291). Os arquivos dos schemas não são alterados.

### Documentação

- Manual das 171 estruturas XML do PL010f v1.04, com campos, escolhas, API, C/XML, navegação e normas atuais; referência dos 35 headers instalados e guias do fluxo completo da libnfe.
- Exemplos reproduzíveis de grupos e mensagens; conferência automática da correspondência com o código, XSD e símbolos públicos. Avisos mostram bases iguais uma vez e mantêm cada rótulo junto de seu hash.

## [1.0.0-rc4] - 2026-10-05

Quarta candidata à 1.0.0. O motor de grupos deixa de escolher um campo quando o caminho é ambíguo, o que corrige a UF do reboque no MDF-e.

### Alterado

- Motor de grupos: o caminho de um campo não é mais adivinhado. O caminho completo a partir do grupo (`"UF"`, `"prop/UF"`) identifica sempre um único campo; um caminho abreviado (`"ICMS10/vBC"`) só é aceito quando casa com um único campo, e com mais de um `nfe_grupo_set`, `nfe_grupo_remove`, `nfe_grupo_add` e `nfe_grupo_quantidade` devolvem `E_VALOR`, `nfe_grupo_get` e `nfe_grupo_item` devolvem `NULL`. Antes o motor escolhia um dos campos pela forma como casavam, e um caminho abreviado podia gravar um campo diferente do esperado sem erro.

### Corrigido

- Motor de grupos: no grupo de um reboque do MDF-e (`veicReboque`), `"UF"` gravava a UF do proprietário (`prop/UF`) e a UF do reboque não podia ser preenchida.

## [1.0.0-rc3] - 2026-10-04

Terceira candidata à 1.0.0. Traz o motor de grupos como API para os outros documentos, usado pela [libmdf](https://github.com/icaroraci/libmdf) (MDF-e), e mais regras da SEFAZ na validação.

### Adicionado

- Motor de grupos para outros documentos: `nfe_grupo_new`, `nfe_grupo_free`, `nfe_grupo_valida`, `nfe_grupo_vazio`, `nfe_grupo_remove_ultimo` e `nfe_grupo_write_xml` (`esquema.h`) passam a fazer parte da API, com `NFE_ESQ_VERSAO`. Uma biblioteca de outro documento (libmdf, libcte) gera as tabelas dos seus schemas e monta os grupos com o mesmo motor ([`docs/ESQUEMAS.md`](docs/ESQUEMAS.md)).
- Os geradores `tools/gerar_esquemas.py`, `gerar_padroes.py`, `gerar_diagramas.py` e `gerar_issues.py` aceitam `--config`, com os schemas, as raízes, os prefixos e as saídas de outro documento (`tools/documento.py`); sem ele, geram os da NF-e como antes. O `make install` os instala em `share/tooldoce/ferramentas`, indicada pela variável `ferramentas` do `libnfe.pc`.
- `nfe_validar_xml` confere mais regras do MOC 7.0 (Anexo I) antes da transmissão:
  - contingência: 556, 557, 570, 711, 714 e 783;
  - séries do contribuinte e do Fisco: 244 e 451;
  - indicativo do intermediador: 434 e 435;
  - NF-e com DANFE NFC-e ou entrega a domicílio: 710 e 794;
  - NFC-e de entrada, com NFref, com finalidade diferente de normal ou não presencial: 706, 708, 715 e 717.

### Alterado

- As regras de NFC-e que devolviam código 0 passam a devolver o cStat da SEFAZ: idDest (707), tpImp (709) e indFinal (716).
- No motor de grupos, quando mais de um campo casa com o caminho, vale o que casa exatamente, com os elementos do caminho consecutivos. Antes valia o primeiro do leiaute: no modal rodoviário do MDF-e, `"veicTracao/UF"` gravava a UF do proprietário (`veicTracao/prop/UF`) em vez da UF do veículo. `nfe_grupo_get` e `nfe_grupo_remove` seguem a mesma regra.
- As tabelas geradas da NF-e (`esq_*`) deixam de ser exportadas pela `libnfe.so`; eram de uso interno e passam a ser declaradas em `src/libnfe/esquemas.h`, também gerado.

## [1.0.0-rc2] - 2026-10-04

Segunda candidata à 1.0.0, com o que entrou depois da rc1.

### Adicionado

- `make install` instala o `libnfe.pc` em `$(LIBDIR)/pkgconfig`: um programa externo compila com `cc programa.c $(pkg-config --cflags --libs libnfe)` (#266).
- `nfe_certificado_assinar` (`assinatura.h`): assina dados avulsos com a chave do certificado (RSA PKCS#1 v1.5, SHA-1), para o QR Code versão 3 da NFC-e em contingência offline (NT 2025.001) (#268).
- Peças genéricas para os outros documentos fiscais que seguem o padrão da NF-e (MDF-e, CT-e), usadas pela [libmdf](https://github.com/icaroraci/libmdf): `nfe_assinar_elemento` e `nfe_verificar_assinatura_elemento` (`assinatura.h`) assinam e conferem o filho da raiz informado (ex.: `infMDFe`); `nfe_sefaz_enviar_ws` (`sefaz.h`) envia a qualquer webservice SOAP 1.2 da SEFAZ, com namespace, operação, elemento e cabeçalho SOAP informados; `nfe_validador_xsd` e `nfe_validar_xsd` (`validar.h`) validam contra um schema qualquer, sem as regras da NF-e.

### Alterado

- `nfe_evento_cancelamento_subst` recusa (`E_VALOR`) a combinação que a SEFAZ rejeita com cStat 920: a nota cancelada tem de ser de emissão normal e a substituta, de contingência offline (`tpEmis` 9). Regra confirmada na homologação do RJ (icaroraci/libnfc, `docs/HOMOLOGACAO.md`).

## [1.0.0-rc1] - 2026-10-04

Candidata à primeira versão estável (1.0.0). Cobre a emissão da NF-e modelo 55, do XML à SEFAZ, testada na homologação real (ver [`docs/HOMOLOGACAO.md`](docs/HOMOLOGACAO.md)).

### Adicionado

- Todos os grupos da NF-e/NFC-e do leiaute 4.00 no PL_010f, com a Reforma Tributária (IBS, CBS e IS) e o CNPJ alfanumérico; o XML gerado valida contra o XSD oficial.
- Chave de acesso e dígito verificador; totais calculados a partir dos itens (`nfe_nfe_calcular_totais`).
- Validação contra os schemas oficiais e regras da SEFAZ que o schema não cobre (`validar.h`), com os schemas instalados por `make install`.
- Assinatura digital XMLDSig com certificado A1 (`assinatura.h`). O certificado A3 (token/cartão) não faz parte do roteiro.
- Comunicação SOAP/TLS com a SEFAZ (`sefaz.h`): status do serviço, autorização síncrona e assíncrona, consulta do recibo e do protocolo, e montagem do `nfeProc`.
- Tabela de endereços dos webservices por UF, ambiente e emissão (normal e SVC).
- Eventos (`evento.h`): cancelamento, cancelamento por substituição e carta de correção.
- Inutilização de numeração e `ProcInutNFe` (`inutilizacao.h`).
- Versão da biblioteca em `<libnfe/versao.h>` (`NFE_VERSAO`) e em tempo de execução (`nfe_versao()`).
- Testes com AddressSanitizer e UBSan, exemplos em `examples/` e CI com gcc e clang.

### Alterado

- API refeita em relação ao código de 2018: construtores `nfe_*_new` com setters validados no lugar de funções com dezenas de parâmetros; a biblioteca não imprime nada e devolve só códigos de erro (`nfe_strerror`).
- `SONAME` passa de `libnfe.so.0` para `libnfe.so.1`.
- Licença trocada de GPLv3+ para LGPLv3+, o que permite usar a biblioteca em programas de qualquer licença.

[Não lançado]: https://github.com/icaroraci/tooldoce/compare/v1.0.0-rc4...HEAD
[1.0.0-rc4]: https://github.com/icaroraci/tooldoce/compare/v1.0.0-rc3...v1.0.0-rc4
[1.0.0-rc3]: https://github.com/icaroraci/tooldoce/compare/v1.0.0-rc2...v1.0.0-rc3
[1.0.0-rc2]: https://github.com/icaroraci/tooldoce/compare/v1.0.0-rc1...v1.0.0-rc2
[1.0.0-rc1]: https://github.com/icaroraci/tooldoce/releases/tag/v1.0.0-rc1
