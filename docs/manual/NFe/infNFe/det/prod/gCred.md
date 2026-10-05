# gCred — Grupo de informações sobre o CréditoPresumido

[Manual](../../../../README.md) › [NFe](../../../../NFe.md) › [infNFe](../../../infNFe.md) › [det](../../det.md) › [prod](../prod.md) › gCred

<!-- estado-xsd:inicio -->
> **Estado: documentado.** Estrutura conferida contra a base XSD registrada.
>
> `Base atual e revisada: c537394250f913b591f3984f1de2e9d1a1ec94d334a0086e7a41f9850f5f7917`
<!-- estado-xsd:fim -->

| Informação | Base conferida |
|---|---|
| Caminho XML | `NFe/infNFe/det/prod/gCred` |
| Leiaute / pacote XSD | NF-e/NFC-e 4.00 / PL010f v1.04, 31/08/2026 |
| Biblioteca conferida | libnfe 1.0.0-rc4, commit `1dd412eebca80dd80d884b3f03a1a31cfaf42279` |
| Revisão | 2026-10-05 |
| Contexto no leiaute | `0..4` no pai, sujeito às sequências e escolhas abaixo |

## Finalidade e quando preencher

Os campos identificam e quantificam o produto ou serviço deste item. Dados comerciais e tributáveis podem usar unidades diferentes; quantidades, valores unitários e totais precisam ser coerentes. A API confere formatos, mas não transforma automaticamente unidades nem escolhe CFOP, NCM ou tratamento tributário.

**Atualização publicada:** a NT 2026.008 v1.00 prevê vUnComLiq, vProdLiq, vProdLiqTot e ICMSPrevistoPagtoAntecip/vICMSPrevisto. Esses campos/grupo **não constam no PL010f atualmente disponível no Portal e adotado pelo projeto**, e não são apresentados aqui como API existente. A NT registra homologação em 05/10/2026 e produção em 03/11/2026 para a primeira etapa; sua regra NB01-30 tem cronograma próprio em 2027. Confira [bases e transições](../../../../BASES.md).

GTIN, NCM e CFOP dependem também de cadastros e tabelas fiscais. A NT 2021.003 v1.50 trata da validação de GTIN; a NT 2026.009 v1.00 altera I08-140; a NT 2023.003 v1.40 contém exceções de NFC-e por UF. A aceitação lexical de um código não consulta esses cadastros.

Descrição e observações do XSD adotado: Grupo de informações sobre o CréditoPresumido. A estrutura e as restrições são as do pacote indicado; notas históricas de uso devem ser lidas junto às atualizações listadas nas referências.

## Estrutura, ordem e escolhas

![gCred: estrutura do XSD](../../../../../diagramas/NFe/infNFe/det/prod/gCred.svg)

[Abrir o diagrama](../../../../../diagramas/NFe/infNFe/det/prod/gCred.svg).

Uma ocorrência `1..1` dentro de uma alternativa exige o campo quando essa alternativa é selecionada; não obriga selecionar esse ramo em toda nota. Grupos opcionais podem conter filhos obrigatórios. Os elementos de sequência seguem a ordem indicada.

- **Sequência** `1..1`
  - `cCredPresumido` `1..1`
  - `pCredPresumido` `1..1`
  - `vCredPresumido` `1..1`

## Campos e atributos

| Tag / atributo | Significado e observações | Tipo XML | Ocorrência | Formato / limites | Condição no contexto |
|---|---|---|---|---|---|
| `cCredPresumido` | Código de Benefício Fiscal de Crédito Presumido na UF aplicado ao item | `string` | `1..1` | Padrão: `[!-ÿ]{8}\|[!-ÿ]{10}`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `pCredPresumido` | Percentual do Crédito Presumido | `TDec_0302a04` | `1..1` | Padrão: `0\|0\.[0-9]{2,4}\|[1-9]{1}[0-9]{0,2}(\.[0-9]{2,4})?`<br>Tratamento de espaços: preserve | Obrigatório no contexto |
| `vCredPresumido` | Valor do Crédito Presumido | `TDec_1302` | `1..1` | Padrão: `0\|0\.[0-9]{2}\|[1-9]{1}[0-9]{0,12}(\.[0-9]{2})?`<br>Tratamento de espaços: preserve | Obrigatório no contexto |

Campos decimais usam o separador `.` e a precisão indicada pelo tipo; preservar a representação textual evita perda por ponto flutuante. Ausência, texto vazio e zero são valores distintos. Padrões, comprimentos e enumerações precisam ser atendidos em conjunto; valores de códigos com zeros significativos devem conservar esses zeros. Campos de mesmo nome em outros pais têm contexto próprio.

## API C, integração e memória

Acesso pela API pública: `nfe_prod_grupo(prod)`. O grupo pertence ao objeto que o fornece. Use os caminhos completos a partir desse grupo; se um ancestral for uma lista, obtenha uma ocorrência com `nfe_grupo_add` antes de preencher seus filhos.

[Contrato público de prod.h](../../../../api/prod.md) apresenta assinaturas, argumentos, retornos e limitações. [Convenções de memória e erros](../../../../API.md) explica cópia, empréstimo e transferência de posse.

Ao usar um grupo emprestado, libere o objeto proprietário, nunca o grupo separadamente. Os setters de texto copiam seus valores; getters retornam texto interno emprestado. A escolha de outro ramo válido pode apagar o anterior. Os campos obrigatórios do motor são conferidos na escrita; validar um valor isolado não prova completude do grupo. Os contratos específicos prevalecem quando a função indicada não usa o motor.

## Exemplo C conferido

[Fonte completo do cenário teste_subgrupos](../../../../../../tests/test_prod.c) contém o preenchimento, a conferência dos retornos e a liberação dos recursos. O cenário integra o programa de testes indicado, com os headers e apoios do repositório. O XML foi observado na serialização desse cenário e validado, não deduzido de um desenho.

```sh
make obj/test_prod
./obj/test_prod tests
```

A função abaixo é o cenário completo do teste, incluindo tratamento dos retornos e eventuais verificações de erros. O XML publicado foi observado na validação da linha **338** do fonte indicado; a função pode exercitar outras alterações antes/depois desse ponto.

<details>
<summary>Função C completa do cenário teste_subgrupos</summary>

```c
static void teste_subgrupos(void)
{
	nfe_prod *prod = novo();
	nfe_grupo *g = nfe_prod_grupo(prod), *di, *adi, *ras, *cred;
	char *xml;
	int rc = 0;

	VERIFICA(g != NULL);
	if (!g) {
		nfe_prod_free(prod);
		return;
	}
	rc |= nfe_grupo_add(g, "gCred", &cred);
	rc |= nfe_grupo_set(cred, "cCredPresumido", "SP000001");
	rc |= nfe_grupo_set(cred, "pCredPresumido", "1.00");
	rc |= nfe_grupo_set(cred, "vCredPresumido", "0.15");
	rc |= nfe_grupo_add(g, "DI", &di);
	rc |= nfe_grupo_set(di, "nDI", "2612345678");
	rc |= nfe_grupo_set(di, "dDI", "2026-09-30");
	rc |= nfe_grupo_set(di, "xLocDesemb", "SANTOS");
	rc |= nfe_grupo_set(di, "UFDesemb", "SP");
	rc |= nfe_grupo_set(di, "dDesemb", "2026-10-01");
	rc |= nfe_grupo_set(di, "tpViaTransp", "1");
	rc |= nfe_grupo_set(di, "vAFRMM", "10.00");
	rc |= nfe_grupo_set(di, "tpIntermedio", "1");
	rc |= nfe_grupo_set(di, "cExportador", "EXP001");
	rc |= nfe_grupo_add(di, "adi", &adi);
	rc |= nfe_grupo_set(adi, "nAdicao", "1");
	rc |= nfe_grupo_set(adi, "nSeqAdic", "1");
	rc |= nfe_grupo_set(adi, "cFabricante", "FAB01");
	rc |= nfe_grupo_add(g, "rastro", &ras);
	rc |= nfe_grupo_set(ras, "nLote", "L123");
	rc |= nfe_grupo_set(ras, "qLote", "10.000");
	rc |= nfe_grupo_set(ras, "dFab", "2026-01-01");
	rc |= nfe_grupo_set(ras, "dVal", "2027-01-01");
	rc |= nfe_grupo_set(g, "med/cProdANVISA", "1234567890123");
	rc |= nfe_grupo_set(g, "med/vPMC", "20.00");
	VERIFICA_INT(rc, 0);
	VERIFICA_INT(nfe_grupo_quantidade(g, "DI"), 1);
	VERIFICA(nfe_grupo_item(g, "DI", 0) == di);
	VERIFICA(nfe_grupo_item(g, "DI", 1) == NULL);

	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<NCM>96081000</NCM><gCred>"
		                "<cCredPresumido>SP000001</cCredPresumido>") !=
		         NULL);
		VERIFICA(
		        strstr(xml,
		               "<indTot>1</indTot><DI><nDI>2612345678</nDI>") !=
		        NULL);
		VERIFICA(strstr(xml,
		                "<adi><nAdicao>1</nAdicao><nSeqAdic>1"
		                "</nSeqAdic><cFabricante>FAB01</cFabricante>"
		                "</adi></DI><rastro><nLote>L123</nLote>") !=
		         NULL);
		VERIFICA(strstr(xml, "</rastro><med><cProdANVISA>1234567890123"
		                     "</cProdANVISA><vPMC>20.00</vPMC></med>"
		                     "</prod>") != NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* DI sem adição: lista obrigatória vazia é recusada */
	VERIFICA_INT(nfe_grupo_remove(di, "adi"), 0);
	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, E_VALOR);
	free(xml);

	/* Trocar med por comb apaga med (escolha) */
	VERIFICA_INT(nfe_grupo_remove(g, "DI"), 0);
	VERIFICA_INT(nfe_grupo_set(g, "nRECOPI", "12345678901234567890"), 0);
	VERIFICA(nfe_grupo_get(g, "med/vPMC") == NULL);
	xml = teste_gera(escreve, prod, &rc);
	VERIFICA_INT(rc, 0);
	if (xml) {
		VERIFICA(strstr(xml,
		                "<nRECOPI>12345678901234567890</nRECOPI>") !=
		         NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);

	/* Listas: limite (gCred até 4), caminho inexistente e NULL */
	VERIFICA_INT(nfe_grupo_add(g, "gCred", &cred), 0);
	VERIFICA_INT(nfe_grupo_add(g, "gCred", &cred), 0);
	VERIFICA_INT(nfe_grupo_add(g, "gCred", &cred), 0);
	VERIFICA_INT(nfe_grupo_add(g, "gCred", &cred), E_VALOR);
	VERIFICA_INT(nfe_grupo_add(g, "cProd", &cred), E_VALOR);
	VERIFICA_INT(nfe_grupo_add(g, "gCred", NULL), E_ISNULL);
	VERIFICA_INT(nfe_grupo_quantidade(g, "naoexiste"), E_VALOR);
	VERIFICA(nfe_prod_grupo(NULL) == NULL);
	nfe_prod_free(prod);
}
```

</details>

## XML correspondente

Trecho do cenário acima, com namespace preservado. [XML completo do cenário](../../../../exemplos/grupos/test_prod-0006.xml) permite conferir valores e ancestrais. Dados sintéticos; este exemplo comprova estrutura e uso da API, sem transmissão, autorização ou validação de enquadramento fiscal.

```xml
<gCred xmlns="http://www.portalfiscal.inf.br/nfe">
  <cCredPresumido>SP000001</cCredPresumido>
  <pCredPresumido>1.00</pCredPresumido>
  <vCredPresumido>0.15</vCredPresumido>
</gCred>

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

Anterior: [prod](../prod.md) | Próximo: [DI](DI.md)
