/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <stddef.h>

#include <libnfe/erros.h>
#include <libnfe/sefaz.h>

#include "enderecos_dados.h"

int nfe_sefaz_endereco(nfe_uf uf, nfe_ambiente amb, nfe_emissao emissao,
                       nfe_servico servico, const char **url)
{
	size_t i;
	int ambiente, autor;
	const char *endereco;

	if (!url)
		return E_ISNULL;
	if ((amb != NFE_AMBIENTE_PRODUCAO && amb != NFE_AMBIENTE_HOMOLOGACAO) ||
	    servico < NFE_SERVICO_AUTORIZACAO ||
	    servico > NFE_SERVICO_INUTILIZACAO ||
	    (emissao != NFE_EMISSAO_NORMAL &&
	     emissao != NFE_EMISSAO_CONTINGENCIA_SVC_AN &&
	     emissao != NFE_EMISSAO_CONTINGENCIA_SVC_RS))
		return E_VALOR;

	ambiente = amb == NFE_AMBIENTE_HOMOLOGACAO;
	for (i = 0; i < sizeof autorizadores / sizeof autorizadores[0]; i++) {
		if (autorizadores[i].uf != uf)
			continue;
		autor = emissao == NFE_EMISSAO_NORMAL
		                ? autorizadores[i].normal[ambiente]
		                : autorizadores[i].contingencia[ambiente];
		if ((emissao == NFE_EMISSAO_CONTINGENCIA_SVC_AN &&
		     autor != AUTOR_SVC_AN) ||
		    (emissao == NFE_EMISSAO_CONTINGENCIA_SVC_RS &&
		     autor != AUTOR_SVC_RS))
			return E_VALOR;
		endereco = enderecos[ambiente][autor][servico];
		if (!endereco)
			return E_VALOR;
		*url = endereco;
		return 0;
	}
	return E_VALOR;
}
