# valida.h — apoio interno

[Manual](../README.md) · [Índice dos headers](../FUNCOES.md)

Este header instalado declara apenas funções com visibilidade NFE_INTERNO. Elas não são exportadas pela biblioteca e não constituem API de integração. Use os setters dos grupos e o módulo [validar.h](validar.md) para as operações públicas.

[valida.h](../../../include/libnfe/valida.h) descreve o apoio interno de validação lexical de padrões, domínios e textos. A implementação está em [valida.c](../../../src/libnfe/valida.c). O texto foi mantido fora do contrato público para evitar orientar uma aplicação a ligar contra símbolos ocultos.
