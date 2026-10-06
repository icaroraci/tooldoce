# gDevTrib — Grupo de Informações da devolução de tributos

[Manual](../../../../../../../README.md) › [NFe](../../../../../../../NFe.md) › [infNFe](../../../../../../infNFe.md) › [det](../../../../../det.md) › [imposto](../../../../imposto.md) › [IBSCBS](../../../IBSCBS.md) › [gIBSCBS](../../gIBSCBS.md) › [gCBS](../gCBS.md) › gDevTrib

<!-- estado-xsd:inicio -->
> **Estado: documentado.** Estrutura conferida contra a base XSD registrada.
>
> `Base atual e revisada: c537394250f913b591f3984f1de2e9d1a1ec94d334a0086e7a41f9850f5f7917`
<!-- estado-xsd:fim -->

| Informação | Base conferida |
|---|---|
| Caminho XML | `NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDevTrib` |
| Leiaute / pacote XSD | NF-e/NFC-e 4.00 / PL010f v1.04, 31/08/2026 |
| Biblioteca conferida | libnfe 1.0.0-rc4, commit `1dd412eebca80dd80d884b3f03a1a31cfaf42279` |
| Revisão | 2026-10-05 |
| Contexto no leiaute | `0..1` no pai, sujeito às sequências e escolhas abaixo |

## Finalidade e quando preencher

Os tributos são informados por item. Escolha os ramos de acordo com o regime do emitente e a operação; códigos CST/CSOSN e classificações fiscais não são decisões tomadas pelo motor de grupos. Bases, alíquotas e valores são textos decimais, e o preenchimento não recalcula os tributos.

A devolução de tributos pertence ao IBS/CBS do grupo pai. O valor não é o valor da mercadoria devolvida e não substitui o referenciamento do item.

A NT 2025.002 v1.52 atualiza esses grupos e suas regras. Escolha CST e cClassTrib nas tabelas oficiais aplicáveis; valores sintéticos do exemplo não comprovam enquadramento. O formato aceito pelo XSD não verifica a vigência, o crédito permitido ou a correção das contas.

**Atualização publicada:** a NT 2026.008 v1.00 prevê vUnComLiq, vProdLiq, vProdLiqTot e ICMSPrevistoPagtoAntecip/vICMSPrevisto. Esses campos/grupo **não constam no PL010f atualmente disponível no Portal e adotado pelo projeto**, e não são apresentados aqui como API existente. A NT registra homologação em 05/10/2026 e produção em 03/11/2026 para a primeira etapa; sua regra NB01-30 tem cronograma próprio em 2027. Confira [bases e transições](../../../../../../../BASES.md).

Descrição e observações do XSD adotado: Grupo de Informações da devolução de tributos. A estrutura e as restrições são as do pacote indicado; notas históricas de uso devem ser lidas junto às atualizações listadas nas referências.

## Estrutura, ordem e escolhas

![gDevTrib: estrutura do XSD](../../../../../../../../diagramas/NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDevTrib.svg)

[Abrir o diagrama](../../../../../../../../diagramas/NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDevTrib.svg).

Uma ocorrência `1..1` dentro de uma alternativa exige o campo quando essa alternativa é selecionada; não obriga selecionar esse ramo em toda nota. Grupos opcionais podem conter filhos obrigatórios. Os elementos de sequência seguem a ordem indicada.

- **Sequência** `1..1`
  - `pDevTrib` `0..1`
  - `vDevTrib` `1..1`

## Campos e atributos

| Tag / atributo | Significado e observações | Tipo XML | Ocorrência | Formato / limites | Condição no contexto |
|---|---|---|---|---|---|
| `pDevTrib` | Percentual de devolução do tributo, conforme LC 214/25 art. 118. | `TDec_0302_04RTC` | `0..1` | Padrão: `0\|0\.[0-9]{2,4}\|[1-9]{1}[0-9]{0,2}(\.[0-9]{2,4})?`<br>Tratamento de espaços: preserve | Opcional no contexto |
| `vDevTrib` | Valor do tributo devolvido ("cashback" de desconto na própria Nota Fiscal / Fatura) | `TDec1302RTC` | `1..1` | Padrão: `0\|0\.[0-9]{2}\|[1-9]{1}[0-9]{0,12}(\.[0-9]{2})?`<br>Tratamento de espaços: preserve | Obrigatório no contexto |

Campos decimais usam o separador `.` e a precisão indicada pelo tipo; preservar a representação textual evita perda por ponto flutuante. Ausência, texto vazio e zero são valores distintos. Padrões, comprimentos e enumerações precisam ser atendidos em conjunto; valores de códigos com zeros significativos devem conservar esses zeros. Campos de mesmo nome em outros pais têm contexto próprio.

## API C, integração e memória

Acesso pela API pública: `nfe_imposto_grupo(imp)`. O grupo pertence ao objeto que o fornece. Use os caminhos completos a partir desse grupo; se um ancestral for uma lista, obtenha uma ocorrência com `nfe_grupo_add` antes de preencher seus filhos.

[Contrato público de imposto.h](../../../../../../../api/imposto.md) apresenta assinaturas, argumentos, retornos e limitações. [Convenções de memória e erros](../../../../../../../API.md) explica cópia, empréstimo e transferência de posse.

Ao usar um grupo emprestado, libere o objeto proprietário, nunca o grupo separadamente. Os setters de texto copiam seus valores; getters retornam texto interno emprestado. A escolha de outro ramo válido pode apagar o anterior. Os campos obrigatórios do motor são conferidos na escrita; validar um valor isolado não prova completude do grupo. Os contratos específicos prevalecem quando a função indicada não usa o motor.

## Exemplo C conferido

[Fonte completo do cenário caso_012](../../../../../../../../../examples/manual/casos.inc) contém o preenchimento, a conferência dos retornos e a liberação dos recursos. [Programa que cria o objeto e obtém o grupo](../../../../../../../../../examples/manual_grupos.c) fornece os headers e a execução desse cenário.

```sh
make exemplos
./obj/manual_grupos NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDevTrib
```

Recorte do cenário executado (o programa completo define CONFERE e fornece o grupo do objeto proprietário):

```c
static int caso_012(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/gDevTrib/vDevTrib",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}
```

## XML correspondente

Trecho do cenário acima, com namespace preservado. [XML completo do cenário](../../../../../../../exemplos/grupos/caso_012.xml) e [nota de contexto validada](../../../../../../../exemplos/contextos/caso_012.xml) permitem conferir valores e ancestrais. Dados sintéticos; este exemplo comprova estrutura e uso da API, sem transmissão, autorização ou validação de enquadramento fiscal.

```xml
<gDevTrib xmlns="http://www.portalfiscal.inf.br/nfe">
  <vDevTrib>1.00</vDevTrib>
</gDevTrib>

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

Os códigos negativos são da biblioteca, não códigos cStat. [Validação e regras implementadas](../../../../../../../VALIDACAO.md) distingue XSD, verificações locais e retorno da SEFAZ. A referência da API identifica exceções aos comportamentos gerais desta tabela.

## Referências e grupos relacionados

- XSD atual: [leiaute](../../../../../../../../../tests/schemas/nfe/leiauteNFe_v4.00.xsd), [tipos básicos](../../../../../../../../../tests/schemas/nfe/tiposBasico_v4.00.xsd) e [tipos DFe/RTC](../../../../../../../../../tests/schemas/nfe/DFeTiposBasicos_v1.00.xsd).
- MOC 7.0, Anexo I: M–U, pp. 25–57. [Fontes oficiais, versões e transições](../../../../../../../BASES.md) relaciona atualizações posteriores, com vigência separada da publicação.
- API: [imposto.h](../../../../../../../../../include/libnfe/imposto.h) e [referência das funções](../../../../../../../api/imposto.md).
- Grupo pai: [gCBS](../gCBS.md).

Anterior: [gDif](gDif.md) | Próximo: [gRed](gRed.md)
