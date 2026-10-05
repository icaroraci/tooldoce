# ICMS — Dados do ICMS Normal e ST

[Manual](../../../../README.md) › [NFe](../../../../NFe.md) › [infNFe](../../../infNFe.md) › [det](../../det.md) › [imposto](../imposto.md) › ICMS

<!-- estado-xsd:inicio -->
> **Estado: documentado.** Estrutura conferida contra a base XSD registrada.
>
> `Base atual e revisada: c537394250f913b591f3984f1de2e9d1a1ec94d334a0086e7a41f9850f5f7917`
<!-- estado-xsd:fim -->

| Informação | Base conferida |
|---|---|
| Caminho XML | `NFe/infNFe/det/imposto/ICMS` |
| Leiaute / pacote XSD | NF-e/NFC-e 4.00 / PL010f v1.04, 31/08/2026 |
| Biblioteca conferida | libnfe 1.0.0-rc4, commit `1dd412eebca80dd80d884b3f03a1a31cfaf42279` |
| Revisão | 2026-10-05 |
| Contexto no leiaute | `1..1` no pai, sujeito às sequências e escolhas abaixo |

## Finalidade e quando preencher

Os tributos são informados por item. Escolha os ramos de acordo com o regime do emitente e a operação; códigos CST/CSOSN e classificações fiscais não são decisões tomadas pelo motor de grupos. Bases, alíquotas e valores são textos decimais, e o preenchimento não recalcula os tributos.

**Atualização publicada:** a NT 2026.008 v1.00 prevê vUnComLiq, vProdLiq, vProdLiqTot e ICMSPrevistoPagtoAntecip/vICMSPrevisto. Esses campos/grupo **não constam no PL010f atualmente disponível no Portal e adotado pelo projeto**, e não são apresentados aqui como API existente. A NT registra homologação em 05/10/2026 e produção em 03/11/2026 para a primeira etapa; sua regra NB01-30 tem cronograma próprio em 2027. Confira [bases e transições](../../../../BASES.md).

Descrição e observações do XSD adotado: Dados do ICMS Normal e ST. A estrutura e as restrições são as do pacote indicado; notas históricas de uso devem ser lidas junto às atualizações listadas nas referências.

## Estrutura, ordem e escolhas

![ICMS: estrutura do XSD](../../../../../diagramas/NFe/infNFe/det/imposto/ICMS.svg)

[Abrir o diagrama](../../../../../diagramas/NFe/infNFe/det/imposto/ICMS.svg).

Uma ocorrência `1..1` dentro de uma alternativa exige o campo quando essa alternativa é selecionada; não obriga selecionar esse ramo em toda nota. Grupos opcionais podem conter filhos obrigatórios. Os elementos de sequência seguem a ordem indicada.

- **Escolha exclusiva** `1..1`
  - `ICMS00` `1..1`
  - `ICMS02` `1..1`
  - `ICMS10` `1..1`
  - `ICMS15` `1..1`
  - `ICMS20` `1..1`
  - `ICMS30` `1..1`
  - `ICMS40` `1..1`
  - `ICMS51` `1..1`
  - `ICMS53` `1..1`
  - `ICMS60` `1..1`
  - `ICMS61` `1..1`
  - `ICMS70` `1..1`
  - `ICMS90` `1..1`
  - `ICMSPart` `1..1`
  - `ICMSST` `1..1`
  - `ICMSSN101` `1..1`
  - `ICMSSN102` `1..1`
  - `ICMSSN201` `1..1`
  - `ICMSSN202` `1..1`
  - `ICMSSN500` `1..1`
  - `ICMSSN900` `1..1`

## Campos e atributos

| Tag / atributo | Significado e observações | Tipo XML | Ocorrência | Formato / limites | Condição no contexto |
|---|---|---|---|---|---|
| [`ICMS00`](ICMS/ICMS00.md) | Tributação pelo ICMS 00 - Tributada integralmente | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS02`](ICMS/ICMS02.md) | Tributação monofásica própria sobre combustíveis | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS10`](ICMS/ICMS10.md) | Tributação pelo ICMS 10 - Tributada e com cobrança do ICMS por substituição tributária | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS15`](ICMS/ICMS15.md) | Tributação monofásica própria e com responsabilidade pela retenção sobre combustíveis | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS20`](ICMS/ICMS20.md) | Tributção pelo ICMS 20 - Com redução de base de cálculo | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS30`](ICMS/ICMS30.md) | Tributação pelo ICMS 30 - Isenta ou não tributada e com cobrança do ICMS por substituição tributária | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS40`](ICMS/ICMS40.md) | Tributação pelo ICMS 40 - Isenta 41 - Não tributada 50 - Suspensão | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS51`](ICMS/ICMS51.md) | Tributção pelo ICMS 51 - Diferimento. A exigência do preenchimento das informações do ICMS diferido fica à critério de cada UF. | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS53`](ICMS/ICMS53.md) | Tributação monofásica sobre combustíveis com recolhimento diferido | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS60`](ICMS/ICMS60.md) | Tributação pelo ICMS 60 - ICMS cobrado anteriormente por substituição tributária | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS61`](ICMS/ICMS61.md) | Tributação monofásica sobre combustíveis cobrada anteriormente; | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS70`](ICMS/ICMS70.md) | Tributação pelo ICMS 70 - Com redução de base de cálculo e cobrança do ICMS por substituição tributária | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMS90`](ICMS/ICMS90.md) | Tributação pelo ICMS 90 - Outras | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSPart`](ICMS/ICMSPart.md) | Partilha do ICMS entre a UF de origem e UF de destino ou a UF definida na legislação Operação interestadual para consumidor final com partilha do ICMS devido na operação entre a UF de origem e a UF do destinatário ou ou a UF definida na legislação. (Ex. UF da concessionária de entrega do veículos) | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSST`](ICMS/ICMSST.md) | Grupo de informação do ICMSST devido para a UF de destino, nas operações interestaduais de produtos que tiveram retenção antecipada de ICMS por ST na UF do remetente. Repasse via Substituto Tributário. | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSSN101`](ICMS/ICMSSN101.md) | Tributação do ICMS pelo SIMPLES NACIONAL e CSOSN=101 (v.2.0) | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSSN102`](ICMS/ICMSSN102.md) | Tributação do ICMS pelo SIMPLES NACIONAL e CSOSN=102, 103, 300 ou 400 (v.2.0)) | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSSN201`](ICMS/ICMSSN201.md) | Tributação do ICMS pelo SIMPLES NACIONAL e CSOSN=201 (v.2.0) | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSSN202`](ICMS/ICMSSN202.md) | Tributação do ICMS pelo SIMPLES NACIONAL e CSOSN=202 ou 203 (v.2.0) | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSSN500`](ICMS/ICMSSN500.md) | Tributação do ICMS pelo SIMPLES NACIONAL,CRT=1 – Simples Nacional e CSOSN=500 (v.2.0) | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |
| [`ICMSSN900`](ICMS/ICMSSN900.md) | Tributação do ICMS pelo SIMPLES NACIONAL, CRT=1 – Simples Nacional, CRT=4 - MEI e CSOSN=900 (v2.0) | `complexType anônimo` | `1..1` | Grupo estruturado | Obrigatório no contexto; apenas na alternativa selecionada |

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
<ICMS xmlns="http://www.portalfiscal.inf.br/nfe">
  <ICMSSN102>
    <orig>0</orig>
    <CSOSN>103</CSOSN>
  </ICMSSN102>
</ICMS>

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
- Filhos: [ICMS00](ICMS/ICMS00.md), [ICMS02](ICMS/ICMS02.md), [ICMS10](ICMS/ICMS10.md), [ICMS15](ICMS/ICMS15.md), [ICMS20](ICMS/ICMS20.md), [ICMS30](ICMS/ICMS30.md), [ICMS40](ICMS/ICMS40.md), [ICMS51](ICMS/ICMS51.md), [ICMS53](ICMS/ICMS53.md), [ICMS60](ICMS/ICMS60.md), [ICMS61](ICMS/ICMS61.md), [ICMS70](ICMS/ICMS70.md), [ICMS90](ICMS/ICMS90.md), [ICMSPart](ICMS/ICMSPart.md), [ICMSST](ICMS/ICMSST.md), [ICMSSN101](ICMS/ICMSSN101.md), [ICMSSN102](ICMS/ICMSSN102.md), [ICMSSN201](ICMS/ICMSSN201.md), [ICMSSN202](ICMS/ICMSSN202.md), [ICMSSN500](ICMS/ICMSSN500.md), [ICMSSN900](ICMS/ICMSSN900.md).

Anterior: [imposto](../imposto.md) | Próximo: [ICMS00](ICMS/ICMS00.md)
