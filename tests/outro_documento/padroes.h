/* Gerado por tools/gerar_padroes.py a partir de tests/schemas/exemplo.
 * Não edite à mão: rode `python3 "$(pkg-config --variable=ferramentas libnfe)/gerar_padroes.py" --config tests/outro_documento/documento.json`.
 *
 * Padrões (EX_PADRAO_*) na sintaxe de expressões regulares do XML Schema,
 * já ancorados ao valor inteiro; use com nfe_valida_padrao() (valida.h). */

#ifndef OUTRO_DOCUMENTO_PADROES_H
#define OUTRO_DOCUMENTO_PADROES_H

/* clang-format off */
/* TString (base xs:string) */
#define EX_PADRAO_TString "[!-\xC3\xBF]{1}[ -\xC3\xBF]{0,}[!-\xC3\xBF]{1}|[!-\xC3\xBF]{1}"

/* TUf (base xs:string) */
#define EX_VALORES_TUf "RJ", "RS", "SP"

/* TPlaca (base xs:string) */
#define EX_PADRAO_TPlaca "[A-Z]{3}[0-9][A-Z0-9][0-9]{2}"

/* TCpf (base xs:string) */
#define EX_PADRAO_TCpf "[0-9]{11}"
#define EX_TAM_MAX_TCpf 11

/* TCnpj (base xs:string) */
#define EX_PADRAO_TCnpj "[0-9]{14}"
#define EX_TAM_MAX_TCnpj 14

/* TDec_1302 (base xs:string) */
#define EX_PADRAO_TDec_1302 "0|0\\.[0-9]{2}|[1-9]{1}[0-9]{0,12}(\\.[0-9]{2})?"

/* Todos os tipos com EX_PADRAO_*: EX_TIPOS_COM_PADRAO(X) chama
 * X(tipo) para cada um */
#define EX_TIPOS_COM_PADRAO(X) \
	X(TString) \
	X(TPlaca) \
	X(TCpf) \
	X(TCnpj) \
	X(TDec_1302)

/* clang-format on */

#endif
