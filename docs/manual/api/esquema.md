# esquema.h — ABI pública do motor de grupos e operações para tabelas geradas por bibliotecas de documentos. As tabelas internas da NF-e não são símbolos da API pública.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/esquema.h>`. ABI pública do motor de grupos e operações para tabelas geradas por bibliotecas de documentos. As tabelas internas da NF-e não são símbolos da API pública.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/xmlwriter.h>

#include <libnfe/grupo.h>
#include <libnfe/utils.h>

/*
 * Motor genérico de grupos do leiaute.
 *
 * A estrutura de um grupo (elementos, sequências e escolhas do XSD, com as
 * regras de cada campo) vem de tabelas geradas por tools/gerar_esquemas.py
 * a partir dos schemas oficiais do documento. Um nfe_grupo guarda os
 * valores dos campos (folhas) de uma dessas estruturas, valida cada valor
 * ao gravar e escreve o XML na ordem do leiaute, conferindo campos
 * obrigatórios e escolhas.
 *
 * Campos são indicados por caminho: nomes de elementos separados por "/",
 * que devem aparecer, nessa ordem, no caminho do campo a partir da raiz
 * (ex.: "ICMS10/vBC", ou só "vBC" quando não há ambiguidade).
 *
 * Bibliotecas de outros documentos (libmdf, libnfc...) geram as tabelas
 * dos seus próprios schemas com o mesmo gerador (tools/gerar_esquemas.py
 * --config, ver docs/ESQUEMAS.md) e criam os grupos com nfe_grupo_new.
 * As tabelas são compiladas dentro da biblioteca do documento; as da NF-e
 * (src/libnfe/esquemas.h) não fazem parte da API.
 *
 * O leiaute das structs abaixo faz parte da ABI: muda só com a versão maior
 * da libnfe, e com ele NFE_ESQ_VERSAO. Tabelas geradas para outra versão
 * não compilam (o .c gerado confere NFE_ESQ_VERSAO).
 */

#define NFE_ESQ_VERSAO 1

enum nfe_esq_tipo { ESQ_ELEM, ESQ_SEQ, ESQ_CHOICE, ESQ_ATTR, ESQ_LISTA };

struct nfe_esq;

struct nfe_esq_no {
	const char *nome;           /* elemento ou atributo; NULL em
	                               sequência/escolha */
	unsigned char tipo;         /* enum nfe_esq_tipo */
	unsigned char min;          /* minOccurs (0 ou 1) / use="required" */
	unsigned short max;         /* maxOccurs das listas (0: sem limite) */
	const char *padrao;         /* xs:pattern ou NULL */
	const char *const *valores; /* xs:enumeration (lista com NULL) */
	unsigned short tmin, tmax;  /* tamanho em caracteres (0: livre) */
	unsigned char tstring;      /* texto do tipo TString */
	short pai, filho, irmao;    /* índices na tabela (-1: nenhum) */
	short folha;                /* índice do valor; -1 se não é folha */
	short ini, fim;             /* folhas do nó: [ini, fim) */
	short lista;                /* índice da lista; -1 se não é lista */
	short lini, lfim;           /* listas do nó: [lini, lfim) */
	const struct nfe_esq *sub;  /* estrutura dos itens da lista */
};

struct nfe_esq {
	const char *nome;
	const struct nfe_esq_no *nos;
	int nnos;
	int nfolhas;
	int nlistas;
};

/* Cria um grupo vazio com a estrutura esq (uma tabela gerada). Retorna
 * NULL se faltar memória. */
nfe_grupo *nfe_grupo_new(const struct nfe_esq *esq);
void nfe_grupo_free(nfe_grupo *g);

/* Confere se valor é aceito no campo, sem gravar. Retorna 0, E_ISNULL,
 * E_VALOR (campo inexistente ou valor inválido) ou E_TAMANHO. */
int nfe_grupo_valida(const nfe_grupo *g, const char *caminho,
                     const char *valor);
/* Apaga o último item da lista do caminho (desfaz um nfe_grupo_add) */
void nfe_grupo_remove_ultimo(nfe_grupo *g, const char *caminho);
/* Algum campo informado (ou item de lista) no grupo */
int nfe_grupo_vazio(const nfe_grupo *g);
/* Escreve o elemento raiz. Retorna 0, E_ISNULL, E_VALOR (falta campo
 * obrigatório ou mais de um ramo de uma escolha) ou E_XML. */
int nfe_grupo_write_xml(xmlTextWriterPtr writer, const nfe_grupo *g);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/esquema.h).
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
