# Validação estrutural, regras locais e autorização

[Manual](README.md) · [validar.h](api/validar.md) · [Bases oficiais](BASES.md)

O XSD é a referência da estrutura. Ele confere namespace, nomes, ordem, cardinalidade, escolhas, tipos e formatos; regras fiscais dependem também de contexto, cálculos, cadastros e vigência. A libnfe acrescenta verificações locais específicas. A SEFAZ aplica o conjunto de regras do autorizador e decide sobre a autorização.

## Escolher o validador

| API | Uso |
|---|---|
| `nfe_validador_new(NULL)` | Carrega os XSDs da NF-e instalados no caminho de `nfe_dir_schemas()` |
| `nfe_validador_new("tests/schemas/nfe")` | Usa explicitamente a base da nota no checkout |
| `nfe_validar_xml` | Exige NFe; confere XSD e, se estruturalmente válido, as regras locais listadas abaixo |
| `nfe_validador_xsd(caminho)` | Carrega o schema completo de outra mensagem/documento |
| `nfe_validar_xsd` | Confere apenas o schema carregado, sem impor NFe ou aplicar as regras locais da nota |

Reutilize o validador carregado; libere-o com `nfe_validador_free`. Crie uma lista com `nfe_erros_new` e libere com `nfe_erros_free`. Cada validação limpa a lista anterior. Getters de mensagens/campos devolvem texto pertencente à lista; copie-o se precisar guardá-lo depois de limpar ou destruir a lista.

Uma nota não assinada pode ser validada como se tivesse a estrutura de assinatura. Em `nfe_validar_xsd`, o argumento `completar_assinatura` controla esse apoio. Isso não assina o documento nem verifica criptografia; para mensagens não assinadas, use 0. Use [assinatura.h](api/assinatura.md) para verificar uma assinatura real.

## Regras locais presentes na rc4

O [contrato](../../include/libnfe/validar.h), a [implementação](../../src/libnfe/validar.c) e o [teste](../../tests/test_validar.c) delimitam a cobertura:

| Conferência | Códigos cStat associados pela biblioteca |
|---|---|
| DV da chave / correspondência com os campos da nota | 236 / 502 |
| Soma de produtos com indTot=1 | 564 |
| Soma de vBC, vICMS, vBCST e vST | 531, 532, 533, 534 |
| Fórmula de vNF, com as alternativas previstas de vICMSDeson | 610 |
| UF e município do emitente/fato gerador | 0: regra sem código único no contrato |
| Forma de emissão e dados de contingência | 556, 557, 570, 711, 714, 783 |
| Faixa de série do contribuinte/Fisco | 244, 451 |
| Indicativo de intermediador | 434, 435 |
| NF-e 55: tipo de DANFE e presença | 710, 794 |
| NFC-e 65: saída, destino, referências, DANFE, finalidade, consumidor e presença | 706, 707, 708, 709, 715, 716, 717 |

A lista não inclui todas as regras de RTC, GTIN, CFOP, cadastro ou as NT publicadas em outubro de 2026. `nfe_nfe_calcular_totais` também não calcula IBSCBSTot ou todos os tributos. As regras novas e suas datas são tratadas em [BASES.md](BASES.md).

## Interpretar erros

`nfe_validar_xml` retorna 0 quando as verificações presentes passam, `E_VALOR` se o XML falhar no XSD ou numa regra local, `E_XML` para documento malformado/raiz inadequada, e os demais códigos indicados no contrato. Para cada índice entre 0 e `nfe_erros_qtd(erros)-1`, leia mensagem, campo, linha e código. Linha 0/campo NULL significam informação indisponível; código 0 pode indicar erro de schema, sem cStat único.

Erros XSD normalmente vêm da libxml2 em inglês; regras locais têm mensagens em português. Trate o retorno antes de confiar na saída. Uma falha lexical pode impedir as verificações de regras seguintes: corrija a estrutura e valide novamente.

Os [diagnósticos automáticos](DIAGNOSTICOS.md) acrescentam caminho, valor recebido, faceta e padrão/lista/limite esperado, quando disponíveis. Essas informações vêm do validador XSD, sem uma mensagem específica para cada tag. `nfe_erros_codigo_xml` e `nfe_erros_dominio_xml` identificam o erro nativo da libxml2; `nfe_erros_codigo` continua reservado às associações locais com cStat. Para erros sem faceta estruturada, exiba `nfe_erros_msg`.

## Exemplos e reprodução

O [teste completo](../../tests/test_validar.c) exercita XML válido e rejeições deliberadas, incluindo chave e totais. Os exemplos de cada estrutura têm XML proveniente de C, fonte e cenário identificados no [catálogo](exemplos/catalogo.json). Para grupos sem elemento global, a conferência usa o pai oficial ou uma NFe completa; não basta validar uma tag isolada sem contexto.

```sh
make exemplos
make obj/test_validar
./obj/test_validar tests
python3 tests/verificar_manual.py
```

Os dados dos exemplos são sintéticos. Valores de assinatura de apoio, inclusive o grupo de PAA, satisfazem formato/estrutura e não provam validade criptográfica. A lista de testes da documentação confere isso separadamente da aprovação fiscal.

Anterior: [Emissão](EMISSAO.md) | Próximo: [Assinatura](ASSINATURA.md)
