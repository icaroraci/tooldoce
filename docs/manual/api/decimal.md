# decimal.h — apoio interno

[Manual](../README.md) · [Índice dos headers](../FUNCOES.md)

Este header instalado declara apenas funções com visibilidade NFE_INTERNO. Elas não são exportadas pela biblioteca e não constituem API de integração. Use os setters dos grupos e o módulo [validar.h](validar.md) para as operações públicas.

[decimal.h](../../../include/libnfe/decimal.h) descreve o apoio interno de cálculo em centavos, com duas casas decimais. A implementação está em [decimal.c](../../../src/libnfe/decimal.c). O texto foi mantido fora do contrato público para evitar orientar uma aplicação a ligar contra símbolos ocultos.
