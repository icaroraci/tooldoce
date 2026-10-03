# Testes na homologação da SEFAZ

Em **3 de outubro de 2026**, a biblioteca foi usada contra o ambiente de
homologação real da SEFAZ (`tpAmb` 2), para uma empresa do Rio de Janeiro
(`cUF` 33), autorizada pela SVRS. As notas e os eventos foram assinados com
um certificado A1 real (e-CNPJ) e enviados com `nfe_sefaz_enviar`, usando os
endereços de `nfe_sefaz_endereco`. Todos os serviços da NF-e modelo 55
cobertos por `sefaz.h`, `evento.h` e `inutilizacao.h` foram aceitos.

Os XMLs não estão no repositório: o certificado embutido na assinatura traz
dados pessoais do responsável pela empresa. Ficam registrados aqui só os
números que a SEFAZ devolveu. O registro detalhado está na
[issue #57](https://github.com/icaroraci/tooldoce/issues/57).

## Resultados

Horários no fuso de Brasília (-03:00).

| Serviço | Webservice | Horário | Resultado da SEFAZ |
|---|---|---|---|
| Status do serviço | `NFeStatusServico4` | 18:26:55 | `cStat` 107, Servico em Operacao |
| Autorização síncrona (nota 1) | `NFeAutorizacao4`, `indSinc` 1 | 18:26:56 | Lote `cStat` 104; nota `cStat` 100, protocolo 333260000910420 |
| Cancelamento (nota 1) | `NFeRecepcaoEvento4`, evento 110111 | 19:50:04 | Lote `cStat` 128; evento `cStat` 135, protocolo 333260000910437 |
| Consulta de protocolo (nota 1) | `NFeConsultaProtocolo4` | 19:55:14 | `cStat` 101, Cancelamento de NF-e homologado, com `protNFe` e `procEventoNFe` |
| Inutilização (série 1, números 2 a 4) | `NFeInutilizacao4` | 19:59:28 | `cStat` 102, Inutilizacao de numero homologado, protocolo 333260000910439 |
| Autorização síncrona (nota 5) | `NFeAutorizacao4`, `indSinc` 1 | 20:02:32 | Lote `cStat` 104; nota `cStat` 100, protocolo 333260000910446 |
| Carta de correção (nota 5) | `NFeRecepcaoEvento4`, evento 110110 | 20:03:52 | Lote `cStat` 128; evento `cStat` 135, protocolo 333260000910448 |
| Envio assíncrono (notas 6 e 7) | `NFeAutorizacao4`, `indSinc` 0 | 20:09:03 | `cStat` 103, Lote recebido com sucesso, recibo 333002241736962 |
| Consulta do recibo (notas 6 e 7) | `NFeRetAutorizacao4` | 20:09:03 | Lote `cStat` 104; as duas notas `cStat` 100, protocolos 333260000910452 e 333260000910453 |

Notas usadas (série 1):

| Nota | Chave de acesso |
|---|---|
| 1 | 33261003465862000188550010000000011663183401 |
| 5 | 33261003465862000188550010000000051937796269 |
| 6 | 33261003465862000188550010000000061487384787 |
| 7 | 33261003465862000188550010000000071010159304 |

## O que foi conferido

- O `digVal` de cada protocolo de autorização é igual ao `DigestValue` da
  assinatura gerada pela biblioteca para a nota: a SEFAZ autorizou exatamente
  o documento assinado localmente.
- A consulta de protocolo da nota 1, depois do cancelamento, devolveu o mesmo
  protocolo de autorização e o evento de cancelamento com a assinatura gerada
  pela biblioteca.
- O `ProcInutNFe` montado por `nfe_sefaz_proc_inutilizacao` contém o pedido
  sem alterações e o retorno recebido.
- Na CC-e, o `xCondUso` é o texto oficial e o `Id` do evento segue o leiaute.
- O recibo consultado é o mesmo devolvido no envio assíncrono.

Consultas feitas antes de cada autorização devolveram `cStat` 217 (nota não
consta na base), como esperado.

## O que não foi testado

- Produção (`tpAmb` 1).
- Outros autorizadores além da SVRS, e as contingências SVC-AN e SVC-RS.
- Cancelamento por substituição (NFC-e) e NFC-e em geral.
- Certificado A3.
