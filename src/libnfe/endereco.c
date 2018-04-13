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

#include<stdint.h>
#include<string.h>
#include<stdlib.h>

#include<libnfe/erros.h>
#include<libnfe/nfe.h>

#include"utils.h"
#include"endereco.h"



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
	enum COD_UF_E cUF;
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
	uint16_t cMun;
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
	const char *xLgr;
	const char* nro;
	const char *Cpl;
	const char *xBairro;
	uint32_t CEP;
	uint64_t fone;
	Municipio *municipio;
	} ;

static Pais* _newPais(){
	 Pais temp = {
		.cPais = 1058,
		.xPais = "BRASIL"
	};
	Pais * ptr = (Pais *) malloc( sizeof (struct pais_s));
	if(ptr == NULL){
		error("Erro ao alocar pais_s",E_NEWPAIS);
	}else{
		memcpy(ptr, &temp, sizeof(struct pais_s));
	}
	return ptr;
};

static Uf* _newUf(){
	Uf temp = {
		.cUF = 0,
		.pais = _newPais()
	};
	Uf* ptr = (Uf *) malloc(sizeof (struct uf_s));
	if(ptr == NULL){
		error("Erro ao alocar uf_s", E_NEWUF);
	}else{
		memcpy(ptr, &temp, sizeof(struct uf_s));
	}
	return ptr;
};

static Municipio* _newMunicipio(){
	Municipio temp = {
		.cMun = 0,
		.uf = _newUf()
	};
	Municipio* ptr = (Municipio *) malloc(sizeof(struct municipio_s));
	if(ptr == NULL){
		error("Erro ao alocar municipio_s", E_NEWMUNICIPIO);
	}else{
		memcpy(ptr, &temp, sizeof(struct municipio_s));
	}
	return ptr;
};


Endereco * NewEndereco(){
	Endereco temp = {
		.CEP = 0,
		.fone = 0,
		.municipio = _newMunicipio()
	};

	Endereco * ptr = (Endereco *) malloc(sizeof(struct endereco_s));
	if(ptr == NULL){
		error("Erro ao alocar endereco_s", E_NEWENDERECO);
	}else{
		memcpy(ptr, &temp, sizeof(struct endereco_s));
	}
	return ptr;	
};

static 	void _delPais(Pais* t){
	if(ptrnull(t) == 0){
		free(t);
	}
};

static void _delUf(Uf* t){
	if(ptrnull(t) == 0){
		_delPais(t->pais);
		free(t);
	}
};

static void _delMunicipio(Municipio * t){
	if(ptrnull(t) == 0){
		_delUf(t->uf);
		free(t);
	}
};

void DelEndereco(Endereco* t){
	if(ptrnull(t) == 0){
		_delMunicipio(t->municipio);
		free(t);
	}
};

unsigned int GetCEP(Endereco * end){
	return end->CEP;
};

unsigned int SetCEP(Endereco * end, unsigned int c){
	end->CEP = c;
};
