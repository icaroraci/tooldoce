# PIS — Dados do PIS

[Manual](../../../../README.md) › [NFe](../../../../NFe.md) › [infNFe](../../../infNFe.md) › [det](../../det.md) › [imposto](../imposto.md) › PIS

<!-- estado-xsd:inicio -->
> **Estado: documentado.** Estrutura conferida contra a base XSD registrada.
>
> `Base atual e revisada: c537394250f913b591f3984f1de2e9d1a1ec94d334a0086e7a41f9850f5f7917`
<!-- estado-xsd:fim -->

| Informação | Base conferida |
|---|---|
| Caminho XML | `NFe/infNFe/det/imposto/PIS` |
| Leiaute / pacote XSD | NF-e/NFC-e 4.00 / PL010f v1.04, 31/08/2026 |
| Biblioteca conferida | libnfe 1.0.0-rc4, commit `1dd412eebca80dd80d884b3f03a1a31cfaf42279` |
| Revisão | 2026-10-05 |
| Contexto no leiaute | `0..1` no pai, sujeito às sequências e escolhas abaixo |

## Finalidade e quando preencher

Os tributos são informados por item. Escolha os ramos de acordo com o regime do emitente e a operação; códigos CST/CSOSN e classificações fiscais não são decisões tomadas pelo motor de grupos. Bases, alíquotas e valores são textos decimais, e o preenchimento não recalcula os tributos.

**Atualização publicada:** a NT 2026.008 v1.00 prevê vUnComLiq, vProdLiq, vProdLiqTot e ICMSPrevistoPagtoAntecip/vICMSPrevisto. Esses campos/grupo **não constam no PL010f atualmente disponível no Portal e adotado pelo projeto**, e não são apresentados aqui como API existente. A NT registra homologação em 05/10/2026 e produção em 03/11/2026 para a primeira etapa; sua regra NB01-30 tem cronograma próprio em 2027. Confira [bases e transições](../../../../BASES.md).

Descrição e observações do XSD adotado: Dados do PIS. A estrutura e as restrições são as do pacote indicado; notas históricas de uso devem ser lidas junto às atualizações listadas nas referências.

## Estrutura, ordem e escolhas

![PIS: estrutura do XSD](../../../../../diagramas/NFe/infNFe/det/imposto/PIS.svg)

[Abrir o diagrama](../../../../../diagramas/NFe/infNFe/det/imposto/PIS.svg).

Uma ocorrência `1..1` dentro de uma alternativa exige o campo quando essa alternativa é selecionada; não obriga selecionar esse ramo em toda nota. Grupos opcionais podem conter filhos obrigatórios. Os elementos de sequência seguem a ordem indicada.

- **Escolha exclusiva** `1..1`
  - `PISAliq` `1..1`
  - `PISQtde` `1..1`
  - `PISNT` `1..1`
  - `PISOutr` `1..1`

## Campos e atributos

| Tag / atributo | Significado e observações | Tipo XML | Ocorrência | Formato / limites | Condição no contexto |
|---|---|---|---|---|---|
| [`PISAliq`](PIS/PISAliq.md) | Código de Situação Tributária do PIS. 01 – Operação Tributável - Base de Cálculo = Valor da Operação Alíquota Normal (Cumulativo/Não Cumulativo); 02 - Operação Tributável - Base de Calculo = Valor da Operação (Alíquota Diferenciada); | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`PISQtde`](PIS/PISQtde.md) | Código de Situação Tributária do PIS. 03 - Operação Tributável - Base de Calculo = Quantidade Vendida x Alíquota por Unidade de Produto; | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`PISNT`](PIS/PISNT.md) | Código de Situação Tributária do PIS. 04 - Operação Tributável - Tributação Monofásica - (Alíquota Zero); 06 - Operação Tributável - Alíquota Zero; 07 - Operação Isenta da contribuição; 08 - Operação Sem Incidência da contribuição; 09 - Operação com suspensão da contribuição; | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`PISOutr`](PIS/PISOutr.md) | Código de Situação Tributária do PIS. 99 - Outras Operações. | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |

Campos decimais usam o separador `.` e a precisão indicada pelo tipo; preservar a representação textual evita perda por ponto flutuante. Ausência, texto vazio e zero são valores distintos. Padrões, comprimentos e enumerações precisam ser atendidos em conjunto; valores de códigos com zeros significativos devem conservar esses zeros. Campos de mesmo nome em outros pais têm contexto próprio.

## API C, integração e memória

Acesso pela API pública: `nfe_imposto_grupo(imp)`. O grupo pertence ao objeto que o fornece. Use os caminhos completos a partir desse grupo; se um ancestral for uma lista, obtenha uma ocorrência com `nfe_grupo_add` antes de preencher seus filhos.

[Contrato público de imposto.h](../../../../api/imposto.md) apresenta assinaturas, argumentos, retornos e limitações. [Convenções de memória e erros](../../../../API.md) explica cópia, empréstimo e transferência de posse.

Ao usar um grupo emprestado, libere o objeto proprietário, nunca o grupo separadamente. Os setters de texto copiam seus valores; getters retornam texto interno emprestado. A escolha de outro ramo válido pode apagar o anterior. Os campos obrigatórios do motor são conferidos na escrita; validar um valor isolado não prova completude do grupo. Os contratos específicos prevalecem quando a função indicada não usa o motor.

## Exemplo C conferido

[Fonte completo do cenário teste_valores_invalidos](../../../../../../tests/test_imposto.c) contém o preenchimento, a conferência dos retornos e a liberação dos recursos. O cenário integra o programa de testes indicado, com os headers e apoios do repositório. O XML foi observado na serialização desse cenário e validado, não deduzido de um desenho.

```sh
make obj/test_imposto
./obj/test_imposto tests
```

A função abaixo é o cenário completo do teste, incluindo tratamento dos retornos e eventuais verificações de erros. O XML publicado foi observado na validação da linha **245** do fonte indicado; a função pode exercitar outras alterações antes/depois desse ponto.

<details>
<summary>Função C completa do cenário teste_valores_invalidos</summary>

```c
static void teste_valores_invalidos(void)
{
	nfe_imposto *imp = nfe_imposto_new();
	char *xml;
	int rc;

	VERIFICA(imp != NULL);
	if (!imp)
		return;
	VERIFICA_INT(nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NACIONAL,
	                                       NFE_CSOSN_103),
	             0);
	VERIFICA_INT(nfe_imposto_set_pisnt(imp, NFE_CST_PC_ALIQUOTA_ZERO), 0);

	/* ICMS00 */
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NAO_INFORMADA,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, (nfe_origem)9,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    (nfe_mod_bc)4, "100.00", "18.00",
	                                    "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.0",
	                                    "18.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.0", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "1000.00", "18.00", NULL, NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", "2.00", NULL),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, "100.00",
	                                    "18.00", "18.00", "0", "0.00"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_icms00(imp, NFE_ORIGEM_NACIONAL,
	                                    NFE_MOD_BC_VALOR_OPERACAO, NULL,
	                                    "18.00", "18.00", NULL, NULL),
	             E_ISNULL);

	/* ICMSSN102 */
	VERIFICA_INT(nfe_imposto_set_icmssn102(imp, NFE_ORIGEM_NACIONAL,
	                                       (nfe_csosn_102)101),
	             E_VALOR);
	VERIFICA_INT(
	        nfe_imposto_set_icmssn102(imp, (nfe_origem)-2, NFE_CSOSN_102),
	        E_VALOR);

	/* PIS / COFINS */
	VERIFICA_INT(nfe_imposto_set_pisaliq(imp, NFE_CST_PC_ISENTA, "1.00",
	                                     "1.65", "0.02"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_pisaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA,
	                                     "1.00", "1.6", "0.02"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_cofinsaliq(imp, NFE_CST_PC_ALIQUOTA_BASICA,
	                                        "1,00", "7.60", "0.08"),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_pisnt(imp, NFE_CST_PC_ALIQUOTA_BASICA),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_cofinsnt(imp, (nfe_cst_pis_cofins)10),
	             E_VALOR);
	VERIFICA_INT(nfe_imposto_set_vtottrib(imp, "1.5"), E_VALOR);
	VERIFICA_INT(nfe_imposto_set_vtottrib(NULL, "1.50"), E_ISNULL);

	/* Nada mudou */
	xml = teste_gera(escreve, imp, &rc);
	VERIFICA_INT(rc, 0);
	VERIFICA(xml != NULL);
	if (xml) {
		VERIFICA(strstr(xml, "<CSOSN>103</CSOSN>") != NULL);
		VERIFICA(strstr(xml, "<PISNT><CST>06</CST></PISNT>") != NULL);
		VERIFICA(strstr(xml, "vTotTrib") == NULL);
		VERIFICA(strstr(xml, "COFINS") == NULL);
		VERIFICA_INT(teste_valida(xml), 0);
	}
	free(xml);
	VERIFICA_INT(nfe_imposto_write_xml(NULL, imp), E_ISNULL);
	nfe_imposto_free(imp);
	nfe_imposto_free(NULL);
}
```

</details>

## XML correspondente

Trecho do cenário acima, com namespace preservado. [XML completo do cenário](../../../../exemplos/grupos/test_imposto-0006.xml) permite conferir valores e ancestrais. Dados sintéticos; este exemplo comprova estrutura e uso da API, sem transmissão, autorização ou validação de enquadramento fiscal.

```xml
<PIS xmlns="http://www.portalfiscal.inf.br/nfe">
  <PISNT>
    <CST>06</CST>
  </PISNT>
</PIS>

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
- MOC 7.0, Anexo I: M–U, pp. 25–57. [Fontes oficiais, versões e transições](../../../../BASES.md) relaciona atualizações posteriores, com vigência separada da publicação.
- API: [imposto.h](../../../../../../include/libnfe/imposto.h) e [referência das funções](../../../../api/imposto.md).
- Grupo pai: [imposto](../imposto.md).
- Filhos: [PISAliq](PIS/PISAliq.md), [PISQtde](PIS/PISQtde.md), [PISNT](PIS/PISNT.md), [PISOutr](PIS/PISOutr.md).

Anterior: [ISSQN](ISSQN.md) | Próximo: [PISAliq](PIS/PISAliq.md)
