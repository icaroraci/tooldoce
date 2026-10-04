/* Gerado por tools/gerar_esquemas.py a partir de tests/schemas/exemplo.
 * Não edite à mão: rode `python3 "$(pkg-config --variable=ferramentas libnfe)/gerar_esquemas.py" --config tests/outro_documento/documento.json`. */

#include <stddef.h>

#include "esquemas.h"

#if NFE_ESQ_VERSAO != 1
#error "tabelas geradas para outra versão do motor de grupos da libnfe"
#endif

/* clang-format off */
static const char *const valores_0[] = { "RJ", "RS", "SP", NULL };

static const struct nfe_esq_no nos_infEx_condutor[] = {
	{ "condutor", ESQ_ELEM, 1, 1, NULL, NULL, 0, 0, 0, -1, 1, -1, -1, 0, 2, -1, 0, 0, NULL },
	{ NULL, ESQ_SEQ, 1, 1, NULL, NULL, 0, 0, 0, 0, 2, -1, -1, 0, 2, -1, 0, 0, NULL },
	{ "xNome", ESQ_ELEM, 1, 1, "[!-\xC3\xBF]{1}[ -\xC3\xBF]{0,}[!-\xC3\xBF]{1}|[!-\xC3\xBF]{1}", NULL, 0, 0, 0, 1, -1, 3, 0, 0, 1, -1, 0, 0, NULL },
	{ "CPF", ESQ_ELEM, 1, 1, "[0-9]{11}", NULL, 0, 11, 0, 1, -1, -1, 1, 1, 2, -1, 0, 0, NULL },
};

static const struct nfe_esq ex_esq_infEx_condutor = { "condutor", nos_infEx_condutor, 4, 2, 0 };

static const struct nfe_esq_no nos_infEx[] = {
	{ "infEx", ESQ_ELEM, 1, 1, NULL, NULL, 0, 0, 0, -1, 1, -1, -1, 0, 11, -1, 0, 1, NULL },
	{ "versao", ESQ_ATTR, 1, 1, "1\\.00", NULL, 0, 0, 0, 0, -1, 2, 0, 0, 1, -1, 0, 0, NULL },
	{ NULL, ESQ_SEQ, 1, 1, NULL, NULL, 0, 0, 0, 0, 3, -1, -1, 1, 11, -1, 0, 1, NULL },
	{ "ide", ESQ_ELEM, 1, 1, NULL, NULL, 0, 0, 0, 2, 4, 7, -1, 1, 3, -1, 0, 0, NULL },
	{ NULL, ESQ_SEQ, 1, 1, NULL, NULL, 0, 0, 0, 3, 5, -1, -1, 1, 3, -1, 0, 0, NULL },
	{ "cUF", ESQ_ELEM, 1, 1, NULL, valores_0, 0, 0, 0, 4, -1, 6, 1, 1, 2, -1, 0, 0, NULL },
	{ "xNome", ESQ_ELEM, 1, 1, "[!-\xC3\xBF]{1}[ -\xC3\xBF]{0,}[!-\xC3\xBF]{1}|[!-\xC3\xBF]{1}", NULL, 2, 60, 1, 4, -1, -1, 2, 2, 3, -1, 0, 0, NULL },
	{ "veiculo", ESQ_ELEM, 1, 1, NULL, NULL, 0, 0, 0, 2, 8, 16, -1, 3, 8, -1, 0, 0, NULL },
	{ NULL, ESQ_SEQ, 1, 1, NULL, NULL, 0, 0, 0, 7, 9, -1, -1, 3, 8, -1, 0, 0, NULL },
	{ "placa", ESQ_ELEM, 1, 1, "[A-Z]{3}[0-9][A-Z0-9][0-9]{2}", NULL, 0, 0, 0, 8, -1, 10, 3, 3, 4, -1, 0, 0, NULL },
	{ "tara", ESQ_ELEM, 0, 1, "0|0\\.[0-9]{2}|[1-9]{1}[0-9]{0,12}(\\.[0-9]{2})?", NULL, 0, 0, 0, 8, -1, 11, 4, 4, 5, -1, 0, 0, NULL },
	{ "prop", ESQ_ELEM, 0, 1, NULL, NULL, 0, 0, 0, 8, 12, 15, -1, 5, 7, -1, 0, 0, NULL },
	{ NULL, ESQ_SEQ, 1, 1, NULL, NULL, 0, 0, 0, 11, 13, -1, -1, 5, 7, -1, 0, 0, NULL },
	{ "CPF", ESQ_ELEM, 1, 1, "[0-9]{11}", NULL, 0, 11, 0, 12, -1, 14, 5, 5, 6, -1, 0, 0, NULL },
	{ "UF", ESQ_ELEM, 1, 1, NULL, valores_0, 0, 0, 0, 12, -1, -1, 6, 6, 7, -1, 0, 0, NULL },
	{ "UF", ESQ_ELEM, 1, 1, NULL, valores_0, 0, 0, 0, 8, -1, -1, 7, 7, 8, -1, 0, 0, NULL },
	{ "condutor", ESQ_LISTA, 1, 10, NULL, NULL, 0, 0, 0, 2, -1, 17, -1, 8, 8, 0, 0, 1, &ex_esq_infEx_condutor },
	{ "contratante", ESQ_ELEM, 0, 1, NULL, NULL, 0, 0, 0, 2, 18, 21, -1, 8, 10, -1, 1, 1, NULL },
	{ NULL, ESQ_CHOICE, 1, 1, NULL, NULL, 0, 0, 0, 17, 19, -1, -1, 8, 10, -1, 1, 1, NULL },
	{ "CNPJ", ESQ_ELEM, 1, 1, "[0-9]{14}", NULL, 0, 14, 0, 18, -1, 20, 8, 8, 9, -1, 1, 1, NULL },
	{ "CPF", ESQ_ELEM, 1, 1, "[0-9]{11}", NULL, 0, 11, 0, 18, -1, -1, 9, 9, 10, -1, 1, 1, NULL },
	{ "vCarga", ESQ_ELEM, 1, 1, "0|0\\.[0-9]{2}|[1-9]{1}[0-9]{0,12}(\\.[0-9]{2})?", NULL, 0, 0, 0, 2, -1, -1, 10, 10, 11, -1, 1, 1, NULL },
};

const struct nfe_esq ex_esq_infEx = { "infEx", nos_infEx, 22, 11, 1 };

static const struct nfe_esq_no nos_veic[] = {
	{ "veiculo", ESQ_ELEM, 1, 1, NULL, NULL, 0, 0, 0, -1, 1, -1, -1, 0, 5, -1, 0, 0, NULL },
	{ NULL, ESQ_SEQ, 1, 1, NULL, NULL, 0, 0, 0, 0, 2, -1, -1, 0, 5, -1, 0, 0, NULL },
	{ "placa", ESQ_ELEM, 1, 1, "[A-Z]{3}[0-9][A-Z0-9][0-9]{2}", NULL, 0, 0, 0, 1, -1, 3, 0, 0, 1, -1, 0, 0, NULL },
	{ "tara", ESQ_ELEM, 0, 1, "0|0\\.[0-9]{2}|[1-9]{1}[0-9]{0,12}(\\.[0-9]{2})?", NULL, 0, 0, 0, 1, -1, 4, 1, 1, 2, -1, 0, 0, NULL },
	{ "prop", ESQ_ELEM, 0, 1, NULL, NULL, 0, 0, 0, 1, 5, 8, -1, 2, 4, -1, 0, 0, NULL },
	{ NULL, ESQ_SEQ, 1, 1, NULL, NULL, 0, 0, 0, 4, 6, -1, -1, 2, 4, -1, 0, 0, NULL },
	{ "CPF", ESQ_ELEM, 1, 1, "[0-9]{11}", NULL, 0, 11, 0, 5, -1, 7, 2, 2, 3, -1, 0, 0, NULL },
	{ "UF", ESQ_ELEM, 1, 1, NULL, valores_0, 0, 0, 0, 5, -1, -1, 3, 3, 4, -1, 0, 0, NULL },
	{ "UF", ESQ_ELEM, 1, 1, NULL, valores_0, 0, 0, 0, 1, -1, -1, 4, 4, 5, -1, 0, 0, NULL },
};

const struct nfe_esq ex_esq_veic = { "veiculo", nos_veic, 9, 5, 0 };

/* clang-format on */
