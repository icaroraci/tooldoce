/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <libnfe/erros.h>
#include <libnfe/sefaz.h>

#include "teste.h"

static void verifica_url(nfe_uf uf, nfe_ambiente amb, nfe_emissao emissao,
                         nfe_servico servico, const char *esperado)
{
	const char *url = NULL;
	VERIFICA_INT(nfe_sefaz_endereco(uf, amb, emissao, servico, &url), 0);
	VERIFICA_STR(url, esperado);
}

int main(void)
{
	const nfe_uf ufs[] = { NFE_UF_RO, NFE_UF_AC, NFE_UF_AM, NFE_UF_RR,
		               NFE_UF_PA, NFE_UF_AP, NFE_UF_TO, NFE_UF_MA,
		               NFE_UF_PI, NFE_UF_CE, NFE_UF_RN, NFE_UF_PB,
		               NFE_UF_PE, NFE_UF_AL, NFE_UF_SE, NFE_UF_BA,
		               NFE_UF_MG, NFE_UF_ES, NFE_UF_RJ, NFE_UF_SP,
		               NFE_UF_PR, NFE_UF_SC, NFE_UF_RS, NFE_UF_MS,
		               NFE_UF_MT, NFE_UF_GO, NFE_UF_DF };
	size_t i;
	int amb, servico;
	const char *url = "preservado";

	for (i = 0; i < sizeof ufs / sizeof ufs[0]; i++) {
		for (amb = 1; amb <= 2; amb++) {
			for (servico = NFE_SERVICO_AUTORIZACAO;
			     servico <= NFE_SERVICO_INUTILIZACAO; servico++) {
				const char *prod = NULL, *hom = NULL;
				VERIFICA_INT(nfe_sefaz_endereco(
				                     ufs[i], (nfe_ambiente)amb,
				                     NFE_EMISSAO_NORMAL,
				                     (nfe_servico)servico,
				                     &url),
				             0);
				VERIFICA(url &&
				         strncmp(url, "https://", 8) == 0);
				VERIFICA(url && strstr(url, "?wsdl") == NULL);
				nfe_sefaz_endereco(ufs[i],
				                   NFE_AMBIENTE_PRODUCAO,
				                   NFE_EMISSAO_NORMAL,
				                   (nfe_servico)servico, &prod);
				nfe_sefaz_endereco(ufs[i],
				                   NFE_AMBIENTE_HOMOLOGACAO,
				                   NFE_EMISSAO_NORMAL,
				                   (nfe_servico)servico, &hom);
				VERIFICA(prod && hom && strcmp(prod, hom) != 0);
			}
		}
	}
	verifica_url(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO, NFE_EMISSAO_NORMAL,
	             NFE_SERVICO_AUTORIZACAO,
	             "https://nfe-homologacao.svrs.rs.gov.br/ws/NfeAutorizacao/"
	             "NFeAutorizacao4.asmx");
	verifica_url(NFE_UF_RJ, NFE_AMBIENTE_PRODUCAO, NFE_EMISSAO_NORMAL,
	             NFE_SERVICO_AUTORIZACAO,
	             "https://nfe.svrs.rs.gov.br/ws/NfeAutorizacao/"
	             "NFeAutorizacao4.asmx");
	verifica_url(NFE_UF_MA, NFE_AMBIENTE_HOMOLOGACAO, NFE_EMISSAO_NORMAL,
	             NFE_SERVICO_STATUS,
	             "https://hom.sefazvirtual.fazenda.gov.br/"
	             "NFeStatusServico4/NFeStatusServico4.asmx");
	verifica_url(
	        NFE_UF_GO, NFE_AMBIENTE_HOMOLOGACAO, NFE_EMISSAO_NORMAL,
	        NFE_SERVICO_AUTORIZACAO,
	        "https://homolog.sefaz.go.gov.br/nfe/services/NFeAutorizacao4");
	verifica_url(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO,
	             NFE_EMISSAO_CONTINGENCIA_SVC_AN, NFE_SERVICO_AUTORIZACAO,
	             "https://hom.sefazvirtual.fazenda.gov.br/NFeAutorizacao4/"
	             "NFeAutorizacao4.asmx");
	verifica_url(NFE_UF_AM, NFE_AMBIENTE_HOMOLOGACAO,
	             NFE_EMISSAO_CONTINGENCIA_SVC_RS, NFE_SERVICO_AUTORIZACAO,
	             "https://nfe-homologacao.svrs.rs.gov.br/ws/NfeAutorizacao/"
	             "NFeAutorizacao4.asmx");
	/* As duas páginas oficiais divergem para a contingência do PI. */
	verifica_url(NFE_UF_PI, NFE_AMBIENTE_PRODUCAO,
	             NFE_EMISSAO_CONTINGENCIA_SVC_AN, NFE_SERVICO_STATUS,
	             "https://www.sefazvirtual.fazenda.gov.br/"
	             "NFeStatusServico4/NFeStatusServico4.asmx");
	verifica_url(NFE_UF_PI, NFE_AMBIENTE_HOMOLOGACAO,
	             NFE_EMISSAO_CONTINGENCIA_SVC_RS, NFE_SERVICO_STATUS,
	             "https://nfe-homologacao.svrs.rs.gov.br/ws/"
	             "NfeStatusServico/NfeStatusServico4.asmx");
	/* Confere todas as UFs nos dois tipos de SVC e nos seis serviços. */
	for (i = 0; i < sizeof ufs / sizeof ufs[0]; i++) {
		for (amb = 1; amb <= 2; amb++) {
			int rs = ufs[i] == NFE_UF_AM || ufs[i] == NFE_UF_BA ||
			         ufs[i] == NFE_UF_GO || ufs[i] == NFE_UF_MA ||
			         ufs[i] == NFE_UF_MS || ufs[i] == NFE_UF_MT ||
			         ufs[i] == NFE_UF_PE || ufs[i] == NFE_UF_PR ||
			         (ufs[i] == NFE_UF_PI && amb == 2);
			nfe_emissao emissao =
			        rs ? NFE_EMISSAO_CONTINGENCIA_SVC_RS
			           : NFE_EMISSAO_CONTINGENCIA_SVC_AN;
			nfe_emissao errada =
			        rs ? NFE_EMISSAO_CONTINGENCIA_SVC_AN
			           : NFE_EMISSAO_CONTINGENCIA_SVC_RS;
			for (servico = NFE_SERVICO_AUTORIZACAO;
			     servico <= NFE_SERVICO_INUTILIZACAO; servico++) {
				const char *esperado = NULL;
				int ausente =
				        rs &&
				        servico == NFE_SERVICO_INUTILIZACAO;
				url = "preservado";
				VERIFICA_INT(nfe_sefaz_endereco(
				                     ufs[i], (nfe_ambiente)amb,
				                     emissao,
				                     (nfe_servico)servico,
				                     &url),
				             ausente ? E_VALOR : 0);
				if (ausente) {
					VERIFICA_STR(url, "preservado");
				} else {
					VERIFICA_INT(
					        nfe_sefaz_endereco(
					                rs ? NFE_UF_AM
					                   : NFE_UF_RJ,
					                (nfe_ambiente)amb,
					                emissao,
					                (nfe_servico)servico,
					                &esperado),
					        0);
					VERIFICA_STR(url, esperado);
				}
				url = "preservado";
				VERIFICA_INT(nfe_sefaz_endereco(
				                     ufs[i], (nfe_ambiente)amb,
				                     errada,
				                     (nfe_servico)servico,
				                     &url),
				             E_VALOR);
				VERIFICA_STR(url, "preservado");
			}
		}
	}
	url = "preservado";
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO,
	                                NFE_EMISSAO_CONTINGENCIA_SVC_RS,
	                                NFE_SERVICO_STATUS, &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_AM, NFE_AMBIENTE_HOMOLOGACAO,
	                                NFE_EMISSAO_CONTINGENCIA_SVC_RS,
	                                NFE_SERVICO_INUTILIZACAO, &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco((nfe_uf)0, NFE_AMBIENTE_HOMOLOGACAO,
	                                NFE_EMISSAO_NORMAL, NFE_SERVICO_STATUS,
	                                &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_RJ, (nfe_ambiente)0,
	                                NFE_EMISSAO_NORMAL, NFE_SERVICO_STATUS,
	                                &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_RJ, (nfe_ambiente)3,
	                                NFE_EMISSAO_NORMAL, NFE_SERVICO_STATUS,
	                                &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO,
	                                NFE_EMISSAO_NORMAL, (nfe_servico)-1,
	                                &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO,
	                                NFE_EMISSAO_NORMAL, (nfe_servico)6,
	                                &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO,
	                                NFE_EMISSAO_CONTINGENCIA_OFFLINE_NFCE,
	                                NFE_SERVICO_STATUS, &url),
	             E_VALOR);
	VERIFICA_INT(nfe_sefaz_endereco(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO,
	                                NFE_EMISSAO_NORMAL, NFE_SERVICO_STATUS,
	                                NULL),
	             E_ISNULL);
	VERIFICA_STR(url, "preservado");
	TESTE_FIM();
}
