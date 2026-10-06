# API C: valores, erros e propriedade dos objetos

[Manual](README.md) · [Referência por header](FUNCOES.md) · [Exemplos executáveis](EXEMPLOS.md)

A libnfe 1.0.0-rc4 oferece objetos específicos para os grupos usuais e um motor de grupos para as estruturas descritas em tabelas derivadas do XSD. Inclua somente os headers utilizados. A referência de cada módulo preserva os nomes, argumentos, limites e retornos efetivamente declarados; os contratos variam, inclusive na API histórica de `refNF`.

## Criação, empréstimo e transferência

| Situação | Propriedade e ação |
|---|---|
| `*_new` devolve objeto | Ponteiro NULL indica falha; quem cria libera com o `*_free` correspondente enquanto não transferir a posse |
| `nfe_nfe_set_ide`, `set_emit`, `set_total`, `set_transp`, `set_pag` e `add_det` | Em sucesso a nota assume a posse; não liberar o objeto novamente. Em falha, seguir o contrato do setter e liberar o objeto ainda próprio |
| Setter de grupo opcional específico da nota | Pode aceitar NULL para remover; consultar [nfe_nfe.h](api/nfe_nfe.md), sem estender essa possibilidade a setters obrigatórios |
| `nfe_prod_grupo`, `nfe_imposto_grupo`, `nfe_det_grupo`, `nfe_nfe_grupo` e similares | Ponteiro emprestado; liberar o proprietário, nunca o grupo em separado |
| `nfe_grupo_add` / `nfe_ide_add_nfref` | A nova ocorrência pertence ao grupo/ide; o retorno recebido é emprestado |
| `nfe_grupo_new` com esquema de outra biblioteca | Objeto independente; liberar com `nfe_grupo_free`. O esquema deve continuar válido durante seu uso |
| Texto devolvido por getter | Emprestado, salvo declaração explícita de alocação; pode perder validade depois de mutação ou liberação do proprietário |
| XML devolvido por `nfe_nfe_xml`, mensagens/eventos/inutilização/assinatura/proc | Buffer alocado; liberar com `free` e usar o tamanho em bytes fornecido |
| `nfe_sefaz_endereco` | URL estática; não alterar nem liberar |
| `nfe_sefaz_new(cert)` | A conexão empresta o certificado; destruir a conexão antes de liberar o certificado |

Objetos não são compartilhados implicitamente entre duas notas. Guarde em variáveis próprias apenas os objetos cuja posse ainda é sua. Após uma transferência bem-sucedida, zerar a variável local facilita a limpeza em um único caminho de saída. O [exemplo de montagem](../../examples/gerar_nfe.c) e o [programa de grupos](../../examples/manual_grupos.c) mostram criação e liberação reais.

## Caminhos do motor

Use caminhos **a partir do grupo recebido**, com os nomes XML exatos. No grupo de imposto, `ICMS/ICMS00/vBC` aponta para a base daquele ramo. No grupo de um item de lista, o caminho começa naquele item. Atributos usam o nome no contrato do motor, sem adicionar `@`. Não inclua `NFe/infNFe/...` quando o grupo já é `imposto`.

Desde rc4, um caminho abreviado só é aceito se identificar um único campo; a ambiguidade devolve `E_VALOR`, ou NULL nos getters. O caminho completo evita gravar um campo homônimo em outro pai. Para listas, `nfe_grupo_add` cria uma ocorrência, `nfe_grupo_item` recupera uma existente e `nfe_grupo_quantidade` informa sua quantidade. O índice de item do motor é o declarado no [contrato de grupo.h](api/grupo.md); não confundi-lo com `det/@nItem`, numerado a partir de 1.

`nfe_grupo_set` copia texto e valida as restrições do campo. NULL como valor remove o campo quando o contrato permitir. Um valor válido que seleciona outra alternativa de uma escolha apaga a anterior. A ordem do XML vem do esquema, não da ordem dos setters. A escrita confere obrigatoriedade e completude do ramo selecionado; um setter bem-sucedido não prova que o grupo já está completo.

## Textos, números e datas

CNPJ, CPF, chave, códigos e decimais são representações com significado lexical. Preserve zeros e maiúsculas; use `.` nos decimais e o número de casas do XSD. Zero não equivale a ausência. Os setters específicos podem receber inteiros/enumerações em vez de textos: consulte sua assinatura.

Tamanhos de texto livre são contados em caracteres UTF-8, mas o buffer C precisa de bytes e terminador. [defs.h](api/defs.md) oferece `NFE_TAM_UTF8` e `NFE_TAM_ASCII`. Não use `double` como armazenamento de valores fiscais que precisem conservar precisão. O somador interno de centavos não é uma função pública nem um calculador de todos os tributos.

As datas usam `time_t` e o fuso `nfe_tzd` nos módulos específicos; o motor recebe o texto lexical definido pelo XSD. Uma competência `gYearMonth` usa ano e mês, por exemplo `2026-09`, enquanto AAMM de refNF usa `2609`. Formatos de campos parecidos não são intercambiáveis.

## Retornos e tratamento

| Código da biblioteca | Significado |
|---|---|
| `E_ISNULL` (-1) | Argumento obrigatório nulo |
| `E_TAMANHO` (-2) | Texto ou buffer fora do tamanho permitido |
| `E_VALOR` (-3) | Valor, domínio, caminho ou documento inválido |
| `E_XML` (-4) | Falha de construção/leitura XML |
| `E_ARQUIVO` (-5) | Falha de arquivo |
| `E_REDE` (-6) | Falha HTTP/TLS/SOAP ou comunicação |
| `E_MALLOC` (-101) | Falha de alocação |

Use `nfe_strerror` para esses códigos. Algumas funções retornam quantidade, DV ou valor positivo; construtores retornam ponteiros. Não aplicar `rc == 0` a toda função sem ler seu contrato. cStat é um retorno fiscal da SEFAZ, em outra camada: envio com retorno local 0 pode trazer rejeição. A [lista de erros de validação](VALIDACAO.md) e o [retorno dos serviços](SERVICOS.md) explicam essa diferença.

Anterior: [Instalação](INSTALACAO.md) | Próximo: [Montagem da nota](EMISSAO.md)
