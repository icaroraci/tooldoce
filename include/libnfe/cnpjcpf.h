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

#ifndef LIBNFE_CNPJCPF_H
#define LIBNFE_CNPJCPF_H

/*
 * Validação de CNPJ e CPF (só os caracteres, sem pontuação).
 *
 * CNPJ: 14 posições. As 12 primeiras podem ter dígitos ou letras maiúsculas
 * (CNPJ alfanumérico, IN RFB nº 2.229/2024); as 2 últimas são os dígitos
 * verificadores. Os verificadores são calculados pelo módulo 11, com pesos
 * de 2 a 9 da direita para a esquerda, e cada caractere vale o seu código
 * ASCII menos 48 (0-9 -> 0-9, A-Z -> 17-42); resto 0 ou 1 resulta em 0.
 * Exemplo oficial: 12ABC34501DE35.
 *
 * CPF: 11 dígitos, com os dois verificadores pelo módulo 11.
 *
 * Retornam 0 (válido), E_ISNULL, E_TAMANHO (tamanho errado) ou E_VALOR
 * (caractere inválido, dígito verificador errado ou todos os dígitos
 * iguais).
 */

int nfe_cnpj_validar(const char *cnpj);
int nfe_cpf_validar(const char *cpf);

#endif
