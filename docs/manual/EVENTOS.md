# Cancelamento, substituição e carta de correção

[Manual](README.md) · [evento.h](api/evento.md) · [Serviços](SERVICOS.md)

Os eventos são documentos separados da nota, ligados pela chave. A rc4 oferece os três tipos abaixo. Os schemas de eventos RTC presentes em `tests/schemas/eventos_rtc` são referência para implementação futura, sem funções públicas de montagem nesta versão.

## Dados comuns e estrutura

`nfe_evento_info` recebe ambiente, chave, documento do autor emitente, instante e fuso. Preserve chave/CNPJ/CPF como texto. O construtor escreve evento/infEvento, com Id, cOrgao, tpAmb, identificação do autor, chNFe, dhEvento, tpEvento, nSeqEvento, verEvento e detEvento. O detalhe depende do tipo:

| Campo comum | Origem e condição |
|---|---|
| evento/@versao | Versão do leiaute de eventos gerada pela função |
| infEvento/@Id | Identificador formado do tipo, chave e sequência; referência da assinatura |
| cOrgao | Órgão identificado pela chave da nota no fluxo implementado |
| tpAmb | Ambiente fornecido em info; deve coincidir com a transmissão |
| CNPJ ou CPF | Documento do autor emitente; escolha exclusiva |
| chNFe | Chave da nota, com 44 posições e DV válido |
| dhEvento | Instante convertido com o fuso informado |
| tpEvento | Código fixo do construtor escolhido |
| nSeqEvento | Sequência do evento; na CC-e, controlar o incremento cumulativo |
| verEvento / detEvento/@versao | Versão do detalhe do evento |
| detEvento/descEvento | Descrição fixa do tipo, produzida pela biblioteca |

| Tipo / função | Campos específicos | Condição |
|---|---|---|
| 110111 / `nfe_evento_cancelamento` | nProt, xJust | Protocolo de autorização da nota; justificativa de 15–255 caracteres |
| 110112 / `nfe_evento_cancelamento_subst` | nProt, xJust, chNFeRef, verAplic e dados fixos do leiaute | Somente NFC-e normal substituída por NFC-e offline da mesma venda |
| 110110 / `nfe_evento_cce` | xCorrecao, xCondUso fixo do leiaute | Correção de 15–1000 caracteres; sequência 1–20, acumulando correções anteriores |

O protocolo aceita 15 ou 17 dígitos conforme o contrato. Todos os construtores devolvem XML ainda não assinado, alocado, com tamanho opcional. O preenchimento de texto não decide prazo, situação da nota ou admissibilidade fiscal do evento.

## Cancelamento por substituição

A nota a cancelar não pode ser offline. A substituta precisa ser NFC-e emitida em contingência offline (`tpEmis=9`) para a mesma venda. Uma NFC-e offline é desfeita por cancelamento comum, quando cabível. Outras combinações são recusadas localmente com `E_VALOR`, correspondendo à restrição indicada no contrato (cStat 920 no autorizador). Consulte as condições oficiais e o sistema da UF antes de transmitir.

## Carta de correção

Cada nova carta substitui as anteriores e deve incluir todas as correções que permanecem válidas, aumentando a sequência. Não corrigir por CC-e variáveis que determinem imposto, dados cadastrais do emitente/destinatário ou data da operação, conforme as condições de uso reproduzidas no leiaute e no contrato. A biblioteca acrescenta o texto fixo xCondUso, mas não analisa semanticamente o conteúdo de xCorrecao.

## Assinatura, lote e retorno

Assine com `nfe_assinar_xml` (infEvento é o elemento coberto), monte envEvento com `nfe_sefaz_msg_evento`, envie ao serviço de eventos e examine retEvento. O retorno do lote não substitui o resultado individual. `nfe_sefaz_proc_evento` procura o registro por chave, tipo e sequência e preserva o XML assinado.

cStat **135** informa registro e vinculação à nota; **136** informa registro sem vinculação. Arquivar procEventoNFe não permite interpretar 136 como cancelamento efetivado ou situação fiscal idêntica a 135. A aplicação precisa interpretar o estado do evento e da nota.

## Exemplos C e XML

O [test_evento.c](../../tests/test_evento.c) contém cenários completos de montagem, assinatura e validação dos detalhes, inclusive cancelamento por substituição e CNPJ alfanumérico. [test_sefaz.c](../../tests/test_sefaz.c) contém lote, retorno e procEventoNFe. São programas compiláveis com dados sintéticos:

```sh
make obj/test_evento obj/test_sefaz
./obj/test_evento tests
./obj/test_sefaz tests
```

Os exemplos de mensagens sem rede e seus XMLs estão em [EXEMPLOS.md](EXEMPLOS.md). As regras de cada tipo vêm de [evento.h](../../include/libnfe/evento.h), [implementação](../../src/libnfe/evento.c), [schemas oficiais por pasta](../../tests/schemas/README.md) e [MOC/atualizações](BASES.md).


## XML produzido pelo exemplo C

Os documentos abaixo são a saída do [programa completo](../../examples/manual_servicos.c), conferida automaticamente, com valores sintéticos. O programa trata os retornos, libera o XML e não transmite. Eventos e inutilização ainda precisam de assinatura.

### cancelamento

```sh
./obj/manual_servicos cancelamento
```

```xml
<evento xmlns="http://www.portalfiscal.inf.br/nfe" versao="1.00">
  <infEvento Id="ID1101113510081234567800019555001000000042112345678101">
    <cOrgao>35</cOrgao>
    <tpAmb>2</tpAmb>
    <CNPJ>12345678000195</CNPJ>
    <chNFe>35100812345678000195550010000000421123456781</chNFe>
    <dhEvento>2026-10-05T09:00:00-03:00</dhEvento>
    <tpEvento>110111</tpEvento>
    <nSeqEvento>1</nSeqEvento>
    <verEvento>1.00</verEvento>
    <detEvento versao="1.00">
      <descEvento>Cancelamento</descEvento>
      <nProt>135260000000001</nProt>
      <xJust>EXEMPLO SINTETICO DE CANCELAMENTO</xJust>
    </detEvento>
  </infEvento>
</evento>
```

### cce

```sh
./obj/manual_servicos cce
```

```xml
<evento xmlns="http://www.portalfiscal.inf.br/nfe" versao="1.00">
  <infEvento Id="ID1101103510081234567800019555001000000042112345678101">
    <cOrgao>35</cOrgao>
    <tpAmb>2</tpAmb>
    <CNPJ>12345678000195</CNPJ>
    <chNFe>35100812345678000195550010000000421123456781</chNFe>
    <dhEvento>2026-10-05T09:00:00-03:00</dhEvento>
    <tpEvento>110110</tpEvento>
    <nSeqEvento>1</nSeqEvento>
    <verEvento>1.00</verEvento>
    <detEvento versao="1.00">
      <descEvento>Carta de Correcao</descEvento>
      <xCorrecao>CORRECAO SINTETICA DE INFORMACAO COMPLEMENTAR</xCorrecao>
      <xCondUso>A Carta de Correcao e disciplinada pelo paragrafo 1o-A do art. 7o do Convenio S/N, de 15 de dezembro de 1970 e pode ser utilizada para regularizacao de erro ocorrido na emissao de documento fiscal, desde que o erro nao esteja relacionado com: I - as variaveis que determinam o valor do imposto tais como: base de calculo, aliquota, diferenca de preco, quantidade, valor da operacao ou da prestacao; II - a correcao de dados cadastrais que implique mudanca do remetente ou do destinatario; III - a data de emissao ou de saida.</xCondUso>
    </detEvento>
  </infEvento>
</evento>
```

Anterior: [Serviços](SERVICOS.md) | Próximo: [Inutilização](INUTILIZACAO.md)
