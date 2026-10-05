# escrita.h — apoio interno de serialização

[Manual](../README.md) · [Índice dos headers](../FUNCOES.md)

Este header instalado contém writers e uma macro usados internamente. As funções são NFE_INTERNO, não exportadas; a macro também depende desses símbolos ocultos. Para integrar uma aplicação, use os writers públicos de cada grupo ou nfe_grupo_write_xml.

[escrita.h](../../../include/libnfe/escrita.h) descreve esse apoio; [escrita.c](../../../src/libnfe/escrita.c) implementa escrita de tags e formatação de datas. A [assinatura](assinatura.md) e a [nota completa](nfe_nfe.md) usam APIs próprias para seus documentos.
