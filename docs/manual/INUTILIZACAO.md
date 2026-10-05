# Inutilização de faixa de numeração

[Manual](README.md) · [inutilizacao.h](api/inutilizacao.md) · [Serviços](SERVICOS.md)

A inutilização comunica que uma faixa de números de uma série não será utilizada, por exemplo após uma quebra de numeração. Não cancela nota autorizada. Antes da montagem, a aplicação verifica histórico da série, condição da faixa, prazo e regras oficiais aplicáveis.

## Parâmetros e estrutura

`nfe_inutilizacao` monta inutNFe/infInut, sem assinatura, e devolve um buffer alocado. O Id é formado dos identificadores da solicitação. O XSD oficial de [inutilização](../../tests/schemas/nfe/leiauteInutNFe_v4.00.xsd) declara a ordem dos campos.

| Parâmetro C / campo | Formato / limite |
|---|---|
| amb / tpAmb | Ambiente do pedido e endpoint correspondentes |
| uf / cUF | Código de UF aceito pela biblioteca |
| ano / ano | Inteiro 0–99: para 2026, passar 26 |
| cnpj / CNPJ | Texto do documento do emitente, preservando sua representação |
| mod / mod | Modelo declarado no contrato e no schema |
| serie / serie | 0–999; admissibilidade depende da situação do emitente |
| nini / nNFIni | 1–999999999 |
| nfin / nNFFin | 1–999999999, maior ou igual a nini |
| xjust / xJust | Justificativa de 15–255 caracteres |

Não passar 2026 no argumento ano. Não usar uma faixa já coberta por nota autorizada; o construtor não consulta a SEFAZ nem o histórico do ERP. Em falha, trate `E_ISNULL`, `E_TAMANHO`, `E_VALOR` ou `E_MALLOC`, conforme [contrato](api/inutilizacao.md).

## Assinatura e registro

Assine infInut com `nfe_assinar_xml`, envie o documento ao serviço `NFE_SERVICO_INUTILIZACAO` e leia retInutNFe. **cStat 102** informa homologação da inutilização. `nfe_sefaz_proc_inutilizacao` combina o pedido assinado e o retorno da mesma faixa, gerando **ProcInutNFe**, alocado, para arquivamento. O nome e a capitalização da raiz seguem o schema.

Mantenha o XML original assinado; não recomponha seus campos depois do retorno. Trate rejeição e falha de comunicação separadamente, preservando o estado para consulta/conciliação em caso de resultado desconhecido.

## Exemplos e validação

O [test_inutilizacao.c](../../tests/test_inutilizacao.c) exercita criação, limites, assinatura e schema. A combinação com retorno está em [test_sefaz.c](../../tests/test_sefaz.c). O [programa de mensagens locais](../../examples/manual_servicos.c) imprime um pedido sintético sem transmitir; seu XML está em [exemplos](EXEMPLOS.md).

```sh
make exemplos
./obj/manual_servicos inutilizacao
make obj/test_inutilizacao
./obj/test_inutilizacao tests
```

Fontes: [implementação](../../src/libnfe/inutilizacao.c), [leiaute](../../tests/schemas/nfe/leiauteInutNFe_v4.00.xsd), [ProcInutNFe](../../tests/schemas/nfe/procInutNFe_v4.00.xsd) e [MOC/base normativa](BASES.md).


## XML produzido pelo exemplo C

Os documentos abaixo são a saída do [programa completo](../../examples/manual_servicos.c), conferida automaticamente, com valores sintéticos. O programa trata os retornos, libera o XML e não transmite. Eventos e inutilização ainda precisam de assinatura.

### inutilizacao

```sh
./obj/manual_servicos inutilizacao
```

```xml
<inutNFe xmlns="http://www.portalfiscal.inf.br/nfe" versao="4.00">
  <infInut Id="ID35261234567800019555001000000010000000012">
    <tpAmb>2</tpAmb>
    <xServ>INUTILIZAR</xServ>
    <cUF>35</cUF>
    <ano>26</ano>
    <CNPJ>12345678000195</CNPJ>
    <mod>55</mod>
    <serie>1</serie>
    <nNFIni>10</nNFIni>
    <nNFFin>12</nNFFin>
    <xJust>NUMEROS PULADOS EM EXEMPLO SINTETICO</xJust>
  </infInut>
</inutNFe>
```

Anterior: [Eventos](EVENTOS.md) | Próximo: [Contingência](CONTINGENCIA.md)
