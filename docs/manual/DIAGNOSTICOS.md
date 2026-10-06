# Diagnósticos automáticos do XSD

[Manual](README.md) · [Validação](VALIDACAO.md) · [Contrato da API](api/validar.md)

Disponível a partir da libnfe **1.0.0-rc5**.

Quando o XML é inválido, a aplicação pode exibir tag, caminho, linha, valor recebido e a restrição que falhou. A libxml2 continua sendo o validador: a biblioteca copia seu erro estruturado e os dados do nó antes de liberar o documento. O padrão, conjunto enumerado ou limite vem do schema efetivamente carregado, incluindo tipos herdados, referências, includes e imports. Não existe uma tabela de erros por tag nem uma segunda implementação de validação.

## Dados disponíveis

| Getter em `validar.h` | Informação |
|---|---|
| `nfe_erros_campo` / `nfe_erros_linha` | Nome local da tag e linha; NULL/0 se indisponíveis |
| `nfe_erros_caminho` | Caminho absoluto para exibição; prefixos do XML e índices de irmãos repetidos |
| `nfe_erros_valor` | Conteúdo recebido; string vazia significa valor vazio, NULL significa indisponível |
| `nfe_erros_restricao` | Faceta violada, por exemplo `pattern`, `enumeration`, `maxLength` |
| `nfe_erros_esperado` | Expressão XSD, lista enumerada ou limite fornecido no diagnóstico |
| `nfe_erros_msg` | Mensagem original completa, preservada inclusive quando não há faceta estruturada |
| `nfe_erros_dominio_xml` / `nfe_erros_codigo_xml` | Domínio e código nativos da libxml2; não são cStat |
| `nfe_erros_codigo` | Associação existente com rejeição SEFAZ nas regras locais; 0 para erros XSD |

Os retornos das funções permanecem `0`, `E_VALOR`, `E_XML`, `E_ISNULL` e `E_MALLOC`, conforme seu contrato. Os seis getters novos acrescentam informações à lista opaca sem alterar a assinatura de validação. Eles retornam NULL/0 para uma lista nula ou índice inexistente. Todos os textos pertencem à lista e deixam de ser válidos quando ela é limpa, liberada ou reutilizada numa validação.

O conteúdo do nó já passou pelo parser XML: entidades e normalização XML não preservam byte a byte o arquivo original. Em nós escalares identificados sem atributos, mantém-se o texto anterior à normalização XSD. Quando falta o nó escalar ou o pai pode representar um atributo, prefere-se o valor da faceta, que pode estar normalizado pelo tipo XSD; não se atribui o texto do pai ao erro de um atributo. Grupos com filhos não são concatenados para produzir um suposto valor escalar.

## Facetas e erros estruturais

O adaptador reconhece as classes `pattern`, `enumeration`, `length`, `minLength`, `maxLength`, `minInclusive`, `maxInclusive`, `minExclusive`, `maxExclusive`, `totalDigits` e `fractionDigits`. Essas classes são genéricas: servem para qualquer tag e qualquer schema aceito pelo validador. A expressão regular permanece na sintaxe do XML Schema, sem ser convertida numa máscara inventada. A lista enumerada usa a representação textual da libxml2, que pode conter aspas e vírgulas.

Para ordem, escolha, elemento ausente, tipo composto ou uma classe ainda não reconhecida, `restricao` e `esperado` podem ser NULL. Nesse caso, exiba a mensagem original: ela inclui a estrutura esperada quando a libxml2 a informa. Não se deduz uma lista a partir da mensagem, não se confunde uma alternativa de `choice` com obrigatoriedade simultânea e não se cria um valor para um campo ausente. Corrija os problemas e valide novamente; um erro estrutural pode impedir diagnósticos posteriores.

Algumas versões da libxml2 indicam o elemento pai de um atributo no callback. Nesses casos, campo/caminho representam o pai; o atributo é identificado na mensagem original, e o valor/limite pode vir dos argumentos estruturados da faceta. Quando o nó atributo é fornecido, o caminho usa `/@nome`. A biblioteca não adivinha o atributo pelo valor nem interpreta frases em inglês para descobrir seu nome. O caminho é um identificador para exibição, não uma expressão XPath independente dos namespaces.

Regras fiscais locais continuam com suas mensagens e associações de cStat; caminho/valor são acrescentados quando há um nó identificado, mas não se atribui a elas uma restrição XSD. Uma falha ao copiar os diagnósticos retorna `E_MALLOC`, mantendo a lista liberável.

## Exemplo reproduzível com refNF

O [programa C](../../examples/diagnosticar_xml.c) carrega os arquivos informados, valida apenas contra o XSD e mostra os dados de todos os erros. A [fixture inválida](exemplos/ide-refnf-invalido.xml) conserva o contexto oficial de `ide`, pois `refNF` não é um elemento global desse schema. Os dados são sintéticos.

```sh
make exemplos
./obj/diagnosticar_xml tests/schemas/nfe/tipos_v4.00.xsd docs/manual/exemplos/ide-refnf-invalido.xml
```

O programa termina com código 1 e mostra quatro problemas, com retorno local `E_VALOR` (-3):

| Tag | Recebido | Faceta | Esperado pelo XSD PL010f v1.04 |
|---|---|---|---|
| AAMM | `202610` | pattern | `[0-9]{2}[0]{1}[1-9]{1}\|[0-9]{2}[1]{1}[0-2]{1}` |
| mod | `55` | enumeration | `'01', '02'` |
| serie | `01` | pattern | `0\|[1-9]{1}[0-9]{0,2}` |
| nNF | `0` | pattern | `[1-9]{1}[0-9]{0,8}` |

Por exemplo, o erro de `mod` mostra o caminho `/ide/NFref[1]/refNF/mod`, a linha do campo e o diagnóstico original. Troque, somente dentro de `refNF`, AAMM por `2610`, mod por `01`, serie por `1` e nNF por `10`: a fixture passa no XSD. Isso não significa autorização da SEFAZ.

A biblioteca não imprime nada; a exibição é responsabilidade da aplicação. O exemplo escapa controles e quebras de linha para o terminal. Em HTML, use `textContent` ou escape os valores para o contexto de saída; não insira o texto recebido como marcação.

## Atualização do schema e verificação

Um validador mantém o schema compilado que carregou na criação. Depois de instalar novos XSDs, crie outro validador para usar a nova base; não é necessário alterar um catálogo de campos ou mensagens. Os diagnósticos refletem as restrições que efetivamente falharam, não uma descrição completa de todas as restrições do tipo.

[test_diagnostico.c](../../tests/test_diagnostico.c) reproduz os quatro erros de refNF numa NFe completa e confere tipos importados/herdados, alteração de padrão em outro XSD, limites numéricos e UTF-8, atributos, escolhas incompletas, prefixos, índices, limpeza/reutilização e validade dos dados depois de liberar o XML. Execute `make obj/test_diagnostico` e `./obj/test_diagnostico tests`.

Referências técnicas: [erro estruturado da libxml2](https://gnome.pages.gitlab.gnome.org/libxml2/html/struct__xmlError.html) e [validação XSD e callbacks](https://gnome.pages.gitlab.gnome.org/libxml2/html/xmlschemas_8h.html). A API usa os argumentos nativos das facetas e preserva a mensagem como alternativa para dados não fornecidos ou classes não reconhecidas.
