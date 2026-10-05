# Exemplos C e XML: reprodução e limites

[Manual](README.md) · [Índice XML](INDICE.md) · [Validação](VALIDACAO.md)

Os programas são compilados contra a biblioteca. Dados fiscais são sintéticos: comprovam montagem/estrutura, sem representar cálculo fiscal completo, assinatura ICP-Brasil real ou autorização. Não escolher enquadramento tributário a partir de valores de uma fixture.

## Programas em examples

```sh
make exemplos
```

| Fonte | Execução e finalidade |
|---|---|
| [gerar_ide.c](../../examples/gerar_ide.c) | ./obj/gerar_ide: identificação isolada |
| [gerar_nfe.c](../../examples/gerar_nfe.c) | ./obj/gerar_nfe: NFC-e sintética completa |
| [nfe_ibscbs.c](../../examples/nfe_ibscbs.c) | ./obj/nfe_ibscbs: NF-e de regime normal com RTC e totais informados pela aplicação |
| [assinar_nfe.c](../../examples/assinar_nfe.c) | ./obj/assinar_nfe tests/certificados/teste.pfx teste: assinatura local com certificado de teste |
| [status_sefaz.c](../../examples/status_sefaz.c) | Consulta real com certificado, senha, URL e UF fornecidos. Acessa a rede; seguir [serviços](SERVICOS.md) |
| [referenciar_nf.c](../../examples/referenciar_nf.c) | Sem opção: ide com referências; --generico: mesma saída pelo motor; --isolado: NFref pela API histórica |
| [manual_grupos.c](../../examples/manual_grupos.c), [casos.inc](../../examples/manual/casos.inc) | 83 cenários genéricos; argumento é o caminho XML completo |
| [manual_servicos.c](../../examples/manual_servicos.c) | Mensagens sintéticas, sem certificado/rede; opções abaixo |

Os programas conferem operações necessárias e liberam recursos. Os cenários de grupos usam somente API pública e objetos reais, sem acessar tabelas internas esq_*.

## Os 171 exemplos de estruturas

O [catálogo](exemplos/catalogo.json) relaciona cada caminho à fixture, fonte, função, linha, programa, argumentos, raiz e schema/contexto. A assinatura SHA-256 da fonte normaliza quebras de linha e detecta mudanças no código que produziu o exemplo.

83 caminhos usam manual_grupos. Os outros 88 se apoiam em cenários existentes dos testes, cujo XML foi capturado após validação bem-sucedida. Os recortes nas páginas mostram chamadas da função identificada; declaração de variáveis, tratamento e limpeza estão no fonte completo. O teste imprime verificações, não necessariamente o XML; a fixture contém o XML observado naquele cenário.

```sh
./obj/manual_grupos NFe/infNFe/det/imposto/ICMS/ICMS02
make obj/test_prod
./obj/test_prod tests
```

O XML é conferido no schema que declara sua raiz ou numa NFe completa para grupos sem raiz global. [Contextos](exemplos/contextos) contém essas notas de apoio. O fragmento da página é comparado com a fixture, incluindo namespace, ordem, atributos e textos.

NFref/refNF preservam os exemplos da primeira referência interna: [ide-referencias.xml](exemplos/ide-referencias.xml) e [nfref-isolado.xml](exemplos/nfref-isolado.xml). Seus modos específico/genérico/isolado continuam sendo executados e comparados.

## Mensagens e documentos de operação

```sh
./obj/manual_servicos status
./obj/manual_servicos recibo
./obj/manual_servicos consulta
./obj/manual_servicos cancelamento
./obj/manual_servicos cce
./obj/manual_servicos inutilizacao
```

| Opção | XML publicado | Uso na aplicação |
|---|---|---|
| status | [consStatServ](exemplos/servicos/status.xml) | Endpoint de status correto |
| recibo | [consReciNFe](exemplos/servicos/recibo.xml) | Recibo real do lote |
| consulta | [consSitNFe](exemplos/servicos/consulta.xml) | Chave real a consultar |
| cancelamento | [evento](exemplos/servicos/cancelamento.xml) | Conferir condições, assinar e montar lote |
| cce | [evento](exemplos/servicos/cce.xml) | Conferir correção/seq., assinar e montar lote |
| inutilizacao | [inutNFe](exemplos/servicos/inutilizacao.xml) | Conferir faixa, assinar e enviar |

Chave e protocolos são sintéticos; não transmitir fixtures como pedidos reais. Para assinatura, lotes e processados, veja [test_evento.c](../../tests/test_evento.c), [test_inutilizacao.c](../../tests/test_inutilizacao.c) e [test_sefaz.c](../../tests/test_sefaz.c).

## Conferência automática

```sh
make test
make exemplos
python3 tests/verificar_manual.py
```

A verificação do manual cobre exemplos novos, XML/XSD, fontes, links, API e alterações simuladas na base. Os testes da biblioteca incluem casos válidos/inválidos de cada módulo; aprovação dos testes não significa autorização dos exemplos pela SEFAZ.

Anterior: [Contingência](CONTINGENCIA.md) | Próximo: [Ferramentas](FERRAMENTAS.md)
