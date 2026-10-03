/* Gerado por tools/gerar_enderecos.py; não edite. */
/* Fonte e captura: arquivos .txt em docs/webservices. */
enum { AUTOR_AM, AUTOR_BA, AUTOR_GO, AUTOR_MG, AUTOR_MS, AUTOR_MT, AUTOR_PE, AUTOR_PR, AUTOR_RS, AUTOR_SP, AUTOR_SVAN, AUTOR_SVRS, AUTOR_SVC_AN, AUTOR_SVC_RS, AUTOR_TOTAL };
static const struct {
	nfe_uf uf;
	int normal[2], contingencia[2];
} autorizadores[] = {
	{ NFE_UF_RO, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_AC, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_AM, { AUTOR_AM, AUTOR_AM }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_RR, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_PA, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_AP, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_TO, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_MA, { AUTOR_SVAN, AUTOR_SVAN }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_PI, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_RS } },
	{ NFE_UF_CE, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_RN, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_PB, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_PE, { AUTOR_PE, AUTOR_PE }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_AL, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_SE, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_BA, { AUTOR_BA, AUTOR_BA }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_MG, { AUTOR_MG, AUTOR_MG }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_ES, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_RJ, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_SP, { AUTOR_SP, AUTOR_SP }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_PR, { AUTOR_PR, AUTOR_PR }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_SC, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_RS, { AUTOR_RS, AUTOR_RS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
	{ NFE_UF_MS, { AUTOR_MS, AUTOR_MS }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_MT, { AUTOR_MT, AUTOR_MT }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_GO, { AUTOR_GO, AUTOR_GO }, { AUTOR_SVC_RS, AUTOR_SVC_RS } },
	{ NFE_UF_DF, { AUTOR_SVRS, AUTOR_SVRS }, { AUTOR_SVC_AN, AUTOR_SVC_AN } },
};

static const char *const enderecos[2][AUTOR_TOTAL][NFE_SERVICO_INUTILIZACAO + 1] = {
	[0] = {
		[AUTOR_AM] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefaz.am.gov.br/services2/services/NfeAutorizaca"
				"o4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefaz.am.gov.br/services2/services/NfeRetAutoriz"
				"acao4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefaz.am.gov.br/services2/services/NfeConsulta4",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefaz.am.gov.br/services2/services/NfeStatusServ"
				"ico4",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefaz.am.gov.br/services2/services/RecepcaoEvent"
				"o4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefaz.am.gov.br/services2/services/NfeInutilizac"
				"ao4",
		},
		[AUTOR_BA] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefaz.ba.gov.br/webservices/NFeAutorizacao4/NFeA"
				"utorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefaz.ba.gov.br/webservices/NFeRetAutorizacao4/N"
				"FeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefaz.ba.gov.br/webservices/NFeConsultaProtocolo"
				"4/NFeConsultaProtocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefaz.ba.gov.br/webservices/NFeStatusServico4/NF"
				"eStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefaz.ba.gov.br/webservices/NFeRecepcaoEvento4/N"
				"FeRecepcaoEvento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefaz.ba.gov.br/webservices/NFeInutilizacao4/NFe"
				"Inutilizacao4.asmx",
		},
		[AUTOR_GO] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefaz.go.gov.br/nfe/services/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefaz.go.gov.br/nfe/services/NFeRetAutorizacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefaz.go.gov.br/nfe/services/NFeConsultaProtocol"
				"o4",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefaz.go.gov.br/nfe/services/NFeStatusServico4",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefaz.go.gov.br/nfe/services/NFeRecepcaoEvento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefaz.go.gov.br/nfe/services/NFeInutilizacao4",
		},
		[AUTOR_MG] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.fazenda.mg.gov.br/nfe2/services/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.fazenda.mg.gov.br/nfe2/services/NFeRetAutorizaca"
				"o4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.fazenda.mg.gov.br/nfe2/services/NFeConsultaProto"
				"colo4",
			[NFE_SERVICO_STATUS] =
				"https://nfe.fazenda.mg.gov.br/nfe2/services/NFeStatusServico"
				"4",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.fazenda.mg.gov.br/nfe2/services/NFeRecepcaoEvent"
				"o4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.fazenda.mg.gov.br/nfe2/services/NFeInutilizacao4",
		},
		[AUTOR_MS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefaz.ms.gov.br/ws/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefaz.ms.gov.br/ws/NFeRetAutorizacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefaz.ms.gov.br/ws/NFeConsultaProtocolo4",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefaz.ms.gov.br/ws/NFeStatusServico4",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefaz.ms.gov.br/ws/NFeRecepcaoEvento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefaz.ms.gov.br/ws/NFeInutilizacao4",
		},
		[AUTOR_MT] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefaz.mt.gov.br/nfews/v2/services/NfeAutorizacao"
				"4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefaz.mt.gov.br/nfews/v2/services/NfeRetAutoriza"
				"cao4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefaz.mt.gov.br/nfews/v2/services/NfeConsulta4",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefaz.mt.gov.br/nfews/v2/services/NfeStatusServi"
				"co4",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefaz.mt.gov.br/nfews/v2/services/RecepcaoEvento"
				"4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefaz.mt.gov.br/nfews/v2/services/NfeInutilizaca"
				"o4",
		},
		[AUTOR_PE] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefaz.pe.gov.br/nfe-service/services/NFeAutoriza"
				"cao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefaz.pe.gov.br/nfe-service/services/NFeRetAutor"
				"izacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefaz.pe.gov.br/nfe-service/services/NFeConsulta"
				"Protocolo4",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefaz.pe.gov.br/nfe-service/services/NFeStatusSe"
				"rvico4",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefaz.pe.gov.br/nfe-service/services/NFeRecepcao"
				"Evento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefaz.pe.gov.br/nfe-service/services/NFeInutiliz"
				"acao4",
		},
		[AUTOR_PR] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefa.pr.gov.br/nfe/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefa.pr.gov.br/nfe/NFeRetAutorizacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefa.pr.gov.br/nfe/NFeConsultaProtocolo4",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefa.pr.gov.br/nfe/NFeStatusServico4",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefa.pr.gov.br/nfe/NFeRecepcaoEvento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefa.pr.gov.br/nfe/NFeInutilizacao4",
		},
		[AUTOR_RS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.sefazrs.rs.gov.br/ws/NfeAutorizacao/NFeAutorizac"
				"ao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.sefazrs.rs.gov.br/ws/NfeRetAutorizacao/NFeRetAut"
				"orizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.sefazrs.rs.gov.br/ws/NfeConsulta/NfeConsulta4.as"
				"mx",
			[NFE_SERVICO_STATUS] =
				"https://nfe.sefazrs.rs.gov.br/ws/NfeStatusServico/NfeStatusS"
				"ervico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.sefazrs.rs.gov.br/ws/recepcaoevento/recepcaoeven"
				"to4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.sefazrs.rs.gov.br/ws/nfeinutilizacao/nfeinutiliz"
				"acao4.asmx",
		},
		[AUTOR_SP] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.fazenda.sp.gov.br/ws/nfeautorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.fazenda.sp.gov.br/ws/nferetautorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.fazenda.sp.gov.br/ws/nfeconsultaprotocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://nfe.fazenda.sp.gov.br/ws/nfestatusservico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.fazenda.sp.gov.br/ws/nferecepcaoevento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.fazenda.sp.gov.br/ws/nfeinutilizacao4.asmx",
		},
		[AUTOR_SVAN] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeAutorizacao4/NFeA"
				"utorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeRetAutorizacao4/N"
				"FeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeConsultaProtocolo"
				"4/NFeConsultaProtocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeStatusServico4/NF"
				"eStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeRecepcaoEvento4/N"
				"FeRecepcaoEvento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeInutilizacao4/NFe"
				"Inutilizacao4.asmx",
		},
		[AUTOR_SVRS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.svrs.rs.gov.br/ws/NfeAutorizacao/NFeAutorizacao4"
				".asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.svrs.rs.gov.br/ws/NfeRetAutorizacao/NFeRetAutori"
				"zacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.svrs.rs.gov.br/ws/NfeConsulta/NfeConsulta4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://nfe.svrs.rs.gov.br/ws/NfeStatusServico/NfeStatusServ"
				"ico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.svrs.rs.gov.br/ws/recepcaoevento/recepcaoevento4"
				".asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe.svrs.rs.gov.br/ws/nfeinutilizacao/nfeinutilizaca"
				"o4.asmx",
		},
		[AUTOR_SVC_AN] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeAutorizacao4/NFeA"
				"utorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeRetAutorizacao4/N"
				"FeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeConsultaProtocolo"
				"4/NFeConsultaProtocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeStatusServico4/NF"
				"eStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeRecepcaoEvento4/N"
				"FeRecepcaoEvento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://www.sefazvirtual.fazenda.gov.br/NFeInutilizacao4/NFe"
				"Inutilizacao4.asmx",
		},
		[AUTOR_SVC_RS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe.svrs.rs.gov.br/ws/NfeAutorizacao/NFeAutorizacao4"
				".asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe.svrs.rs.gov.br/ws/NfeRetAutorizacao/NFeRetAutori"
				"zacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe.svrs.rs.gov.br/ws/NfeConsulta/NfeConsulta4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://nfe.svrs.rs.gov.br/ws/NfeStatusServico/NfeStatusServ"
				"ico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe.svrs.rs.gov.br/ws/recepcaoevento/recepcaoevento4"
				".asmx",
		},
	},
	[1] = {
		[AUTOR_AM] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://homnfe.sefaz.am.gov.br/services2/services/NfeAutoriz"
				"acao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://homnfe.sefaz.am.gov.br/services2/services/NfeRetAuto"
				"rizacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://homnfe.sefaz.am.gov.br/services2/services/NfeConsult"
				"a4",
			[NFE_SERVICO_STATUS] =
				"https://homnfe.sefaz.am.gov.br/services2/services/NfeStatusS"
				"ervico4",
			[NFE_SERVICO_EVENTO] =
				"https://homnfe.sefaz.am.gov.br/services2/services/RecepcaoEv"
				"ento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://homnfe.sefaz.am.gov.br/services2/services/NfeInutili"
				"zacao4",
		},
		[AUTOR_BA] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://hnfe.sefaz.ba.gov.br/webservices/NFeAutorizacao4/NFe"
				"Autorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://hnfe.sefaz.ba.gov.br/webservices/NFeRetAutorizacao4/"
				"NFeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://hnfe.sefaz.ba.gov.br/webservices/NFeConsultaProtocol"
				"o4/NFeConsultaProtocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://hnfe.sefaz.ba.gov.br/webservices/NFeStatusServico4/N"
				"FeStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://hnfe.sefaz.ba.gov.br/webservices/NFeRecepcaoEvento4/"
				"NFeRecepcaoEvento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://hnfe.sefaz.ba.gov.br/webservices/NFeInutilizacao4/NF"
				"eInutilizacao4.asmx",
		},
		[AUTOR_GO] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://homolog.sefaz.go.gov.br/nfe/services/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://homolog.sefaz.go.gov.br/nfe/services/NFeRetAutorizac"
				"ao4",
			[NFE_SERVICO_CONSULTA] =
				"https://homolog.sefaz.go.gov.br/nfe/services/NFeConsultaProt"
				"ocolo4",
			[NFE_SERVICO_STATUS] =
				"https://homolog.sefaz.go.gov.br/nfe/services/NFeStatusServic"
				"o4",
			[NFE_SERVICO_EVENTO] =
				"https://homolog.sefaz.go.gov.br/nfe/services/NFeRecepcaoEven"
				"to4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://homolog.sefaz.go.gov.br/nfe/services/NFeInutilizacao"
				"4",
		},
		[AUTOR_MG] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://hnfe.fazenda.mg.gov.br/nfe2/services/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://hnfe.fazenda.mg.gov.br/nfe2/services/NFeRetAutorizac"
				"ao4",
			[NFE_SERVICO_CONSULTA] =
				"https://hnfe.fazenda.mg.gov.br/nfe2/services/NFeConsultaProt"
				"ocolo4",
			[NFE_SERVICO_STATUS] =
				"https://hnfe.fazenda.mg.gov.br/nfe2/services/NFeStatusServic"
				"o4",
			[NFE_SERVICO_EVENTO] =
				"https://hnfe.fazenda.mg.gov.br/nfe2/services/NFeRecepcaoEven"
				"to4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://hnfe.fazenda.mg.gov.br/nfe2/services/NFeInutilizacao"
				"4",
		},
		[AUTOR_MS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://hom.nfe.sefaz.ms.gov.br/ws/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://hom.nfe.sefaz.ms.gov.br/ws/NFeRetAutorizacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://hom.nfe.sefaz.ms.gov.br/ws/NFeConsultaProtocolo4",
			[NFE_SERVICO_STATUS] =
				"https://hom.nfe.sefaz.ms.gov.br/ws/NFeStatusServico4",
			[NFE_SERVICO_EVENTO] =
				"https://hom.nfe.sefaz.ms.gov.br/ws/NFeRecepcaoEvento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://hom.nfe.sefaz.ms.gov.br/ws/NFeInutilizacao4",
		},
		[AUTOR_MT] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://homologacao.sefaz.mt.gov.br/nfews/v2/services/NfeAut"
				"orizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://homologacao.sefaz.mt.gov.br/nfews/v2/services/NfeRet"
				"Autorizacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://homologacao.sefaz.mt.gov.br/nfews/v2/services/NfeCon"
				"sulta4",
			[NFE_SERVICO_STATUS] =
				"https://homologacao.sefaz.mt.gov.br/nfews/v2/services/NfeSta"
				"tusServico4",
			[NFE_SERVICO_EVENTO] =
				"https://homologacao.sefaz.mt.gov.br/nfews/v2/services/Recepc"
				"aoEvento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://homologacao.sefaz.mt.gov.br/nfews/v2/services/NfeInu"
				"tilizacao4",
		},
		[AUTOR_PE] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfehomolog.sefaz.pe.gov.br/nfe-service/services/NFeA"
				"utorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfehomolog.sefaz.pe.gov.br/nfe-service/services/NFeR"
				"etAutorizacao4",
			[NFE_SERVICO_CONSULTA] =
				"https://nfehomolog.sefaz.pe.gov.br/nfe-service/services/NFeC"
				"onsultaProtocolo4",
			[NFE_SERVICO_STATUS] =
				"https://nfehomolog.sefaz.pe.gov.br/nfe-service/services/NFeS"
				"tatusServico4",
			[NFE_SERVICO_EVENTO] =
				"https://nfehomolog.sefaz.pe.gov.br/nfe-service/services/NFeR"
				"ecepcaoEvento4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfehomolog.sefaz.pe.gov.br/nfe-service/services/NFeI"
				"nutilizacao4",
		},
		[AUTOR_PR] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://homologacao.nfe.sefa.pr.gov.br/nfe/NFeAutorizacao4",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://homologacao.nfe.sefa.pr.gov.br/nfe/NFeRetAutorizacao"
				"4",
			[NFE_SERVICO_CONSULTA] =
				"https://homologacao.nfe.sefa.pr.gov.br/nfe/NFeConsultaProtoc"
				"olo4",
			[NFE_SERVICO_STATUS] =
				"https://homologacao.nfe.sefa.pr.gov.br/nfe/NFeStatusServico4",
			[NFE_SERVICO_EVENTO] =
				"https://homologacao.nfe.sefa.pr.gov.br/nfe/NFeRecepcaoEvento"
				"4",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://homologacao.nfe.sefa.pr.gov.br/nfe/NFeInutilizacao4",
		},
		[AUTOR_RS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe-homologacao.sefazrs.rs.gov.br/ws/NfeAutorizacao/"
				"NFeAutorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe-homologacao.sefazrs.rs.gov.br/ws/NfeRetAutorizac"
				"ao/NFeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe-homologacao.sefazrs.rs.gov.br/ws/NfeConsulta/Nfe"
				"Consulta4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://nfe-homologacao.sefazrs.rs.gov.br/ws/NfeStatusServic"
				"o/NfeStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe-homologacao.sefazrs.rs.gov.br/ws/recepcaoevento/"
				"recepcaoevento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe-homologacao.sefazrs.rs.gov.br/ws/nfeinutilizacao"
				"/nfeinutilizacao4.asmx",
		},
		[AUTOR_SP] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://homologacao.nfe.fazenda.sp.gov.br/ws/nfeautorizacao4"
				".asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://homologacao.nfe.fazenda.sp.gov.br/ws/nferetautorizac"
				"ao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://homologacao.nfe.fazenda.sp.gov.br/ws/nfeconsultaprot"
				"ocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://homologacao.nfe.fazenda.sp.gov.br/ws/nfestatusservic"
				"o4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://homologacao.nfe.fazenda.sp.gov.br/ws/nferecepcaoeven"
				"to4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://homologacao.nfe.fazenda.sp.gov.br/ws/nfeinutilizacao"
				"4.asmx",
		},
		[AUTOR_SVAN] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeAutorizacao4/NFeA"
				"utorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeRetAutorizacao4/N"
				"FeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeConsultaProtocolo"
				"4/NFeConsultaProtocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeStatusServico4/NF"
				"eStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeRecepcaoEvento4/N"
				"FeRecepcaoEvento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeInutilizacao4/NFe"
				"Inutilizacao4.asmx",
		},
		[AUTOR_SVRS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeAutorizacao/NFe"
				"Autorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeRetAutorizacao/"
				"NFeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeConsulta/NfeCon"
				"sulta4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeStatusServico/N"
				"feStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/recepcaoevento/rec"
				"epcaoevento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/nfeinutilizacao/nf"
				"einutilizacao4.asmx",
		},
		[AUTOR_SVC_AN] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeAutorizacao4/NFeA"
				"utorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeRetAutorizacao4/N"
				"FeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeConsultaProtocolo"
				"4/NFeConsultaProtocolo4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeStatusServico4/NF"
				"eStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeRecepcaoEvento4/N"
				"FeRecepcaoEvento4.asmx",
			[NFE_SERVICO_INUTILIZACAO] =
				"https://hom.sefazvirtual.fazenda.gov.br/NFeInutilizacao4/NFe"
				"Inutilizacao4.asmx",
		},
		[AUTOR_SVC_RS] = {
			[NFE_SERVICO_AUTORIZACAO] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeAutorizacao/NFe"
				"Autorizacao4.asmx",
			[NFE_SERVICO_RET_AUTORIZACAO] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeRetAutorizacao/"
				"NFeRetAutorizacao4.asmx",
			[NFE_SERVICO_CONSULTA] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeConsulta/NfeCon"
				"sulta4.asmx",
			[NFE_SERVICO_STATUS] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/NfeStatusServico/N"
				"feStatusServico4.asmx",
			[NFE_SERVICO_EVENTO] =
				"https://nfe-homologacao.svrs.rs.gov.br/ws/recepcaoevento/rec"
				"epcaoevento4.asmx",
		},
	},
};
