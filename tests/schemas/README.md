# Schemas da NF-e usados nos testes e nos diagramas

`nfe/` contém os schemas oficiais do leiaute 4.00 da NF-e/NFC-e, do **Pacote de Liberação nº 010f (PL_010f, v1.04, de 31/08/2026)**, publicado no [Portal Nacional da NF-e](https://www.nfe.fazenda.gov.br/portal/listaConteudo.aspx?tipoConteudo=BMPFMBoln3w=). Inclui a NT 2025.002 (IBS/CBS/IS da Reforma Tributária) e o CNPJ alfanumérico. Os arquivos são mantidos sem alteração:

- `leiauteNFe_v4.00.xsd`
- `nfe_v4.00.xsd`
- `tiposBasico_v4.00.xsd`
- `DFeTiposBasicos_v1.00.xsd`
- `xmldsig-core-schema_v1.01.xsd`

`nfe/tipos_v4.00.xsd` **não é oficial**: é gerado por `gerar_tipos_xsd.py` a partir de `leiauteNFe_v4.00.xsd`. Ele inclui o leiaute e declara como elementos globais os grupos que a biblioteca já gera sozinhos (`ide`, `emit`, `dest`, `det`, `prod`, `imposto`, `total`, `transp`, `pag`, `infNFe`, `enderEmit`, `enderDest`), para validá-los nos testes enquanto a biblioteca ainda não gera a nota completa. O CI confere se ele está atualizado.

Os diagramas de `docs/diagramas/` e o `TODO.md` também são gerados a partir destes schemas (`tools/gerar_diagramas.py`).

## Atualizar

1. Baixe o pacote novo no Portal Nacional da NF-e (Documentos › Esquemas XML).
2. Substitua os arquivos oficiais em `nfe/` e atualize a versão do pacote acima.
3. Rode `python3 tests/schemas/gerar_tipos_xsd.py`.
4. Rode `python3 tools/gerar_diagramas.py --todo`.
5. Rode `make test`.
