# Schemas da NF-e usados nos testes

`PL_009_V4/` contém os schemas oficiais do leiaute 4.00 da NF-e, publicados no [Portal Nacional da NF-e](https://www.nfe.fazenda.gov.br/) (pacote de liberação PL_009, com as alterações listadas no cabeçalho de `leiauteNFe_v4.00.xsd`, até a NT 2024.003). Os arquivos foram obtidos do espelho mantido pelo projeto [sped-nfe](https://github.com/nfephp-org/sped-nfe/tree/master/schemes/PL_009_V4) e são mantidos sem alteração:

- `leiauteNFe_v4.00.xsd`
- `tiposBasico_v4.00.xsd`
- `xmldsig-core-schema_v1.01.xsd`

`PL_009_V4/ide_v4.00.xsd` **não é oficial**: é gerado por `gerar_ide_xsd.py` a partir de `leiauteNFe_v4.00.xsd`, para validar o grupo `<ide>` isoladamente enquanto a biblioteca ainda não gera a nota completa.

## Atualizar

1. Substitua os arquivos oficiais em `PL_009_V4/` pela versão nova.
2. Rode `python3 tests/schemas/gerar_ide_xsd.py`.
3. Rode `make test`.
