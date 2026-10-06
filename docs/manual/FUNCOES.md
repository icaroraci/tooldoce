# Referência pública da API C

Referência da libnfe `1.0.0-rc5`. Os contratos abaixo reproduzem os tipos, assinaturas e comentários públicos dos headers desta revisão; funções `NFE_INTERNO` são excluídas. O comportamento específico prevalece sobre as convenções gerais. As bases históricas de conferência das páginas XML estão no [manifesto](schema-manifest.json).

Na rc5, seis getters de diagnóstico foram acrescentados em `validar.h`, descritos em [DIAGNOSTICOS.md](DIAGNOSTICOS.md).

[Manual](README.md) · [Memória e erros](API.md) · [Fluxo completo](EMISSAO.md)

| Header | Finalidade | Funções públicas |
|---|---|---|
| [assinatura.h](api/assinatura.md) | Carga de certificado A1, assinatura XML ou binária e verificação criptográfica. O certificado deve continuar existindo durante as operações que o usam; a conferência matemática não valida a cadeia ICP-Brasil nem a revogação. | 10 |
| [chave.h](api/chave.md) | Composição lexical da chave e cálculo/conferência do DV. As posições do CNPJ admitem letras maiúsculas no formato atualizado; preserve tamanho, posições e zeros. | 2 |
| [cnpjcpf.h](api/cnpjcpf.md) | Validação de documentos numéricos e CNPJ alfanumérico, com formatos e dígitos verificadores. Um documento válido matematicamente não comprova existência ou situação cadastral. | 2 |
| [cobr.h](api/cobr.md) | Fatura e duplicatas da cobrança, com valores e vencimentos. Estes dados não criam automaticamente formas de pagamento. | 6 |
| [decimal.h](api/decimal.md) | A aritmética decimal usada internamente preserva escala sem ponto flutuante binário; os limites, conversões e arredondamentos são os publicados no header. Confira a visibilidade das funções antes de usá-las externamente. | 0 |
| [defs.h](api/defs.md) |  | 0 |
| [dest.h](api/dest.md) | Identificação do destinatário, documentos, endereço e inscrições. Setters de identificação e ligação ao endereço têm efeitos específicos de escolha e transferência de posse. | 13 |
| [det.h](api/det.md) | Item da nota, produto, tributos, observações e referência por item. Um produto/imposto anexado pertence ao item; um item anexado pertence à nota. | 9 |
| [emit.h](api/emit.md) | Estabelecimento emitente, documento, endereço, regime e inscrições. CNPJ/CPF são validados matematicamente; cadastro e autorização fiscal são etapas distintas. | 14 |
| [endereco.h](api/endereco.md) | Endereço do emitente/destinatário, inclusive exterior nas condições permitidas. A serialização recebe um tipo para aplicar as exigências do grupo selecionado. | 14 |
| [erros.h](api/erros.md) | Códigos negativos e descrição de falhas locais da biblioteca. Não são códigos cStat do autorizador; funções que retornam quantidades ou dígitos têm contratos próprios. | 1 |
| [escrita.h](api/escrita.md) |  | 0 |
| [esquema.h](api/esquema.md) | ABI pública do motor de grupos e operações para tabelas geradas por bibliotecas de documentos. As tabelas internas da NF-e não são símbolos da API pública. | 6 |
| [evento.h](api/evento.md) | Montagem de cancelamento, cancelamento por substituição e carta de correção. Os documentos produzidos ainda precisam de assinatura, envio e avaliação do retorno. | 3 |
| [grupo.h](api/grupo.md) | Preenchimento por caminhos, escolhas, remoção e listas. Caminhos completos evitam ambiguidade; itens de lista pertencem ao grupo; getters retornam dados emprestados. | 6 |
| [ide.h](api/ide.md) | Identificação, chave, referências e dados das operações governamentais/antecipação. Os construtores fornecem padrões; campos sem padrão continuam obrigatórios. | 39 |
| [imposto.h](api/imposto.md) | Tributos do item, motor de grupos e atalhos mais usados. Formatos e presença são conferidos; o enquadramento e as contas continuam sob responsabilidade do emissor. | 17 |
| [infadic.h](api/infadic.md) | Informações ao Fisco/contribuinte, observações e processos. Cada tipo tem limites próprios; texto livre não substitui campos estruturados. | 10 |
| [inutilizacao.h](api/inutilizacao.md) | Montagem do pedido de inutilização para série e faixa numérica. Aceitação local do pedido não significa homologação da faixa pela SEFAZ. | 1 |
| [local.h](api/local.md) | Locais de retirada e entrega. O grupo recebe um endereço e passa a possuí-lo após sucesso; identificação e endereço não são substitutos do destinatário da nota. | 9 |
| [nfe.h](api/nfe.md) | Tipos, enumerações e códigos usados nos argumentos C. A tabela fiscal vigente e a enumeração da versão instalada são referências diferentes; valores presentes em uma norma futura podem ainda não existir na API. | 0 |
| [nfe_nfe.h](api/nfe_nfe.md) | Montagem da nota completa, integração dos filhos, chave, totais, escrita, arquivo e assinatura. A nota passa a possuir os grupos anexados após sucesso. | 24 |
| [padroes.h](api/padroes.md) | Padrões gerados dos tipos XSD para uso da implementação. Atualize pelo gerador; não edite o header gerado para ampliar artificialmente o leiaute aceito. | 0 |
| [pag.h](api/pag.md) | Formas de pagamento, dados eletrônicos e troco. Cada detPag transferido pertence a pag; o valor do pagamento não é recalculado pelo valor da fatura. | 15 |
| [prod.h](api/prod.md) | Produto/serviço, quantidades, valores, códigos e subgrupos especializados. Strings decimais mantêm precisão; o grupo genérico pertence ao produto. | 31 |
| [refNF.h](api/refNF.md) | Nota não eletrônica referenciada. Consulte a página editorial refNF para as diferenças entre domínio do XSD e setters legados. | 15 |
| [refNFe.h](api/refNFe.md) | Referência de NF-e por chave eletrônica. A API específica confere a chave/DV; não fornece automaticamente o fluxo de referência com sigilo. | 5 |
| [resptec.h](api/resptec.md) | Responsável técnico e hash CSRT já calculado. O setter do hash não calcula SHA-1 nem gera o segredo CSRT. | 5 |
| [sefaz.h](api/sefaz.md) | Mensagens, conexão SOAP/HTTPS, leitura de retorno e documentos processados. Retorno 0 indica êxito da operação local; a situação fiscal é informada pelo cStat adequado. | 18 |
| [total.h](api/total.md) | Totais de ICMSTot e demais grupos fiscais. A soma automática da nota não cobre automaticamente todos os novos tributos ou retenções. | 6 |
| [transp.h](api/transp.md) | Frete, transportador, veículo, reboques, volumes e lacres. Listas e seus subgrupos pertencem ao objeto de transporte. | 14 |
| [utils.h](api/utils.md) | Tipos e rotinas auxiliares da biblioteca. O contrato público exclui funções marcadas NFE_INTERNO; não use os símbolos ocultos como API de integração. | 0 |
| [valida.h](api/valida.md) | Validadores lexicais, domínios, datas e textos. O texto deve atender ao padrão e aos limites; validação local não consulta tabelas ou cadastros remotos. | 0 |
| [validar.h](api/validar.md) | Validação XSD, regras locais e diagnósticos automáticos com caminho, valor e restrição. Reaproveite o validador; as regras locais não cobrem todo o catálogo da SEFAZ. | 20 |
| [versao.h](api/versao.md) | Versão compilada e versão em execução da biblioteca. Registre também o pacote XSD e as normas aplicáveis, pois não são versões intercambiáveis. | 1 |
