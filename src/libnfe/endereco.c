/* Copyright (c) 2017, 2018 Gabriel Lampa da Cunha <gabriellampa@gmail.com>
 *
 * This file is part of tooldoce.
 *
 * tooldoce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tooldoce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tooldoce.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <libnfe/defs.h>
#include <libnfe/erros.h>
#include <libnfe/nfe.h>

#include <libnfe/utils.h>
#include <libnfe/endereco.h>



/**
 * pais_s:
 * @cPais: Código do país
 * @xPais: Nome do país
 *
 * País
 */
struct pais_s{
	const char *xPais;
	uint16_t cPais;
};

/**
 * uf_s:
 * @cUF: Código IBGE da UF 
 * @xUF: Nome da UF
 * @pais: Pais
 *
 * Unidade federada
 */
struct uf_s{
	const char *xUF;
	nfe_uf cUF;
	Pais *pais;
};

/**
 * MUNICIPIO:
 * @xMun: Nome do municício 
 * @cMun: Código IBGE do municício 
 * @uf: UF
 *
 * Informação do Município
 */
struct municipio_s{
	const char *xMun;
	uint32_t cMun; /* 7 dígitos (ex.: 3550308) */
	Uf *uf;
} ;

/**
 * ENDERECO:
 * @xLgr: Rua do endereço
 * @nro: Número do endereço na rua
 * @Cpl: Complemento do endereço
 * @xBairro: Bairro do endereço
 * @municipio: Município do endereço
 * @CEP: CEP do endereço
 * @fone: O telefone é usado em algumas tags, juntamente com o endereço
 *
 * Endereço
 */
struct endereco_s{
	char xLgr[NFE_TAM_UTF8(NFE_TAM_XLGR)];
	char nro[NFE_TAM_UTF8(NFE_TAM_NRO)];
	char Cpl[NFE_TAM_UTF8(NFE_TAM_XCPL)];
	char xBairro[NFE_TAM_UTF8(NFE_TAM_XBAIRRO)];
	uint32_t CEP;
	uint64_t fone;
	Municipio *municipio;
	} ;

static void _delPais(Pais* t);
static void _delUf(Uf* t);
static void _delMunicipio(Municipio * t);

static Pais* _newPais(void){
	 Pais temp = {
		.cPais = 1058,
		.xPais = "BRASIL"
	};
	Pais * ptr = (Pais *) malloc( sizeof (struct pais_s));
	if(ptr != NULL){
		memcpy(ptr, &temp, sizeof(struct pais_s));
	}
	return ptr;
}

static Uf* _newUf(void){
	Uf temp = {
		.cUF = 0
	};
	Uf* ptr;

	temp.pais = _newPais();
	if(temp.pais == NULL){
		return NULL;
	}
	ptr = (Uf *) malloc(sizeof (struct uf_s));
	if(ptr == NULL){
		_delPais(temp.pais);
		return NULL;
	}
	memcpy(ptr, &temp, sizeof(struct uf_s));
	return ptr;
}

static Municipio* _newMunicipio(void){
	Municipio temp = {
		.cMun = 0
	};
	Municipio* ptr;

	temp.uf = _newUf();
	if(temp.uf == NULL){
		return NULL;
	}
	ptr = (Municipio *) malloc(sizeof(struct municipio_s));
	if(ptr == NULL){
		_delUf(temp.uf);
		return NULL;
	}
	memcpy(ptr, &temp, sizeof(struct municipio_s));
	return ptr;
}


Endereco * NewEndereco(void){
	Endereco temp = {
		.CEP = 0,
		.fone = 0
	};
	Endereco * ptr;

	temp.municipio = _newMunicipio();
	if(temp.municipio == NULL){
		return NULL;
	}
	ptr = (Endereco *) malloc(sizeof(struct endereco_s));
	if(ptr == NULL){
		_delMunicipio(temp.municipio);
		return NULL;
	}
	memcpy(ptr, &temp, sizeof(struct endereco_s));
	return ptr;	
}

static 	void _delPais(Pais* t){
	if(nfe_ptrnull(t) == 0){
		free(t);
	}
}

static void _delUf(Uf* t){
	if(nfe_ptrnull(t) == 0){
		_delPais(t->pais);
		free(t);
	}
}

static void _delMunicipio(Municipio * t){
	if(nfe_ptrnull(t) == 0){
		_delUf(t->uf);
		free(t);
	}
}

void DelEndereco(Endereco* t){
	if(nfe_ptrnull(t) == 0){
		_delMunicipio(t->municipio);
		free(t);
	}
}

uint32_t GetCEP(Endereco * end){
	int rc;
	rc = nfe_ptrnull(end);
	if (rc == 0){
		return end->CEP;
	}else{
		return 0;
	}
				
}

int  SetCEP(Endereco * end, uint32_t cep){
	int rc;
	rc = nfe_ptrnull(end);
	if (rc == 0){
		end->CEP = cep;
		return 0;
	}else{
		return rc;
	}
}

int SetFone(Endereco* end, uint64_t fone){
	int rc;
	rc = nfe_ptrnull(end);
	if (rc == 0){
		end->fone = fone;
		return 0;
	}else{
		return rc;
	}
}

uint64_t GetFone(Endereco* end){
	int rc;
	rc = nfe_ptrnull(end);
	if (rc == 0){
	 	return	end->fone ;
	}else{
		return 0 ;
	}
}

char* GetLgr(Endereco* end){
	if(nfe_ptrnull(end) != 0){
		return NULL;
	}
	return end->xLgr;
}

char* GetNro(Endereco* end){
	if(nfe_ptrnull(end) != 0){
		return NULL;
	}
	return end->nro;
}

char* GetCpl(Endereco* end){
	if(nfe_ptrnull(end) != 0){
		return NULL;
	}
	return end->Cpl;
}

char* GetBairro(Endereco* end){
	if(nfe_ptrnull(end) != 0){
		return NULL;
	}
	return end->xBairro;
}

Municipio* GetMunicipio(Endereco* end){
	if(nfe_ptrnull(end) != 0){
		return NULL;
	}
	return end->municipio;
}

int SetLgr(Endereco* end, const char* xlgr){
	int rc = nfe_ptrnull(end);
	if(rc != 0){
		return rc;
	}
	return nfe_copia_texto(end->xLgr, sizeof end->xLgr, xlgr, 2, NFE_TAM_XLGR);
}

int SetNro(Endereco* end, const char* nro){
	int rc = nfe_ptrnull(end);
	if(rc != 0){
		return rc;
	}
	return nfe_copia_texto(end->nro, sizeof end->nro, nro, 1, NFE_TAM_NRO);
}

int SetCpl(Endereco* end, const char* cpl){
	int rc = nfe_ptrnull(end);
	if(rc != 0){
		return rc;
	}
	return nfe_copia_texto(end->Cpl, sizeof end->Cpl, cpl, 1, NFE_TAM_XCPL);
}

int SetBairro(Endereco* end, const char* bairro){
	int rc = nfe_ptrnull(end);
	if(rc != 0){
		return rc;
	}
	return nfe_copia_texto(end->xBairro, sizeof end->xBairro, bairro, 2, NFE_TAM_XBAIRRO);
}

/* O endereço passa a ser dono de muni, e o município anterior é liberado */
int SetMunicipio(Endereco* end, Municipio* muni){
	int rc = nfe_ptrnull(end);
	if(rc != 0){
		return rc;
	}
	rc = nfe_ptrnull(muni);
	if(rc != 0){
		return rc;
	}
	if(end->municipio != muni){
		_delMunicipio(end->municipio);
		end->municipio = muni;
	}
	return 0;
}
