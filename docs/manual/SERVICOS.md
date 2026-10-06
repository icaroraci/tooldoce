# Serviços da SEFAZ, retornos e protocolos

[Manual](README.md) · [sefaz.h](api/sefaz.md) · [Endereços por UF](../WEBSERVICES.md)

O módulo monta mensagens, envia SOAP sobre HTTPS com certificado A1, lê retornos e combina o XML original com seu protocolo. As funções de montagem não acessam a rede. `nfe_sefaz_enviar` é a operação que transmite. O ambiente da mensagem, da nota e do endpoint precisa coincidir.

## Endereço e conexão

Para NF-e modelo 55, `nfe_sefaz_endereco` consulta a tabela local por UF, ambiente, tipo de emissão e serviço. A URL retornada é estática. Emissão normal e SVC têm rotas próprias; a biblioteca não decide entrar em contingência nem verifica sua ativação. Não usar essa tabela para NFC-e. Consulte [contingência](CONTINGENCIA.md) e a [origem/atualização dos endereços](../WEBSERVICES.md).

A NT 2026.007 prevê autorização centralizada na SVRS para contribuintes exclusivos do IBS/CBS, nas condições e datas da [base normativa](BASES.md). Essa identificação e o roteamento não são automáticos na rc4. A aplicação deve escolher o endpoint oficial apropriado; a UF, isoladamente, não resolve essa situação.

Carregue o certificado e crie `nfe_sefaz_new(cert)`. O certificado deve viver mais que a conexão. `nfe_sefaz_set_ca` permite informar arquivo PEM de autoridades confiáveis; NULL volta ao conjunto do sistema. `nfe_sefaz_set_timeout` aceita segundos positivos, com padrão 60. Para falhas de cadeia TLS, consulte [TLS.md](../TLS.md).

## Mensagens e serviços

Nas mensagens de consulta, a raiz tem namespace NF-e e versão 4.00. consStatServ leva tpAmb, cUF e xServ=STATUS; consReciNFe leva tpAmb e nRec; consSitNFe leva tpAmb, xServ=CONSULTAR e chNFe. enviNFe acrescenta idLote e indSinc às notas assinadas; envEvento acrescenta idLote aos eventos assinados. Os XMLs executáveis estão no [programa de mensagens](../../examples/manual_servicos.c) e nas [fixtures](EXEMPLOS.md).

| Montagem | Documento / limite | Serviço |
|---|---|---|
| `nfe_sefaz_msg_status` | consStatServ, ambiente e UF | NFE_SERVICO_STATUS |
| `nfe_sefaz_msg_lote` | enviNFe, idLote com 1–15 dígitos, 1–50 notas assinadas; síncrono apenas para uma nota | NFE_SERVICO_AUTORIZACAO |
| `nfe_sefaz_msg_recibo` | consReciNFe, recibo de 15 dígitos | NFE_SERVICO_RET_AUTORIZACAO |
| `nfe_sefaz_msg_consulta` | consSitNFe, chave da nota | NFE_SERVICO_CONSULTA |
| `nfe_sefaz_msg_evento` | envEvento, idLote com 1–15 dígitos, 1–20 eventos assinados | NFE_SERVICO_EVENTO |
| `nfe_inutilizacao` + assinatura | inutNFe | NFE_SERVICO_INUTILIZACAO |

Mensagens e respostas devolvidas são buffers alocados. Use `free` em todos os caminhos de saída. `nfe_sefaz_enviar` recebe o XML da mensagem sem envelope SOAP e devolve o elemento de retorno, com tamanho em bytes. `nfe_sefaz_enviar_ws` oferece montagem de chamada SOAP 1.2 para outra biblioteca; ela precisa informar namespace WSDL, operação, elemento e eventual cabeçalho corretos.

## Retorno local e cStat

`E_REDE` cobre falha de comunicação, TLS, HTTP e SOAP Fault; o detalhe vem de `nfe_sefaz_erro`, emprestado à conexão. Um retorno local 0 significa que a resposta foi obtida e extraída, podendo conter rejeição fiscal.

`nfe_sefaz_cstat` lê o cStat/xMotivo do elemento entregue: no retorno do lote, é o resultado do lote; num protNFe ou retEvento, é o resultado daquela nota/evento. Aceita cStat com três ou quatro dígitos. Não tratar todo cStat como número de três posições.

| Retorno fiscal | Próximo passo |
|---|---|
| Status 107 | Serviço em operação; não é autorização de nota |
| Lote 103 | Guardar nRec e consultar o recibo |
| Recibo 105 | Lote ainda em processamento; a aplicação agenda consulta conforme orientação do serviço |
| Lote 104 | Ler o protocolo de cada nota: pode haver autorização ou rejeição |
| protNFe 100 | Autorização de uso da chave correspondente; combinar nota e protocolo |
| Rejeição no protocolo | Guardar código/motivo e corrigir a causa; transporte bem-sucedido não muda o resultado |

`nfe_sefaz_protocolo` procura a chave no retorno; passar NULL pede o primeiro protocolo, portanto prefira a chave quando houver lote. O protocolo é alocado. `nfe_sefaz_proc` confere a associação e gera nfeProc, copiando a nota assinada sem alterá-la. A aplicação deve examinar cStat antes de registrar o estado autorizado.

Para eventos, leia o retEvento correto e use `nfe_sefaz_proc_evento`. Para inutilização, leia retInutNFe e use `nfe_sefaz_proc_inutilizacao`. A combinação verifica a correspondência de chave/tipo/sequência ou faixa. Esses documentos e seus estados são detalhados em [eventos](EVENTOS.md) e [inutilização](INUTILIZACAO.md).

## Exemplos e evidência

O [status_sefaz.c](../../examples/status_sefaz.c) é exemplo de comunicação real, com certificado e URL fornecidos pela aplicação; não integra a reprodução local sem rede. O [teste de SEFAZ](../../tests/test_sefaz.c) exercita mensagens, retornos e montagem de processados usando XMLs sintéticos; seus cenários podem ser executados localmente. Os [resultados de homologação](../HOMOLOGACAO.md) registram a evidência histórica do projeto e não ampliam o conjunto de regras que o validador local implementa.


## XML produzido pelo exemplo C

Os documentos abaixo são a saída do [programa completo](../../examples/manual_servicos.c), conferida automaticamente, com valores sintéticos. O programa trata os retornos, libera o XML e não transmite. Eventos e inutilização ainda precisam de assinatura.

### status

```sh
./obj/manual_servicos status
```

```xml
<consStatServ xmlns="http://www.portalfiscal.inf.br/nfe" versao="4.00">
  <tpAmb>2</tpAmb>
  <cUF>35</cUF>
  <xServ>STATUS</xServ>
</consStatServ>
```

### recibo

```sh
./obj/manual_servicos recibo
```

```xml
<consReciNFe xmlns="http://www.portalfiscal.inf.br/nfe" versao="4.00">
  <tpAmb>2</tpAmb>
  <nRec>123456789012345</nRec>
</consReciNFe>
```

### consulta

```sh
./obj/manual_servicos consulta
```

```xml
<consSitNFe xmlns="http://www.portalfiscal.inf.br/nfe" versao="4.00">
  <tpAmb>2</tpAmb>
  <xServ>CONSULTAR</xServ>
  <chNFe>35100812345678000195550010000000421123456781</chNFe>
</consSitNFe>
```

Anterior: [Assinatura](ASSINATURA.md) | Próximo: [Eventos](EVENTOS.md)
