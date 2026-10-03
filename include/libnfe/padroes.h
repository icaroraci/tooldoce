/* Gerado por tools/gerar_padroes.py a partir de tests/schemas/nfe.
 * Não edite à mão: rode `python3 tools/gerar_padroes.py`.
 *
 * Padrões (NFE_PADRAO_*) na sintaxe de expressões regulares do XML Schema,
 * já ancorados ao valor inteiro; use com nfe_valida_padrao() (valida.h). */

#ifndef LIBNFE_PADROES_H
#define LIBNFE_PADROES_H

/* clang-format off */
/* TCodUfIBGE (base xs:string) */
#define NFE_VALORES_TCodUfIBGE "11", "12", "13", "14", "15", "16", "17", "21", "22", "23", "24", "25", "26", "27", "28", "29", "31", "32", "33", "35", "41", "42", "43", "50", "51", "52", "53"

/* TCodMunIBGE (base xs:string) */
#define NFE_PADRAO_TCodMunIBGE "[0-9]{7}"

/* TChNFe (base xs:string) */
#define NFE_PADRAO_TChNFe "[0-9]{6}[0-9A-Z]{12}[0-9]{26}"
#define NFE_TAM_MAX_TChNFe 44

/* TProt (base xs:string) */
#define NFE_PADRAO_TProt "[0-9]{15}|[0-9]{17}"
#define NFE_TAM_MAX_TProt 17

/* TRec (base xs:string) */
#define NFE_PADRAO_TRec "[0-9]{15}"
#define NFE_TAM_MAX_TRec 15

/* TStat (base xs:string) */
#define NFE_PADRAO_TStat "[0-9]{3,4}"
#define NFE_TAM_MAX_TStat 4

/* TCnpj (base xs:string) */
#define NFE_PADRAO_TCnpj "[0-9A-Z]{12}[0-9]{2}"
#define NFE_TAM_MAX_TCnpj 14

/* TCnpjVar (base xs:string) */
#define NFE_PADRAO_TCnpjVar "[0-9A-Z]{12}[0-9]{2}"
#define NFE_TAM_MAX_TCnpjVar 14

/* TCnpjOpc (base xs:string) */
#define NFE_PADRAO_TCnpjOpc "[0-9]{0}|[0-9A-Z]{12}[0-9]{2}"
#define NFE_TAM_MAX_TCnpjOpc 14

/* TCpf (base xs:string) */
#define NFE_PADRAO_TCpf "[0-9]{11}"
#define NFE_TAM_MAX_TCpf 11

/* TCpfVar (base xs:string) */
#define NFE_PADRAO_TCpfVar "[0-9]{3,11}"
#define NFE_TAM_MAX_TCpfVar 11

/* TDec_0104v (base xs:string) */
#define NFE_PADRAO_TDec_0104v "0|0\\.[0-9]{1,4}|[1-9]{1}(\\.[0-9]{1,4})?"

/* TDec_0204v (base xs:string) */
#define NFE_PADRAO_TDec_0204v "0|0\\.[0-9]{1,4}|[1-9]{1}[0-9]{0,1}(\\.[0-9]{1,4})?"

/* TDec_0302a04 (base xs:string) */
#define NFE_PADRAO_TDec_0302a04 "0|0\\.[0-9]{2,4}|[1-9]{1}[0-9]{0,2}(\\.[0-9]{2,4})?"

/* TDec_0302a04Opc (base xs:string) */
#define NFE_PADRAO_TDec_0302a04Opc "0\\.[0-9]{2,4}|[1-9]{1}[0-9]{0,2}(\\.[0-9]{2,4})?"

/* TDec_0302Max100 (base xs:string) */
#define NFE_PADRAO_TDec_0302Max100 "0(\\.[0-9]{2})?|100(\\.00)?|[1-9]{1}[0-9]{0,1}(\\.[0-9]{2})?"

/* TDec_0304Max100 (base xs:string) */
#define NFE_PADRAO_TDec_0304Max100 "0(\\.[0-9]{4})?|100(\\.00)?|[1-9]{1}[0-9]{0,1}(\\.[0-9]{4})?"

/* TDec_03v00a04Max100Opc (base xs:string) */
#define NFE_PADRAO_TDec_03v00a04Max100Opc "0(\\.[1-9][0-9]{0,3})|0(\\.[0][1-9][0-9]{0,2})|0(\\.[0][0][1-9][0-9]{0,1})|0(\\.[0][0][0][1-9])|100(\\.[0]{1,4})?|[1-9]{1}[0-9]{0,1}(\\.[0-9]{1,4})?"

/* TDec_0302a04Max100 (base xs:string) */
#define NFE_PADRAO_TDec_0302a04Max100 "0(\\.[0-9]{2,4})?|[1-9]{1}[0-9]{0,1}(\\.[0-9]{2,4})?|100(\\.0{2,4})?"

/* TDec_0803v (base xs:string) */
#define NFE_PADRAO_TDec_0803v "0|0\\.[0-9]{3}|[1-9]{1}[0-9]{0,7}(\\.[0-9]{1,3})?"

/* TDec_1104 (base xs:string) */
#define NFE_PADRAO_TDec_1104 "0|0\\.[0-9]{4}|[1-9]{1}[0-9]{0,10}(\\.[0-9]{4})?"

/* TDec_1104v (base xs:string) */
#define NFE_PADRAO_TDec_1104v "0|0\\.[0-9]{1,4}|[1-9]{1}[0-9]{0,10}|[1-9]{1}[0-9]{0,10}(\\.[0-9]{1,4})?"

/* TDec_1104Opc (base xs:string) */
#define NFE_PADRAO_TDec_1104Opc "0\\.[1-9]{1}[0-9]{3}|0\\.[0-9]{3}[1-9]{1}|0\\.[0-9]{2}[1-9]{1}[0-9]{1}|0\\.[0-9]{1}[1-9]{1}[0-9]{2}|[1-9]{1}[0-9]{0,10}(\\.[0-9]{4})?"

/* TDec_1110v (base xs:string) */
#define NFE_PADRAO_TDec_1110v "0|0\\.[0-9]{1,10}|[1-9]{1}[0-9]{0,10}|[1-9]{1}[0-9]{0,10}(\\.[0-9]{1,10})?"

/* TDec_1203 (base xs:string) */
#define NFE_PADRAO_TDec_1203 "0|0\\.[0-9]{3}|[1-9]{1}[0-9]{0,11}(\\.[0-9]{3})?"

/* TDec_1204 (base xs:string) */
#define NFE_PADRAO_TDec_1204 "0|0\\.[0-9]{1,4}|[1-9]{1}[0-9]{0,11}|[1-9]{1}[0-9]{0,11}(\\.[0-9]{4})?"

/* TDec_1204v (base xs:string) */
#define NFE_PADRAO_TDec_1204v "0|0\\.[0-9]{1,4}|[1-9]{1}[0-9]{0,11}|[1-9]{1}[0-9]{0,11}(\\.[0-9]{1,4})?"

/* TDec_1204Opc (base xs:string) */
#define NFE_PADRAO_TDec_1204Opc "0\\.[0-9]{1,4}|[1-9]{1}[0-9]{0,11}|[1-9]{1}[0-9]{0,11}(\\.[0-9]{1,4})?"

/* TDec_1204temperatura (base xs:string) */
#define NFE_PADRAO_TDec_1204temperatura "0\\.[1-9]{1}[0-9]{3}|0\\.[0-9]{3}[1-9]{1}|0\\.[0-9]{2}[1-9]{1}[0-9]{1}|0\\.[0-9]{1}[1-9]{1}[0-9]{2}|[1-9]{1}[0-9]{0,11}(\\.[0-9]{4})?"

/* TDec_1302 (base xs:string) */
#define NFE_PADRAO_TDec_1302 "0|0\\.[0-9]{2}|[1-9]{1}[0-9]{0,12}(\\.[0-9]{2})?"

/* TDec_1302Opc (base xs:string) */
#define NFE_PADRAO_TDec_1302Opc "0\\.[0-9]{1}[1-9]{1}|0\\.[1-9]{1}[0-9]{1}|[1-9]{1}[0-9]{0,12}(\\.[0-9]{2})?"

/* TIeDest (base xs:string) */
#define NFE_PADRAO_TIeDest "ISENTO|[0-9]{2,14}"
#define NFE_TAM_MAX_TIeDest 14

/* TIeDestNaoIsento (base xs:string) */
#define NFE_PADRAO_TIeDestNaoIsento "[0-9]{2,14}"
#define NFE_TAM_MAX_TIeDestNaoIsento 14

/* TIeST (base xs:string) */
#define NFE_PADRAO_TIeST "[0-9]{2,14}"
#define NFE_TAM_MAX_TIeST 14

/* TIe (base xs:string) */
#define NFE_PADRAO_TIe "[0-9]{2,14}|ISENTO"
#define NFE_TAM_MAX_TIe 14

/* TMod (base xs:string) */
#define NFE_VALORES_TMod "55", "65"

/* TNF (base xs:string) */
#define NFE_PADRAO_TNF "[1-9]{1}[0-9]{0,8}"

/* TSerie (base xs:string) */
#define NFE_PADRAO_TSerie "0|[1-9]{1}[0-9]{0,2}"

/* TUf (base xs:string) */
#define NFE_VALORES_TUf "AC", "AL", "AM", "AP", "BA", "CE", "DF", "ES", "GO", "MA", "MG", "MS", "MT", "PA", "PB", "PE", "PI", "PR", "RJ", "RN", "RO", "RR", "RS", "SC", "SE", "SP", "TO", "EX"

/* TUfEmi (base xs:string) */
#define NFE_VALORES_TUfEmi "AC", "AL", "AM", "AP", "BA", "CE", "DF", "ES", "GO", "MA", "MG", "MS", "MT", "PA", "PB", "PE", "PI", "PR", "RJ", "RN", "RO", "RR", "RS", "SC", "SE", "SP", "TO"

/* TAmb (base xs:string) */
#define NFE_VALORES_TAmb "1", "2"

/* TVerAplic (base nfe:TString) */
#define NFE_PADRAO_TVerAplic NFE_PADRAO_TString
#define NFE_TAM_MIN_TVerAplic 1
#define NFE_TAM_MAX_TVerAplic 20

/* TMotivo (base nfe:TString) */
#define NFE_PADRAO_TMotivo NFE_PADRAO_TString
#define NFE_TAM_MIN_TMotivo 1
#define NFE_TAM_MAX_TMotivo 255

/* TJust (base nfe:TString) */
#define NFE_PADRAO_TJust NFE_PADRAO_TString
#define NFE_TAM_MIN_TJust 15
#define NFE_TAM_MAX_TJust 255

/* TServ (base nfe:TString) */
#define NFE_PADRAO_TServ NFE_PADRAO_TString

/* Tano (base xs:string) */
#define NFE_PADRAO_Tano "[0-9]{2}"

/* TMed (base xs:string) */
#define NFE_PADRAO_TMed "[0-9]{1,4}"

/* TString (base xs:string) */
#define NFE_PADRAO_TString "[!-\xC3\xBF]{1}[ -\xC3\xBF]{0,}[!-\xC3\xBF]{1}|[!-\xC3\xBF]{1}"

/* TData (base xs:string) */
#define NFE_PADRAO_TData "(((20(([02468][048])|([13579][26]))-02-29))|(20[0-9][0-9])-((((0[1-9])|(1[0-2]))-((0[1-9])|(1\\d)|(2[0-8])))|((((0[13578])|(1[02]))-31)|(((0[1,3-9])|(1[0-2]))-(29|30)))))"

/* TTime (base xs:string) */
#define NFE_PADRAO_TTime "(([0-1][0-9])|([2][0-3])):([0-5][0-9]):([0-5][0-9])"

/* TDateTimeUTC (base xs:string) */
#define NFE_PADRAO_TDateTimeUTC "(((20(([02468][048])|([13579][26]))-02-29))|(20[0-9][0-9])-((((0[1-9])|(1[0-2]))-((0[1-9])|(1\\d)|(2[0-8])))|((((0[13578])|(1[02]))-31)|(((0[1,3-9])|(1[0-2]))-(29|30)))))T(20|21|22|23|[0-1]\\d):[0-5]\\d:[0-5]\\d([\\-,\\+](0[0-9]|10|11):00|([\\+](12):00))"

/* TPlaca (base xs:string) */
#define NFE_PADRAO_TPlaca "[A-Z]{2,3}[0-9]{4}|[A-Z]{3,4}[0-9]{3}|[A-Z0-9]{7}"

/* TCOrgaoIBGE (base xs:string) */
#define NFE_VALORES_TCOrgaoIBGE "11", "12", "13", "14", "15", "16", "17", "21", "22", "23", "24", "25", "26", "27", "28", "29", "31", "32", "33", "35", "41", "42", "43", "50", "51", "52", "53", "90", "91", "92"

/* TChDFeRTC (base xs:string) */
#define NFE_PADRAO_TChDFeRTC "[0-9]{6}[A-Z0-9]{12}[0-9]{26}"
#define NFE_TAM_MAX_TChDFeRTC 44

/* TStringRTC (base xs:string) */
#define NFE_PADRAO_TStringRTC "[!-\xC3\xBF]{1}[ -\xC3\xBF]{0,}[!-\xC3\xBF]{1}|[!-\xC3\xBF]{1}"

/* TCST (base xs:string) */
#define NFE_PADRAO_TCST "\\d{3}"

/* TcClassTrib (base xs:string) */
#define NFE_PADRAO_TcClassTrib "\\d{6}"

/* TcCredPres (base xs:string) */
#define NFE_PADRAO_TcCredPres "\\d{2}"

/* TDec1104RTC (base xs:string) */
#define NFE_PADRAO_TDec1104RTC "0|0\\.[0-9]{4}|[1-9]{1}[0-9]{0,10}(\\.[0-9]{4})?"

/* TDec_1104OpRTC (base xs:string) */
#define NFE_PADRAO_TDec_1104OpRTC "0\\.[1-9]{1}[0-9]{3}|0\\.[0-9]{3}[1-9]{1}|0\\.[0-9]{2}[1-9]{1}[0-9]{1}|0\\.[0-9]{1}[1-9]{1}[0-9]{2}|[1-9]{1}[0-9]{0,10}(\\.[0-9]{4})?"

/* TCnpjBaseRTC (base xs:string) */
#define NFE_PADRAO_TCnpjBaseRTC "[A-Z0-9]{8}"

/* TCnpjRTC (base xs:string) */
#define NFE_PADRAO_TCnpjRTC "[A-Z0-9]{12}[0-9]{2}"

/* TDec1302RTC (base xs:string) */
#define NFE_PADRAO_TDec1302RTC "0|0\\.[0-9]{2}|[1-9]{1}[0-9]{0,12}(\\.[0-9]{2})?"

/* TDec_0302_04RTC (base xs:string) */
#define NFE_PADRAO_TDec_0302_04RTC "0|0\\.[0-9]{2,4}|[1-9]{1}[0-9]{0,2}(\\.[0-9]{2,4})?"

/* TOperCompraGov (base xs:string) */
#define NFE_VALORES_TOperCompraGov "1", "2", "3", "4"

/* TRBSN (base xs:string) */
#define NFE_VALORES_TRBSN "0", "1", "2", "3", "4", "5", "9"

/* TEnteGov (base xs:string) */
#define NFE_VALORES_TEnteGov "1", "2", "3", "4", "5", "6"

/* TTpCredPresIBSZFM (base xs:string) */
#define NFE_VALORES_TTpCredPresIBSZFM "0", "1", "2", "3", "4"

/* TIndDoacao (base xs:string) */
#define NFE_VALORES_TIndDoacao "1"

/* TCompetApur (base xs:gYearMonth) */
#define NFE_PADRAO_TCompetApur "[0-9]{4}-(0[1-9]|1[0-2])"

/* Torig (base xs:string) */
#define NFE_VALORES_Torig "0", "1", "2", "3", "4", "5", "6", "7", "8"

/* TFinNFe (base xs:string) */
#define NFE_VALORES_TFinNFe "1", "2", "3", "4", "5", "6"

/* TTpNFDebito (base xs:string) */
#define NFE_VALORES_TTpNFDebito "01", "02", "03", "04", "05", "06", "07", "08"

/* TTpNFCredito (base xs:string) */
#define NFE_VALORES_TTpNFCredito "01", "02", "03", "04", "05", "06"

/* TProcEmi (base xs:string) */
#define NFE_VALORES_TProcEmi "0", "1", "2", "3", "4"

/* TCListServ (base xs:string) */
#define NFE_PADRAO_TCListServ "[0-9]{2}.[0-9]{2}"

/* TIdLote (base xs:string) */
#define NFE_PADRAO_TIdLote "[0-9]{1,15}"

/* TVerNFe (base xs:string) */
#define NFE_PADRAO_TVerNFe "4\\.00"

/* TGuid (base xs:string) */
#define NFE_PADRAO_TGuid "[A-F0-9]{8}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{12}"

/* Todos os tipos com NFE_PADRAO_*: NFE_TIPOS_COM_PADRAO(X) chama
 * X(tipo) para cada um */
#define NFE_TIPOS_COM_PADRAO(X) \
	X(TCodMunIBGE) \
	X(TChNFe) \
	X(TProt) \
	X(TRec) \
	X(TStat) \
	X(TCnpj) \
	X(TCnpjVar) \
	X(TCnpjOpc) \
	X(TCpf) \
	X(TCpfVar) \
	X(TDec_0104v) \
	X(TDec_0204v) \
	X(TDec_0302a04) \
	X(TDec_0302a04Opc) \
	X(TDec_0302Max100) \
	X(TDec_0304Max100) \
	X(TDec_03v00a04Max100Opc) \
	X(TDec_0302a04Max100) \
	X(TDec_0803v) \
	X(TDec_1104) \
	X(TDec_1104v) \
	X(TDec_1104Opc) \
	X(TDec_1110v) \
	X(TDec_1203) \
	X(TDec_1204) \
	X(TDec_1204v) \
	X(TDec_1204Opc) \
	X(TDec_1204temperatura) \
	X(TDec_1302) \
	X(TDec_1302Opc) \
	X(TIeDest) \
	X(TIeDestNaoIsento) \
	X(TIeST) \
	X(TIe) \
	X(TNF) \
	X(TSerie) \
	X(TVerAplic) \
	X(TMotivo) \
	X(TJust) \
	X(TServ) \
	X(Tano) \
	X(TMed) \
	X(TString) \
	X(TData) \
	X(TTime) \
	X(TDateTimeUTC) \
	X(TPlaca) \
	X(TChDFeRTC) \
	X(TStringRTC) \
	X(TCST) \
	X(TcClassTrib) \
	X(TcCredPres) \
	X(TDec1104RTC) \
	X(TDec_1104OpRTC) \
	X(TCnpjBaseRTC) \
	X(TCnpjRTC) \
	X(TDec1302RTC) \
	X(TDec_0302_04RTC) \
	X(TCompetApur) \
	X(TCListServ) \
	X(TIdLote) \
	X(TVerNFe) \
	X(TGuid)

/* clang-format on */

#endif
