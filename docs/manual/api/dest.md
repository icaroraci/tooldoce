# dest.h — Identificação do destinatário, documentos, endereço e inscrições. Setters de identificação e ligação ao endereço têm efeitos específicos de escolha e transferência de posse.

[Manual](../README.md) · [Índice da API](../FUNCOES.md) · [Memória e erros](../API.md)

Header de inclusão: `<libnfe/dest.h>`. Identificação do destinatário, documentos, endereço e inscrições. Setters de identificação e ligação ao endereço têm efeitos específicos de escolha e transferência de posse.

## Contrato, argumentos e retornos

Os comentários preservam os detalhes por função: limites, NULL, códigos, cópia, empréstimo, propriedade e estado em falha. Os tipos/enumerações têm seus nomes reais; as funções não foram renomeadas para uniformizar a documentação.

```c
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <libnfe/endereco.h>
#include <libnfe/nfe.h>

/*
 * Identificação do destinatário (grupo dest).
 *
 * Uso típico:
 *   nfe_dest *dest = nfe_dest_new();
 *   nfe_dest_set_cpf(dest, "12345678909");
 *   nfe_dest_set_xnome(dest, "FULANO DE TAL");
 *   nfe_dest_set_endereco(dest, end);  (o dest passa a ser dono de end)
 *   nfe_dest_set_indiedest(dest, NFE_IE_DEST_NAO_CONTRIBUINTE);
 *   nfe_dest_write_xml(writer, dest);
 *   nfe_dest_free(dest);
 *
 * Os setters validam o valor contra o leiaute e retornam 0, E_ISNULL,
 * E_TAMANHO (texto fora dos limites) ou E_VALOR (valor fora do domínio ou
 * do formato); em caso de erro o campo não é alterado. Textos: limites em
 * caracteres UTF-8, só caracteres de U+0020 a U+00FF e sem espaço no início
 * ou no fim (tipo TString). Campos opcionais são removidos com NULL.
 */

typedef struct nfe_dest nfe_dest;

/* Cria um destinatário vazio. Retorna NULL se faltar memória. */
nfe_dest *nfe_dest_new(void);

/* Libera o destinatário e o endereço que ele possui; aceita NULL */
void nfe_dest_free(nfe_dest *dest);

/* Identificação: CNPJ (14 posições, numérico ou alfanumérico), CPF (11
 * dígitos), ambos com os dígitos verificadores corretos (ver cnpjcpf.h), ou
 * idEstrangeiro (documento de estrangeiro: 5 a 20 caracteres sem espaço, ou
 * "" quando não houver). Informar um substitui os outros. */
int nfe_dest_set_cnpj(nfe_dest *dest, const char *cnpj);
int nfe_dest_set_cpf(nfe_dest *dest, const char *cpf);
int nfe_dest_set_idestrangeiro(nfe_dest *dest, const char *idestrangeiro);
int nfe_dest_set_xnome(nfe_dest *dest, const char *xnome); /* 2 a 60 */
/* enderDest. Em caso de sucesso o destinatário passa a ser dono de end (e
 * libera o endereço anterior); NULL remove o endereço. */
int nfe_dest_set_endereco(nfe_dest *dest, nfe_endereco *end);
int nfe_dest_set_indiedest(nfe_dest *dest, nfe_ind_ie_dest indiedest);
/* IE: 2 a 14 dígitos ("ISENTO" não é aceito: use NFE_IE_DEST_ISENTO) */
int nfe_dest_set_ie(nfe_dest *dest, const char *ie);
/* ISUF: inscrição na SUFRAMA, 8 ou 9 dígitos */
int nfe_dest_set_isuf(nfe_dest *dest, const char *isuf);
int nfe_dest_set_im(nfe_dest *dest, const char *im);       /* 1 a 15 */
int nfe_dest_set_email(nfe_dest *dest, const char *email); /* 1 a 60 */

/* Escreve o elemento <dest>. Retorna 0, E_ISNULL, E_VALOR (falta a
 * identificação ou indIEDest, ou endereço incompleto) ou E_XML. */
int nfe_dest_write_xml(xmlTextWriterPtr writer, const nfe_dest *dest);
```

## Fonte e exemplos

- [Header conferido](../../../include/libnfe/dest.h).
- [Implementação dest.c](../../../src/libnfe/dest.c).
- [Programa de testes compilável](../../../tests/test_dest.c): `make obj/test_dest` e `./obj/test_dest tests`.
- [Exemplos e execução](../EXEMPLOS.md), com a distinção entre casos estruturais, assinatura de teste e comunicação real.
