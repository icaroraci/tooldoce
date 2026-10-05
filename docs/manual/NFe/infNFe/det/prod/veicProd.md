# veicProd — Veículos novos

[Manual](../../../../README.md) › [NFe](../../../../NFe.md) › [infNFe](../../../infNFe.md) › [det](../../det.md) › [prod](../prod.md) › veicProd

<!-- estado-xsd:inicio -->
> **Estado: documentado.** Estrutura conferida contra a base XSD registrada.
>
> `Base atual e revisada: c537394250f913b591f3984f1de2e9d1a1ec94d334a0086e7a41f9850f5f7917`
<!-- estado-xsd:fim -->

| Informação | Base conferida |
|---|---|
| Caminho XML | `NFe/infNFe/det/prod/veicProd` |
| Leiaute / pacote XSD | NF-e/NFC-e 4.00 / PL010f v1.04, 31/08/2026 |
| Biblioteca conferida | libnfe 1.0.0-rc4, commit `1dd412eebca80dd80d884b3f03a1a31cfaf42279` |
| Revisão | 2026-10-05 |
| Contexto no leiaute | `1..1` no pai, sujeito às sequências e escolhas abaixo |

## Finalidade e quando preencher

Os campos identificam e quantificam o produto ou serviço deste item. Dados comerciais e tributáveis podem usar unidades diferentes; quantidades, valores unitários e totais precisam ser coerentes. A API confere formatos, mas não transforma automaticamente unidades nem escolhe CFOP, NCM ou tratamento tributário.

O detalhamento pertence ao veículo novo deste item: chassi, características, códigos e situação. O exemplo verifica tamanho e domínio; não comprova VIN ou cadastro de veículo existente.

**Atualização publicada:** a NT 2026.008 v1.00 prevê vUnComLiq, vProdLiq, vProdLiqTot e ICMSPrevistoPagtoAntecip/vICMSPrevisto. Esses campos/grupo **não constam no PL010f atualmente disponível no Portal e adotado pelo projeto**, e não são apresentados aqui como API existente. A NT registra homologação em 05/10/2026 e produção em 03/11/2026 para a primeira etapa; sua regra NB01-30 tem cronograma próprio em 2027. Confira [bases e transições](../../../../BASES.md).

GTIN, NCM e CFOP dependem também de cadastros e tabelas fiscais. A NT 2021.003 v1.50 trata da validação de GTIN; a NT 2026.009 v1.00 altera I08-140; a NT 2023.003 v1.40 contém exceções de NFC-e por UF. A aceitação lexical de um código não consulta esses cadastros.

Descrição e observações do XSD adotado: Veículos novos. A estrutura e as restrições são as do pacote indicado; notas históricas de uso devem ser lidas junto às atualizações listadas nas referências.

## Estrutura, ordem e escolhas

![veicProd: estrutura do XSD](../../../../../diagramas/NFe/infNFe/det/prod/veicProd.svg)

[Abrir o diagrama](../../../../../diagramas/NFe/infNFe/det/prod/veicProd.svg).

Uma ocorrência `1..1` dentro de uma alternativa exige o campo quando essa alternativa é selecionada; não obriga selecionar esse ramo em toda nota. Grupos opcionais podem conter filhos obrigatórios. Os elementos de sequência seguem a ordem indicada.

- **Sequência** `1..1`
  - `tpOp` `1..1`
  - `chassi` `1..1`
  - `cCor` `1..1`
  - `xCor` `1..1`
  - `pot` `1..1`
  - `cilin` `1..1`
  - `pesoL` `1..1`
  - `pesoB` `1..1`
  - `nSerie` `1..1`
  - `tpComb` `1..1`
  - `nMotor` `1..1`
  - `CMT` `1..1`
  - `dist` `1..1`
  - `anoMod` `1..1`
  - `anoFab` `1..1`
  - `tpPint` `1..1`
  - `tpVeic` `1..1`
  - `espVeic` `1..1`
  - `VIN` `1..1`
  - `condVeic` `1..1`
  - `cMod` `1..1`
  - `cCorDENATRAN` `1..1`
  - `lota` `1..1`
  - `tpRest` `1..1`

## Campos e atributos

| Tag / atributo | Significado e observações | Tipo XML | Ocorrência | Formato / limites | Condição no contexto |
|---|---|---|---|---|---|
| `tpOp` | Tipo da Operação (1 - Venda concessionária; 2 - Faturamento direto; 3 - Venda direta; 0 - Outros) | `string` | `1..1` | Domínio: `0`, `1`, `2`, `3`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `chassi` | Chassi do veículo - VIN (código-identificação-veículo) | `string` | `1..1` | Comprimento exato: 17<br>Padrão: `[A-Z0-9]+`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `cCor` | Cor do veículo (código de cada montadora) | `TString` | `1..1` | Mínimo: 1<br>Máximo: 4<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `xCor` | Descrição da cor | `TString` | `1..1` | Mínimo: 1<br>Máximo: 40<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `pot` | Potência máxima do motor do veículo em cavalo vapor (CV). (potência-veículo) | `TString` | `1..1` | Mínimo: 1<br>Máximo: 4<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `cilin` | Capacidade voluntária do motor expressa em centímetros cúbicos (CC). (cilindradas) | `TString` | `1..1` | Mínimo: 1<br>Máximo: 4<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `pesoL` | Peso líquido | `TString` | `1..1` | Mínimo: 1<br>Máximo: 9<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `pesoB` | Peso bruto | `TString` | `1..1` | Mínimo: 1<br>Máximo: 9<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `nSerie` | Serial (série) | `TString` | `1..1` | Mínimo: 1<br>Máximo: 9<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `tpComb` | Tipo de combustível-Tabela RENAVAM: 01-Álcool; 02-Gasolina; 03-Diesel; 16-Álcool/Gas.; 17-Gas./Álcool/GNV; 18-Gasolina/Elétrico | `TString` | `1..1` | Mínimo: 1<br>Máximo: 2<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `nMotor` | Número do motor | `TString` | `1..1` | Mínimo: 1<br>Máximo: 21<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `CMT` | CMT-Capacidade Máxima de Tração - em Toneladas 4 casas decimais | `TString` | `1..1` | Mínimo: 1<br>Máximo: 9<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `dist` | Distância entre eixos | `TString` | `1..1` | Mínimo: 1<br>Máximo: 4<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `anoMod` | Ano Modelo de Fabricação | `string` | `1..1` | Padrão: `[0-9]{4}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `anoFab` | Ano de Fabricação | `string` | `1..1` | Padrão: `[0-9]{4}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `tpPint` | Tipo de pintura | `TString` | `1..1` | Comprimento exato: 1<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `tpVeic` | Tipo de veículo (utilizar tabela RENAVAM) | `string` | `1..1` | Padrão: `[0-9]{1,2}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `espVeic` | Espécie de veículo (utilizar tabela RENAVAM) | `string` | `1..1` | Padrão: `[0-9]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `VIN` | Informa-se o veículo tem VIN (chassi) remarcado. R-Remarcado N-NormalVIN | `TString` | `1..1` | Comprimento exato: 1<br>Domínio: `R`, `N`<br>Padrão: `[!-ÿ]{1}[ -ÿ]{0,}[!-ÿ]{1}\|[!-ÿ]{1}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `condVeic` | Condição do veículo (1 - acabado; 2 - inacabado; 3 - semi-acabado) | `string` | `1..1` | Domínio: `1`, `2`, `3`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `cMod` | Código Marca Modelo (utilizar tabela RENAVAM) | `string` | `1..1` | Padrão: `[0-9]{1,6}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `cCorDENATRAN` | Código da Cor Segundo as regras de pré-cadastro do DENATRAN: 01-AMARELO;02-AZUL;03-BEGE;04-BRANCA;05-CINZA;06-DOURADA;07-GRENA 08-LARANJA;09-MARROM;10-PRATA;11-PRETA;12-ROSA;13-ROXA;14-VERDE;15-VERMELHA;16-FANTASIA | `string` | `1..1` | Mínimo: 1<br>Máximo: 2<br>Padrão: `[0-9]{1,2}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `lota` | Quantidade máxima de permitida de passageiros sentados, inclusive motorista. | `string` | `1..1` | Mínimo: 1<br>Máximo: 3<br>Padrão: `[0-9]{1,3}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `tpRest` | Restrição 0 - Não há; 1 - Alienação Fiduciária; 2 - Arrendamento Mercantil; 3 - Reserva de Domínio; 4 - Penhor de Veículos; 9 - outras. | `string` | `1..1` | Domínio: `0`, `1`, `2`, `3`, `4`, `9`<br>Tratamento de espaços: preserve | Obrigatório no contexto |

Campos decimais usam o separador `.` e a precisão indicada pelo tipo; preservar a representação textual evita perda por ponto flutuante. Ausência, texto vazio e zero são valores distintos. Padrões, comprimentos e enumerações precisam ser atendidos em conjunto; valores de códigos com zeros significativos devem conservar esses zeros. Campos de mesmo nome em outros pais têm contexto próprio.

## API C, integração e memória

Acesso pela API pública: `nfe_prod_grupo(prod)`. O grupo pertence ao objeto que o fornece. Use os caminhos completos a partir desse grupo; se um ancestral for uma lista, obtenha uma ocorrência com `nfe_grupo_add` antes de preencher seus filhos.

[Contrato público de prod.h](../../../../api/prod.md) apresenta assinaturas, argumentos, retornos e limitações. [Convenções de memória e erros](../../../../API.md) explica cópia, empréstimo e transferência de posse.

Ao usar um grupo emprestado, libere o objeto proprietário, nunca o grupo separadamente. Os setters de texto copiam seus valores; getters retornam texto interno emprestado. A escolha de outro ramo válido pode apagar o anterior. Os campos obrigatórios do motor são conferidos na escrita; validar um valor isolado não prova completude do grupo. Os contratos específicos prevalecem quando a função indicada não usa o motor.

## Exemplo C conferido

[Fonte completo do cenário caso_073](../../../../../../examples/manual/casos.inc) contém o preenchimento, a conferência dos retornos e a liberação dos recursos. [Programa que cria o objeto e obtém o grupo](../../../../../../examples/manual_grupos.c) fornece os headers e a execução desse cenário.

```sh
make exemplos
./obj/manual_grupos NFe/infNFe/det/prod/veicProd
```

Recorte do cenário executado (o programa completo define CONFERE e fornece o grupo do objeto proprietário):

```c
static int caso_073(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpOp", "0"));
	CONFERE(nfe_grupo_set(g, "veicProd/chassi", "AAAAAAAAAAAAAAAAA"));
	CONFERE(nfe_grupo_set(g, "veicProd/cCor", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/xCor", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/pot", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/cilin", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/pesoL", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/pesoB", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/nSerie", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpComb", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/nMotor", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/CMT", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/dist", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/anoMod", "0000"));
	CONFERE(nfe_grupo_set(g, "veicProd/anoFab", "0000"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpPint", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpVeic", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/espVeic", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/VIN", "R"));
	CONFERE(nfe_grupo_set(g, "veicProd/condVeic", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/cMod", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/cCorDENATRAN", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/lota", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpRest", "0"));
fim:
	return rc;
}
```

## XML correspondente

Trecho do cenário acima, com namespace preservado. [XML completo do cenário](../../../../exemplos/grupos/caso_073.xml) e [nota de contexto validada](../../../../exemplos/contextos/caso_073.xml) permitem conferir valores e ancestrais. Dados sintéticos; este exemplo comprova estrutura e uso da API, sem transmissão, autorização ou validação de enquadramento fiscal.

```xml
<veicProd xmlns="http://www.portalfiscal.inf.br/nfe">
  <tpOp>0</tpOp>
  <chassi>AAAAAAAAAAAAAAAAA</chassi>
  <cCor>1.00</cCor>
  <xCor>EXEMPLO</xCor>
  <pot>1.00</pot>
  <cilin>1.00</cilin>
  <pesoL>EXEMPLO</pesoL>
  <pesoB>EXEMPLO</pesoB>
  <nSerie>EXEMPLO</nSerie>
  <tpComb>1</tpComb>
  <nMotor>EXEMPLO</nMotor>
  <CMT>EXEMPLO</CMT>
  <dist>1.00</dist>
  <anoMod>0000</anoMod>
  <anoFab>0000</anoFab>
  <tpPint>1</tpPint>
  <tpVeic>1</tpVeic>
  <espVeic>1</espVeic>
  <VIN>R</VIN>
  <condVeic>1</condVeic>
  <cMod>1</cMod>
  <cCorDENATRAN>1</cCorDENATRAN>
  <lota>1</lota>
  <tpRest>0</tpRest>
</veicProd>

```

O fragmento é validado no contexto que o declara no leiaute, com namespace e ancestrais. O wrapper tipos_v4.00.xsd, quando usado nos testes, extrai essas declarações do schema oficial e é identificado como apoio de testes, sem substituir o pacote oficial.

## Validação, erros e limites

| Situação | Etapa / resultado | Tratamento |
|---|---|---|
| Ponteiro obrigatório nulo | API: `E_ISNULL` (-1), conforme contrato da função | Conferir criação e argumentos |
| Texto fora do comprimento | Setter/motor: `E_TAMANHO` (-2) | Usar os limites do tipo e do argumento C |
| Domínio, padrão ou caminho inválido/ambíguo | Setter/motor: `E_VALOR` (-3) | Corrigir o valor e usar caminho completo |
| Campo obrigatório ou escolha incompleta | Escrita do motor: `E_VALOR`; XSD rejeita | Completar o ramo selecionado |
| Falha de escrita XML | Writer: `E_XML` (-4) | Descartar saída parcial e tratar a falha |
| Falta de memória | Criação: NULL; operações que alocam podem retornar `E_MALLOC` (-101) | Encerrar com limpeza dos recursos ainda próprios |
| Regra fiscal, cadastro, cálculo ou vigência | Pode ser aceito lexicalmente; depende da regra e do autorizador | Conferir fontes e contratos; não confundir 0 com autorização |

Os códigos negativos são da biblioteca, não códigos cStat. [Validação e regras implementadas](../../../../VALIDACAO.md) distingue XSD, verificações locais e retorno da SEFAZ. A referência da API identifica exceções aos comportamentos gerais desta tabela.

## Referências e grupos relacionados

- XSD atual: [leiaute](../../../../../../tests/schemas/nfe/leiauteNFe_v4.00.xsd), [tipos básicos](../../../../../../tests/schemas/nfe/tiposBasico_v4.00.xsd) e [tipos DFe/RTC](../../../../../../tests/schemas/nfe/DFeTiposBasicos_v1.00.xsd).
- MOC 7.0, Anexo I: I, pp. 17–25. [Fontes oficiais, versões e transições](../../../../BASES.md) relaciona atualizações posteriores, com vigência separada da publicação.
- API: [prod.h](../../../../../../include/libnfe/prod.h) e [referência das funções](../../../../api/prod.md).
- Grupo pai: [prod](../prod.md).

Anterior: [infProdEmb](infProdEmb.md) | Próximo: [med](med.md)
