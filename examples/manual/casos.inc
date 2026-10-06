/* Exemplos sinteticos conferidos contra PL010f; nao representam enquadramento
 * fiscal. */
/* NFe/infNFe/agropecuario/guiaTransito */
static int caso_000(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "guiaTransito/tpGuia", "1"));
	CONFERE(nfe_grupo_set(g, "guiaTransito/UFGuia", "AC"));
	CONFERE(nfe_grupo_set(g, "guiaTransito/nGuia", "1"));
fim:
	return rc;
}

/* NFe/infNFe/cana/deduc */
static int caso_001(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "safra", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "ref", "01/2000"));
	nfe_grupo *item1 = NULL;
	CONFERE(nfe_grupo_add(g, "forDia", &item1));
	CONFERE(nfe_grupo_set(item1, "dia", "1"));
	CONFERE(nfe_grupo_set(item1, "qtde", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTotMes", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTotAnt", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTotGer", "1.00"));
	nfe_grupo *item2 = NULL;
	CONFERE(nfe_grupo_add(g, "deduc", &item2));
	CONFERE(nfe_grupo_set(item2, "xDed", "EXEMPLO"));
	CONFERE(nfe_grupo_set(item2, "vDed", "1.00"));
	CONFERE(nfe_grupo_set(g, "vFor", "1.00"));
	CONFERE(nfe_grupo_set(g, "vTotDed", "1.00"));
	CONFERE(nfe_grupo_set(g, "vLiqFor", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/COFINS/COFINSOutr */
static int caso_002(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSOutr/CST", "49"));
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSOutr/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSOutr/pCOFINS", "1.00"));
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSOutr/vCOFINS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/COFINS/COFINSQtde */
static int caso_003(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSQtde/CST", "03"));
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSQtde/qBCProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSQtde/vAliqProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "COFINS/COFINSQtde/vCOFINS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/COFINSST */
static int caso_004(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "COFINSST/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "COFINSST/pCOFINS", "1.00"));
	CONFERE(nfe_grupo_set(g, "COFINSST/vCOFINS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gAjusteCompet */
static int caso_005(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gAjusteCompet/competApur", "2026-09"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gAjusteCompet/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gAjusteCompet/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gCredPresIBSZFM */
static int caso_006(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresIBSZFM/competApur",
	                      "2026-09"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresIBSZFM/tpCredPresIBSZFM",
	                      "0"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresIBSZFM/vCredPresIBSZFM",
	                      "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gCredPresOper */
static int caso_007(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/vBCCredPres", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/cCredPres", "01"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gCredPresOper/gCBSCredPres */
static int caso_008(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/vBCCredPres", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/cCredPres", "01"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/gCBSCredPres/pCredPres",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/gCBSCredPres/vCredPres",
	                      "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gCredPresOper/gIBSCredPres */
static int caso_009(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/vBCCredPres", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/cCredPres", "01"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/gIBSCredPres/pCredPres",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gCredPresOper/gIBSCredPres/vCredPres",
	                      "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gEstornoCred */
static int caso_010(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gEstornoCred/vIBSEstCred", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gEstornoCred/vCBSEstCred", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gALCZFMCBS */
static int caso_011(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/gALCZFMCBS/tpALCZFMCBS",
	                      "1"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBS/gCBS/gALCZFMCBS/pAliqEfetRegCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/gALCZFMCBS/vTribRegCBS",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDevTrib */
static int caso_012(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/gDevTrib/vDevTrib",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDif */
static int caso_013(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/gDif/pDif", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/gDif/vDif", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSMun/gDevTrib */
static int caso_014(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/gDevTrib/vDevTrib",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSMun/gDif */
static int caso_015(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/gDif/pDif", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/gDif/vDif", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSMun/gRed */
static int caso_016(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/gRed/pRedAliq",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/gRed/pAliqEfet",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSUF/gDevTrib */
static int caso_017(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/gDevTrib/vDevTrib",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSUF/gDif */
static int caso_018(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/gDif/pDif", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/gDif/vDif", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gTribCompraGov */
static int caso_019(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribCompraGov/pAliqIBSUF",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribCompraGov/vTribIBSUF",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribCompraGov/pAliqIBSMun",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribCompraGov/vTribIBSMun",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribCompraGov/pAliqCBS",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribCompraGov/vTribCBS",
	                      "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gTribRegular */
static int caso_020(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/pIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSUF/vIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/pIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gIBSMun/vIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/pCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gCBS/vCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribRegular/CSTReg", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribRegular/cClassTribReg",
	                      "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBS/gTribRegular/pAliqEfetRegIBSUF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribRegular/vTribRegIBSUF",
	                      "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBS/gTribRegular/pAliqEfetRegIBSMun", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribRegular/vTribRegIBSMun",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribRegular/pAliqEfetRegCBS",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBS/gTribRegular/vTribRegCBS",
	                      "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono */
static int caso_021(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem */
static int caso_022(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao/qBCMono",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao/adRemCBS",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao/vCBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao */
static int caso_023(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao/qBCMono",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao/adRemCBS",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao/vCBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoRet */
static int caso_024(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoRet/vCBSMonoRet",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoReten */
static int caso_025(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoReten/qBCMonoReten",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoReten/adRemCBSReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoReten/vCBSMonoReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gpBioDiferenca */
static int caso_026(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gpBioDiferenca/qBCBioComb",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gpBioDiferenca/vCBSDiferenca",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem */
static int caso_027(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoPadrao/vBCMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoPadrao/pAliqMonoCBS",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoPadrao/vCBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoPadrao */
static int caso_028(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoPadrao/vBCMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoPadrao/pAliqMonoCBS",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoPadrao/vCBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoRet */
static int caso_029(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoRet/vCBSMonoRet",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoReten */
static int caso_030(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoReten/vBCMonoReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(g,
	                      "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoReten/"
	                      "pAliqMonoReten",
	                      "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gMonoReten/vCBSMonoReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/gpBioDiferenca */
static int caso_031(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g,
	                      "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/"
	                      "gpBioDiferenca/qBCBioComb",
	                      "1"));
	CONFERE(nfe_grupo_set(g,
	                      "IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/"
	                      "gpBioDiferenca/vCBSDiferenca",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem */
static int caso_032(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao/qBCMono",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao/adRemIBS",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao/vIBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao */
static int caso_033(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao/qBCMono",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao/adRemIBS",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao/vIBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoRet */
static int caso_034(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoRet/vIBSMonoRet",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoReten */
static int caso_035(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoReten/qBCMonoReten",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoReten/adRemIBSReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoReten/vIBSMonoReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gpBioDiferenca */
static int caso_036(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gpBioDiferenca/qBCBioComb",
	        "1"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gpBioDiferenca/vIBSDiferenca",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem */
static int caso_037(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vBCMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/pAliqMonoUF",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vIBSMonoUF",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/pAliqMonoMun",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vIBSMonoMun",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vIBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao */
static int caso_038(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vBCMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/pAliqMonoUF",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vIBSMonoUF",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/pAliqMonoMun",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vIBSMonoMun",
	        "1.00"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoPadrao/vIBSMono",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoRet */
static int caso_039(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g, "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoRet/vIBSMonoRet",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoReten */
static int caso_040(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoReten/vBCMonoReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(g,
	                      "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoReten/"
	                      "pAliqMonoReten",
	                      "1.00"));
	CONFERE(nfe_grupo_set(
	        g,
	        "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gMonoReten/vIBSMonoReten",
	        "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/gpBioDiferenca */
static int caso_041(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g,
	                      "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/"
	                      "gpBioDiferenca/qBCBioComb",
	                      "1"));
	CONFERE(nfe_grupo_set(g,
	                      "IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/"
	                      "gpBioDiferenca/vIBSDiferenca",
	                      "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotIBSMonoItem", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gIBSCBSMono/vTotCBSMonoItem", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IBSCBS/gTransfCred */
static int caso_042(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "IBSCBS/CST", "111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/cClassTrib", "111111"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gTransfCred/vIBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBS/gTransfCred/vCBS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS02 */
static int caso_043(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS02/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS02/CST", "02"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS02/adRemICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS02/vICMSMono", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS15 */
static int caso_044(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS15/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS15/CST", "15"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS15/adRemICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS15/vICMSMono", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS15/adRemICMSReten", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS15/vICMSMonoReten", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS30 */
static int caso_045(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS30/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS30/CST", "30"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS30/modBCST", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS30/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS30/pICMSST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS30/vICMSST", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS51 */
static int caso_046(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS51/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS51/CST", "51"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS53 */
static int caso_047(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS53/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS53/CST", "53"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS60 */
static int caso_048(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS60/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS60/CST", "60"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS61 */
static int caso_049(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS61/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS61/CST", "61"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS61/adRemICMSRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS61/vICMSMonoRet", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS70 */
static int caso_050(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/CST", "70"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/modBC", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/pRedBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/pICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/vICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/modBCST", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/pICMSST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS70/vICMSST", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMS90 */
static int caso_051(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS90/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS90/CST", "90"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMSPart */
static int caso_052(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/CST", "10"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/modBC", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/pICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/vICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/modBCST", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/pICMSST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/vICMSST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/pBCOp", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSPart/UFST", "AC"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMSSN101 */
static int caso_053(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN101/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN101/CSOSN", "101"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN101/pCredSN", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN101/vCredICMSSN", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMSSN201 */
static int caso_054(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/CSOSN", "201"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/modBCST", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/pICMSST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/vICMSST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/pCredSN", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN201/vCredICMSSN", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMSSN202 */
static int caso_055(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN202/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN202/CSOSN", "202"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN202/modBCST", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN202/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN202/pICMSST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN202/vICMSST", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMSSN500 */
static int caso_056(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN500/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN500/CSOSN", "500"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMSSN900 */
static int caso_057(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSSN900/CSOSN", "900"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ICMS/ICMSST */
static int caso_058(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSST/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSST/CST", "41"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSST/vBCSTRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSST/vICMSSTRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSST/vBCSTDest", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMSST/vICMSSTDest", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/IPI/IPINT */
static int caso_059(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS00/orig", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS00/CST", "00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS00/modBC", "0"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS00/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS00/pICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMS/ICMS00/vICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IPI/cEnq", "1"));
	CONFERE(nfe_grupo_set(g, "IPI/IPINT/CST", "01"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/ISSQN */
static int caso_060(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ISSQN/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ISSQN/vAliq", "1.00"));
	CONFERE(nfe_grupo_set(g, "ISSQN/vISSQN", "1.00"));
	CONFERE(nfe_grupo_set(g, "ISSQN/cMunFG", "3550308"));
	CONFERE(nfe_grupo_set(g, "ISSQN/cListServ", "00A00"));
	CONFERE(nfe_grupo_set(g, "ISSQN/indISS", "1"));
	CONFERE(nfe_grupo_set(g, "ISSQN/indIncentivo", "1"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/PIS/PISOutr */
static int caso_061(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "PIS/PISOutr/CST", "49"));
	CONFERE(nfe_grupo_set(g, "PIS/PISOutr/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "PIS/PISOutr/pPIS", "1.00"));
	CONFERE(nfe_grupo_set(g, "PIS/PISOutr/vPIS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/imposto/PIS/PISQtde */
static int caso_062(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "PIS/PISQtde/CST", "03"));
	CONFERE(nfe_grupo_set(g, "PIS/PISQtde/qBCProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "PIS/PISQtde/vAliqProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "PIS/PISQtde/vPIS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/obsItem/obsFisco */
static int caso_063(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "obsFisco/xCampo", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "obsFisco/xTexto", "EXEMPLO"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/arma */
static int caso_064(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	nfe_grupo *item1 = NULL;
	CONFERE(nfe_grupo_add(g, "arma", &item1));
	CONFERE(nfe_grupo_set(item1, "tpArma", "0"));
	CONFERE(nfe_grupo_set(item1, "nSerie", "EXEMPLO"));
	CONFERE(nfe_grupo_set(item1, "nCano", "EXEMPLO"));
	CONFERE(nfe_grupo_set(item1, "descr", "2026-10-05"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/comb */
static int caso_065(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "comb/cProdANP", "000000000"));
	CONFERE(nfe_grupo_set(g, "comb/descANP", "2026-10-05"));
	CONFERE(nfe_grupo_set(g, "comb/UFCons", "AC"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/comb/CIDE */
static int caso_066(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "comb/cProdANP", "000000000"));
	CONFERE(nfe_grupo_set(g, "comb/descANP", "2026-10-05"));
	CONFERE(nfe_grupo_set(g, "comb/UFCons", "AC"));
	CONFERE(nfe_grupo_set(g, "comb/CIDE/qBCProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "comb/CIDE/vAliqProd", "1"));
	CONFERE(nfe_grupo_set(g, "comb/CIDE/vCIDE", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/comb/encerrante */
static int caso_067(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "comb/cProdANP", "000000000"));
	CONFERE(nfe_grupo_set(g, "comb/descANP", "2026-10-05"));
	CONFERE(nfe_grupo_set(g, "comb/UFCons", "AC"));
	CONFERE(nfe_grupo_set(g, "comb/encerrante/nBico", "1"));
	CONFERE(nfe_grupo_set(g, "comb/encerrante/nTanque", "1"));
	CONFERE(nfe_grupo_set(g, "comb/encerrante/vEncIni", "1"));
	CONFERE(nfe_grupo_set(g, "comb/encerrante/vEncFin", "1"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/comb/origComb */
static int caso_068(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "comb/cProdANP", "000000000"));
	CONFERE(nfe_grupo_set(g, "comb/descANP", "2026-10-05"));
	CONFERE(nfe_grupo_set(g, "comb/UFCons", "AC"));
	nfe_grupo *item1 = NULL;
	CONFERE(nfe_grupo_add(g, "comb/origComb", &item1));
	CONFERE(nfe_grupo_set(item1, "indImport", "0"));
	CONFERE(nfe_grupo_set(item1, "cUFOrig", "11"));
	CONFERE(nfe_grupo_set(item1, "pOrig", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/detExport */
static int caso_069(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	nfe_grupo *item1 = NULL;
	CONFERE(nfe_grupo_add(g, "detExport", &item1));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/detExport/exportInd */
static int caso_070(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	nfe_grupo *item1 = NULL;
	CONFERE(nfe_grupo_add(g, "detExport", &item1));
	CONFERE(nfe_grupo_set(item1, "exportInd/nRE", "1"));
	CONFERE(nfe_grupo_set(item1, "exportInd/chNFe",
	                      "35261012345678000195550010000000011123456781"));
	CONFERE(nfe_grupo_set(item1, "exportInd/qExport", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/infProdEmb */
static int caso_071(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "infProdEmb/xEmb", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "infProdEmb/qVolEmb", "1.00"));
	CONFERE(nfe_grupo_set(g, "infProdEmb/uEmb", "EXEMPLO"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/infProdNFF */
static int caso_072(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "infProdNFF/cProdFisco", "EXEMPLO0000001"));
	CONFERE(nfe_grupo_set(g, "infProdNFF/cOperNFF", "1"));
fim:
	return rc;
}

/* NFe/infNFe/det/prod/veicProd */
static int caso_073(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "cProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "cEAN", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "xProd", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "NCM", "01"));
	CONFERE(nfe_grupo_set(g, "CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "uCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "qCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnCom", "1.00"));
	CONFERE(nfe_grupo_set(g, "vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "cEANTrib", "SEM GTIN"));
	CONFERE(nfe_grupo_set(g, "uTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "qTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "vUnTrib", "1.00"));
	CONFERE(nfe_grupo_set(g, "indTot", "0"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpOp", "0"));
	CONFERE(nfe_grupo_set(g, "veicProd/chassi", "AAAAAAAAAAAAAAAAA"));
	CONFERE(nfe_grupo_set(g, "veicProd/cCor", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/xCor", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/pot", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/cilin", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/pesoL", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/pesoB", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/nSerie", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpComb", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/nMotor", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/CMT", "EXEMPLO"));
	CONFERE(nfe_grupo_set(g, "veicProd/dist", "1.00"));
	CONFERE(nfe_grupo_set(g, "veicProd/anoMod", "0000"));
	CONFERE(nfe_grupo_set(g, "veicProd/anoFab", "0000"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpPint", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpVeic", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/espVeic", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/VIN", "R"));
	CONFERE(nfe_grupo_set(g, "veicProd/condVeic", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/cMod", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/cCorDENATRAN", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/lota", "1"));
	CONFERE(nfe_grupo_set(g, "veicProd/tpRest", "0"));
fim:
	return rc;
}

/* NFe/infNFe/infPAA */
static int caso_074(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "CNPJPAA", "12345678000195"));
	CONFERE(nfe_grupo_set(g, "PAASignature/SignatureValue", "AQ=="));
	CONFERE(nfe_grupo_set(g, "PAASignature/RSAKeyValue/Modulus", "AQ=="));
	CONFERE(nfe_grupo_set(g, "PAASignature/RSAKeyValue/Exponent", "AQ=="));
fim:
	return rc;
}

/* NFe/infNFe/infPAA/PAASignature */
static int caso_075(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "CNPJPAA", "12345678000195"));
	CONFERE(nfe_grupo_set(g, "PAASignature/SignatureValue", "AQ=="));
	CONFERE(nfe_grupo_set(g, "PAASignature/RSAKeyValue/Modulus", "AQ=="));
	CONFERE(nfe_grupo_set(g, "PAASignature/RSAKeyValue/Exponent", "AQ=="));
fim:
	return rc;
}

/* NFe/infNFe/infPAA/PAASignature/RSAKeyValue */
static int caso_076(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "CNPJPAA", "12345678000195"));
	CONFERE(nfe_grupo_set(g, "PAASignature/SignatureValue", "AQ=="));
	CONFERE(nfe_grupo_set(g, "PAASignature/RSAKeyValue/Modulus", "AQ=="));
	CONFERE(nfe_grupo_set(g, "PAASignature/RSAKeyValue/Exponent", "AQ=="));
fim:
	return rc;
}

/* NFe/infNFe/infSolicNFF */
static int caso_077(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "xSolic", "EXEMPLO"));
fim:
	return rc;
}

/* NFe/infNFe/total/IBSCBSTot/gEstornoCred */
static int caso_078(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMSDeson", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCP", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPSTRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFrete", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vSeg", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vDesc", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vII", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPI", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPIDevol", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vPIS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vCOFINS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vOutro", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vNF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/vBCIBSCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gEstornoCred/vIBSEstCred", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gEstornoCred/vCBSEstCred", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/total/IBSCBSTot/gMono */
static int caso_079(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMSDeson", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCP", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPSTRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFrete", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vSeg", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vDesc", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vII", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPI", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPIDevol", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vPIS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vCOFINS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vOutro", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vNF", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/vBCIBSCBS", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gMono/vIBSMono", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gMono/vCBSMono", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gMono/vIBSMonoReten", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gMono/vCBSMonoReten", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gMono/vIBSMonoRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "IBSCBSTot/gMono/vCBSMonoRet", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/total/ISSQNtot */
static int caso_080(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMSDeson", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCP", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPSTRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFrete", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vSeg", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vDesc", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vII", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPI", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPIDevol", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vPIS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vCOFINS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vOutro", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vNF", "1.00"));
	CONFERE(nfe_grupo_set(g, "ISSQNtot/dCompet", "2026-10-05"));
fim:
	return rc;
}

/* NFe/infNFe/total/retTrib */
static int caso_081(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBC", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vICMSDeson", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCP", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vBCST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPST", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFCPSTRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vProd", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vFrete", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vSeg", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vDesc", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vII", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPI", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vIPIDevol", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vPIS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vCOFINS", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vOutro", "1.00"));
	CONFERE(nfe_grupo_set(g, "ICMSTot/vNF", "1.00"));
	CONFERE(nfe_grupo_set(g, "retTrib/vRetPIS", "1.00"));
fim:
	return rc;
}

/* NFe/infNFe/transp/retTransp */
static int caso_082(nfe_grupo *g)
{
	int rc = 0;
	CONFERE(nfe_grupo_set(g, "modFrete", "0"));
	CONFERE(nfe_grupo_set(g, "retTransp/vServ", "1.00"));
	CONFERE(nfe_grupo_set(g, "retTransp/vBCRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "retTransp/pICMSRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "retTransp/vICMSRet", "1.00"));
	CONFERE(nfe_grupo_set(g, "retTransp/CFOP", "1000"));
	CONFERE(nfe_grupo_set(g, "retTransp/cMunFG", "3550308"));
fim:
	return rc;
}

static const struct caso casos[] = {
	{ "NFe/infNFe/agropecuario/guiaTransito", "agropecuario", caso_000 },
	{ "NFe/infNFe/cana/deduc", "cana", caso_001 },
	{ "NFe/infNFe/det/imposto/COFINS/COFINSOutr", "imposto", caso_002 },
	{ "NFe/infNFe/det/imposto/COFINS/COFINSQtde", "imposto", caso_003 },
	{ "NFe/infNFe/det/imposto/COFINSST", "imposto", caso_004 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gAjusteCompet", "imposto", caso_005 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gCredPresIBSZFM", "imposto",
	  caso_006 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gCredPresOper", "imposto", caso_007 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gCredPresOper/gCBSCredPres", "imposto",
	  caso_008 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gCredPresOper/gIBSCredPres", "imposto",
	  caso_009 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gEstornoCred", "imposto", caso_010 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gALCZFMCBS", "imposto",
	  caso_011 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDevTrib", "imposto",
	  caso_012 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gCBS/gDif", "imposto",
	  caso_013 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSMun/gDevTrib", "imposto",
	  caso_014 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSMun/gDif", "imposto",
	  caso_015 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSMun/gRed", "imposto",
	  caso_016 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSUF/gDevTrib", "imposto",
	  caso_017 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gIBSUF/gDif", "imposto",
	  caso_018 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gTribCompraGov", "imposto",
	  caso_019 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBS/gTribRegular", "imposto",
	  caso_020 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono", "imposto", caso_021 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem", "imposto",
	  caso_022 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoPadrao",
	  "imposto", caso_023 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoRet",
	  "imposto", caso_024 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/gMonoReten",
	  "imposto", caso_025 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdRem/"
	  "gpBioDiferenca",
	  "imposto", caso_026 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem",
	  "imposto", caso_027 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/"
	  "gMonoPadrao",
	  "imposto", caso_028 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/"
	  "gMonoRet",
	  "imposto", caso_029 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/"
	  "gMonoReten",
	  "imposto", caso_030 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gCBSMonoAdValorem/"
	  "gpBioDiferenca",
	  "imposto", caso_031 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem", "imposto",
	  caso_032 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoPadrao",
	  "imposto", caso_033 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoRet",
	  "imposto", caso_034 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/gMonoReten",
	  "imposto", caso_035 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdRem/"
	  "gpBioDiferenca",
	  "imposto", caso_036 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem",
	  "imposto", caso_037 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/"
	  "gMonoPadrao",
	  "imposto", caso_038 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/"
	  "gMonoRet",
	  "imposto", caso_039 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/"
	  "gMonoReten",
	  "imposto", caso_040 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gIBSCBSMono/gIBSMonoAdValorem/"
	  "gpBioDiferenca",
	  "imposto", caso_041 },
	{ "NFe/infNFe/det/imposto/IBSCBS/gTransfCred", "imposto", caso_042 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS02", "imposto", caso_043 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS15", "imposto", caso_044 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS30", "imposto", caso_045 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS51", "imposto", caso_046 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS53", "imposto", caso_047 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS60", "imposto", caso_048 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS61", "imposto", caso_049 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS70", "imposto", caso_050 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMS90", "imposto", caso_051 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMSPart", "imposto", caso_052 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMSSN101", "imposto", caso_053 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMSSN201", "imposto", caso_054 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMSSN202", "imposto", caso_055 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMSSN500", "imposto", caso_056 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMSSN900", "imposto", caso_057 },
	{ "NFe/infNFe/det/imposto/ICMS/ICMSST", "imposto", caso_058 },
	{ "NFe/infNFe/det/imposto/IPI/IPINT", "imposto", caso_059 },
	{ "NFe/infNFe/det/imposto/ISSQN", "imposto", caso_060 },
	{ "NFe/infNFe/det/imposto/PIS/PISOutr", "imposto", caso_061 },
	{ "NFe/infNFe/det/imposto/PIS/PISQtde", "imposto", caso_062 },
	{ "NFe/infNFe/det/obsItem/obsFisco", "obsItem", caso_063 },
	{ "NFe/infNFe/det/prod/arma", "prod", caso_064 },
	{ "NFe/infNFe/det/prod/comb", "prod", caso_065 },
	{ "NFe/infNFe/det/prod/comb/CIDE", "prod", caso_066 },
	{ "NFe/infNFe/det/prod/comb/encerrante", "prod", caso_067 },
	{ "NFe/infNFe/det/prod/comb/origComb", "prod", caso_068 },
	{ "NFe/infNFe/det/prod/detExport", "prod", caso_069 },
	{ "NFe/infNFe/det/prod/detExport/exportInd", "prod", caso_070 },
	{ "NFe/infNFe/det/prod/infProdEmb", "prod", caso_071 },
	{ "NFe/infNFe/det/prod/infProdNFF", "prod", caso_072 },
	{ "NFe/infNFe/det/prod/veicProd", "prod", caso_073 },
	{ "NFe/infNFe/infPAA", "infPAA", caso_074 },
	{ "NFe/infNFe/infPAA/PAASignature", "infPAA", caso_075 },
	{ "NFe/infNFe/infPAA/PAASignature/RSAKeyValue", "infPAA", caso_076 },
	{ "NFe/infNFe/infSolicNFF", "infSolicNFF", caso_077 },
	{ "NFe/infNFe/total/IBSCBSTot/gEstornoCred", "total", caso_078 },
	{ "NFe/infNFe/total/IBSCBSTot/gMono", "total", caso_079 },
	{ "NFe/infNFe/total/ISSQNtot", "total", caso_080 },
	{ "NFe/infNFe/total/retTrib", "total", caso_081 },
	{ "NFe/infNFe/transp/retTransp", "transp", caso_082 },
};
