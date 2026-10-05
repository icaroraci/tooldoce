# Como documentar o tooldoce e a libnfe

Este guia define o padrão para transformar os diagramas do XML em um manual de uso da biblioteca. Cada página deve permitir que um desenvolvedor entenda a finalidade do nó, saiba quando preenchê-lo, encontre os campos e as funções correspondentes e confira o XML produzido.

O diagrama permanece como parte central da explicação. A página deve reunir a imagem, a descrição do leiaute, as orientações de preenchimento e o uso real da API C.

## 1. Público e resultado esperado

Escreva para quem está integrando a libnfe a um emissor ou ERP. O leitor conhece programação, mas pode não conhecer o leiaute fiscal, as relações entre os grupos ou as convenções da biblioteca.

Uma página está completa quando responde:

- Para que serve este nó e em qual parte do documento aparece?
- Em quais situações ele deve ser informado, omitido ou substituído por outro grupo?
- Quais dados são necessários, em que formato e com quais limites?
- Como criar, preencher, integrar ao pai, serializar e liberar o grupo pela libnfe?
- Quais erros podem ocorrer e em qual etapa são detectados?
- Que XML resulta do exemplo?
- Onde consultar o grupo pai, os filhos, as alternativas e a definição oficial?

## 2. Organização das páginas

Use uma página Markdown para cada estrutura que já possui um diagrama. Documente os campos simples na página do grupo que os contém; crie páginas próprias para eles quando a explicação exigir mais espaço, mantendo links nos dois sentidos.

Organização proposta:

```text
docs/
  COMO_DOCUMENTAR.md
  manual/
    README.md
    NFe.md
    NFe/
      infNFe.md
      infNFe/
        ide.md
        ide/
          NFref.md
          NFref/
            refNF.md
  diagramas/
    README.md
    NFe.svg
    NFe/
      infNFe/
        ide/
          NFref/
            refNF.svg
```

O caminho da página deve espelhar o caminho XML. Preserve maiúsculas e minúsculas das tags. Dois nós com o mesmo nome em pais diferentes recebem páginas diferentes; podem compartilhar explicações por links quando isso fizer sentido.

Mantenha o texto editorial em `docs/manual/`. Preserve a regeneração completa da árvore de SVGs e do índice: ao atualizar os XSD, o gerador pode apagar e recriar essas saídas. Para um caminho XML que continua existindo, mantenha o nome e o local do SVG; assim, a página Markdown passa a exibir o diagrama atualizado pelo mesmo link. O texto editorial permanece salvo e recebe uma marca de obsolescência quando a estrutura documentada muda.

Cada página deve ter:

- título com a tag e seu significado;
- navegação por índice, ancestrais e grupo pai;
- links para filhos e alternativas;
- links anterior e próximo, quando houver uma ordem de leitura definida;
- diagrama incorporado com texto alternativo e link para abrir o SVG.

Use links relativos para navegar dentro do repositório. Calcule o caminho da imagem a partir da localização real do Markdown e confira sua renderização no GitHub. Links internos do SVG podem continuar levando a outros diagramas; a navegação textual entre páginas deve funcionar independentemente deles.

## 3. Índice do manual

`docs/manual/README.md` será a entrada principal. Inclua uma apresentação curta, as versões cobertas e caminhos de leitura para instalação, primeira emissão, consulta do leiaute e tratamento de erros.

Organize o sumário em duas partes:

1. **Uso da biblioteca:** instalação, dependências, convenções da API, montagem, validação, assinatura, transmissão, retorno, eventos e exemplos completos.
2. **Referência do XML:** árvore dos grupos na ordem do leiaute, com links para as páginas Markdown, descrições úteis e indicação de disponibilidade na biblioteca.

Apresente os grandes grupos em seções navegáveis. Use tabelas curtas para orientar a escolha entre estruturas, como as alternativas de referência de documentos. Uma lista extensa de tags deve ter contexto e pontos de entrada claros.

No índice atual de diagramas, acrescente links para o manual por meio do gerador. Para cada estrutura documentada, ofereça acesso à página completa e ao SVG. O README principal do projeto deve apontar para o manual e manter acesso aos diagramas.

Distinga os estados `documentado`, `obsoleto`, `em revisão` e `pendente` dos estados de implementação. Um grupo implementado pode ainda ter documentação incompleta. Uma página obsoleta tem conteúdo preservado, mas precisa de revisão após uma mudança na base documentada. Gere a cobertura a partir de um inventário verificável para evitar que o índice prometa páginas inexistentes.

## 4. Conteúdo obrigatório de cada grupo

### Identificação e finalidade

Informe a tag, o caminho XML completo, o documento/modelo aplicável, a versão do leiaute, o pacote de schemas usado e a versão da biblioteca conferida. Registre também a data da última revisão.

Explique a finalidade em linguagem direta. Diga a quem pertencem os dados: emitente da nota atual, destinatário, documento referenciado ou outro participante. Essa distinção deve aparecer também nas descrições dos campos.

### Quando preencher

Descreva as condições de uso, obrigatoriedade, proibições e alternativas. Para cada condição fiscal, identifique a regra oficial aplicável, sua versão e sua abrangência. Não conclua que um campo é dispensável só porque o XSD permite omiti-lo.

Quando o grupo participar de uma escolha, explique qual alternativa selecionar e quais combinações são incompatíveis. Informe onde ocorre a repetição: no próprio grupo, no pai ou em uma sequência intermediária.

### Estrutura e diagrama

Incorpore o SVG existente. Explique a ordem dos elementos, os atributos, as escolhas e as ocorrências relevantes. Indique se a cardinalidade mostrada depende de um grupo pai opcional ou de um ramo de escolha.

Uma ocorrência `1..1` dentro de uma alternativa não significa que esse nó aparece em todo documento. Preserve o contexto do XSD ao descrever obrigatoriedade.

### Campos e preenchimento

Liste todos os campos e atributos na ordem do XML. Use a tabela:

| Tag/atributo | Significado | Tipo XML | Ocorrência | Formato e limites | Condição de preenchimento |
|---|---|---|---|---|---|
| Nome exato | A informação representada | Tipo e restrições do XSD | Mínimo e máximo no contexto | Tamanho, padrão, escala ou domínio | Quando informar e a quem o dado pertence |

Para cada campo, explique os detalhes que a tabela não comporta: zeros à esquerda, separadores, casas decimais, arredondamento, datas e fusos, unidades, códigos permitidos, distinção entre ausência, vazio e zero, além de relações com outros campos.

Apresente enumerações com código e significado. Reproduza as restrições efetivas do tipo, incluindo as herdadas. Combine `pattern`, tamanho e demais facetas: uma descrição isolada de tamanho pode admitir valores que o padrão rejeita. Caracteres especiais da expressão regular devem ser escapados na tabela Markdown.

### API C correspondente

Use os nomes e as assinaturas que realmente existem nos headers da versão documentada. Não renomeie funções antigas para fazê-las parecer uniformes.

Inclua o header necessário e as funções de criação, preenchimento, leitura, ligação ao pai, serialização e liberação. Para cada função, documente:

- assinatura e propósito;
- argumentos, tipos C e sua relação com os campos XML;
- valores aceitos, conversões e normalizações;
- retorno e erros;
- comportamento diante de `NULL`, campos ausentes e valores inválidos;
- propriedade da memória, cópia ou empréstimo de dados e tempo de vida;
- estado do objeto após sucesso ou falha.

Separe a tabela dos campos XML da tabela de argumentos C. Um campo pode ser preenchido por vários argumentos, e um tipo C pode ter representação XML diferente. Se a biblioteca usar o motor genérico de grupos, mostre o caminho exato e as operações reais; se não houver suporte, declare a limitação.

### Exemplos C e XML

Forneça um exemplo mínimo que possa ser compilado na versão indicada. Ele deve incluir os headers, verificar os retornos e liberar recursos em todos os caminhos de saída. Mostre como o grupo entra no documento ou vincule um exemplo completo que faça essa integração.

Explique a propriedade da memória depois de adicionar um filho ao pai. Não omita a abertura e o fechamento dos elementos que ficam sob responsabilidade do chamador.

Mostre o XML correspondente ao mesmo exemplo, com campos na ordem correta e valores idênticos aos do código. Identifique se é um fragmento ou documento completo. Um fragmento deve ser validado dentro de um documento de teste adequado, com namespace e ancestrais necessários.

Para grupos repetíveis ou escolhas, acrescente um segundo exemplo quando ele esclarecer o uso. Use dados sintéticos, sem certificados, senhas ou dados reais. Validação de XSD não equivale a autorização pela SEFAZ; só declare homologação quando houver evidência específica.

### Validação e erros

Distinga três camadas: validação dos setters, validação estrutural pelo XSD e regras de negócio verificadas pela biblioteca ou pela SEFAZ.

| Situação | Onde é detectada | Código/retorno confirmado | Como corrigir |
|---|---|---|---|
| Condição reproduzível | Função, validador ou serviço | Código e significado | Correção concreta |

Informe quais verificações a biblioteca implementa e quais dependem de outra etapa. Não invente códigos de rejeição nem atribua a um setter uma verificação feita somente na serialização ou no servidor. Ausência de uma verificação implementada deve ser registrada como limitação.

### Referências e alterações

Vincule o XSD, seus tipos, os headers, a implementação, os testes e os exemplos pertinentes. Para regras fiscais, use documentos oficiais com identificação de versão, seção e vigência quando aplicável.

Registre mudanças que alterem o uso do grupo: campo incluído, formato modificado, nova função, comportamento corrigido ou suporte descontinuado. Diferencie versão do XML, pacote de schemas, versão da libnfe e data de aplicação da regra.

## 5. Apuração antes da escrita

Confira o conteúdo nesta ordem:

1. XSD adotado no projeto e todos os tipos referenciados: estrutura e restrições.
2. MOC 7.0, seus anexos e Notas Técnicas aplicáveis: significado, condições e regras fiscais, considerando as atualizações posteriores ao manual.
3. Headers: contrato público da API.
4. Implementação: comportamento efetivo e tratamento de erros.
5. Testes e exemplos: casos reproduzíveis e integração.
6. Changelog e versões publicadas: disponibilidade e mudanças.

Quando houver divergência, registre exatamente o que cada camada aceita. Não altere a descrição do leiaute para esconder uma limitação da implementação, nem apresente como disponível algo que só existe na especificação.

Em `refNF`, por exemplo, a documentação deve conferir separadamente o domínio de `mod` no schema e o que `RefNFSetmod` aceita. Também deve explicar a transformação dos argumentos de `RefNFSetAAMM` no texto XML e a responsabilidade do chamador por `<NFref>`, indicada no header. Esses pontos exigem leitura do código e exemplos; a imagem sozinha não resolve a integração.

### Uso do Manual de Orientação ao Contribuinte

Use o [Manual de Orientação ao Contribuinte — MOC 7.0 — NF-e e NFC-e](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=LrBx7WT9PuA=), seus anexos e as Notas Técnicas pertinentes como base para a explicação funcional dos nós. O XSD fornece a estrutura; a documentação deve apurar também o significado dos dados, as condições de preenchimento e as regras de validação nos documentos oficiais correspondentes.

O PDF conferido é o volume **MOC 7.0 — Visão Geral**, versão 7.00 de novembro de 2020, com 150 páginas. A seção 1, na página 16, identifica os documentos que compõem o MOC. Selecione a referência conforme o assunto:

| Documento | Aplicação na documentação |
|---|---|
| Visão Geral | Conceitos, padrões técnicos, comunicação, serviços e fluxos operacionais |
| Anexo I — Leiaute NF-e/NFC-e e Regras de Validação | Referência dos grupos e campos e das regras de validação |
| Anexo II — Especificações Técnicas do DANFE e Código de Barras | Assuntos de representação do DANFE e código de barras |
| Anexo III — Manual de Contingência NF-e | Fluxos de contingência do modelo 55 |
| Anexo IV — Manual de Contingência NFC-e | Fluxos de contingência do modelo 65 |
| Manual de Especificações Técnicas do DANFE NFC-e e QR Code | DANFE NFC-e e QR Code, em documento próprio mencionado pela Visão Geral |

Para documentar cada nó, consulte especialmente o Anexo I e suas atualizações. Não atribua ao PDF da Visão Geral tabelas de campos que pertencem a outro volume. Registre separadamente os documentos efetivamente consultados; a consulta da Visão Geral não significa que os anexos foram conferidos.

A seção 4.2.7, na página 56 da Visão Geral, explica as colunas das tabelas de leiaute: referência da tag, campo, categoria de elemento, pai, tipo, ocorrência, tamanho e descrição/observação. Use essa identificação para relacionar a tabela oficial à página do nó. Preserve a distinção entre grupo, elemento e alternativas exclusivas, e diferencie o tipo apresentado no MOC do tipo XSD e do tipo C da API.

Para cada página:

- Localize o grupo e os campos no leiaute oficial e registre seus identificadores quando disponíveis.
- Relacione cada orientação de preenchimento à seção ou tabela que a fundamenta.
- Identifique regras de validação aplicáveis, suas condições, exceções, abrangência por modelo e códigos de rejeição confirmados.
- Confira as Notas Técnicas posteriores que alteram o grupo, o campo ou a regra e registre a versão e a vigência aplicáveis.
- Compare essas definições com o XSD adotado e com o comportamento efetivo da libnfe.

Use uma tabela de rastreabilidade nas páginas que tenham regras ou atualizações relevantes:

| Campo/grupo ou regra | Documento oficial | Localização | Atualização aplicável | Situação na libnfe |
|---|---|---|---|---|
| Tag, identificador ou regra | MOC/anexo/NT e versão | Seção, tabela, identificador e página quando útil | NT, versão e vigência, ou nenhuma identificada na revisão | Implementada, parcial, não implementada ou não aplicável |

O MOC 7.0 deve ser lido junto às atualizações aplicáveis ao pacote de schemas documentado. Quando uma Nota Técnica modificar uma definição anterior, explique a definição válida para a base escolhida e identifique a atualização. Não trate a data de publicação de um documento como data automática de entrada em vigor.

O estado de revisão da página deve considerar também essa base normativa. Registre quais versões de MOC, anexos e Notas Técnicas foram conferidas. Uma atualização de regra ou orientação pode tornar o texto obsoleto mesmo sem mudança no XSD; mantenha um inventário dessas dependências e uma revisão das páginas afetadas quando surgirem atualizações oficiais.

Se uma referência não puder ser consultada, registre a pendência e não atribua a ela regras, seções ou códigos ainda não conferidos.

## 6. Geração automática e texto editorial

Gere automaticamente o inventário de caminhos, relações entre pais e filhos, ordem, ocorrências, tipos, facetas e links dos diagramas. Escreva e revise as orientações de uso, regras fiscais, exemplos e explicações da API.

Mantenha saídas geradas e textos revisados em locais separados. A cada atualização dos XSD, apague e recrie os diagramas e seus índices nos mesmos caminhos. Preserve o conteúdo editorial e atualize seu estado de revisão. Se combinar conteúdo em uma página, estabeleça blocos gerados claramente delimitados ou uma etapa de montagem a partir de fontes separadas.

Atualize `tools/gerar_diagramas.py` para produzir os links do índice de diagramas para o manual. Preserve o modo padrão e o suporte a `--config` para os outros documentos. O inventário deve identificar nós novos, removidos e alterados, sem excluir automaticamente explicações antigas: páginas obsoletas precisam de revisão e indicação de substituição.

### Detecção de mudanças e obsolescência

Mantenha um inventário persistente fora da árvore apagada pelo gerador, por exemplo em `docs/manual/schema-manifest.json`. Para cada página, registre o caminho XML, a versão ou identificação do pacote XSD, uma assinatura da estrutura atual e a assinatura da estrutura efetivamente revisada pelo autor. A regeneração atualiza a assinatura atual; somente a revisão da página atualiza a assinatura revisada.

Calcule a assinatura sobre uma representação normalizada da estrutura resolvida, incluindo campos, atributos, ordem, escolhas, ocorrências, tipos, facetas herdadas, valores padrão/fixos e anotações do XSD usadas na documentação. Resolva dependências de tipos e de arquivos incluídos/importados: uma mudança em um tipo compartilhado pode afetar várias páginas sem alterar a declaração local do nó. Ignore mudanças puramente de formatação, comentários sem conteúdo documental e prefixos de namespace equivalentes.

Compare a estrutura nova com a base revisada de cada página:

| Resultado | Ação sobre o diagrama | Ação sobre a página |
|---|---|---|
| Nó existente sem mudança relevante | Recriar no mesmo nome e local | Manter o estado de revisão |
| Nó existente alterado | Recriar no mesmo nome e local | Marcar como `obsoleto` e listar as diferenças |
| Nó novo | Criar no caminho correspondente | Registrar documentação `pendente` |
| Nó removido | Remover da árvore atual | Preservar a página e marcar como `obsoleto — nó removido` |
| Nó movido ou renomeado | Criar no novo caminho e remover o antigo | Tratar como remoção e inclusão; vincular a substituição após confirmação |

Inclua as dependências que a página efetivamente descreve, como o contexto de ocorrência no pai e os subgrupos usados em exemplos. Uma mudança em um filho deve marcar também as páginas cujo conteúdo depende dele. Esse controle pode ser conservador, mas deve informar o motivo de cada marcação.

Exiba o aviso no início da página e o estado no índice. O aviso deve informar a base revisada, o novo pacote XSD, o motivo e os campos ou grupos afetados. Exemplo de texto:

> **Documentação obsoleta — revisão necessária.** O diagrama apresenta o XSD atual, mas o texto e os exemplos foram revisados para uma base anterior. Mudanças detectadas: indicar os campos e as restrições alterados. Consulte as versões identificadas abaixo antes de usar os exemplos.

Para um nó removido, substitua a incorporação do SVG atual por uma indicação de remoção e, quando disponível, um link para o diagrama histórico em um commit ou tag. Preserve a explicação anterior como referência identificada por versão. Não deixe uma imagem quebrada nem sugira que o nó ainda integra o leiaute atual.

Não retire a marca de obsolescência só porque o gerador foi executado novamente. O estado só volta a `documentado` depois de conferir o texto, a API, os exemplos e o XML contra a nova base e registrar a assinatura revisada. Em uma primeira implantação sem base revisada registrada, marque a página como `pendente de conferência`; não presuma que ela está atualizada.

Gere um relatório de mudanças com páginas afetadas e motivos para orientar a revisão. Atualize inventário, diagramas e estados somente após uma geração bem-sucedida; uma falha não deve registrar a documentação como sincronizada. Mudanças de regras fiscais ou da API que não alterem o XSD precisam de revisão própria, pois esse mecanismo não consegue detectá-las apenas pelo schema.

Não transforme todos os textos do XSD em páginas consideradas completas. A geração fornece a base estrutural; publicação como documentação revisada depende do conteúdo e das verificações deste guia.

## 7. Modelo de página

Copie esta estrutura e substitua todas as instruções por conteúdo conferido antes de publicar. Em páginas simples, reúna seções curtas; mantenha todas as informações aplicáveis.

```markdown
# TAG — significado do grupo

Índice > documento > ancestrais > TAG
(Cada item da navegação deve ser um link.)

| Informação | Valor |
|---|---|
| Caminho XML | Caminho completo |
| Documento/modelo | Aplicabilidade conferida |
| Leiaute e pacote XSD | Versões exatas |
| Estado da documentação | Documentado, obsoleto, em revisão ou pendente |
| Base XSD revisada | Identificação e assinatura registrada |
| Base XSD atual | Identificação usada no diagrama atual |
| Base normativa revisada | MOC, anexos e Notas Técnicas, com versões |
| Biblioteca conferida | Tag ou commit |
| Última revisão | AAAA-MM-DD |

## Finalidade

Explique a informação representada e sua relação com o documento.

## Quando preencher

Descreva condições, alternativas, repetição e regras aplicáveis.

## Estrutura

Incorpore o SVG por caminho relativo e ofereça link para abri-lo.
Explique sequência, escolha, atributos e ocorrências.

## Campos

| Tag/atributo | Significado | Tipo XML | Ocorrência | Formato e limites | Condição |
|---|---|---|---|---|---|

Acrescente domínios e orientações de preenchimento.

## API da libnfe

Header, assinaturas reais, argumentos, retornos e propriedade da memória.
Mapeamento entre funções e campos; integração com o pai.

## Exemplo em C

Código compilável, com tratamento de erros e liberação de recursos.
Link para o arquivo de exemplo e instrução de compilação.

## XML produzido

XML correspondente ao código; identifique fragmento ou documento completo.

## Validação e erros

| Situação | Onde é detectada | Código/retorno | Como corrigir |
|---|---|---|---|

Declare as limitações confirmadas.

## Grupos relacionados

Links para o pai, filhos e alternativas, com explicação da relação.

## Alterações e referências

Histórico relevante, XSD, documentos oficiais, código e testes.

Anterior: link | Grupo pai: link | Próximo: link
```

## 8. Revisão e critérios de conclusão

Antes de marcar uma página como documentada:

- [ ] Todas as tags, atributos, tipos e ocorrências foram conferidos no XSD.
- [ ] As condições de uso têm base oficial identificável.
- [ ] O MOC, os anexos pertinentes e as Notas Técnicas aplicáveis foram conferidos e suas versões registradas.
- [ ] A página distingue restrições do leiaute e comportamento da biblioteca.
- [ ] Assinaturas, retornos e propriedade da memória correspondem ao código.
- [ ] O exemplo C compila e executa na versão declarada.
- [ ] O XML apresentado corresponde à saída e valida no contexto adequado.
- [ ] Escolhas e repetições têm exemplos quando necessários.
- [ ] Erros e limitações são descritos sem afirmações não verificadas.
- [ ] A imagem e os links funcionam na página renderizada no GitHub.
- [ ] O índice aponta para a página completa e informa sua situação.
- [ ] Regenerar os diagramas preserva todo o conteúdo editorial.
- [ ] Uma mudança relevante no XSD marca as páginas afetadas como obsoletas, com motivo visível.
- [ ] A regeneração mantém nomes e caminhos dos diagramas dos nós existentes.
- [ ] Nós removidos não deixam imagens quebradas nas páginas preservadas.
- [ ] A base revisada só é atualizada depois da conferência do texto e dos exemplos.
- [ ] Não há instruções de preenchimento, exemplos fictícios de API ou pendências escondidas no texto publicado.

Automatize a checagem de arquivos e links locais, cobertura do inventário, compilação dos exemplos e validação XML. A revisão humana deve conferir a clareza, o contexto fiscal e a correspondência entre o texto e a implementação.

## 9. Ordem de adoção

Comece com `NFref` e `refNF` para validar o padrão: página do pai, página do filho, campos, escolha, API, exemplo C e XML, navegação e referências. Use essa primeira entrega como referência interna revisada.

Depois, documente um fluxo completo de montagem: identificação, emitente, destinatário, item, produto, tributos, totais e pagamento. Em seguida, amplie para os grupos especializados e para os fluxos de validação, assinatura, autorização e eventos.

Cada entrega deve incluir páginas utilizáveis, exemplos verificados e atualização do índice. A cobertura deve crescer sem comprometer a precisão das páginas já publicadas.
