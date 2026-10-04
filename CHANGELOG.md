# Histórico de mudanças

As mudanças relevantes de cada versão ficam registradas aqui. O formato segue o [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/) e as versões seguem o [versionamento semântico](https://semver.org/lang/pt-BR/): a versão maior muda quando a API ou a ABI deixam de ser compatíveis, e com ela o `SONAME` da biblioteca.

## [Não lançado]

### Adicionado

- `nfe_assinar_dados` (`assinatura.h`): assina um conteúdo qualquer com a chave do certificado (RSA-SHA1) e devolve a assinatura em base64, sem expor a chave. Usada pelo QR Code versão 3 da NFC-e em contingência offline (NT 2025.001; icaroraci/libnfc#3).

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

[Não lançado]: https://github.com/icaroraci/tooldoce/compare/v1.0.0-rc1...HEAD
[1.0.0-rc1]: https://github.com/icaroraci/tooldoce/releases/tag/v1.0.0-rc1
