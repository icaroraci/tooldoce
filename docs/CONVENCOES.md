# Convenções de código

> Regras para todo código novo ou corrigido. O código antigo será adaptado aos poucos, pelas issues de cada fase.

## Nomes

| Elemento | Padrão | Exemplo |
|---|---|---|
| Funções públicas | `nfe_<grupo>_<ação>[_<campo>]`, minúsculas | `nfe_ide_new`, `nfe_ide_set_natop`, `nfe_ide_write_xml` |
| Tipos opacos | `typedef struct nfe_<grupo> nfe_<grupo>;` | `nfe_ide`, `nfe_endereco` |
| Enums | `enum nfe_<nome>` com typedef de mesmo nome | `nfe_uf`, `nfe_ambiente` |
| Constantes de enum | `NFE_<NOME>_<VALOR>`, maiúsculas | `NFE_UF_SP`, `NFE_AMBIENTE_HOMOLOGACAO` |
| Macros e constantes | `NFE_<NOME>`, maiúsculas | `NFE_TAM_CHAVE`, `NFE_FORMATO_DATA_HORA` |
| Códigos de erro | `NFE_E_<NOME>`, valores negativos | `NFE_E_ISNULL`, `NFE_E_TAMANHO` |
| Guardas de header | `LIBNFE_<ARQUIVO>_H` | `LIBNFE_IDE_H` |
| Funções internas | `static` no próprio arquivo; se usadas em mais de um arquivo, prefixo `nfe_` e marcadas com `NFE_INTERNO` (não exportadas) | `nfe_error` |

- Os nomes de campos seguem o nome da tag no leiaute oficial, em minúsculas nas funções (`natOp` → `nfe_ide_set_natop`) e com a grafia original dentro das structs (`natOp`).
- Termos em português, como no leiaute. Sem acentos em identificadores.
- Não usar o sufixo `_t` em tipos novos (reservado pelo POSIX).

## Objetos

Cada grupo do leiaute é um tipo opaco: a struct é definida só no `.c`, e o header expõe apenas o `typedef` e as funções.

```c
nfe_ide *nfe_ide_new(void);           /* NULL se faltar memória      */
void     nfe_ide_free(nfe_ide *ide);  /* aceita NULL                 */
int      nfe_ide_set_natop(nfe_ide *ide, const char *natop);
const char *nfe_ide_get_natop(const nfe_ide *ide);
int      nfe_ide_write_xml(xmlTextWriterPtr writer, const nfe_ide *ide);
```

- `_new` não recebe parâmetros: os campos começam com valores padrão e são preenchidos pelos setters.
- `_free` aceita `NULL` e libera também os objetos filhos que pertencem ao objeto.

## Retorno e erros

- Funções que podem falhar retornam `int`: `0` em caso de sucesso, ou um código negativo `NFE_E_*` de `erros.h`.
- Setters validam o valor (tamanho, faixa, formato) antes de gravar; se o valor for inválido, o objeto não é alterado.
- Getters de texto retornam `const char *` apontando para dentro do objeto (válido até o próximo setter ou o `_free`).
- **A biblioteca não imprime nada** (`printf`, `fprintf`, `perror`). Toda falha é comunicada pelo código de retorno.

## Textos e tamanhos

- Textos são UTF-8. Os limites do leiaute são em **caracteres**, e um caractere pode ocupar até 4 bytes.
- Para cada campo há uma macro em `defs.h` com o limite em caracteres; o buffer usa `NFE_TAM_UTF8(n)`.
- Cópias de texto sempre com limite de tamanho, nunca `strcpy`/`strcat`.

## Headers

- Headers públicos em `include/libnfe/`, incluídos como `<libnfe/arquivo.h>`.
- Cada header inclui tudo de que precisa e compila sozinho.
- `const` em todo ponteiro que a função não altera.

## Formatação

A formatação (tabs ou espaços, largura de linha) será definida com um `.clang-format` na issue #52. Até lá, siga o estilo do arquivo que estiver editando.
