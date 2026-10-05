# emit.h — Estabelecimento emitente, documento, endereço, regime e inscrições. CNPJ/CPF são validados matematicamente; cadastro e autorização fiscal são etapas distintas.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/emit.h>`. Estabelecimento emitente, documento, endereço, regime e inscrições. CNPJ/CPF são validados matematicamente; cadastro e autorização fiscal são etapas distintas.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/endereco.h>
#include <libnfe/nfe.h>
#include <libnfe/utils.h>

/*
 * Identificação do emitente (grupo emit).
 *
 * Uso típico:
 *   nfe_emit *emit = nfe_emit_new();
 *   nfe_emit_set_cnpj(emit, "12345678000195");
 *   nfe_emit_set_xnome(emit, "EMPRESA LTDA");
 *   nfe_emit_set_endereco(emit, end);  (o emit passa a ser dono de end)
 *   nfe_emit_set_ie(emit, "123456789");
 *   nfe_emit_set_crt(emit, NFE_CRT_REGIME_NORMAL);
 *   nfe_emit_write_xml(writer, emit);
 *   nfe_emit_free(emit);
 *
 * Os setters validam o valor contra o leiaute e retornam 0, E_ISNULL,
 * E_TAMANHO (texto fora dos limites) ou E_VALOR (valor fora do domínio ou
 * do formato); em caso de erro o campo não é alterado. Textos: limites em
 * caracteres UTF-8, só caracteres de U+0020 a U+00FF e sem espaço no início
 * ou no fim (tipo TString). Campos opcionais são removidos com NULL.
 */

typedef struct nfe_emit nfe_emit;

/* Cria um emitente vazio. Retorna NULL se faltar memória. */
nfe_emit *nfe_emit_new(void);

/* Libera o emitente e o endereço que ele possui; aceita NULL */
void nfe_emit_free(nfe_emit *emit);

/* CNPJ (14 posições, numérico ou alfanumérico) ou CPF (11 dígitos), com os
 * dígitos verificadores corretos (ver cnpjcpf.h). Informar um substitui o
 * outro. */
int nfe_emit_set_cnpj(nfe_emit *emit, const char *cnpj);
int nfe_emit_set_cpf(nfe_emit *emit, const char *cpf);
int nfe_emit_set_xnome(nfe_emit *emit, const char *xnome); /* 2 a 60 */
int nfe_emit_set_xfant(nfe_emit *emit, const char *xfant); /* 1 a 60 */
/* enderEmit. Em caso de sucesso o emitente passa a ser dono de end (e
 * libera o endereço anterior). */
int nfe_emit_set_endereco(nfe_emit *emit, nfe_endereco *end);
/* IE: 2 a 14 dígitos ou "ISENTO" */
int nfe_emit_set_ie(nfe_emit *emit, const char *ie);
/* IEST: IE do substituto tributário na UF de destino, 2 a 14 dígitos */
int nfe_emit_set_iest(nfe_emit *emit, const char *iest);
/* IM: inscrição municipal, 1 a 15 caracteres. CNAE: 7 dígitos; só pode ser
 * informado junto com a IM. */
int nfe_emit_set_im(nfe_emit *emit, const char *im);
int nfe_emit_set_cnae(nfe_emit *emit, const char *cnae);
int nfe_emit_set_crt(nfe_emit *emit, nfe_crt crt);
/* ISUFEmit: inscrição na SUFRAMA, 8 ou 9 dígitos */
int nfe_emit_set_isufemit(nfe_emit *emit, const char *isufemit);

/* Escreve o elemento <emit>. Retorna 0, E_ISNULL, E_VALOR (falta CNPJ/CPF,
 * xNome, endereço ou CRT; CNAE sem IM; ou endereço inválido para o
 * emitente, ver nfe_endereco_write_xml) ou E_XML. */
int nfe_emit_write_xml(xmlTextWriterPtr writer, const nfe_emit *emit);

/* Uso interno: CNPJ ou CPF informado, ou NULL */
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/emit.h).
- [Implementação emit.c](../../../src/libnfe/emit.c).
- [Programa de testes compilável](../../../tests/test_emit.c): `make obj/test_emit` e `./obj/test_emit tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
