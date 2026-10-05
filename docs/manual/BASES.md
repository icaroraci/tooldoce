# Bases oficiais e transições do leiaute

[Manual](README.md) · [Índice XML](INDICE.md) · [Validação](VALIDACAO.md)

Conferência em **05/10/2026**, com libnfe **1.0.0-rc4**, commit `1dd412eebca80dd80d884b3f03a1a31cfaf42279`. O XSD oficial determina nomes, ordem, escolhas, tipos, formatos e ocorrências. MOC, Notas Técnicas e tabelas oficiais complementam condições, cálculos, cadastros e datas de aplicação. Uma regra publicada pode ter cronograma posterior ou depender de um pacote XSD ainda não disponibilizado.

## Estrutura adotada e origem

O [Portal Nacional — Esquemas XML](https://www.nfe.fazenda.gov.br/portal/listaConteudo.aspx?tipoConteudo=Qd+N9D0fbCI=) apresenta o **PL010f v1.04, de 31/08/2026**, como pacote mais recente do leiaute principal NF-e/NFC-e nesta revisão. Os cinco arquivos da base do projeto foram comparados byte a byte com o [pacote oficial](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=8ITFuBLltXs=):

| Arquivo | Papel |
|---|---|
| [nfe_v4.00.xsd](../../tests/schemas/nfe/nfe_v4.00.xsd) | Declara a raiz NFe |
| [leiauteNFe_v4.00.xsd](../../tests/schemas/nfe/leiauteNFe_v4.00.xsd) | Grupos, ordem, escolhas e tipos da nota |
| [tiposBasico_v4.00.xsd](../../tests/schemas/nfe/tiposBasico_v4.00.xsd) | Tipos básicos do documento |
| [DFeTiposBasicos_v1.00.xsd](../../tests/schemas/nfe/DFeTiposBasicos_v1.00.xsd) | Tipos compartilhados, inclusive RTC |
| [xmldsig-core-schema_v1.01.xsd](../../tests/schemas/nfe/xmldsig-core-schema_v1.01.xsd) | Estrutura XMLDSig |

Os schemas complementares de consultas, inutilização e eventos têm pacotes próprios. A [origem por pasta](../../tests/schemas/README.md) identifica versões e também o wrapper de testes `tipos_v4.00.xsd`, que declara grupos isolados extraídos do leiaute. Esse wrapper não é um pacote oficial. Na conferência com o PL010d v1.03, os 16 arquivos complementares de mesmo nome do projeto e daquele pacote que permanecem compatíveis foram idênticos; isso não troca a base principal PL010f por uma anterior.

## Normas consultadas e consequências

O [MOC 7.0](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=LrBx7WT9PuA=) explica o processo; seu Anexo I organiza campos e regras por grupos. Suas descrições de 2020 precisam ser combinadas com as atualizações posteriores. As versões abaixo foram conferidas no [índice oficial de Notas Técnicas](https://www.nfe.fazenda.gov.br/portal/listaConteudo.aspx?tipoConteudo=04BIflQt1aY=). Cada linha registra o efeito relevante para este manual; não constitui um inventário de toda a legislação fiscal.

| Documento conferido | Consequência na documentação / biblioteca |
|---|---|
| [NT 2025.002 v1.52](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=HXPO8VLbh4o=), publicada em 01/10/2026 | RTC, condições dos grupos IBS/CBS/IS e referências de documentos. VC02-14, com produção em 03/11/2026, exige referência por item nas devoluções abrangidas. VC02-05 impede combinar DFeReferenciado e NFref. nItem tem dispensa específica para nota de débito tipo 03. Não generalizar sua ocorrência opcional. |
| [NT 2022.003 v1.11](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=%2FC5jc3RZhNQ%3D) | NFref admite até 999 ocorrências; referência sigilosa tem cNF zerado. As páginas [NFref](NFe/infNFe/ide/NFref.md) e [refNF](NFe/infNFe/ide/NFref/refNF.md) detalham o contrato. |
| [NT 2026.004 v1.01](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=BTZQzgsO9Ws%3D) | CNPJ alfanumérico: preservar texto, letras maiúsculas, zeros e DV. Não converter CNPJ/chave em inteiro. A API e o PL010f incluem esses formatos. |
| [NT 2026.007 v1.10](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=fpJlwzhffcQ=), publicada em 01/10/2026 | Contribuinte exclusivo do IBS/CBS: IE do emitente pode faltar, com verificações cadastrais específicas e autorização centralizada na SVRS. Produção prevista para 03/11/2026; v1.10 ajusta regras de homologação. Não se aplica à NFC-e nesta etapa. A IE já é opcional no XSD adotado, mas `nfe_sefaz_endereco` não identifica automaticamente esse contribuinte nem implementa seu roteamento. |
| [NT 2026.008 v1.00](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=qAoMot6VyiY=), publicada em 01/10/2026 | Anuncia `vUnComLiq`, `vProdLiq`, `vProdLiqTot` e `ICMSPrevistoPagtoAntecip/vICMSPrevisto`. **Esses campos não constam no PL010f adotado nem na API conferida.** A primeira etapa tem homologação em 05/10/2026 e produção em 03/11/2026; NB01-30 tem homologação em 01/02/2027 e produção em 01/03/2027. Atualizar XSD, tabelas, API, cálculos e exemplos quando o pacote correspondente for disponibilizado. |
| [NT 2026.009 v1.00](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=ADlgg2xjMZI=) | Atualiza I08-140 para CFOP, incluindo 1949/2949 nas hipóteses abrangidas e a exceção de retorno simbólico de gás natural. É alteração de regra; o domínio lexical de CFOP não prova a aplicação correta. |
| [NT 2023.003 v1.40](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=JSxhEY85vRU=), publicada em 25/09/2026 | Exceções de CFOP de NFC-e por UF; a atualização do Amazonas tem produção em 05/10/2026. Não transformar uma exceção estadual em permissão para todas as UFs. |
| [NT 2021.003 v1.50](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=ENot2yWE72Q=), publicada em 25/09/2026 | Atualização da validação de GTIN. `SEM GTIN`, comprimento e dígitos não substituem a conferência cadastral e as condições previstas na NT. A biblioteca não consulta o cadastro centralizado de GTIN. |
| [NT 2026.010 v1.00](https://www.nfe.fazenda.gov.br/portal/exibirArquivo.aspx?conteudo=ugn4MYF5PLg=), publicada em 01/10/2026 | DANFE com informações da Reforma Tributária. A libnfe gera XML; não oferece renderizador de DANFE. A geração do documento auxiliar cabe à aplicação ou biblioteca apropriada. |

## O que a revisão comprova

As 171 páginas de estruturas descrevem o leiaute presente na base oficial adotada e têm exemplo serializado por código C real. Foram conferidos campos, sequências, escolhas, acesso público e validação estrutural dos XMLs. Os exemplos usam dados sintéticos; classificação tributária, cadastro do contribuinte, assinatura real e autorização não são comprovados pela validação XSD.

Os avisos de base usam a assinatura normalizada das dependências transitivas, registrada no [manifesto](schema-manifest.json). A assinatura detecta uma mudança no conjunto; não afirma qual campo mudou. Normas podem mudar sem alteração de XSD. Consulte [estado das páginas](ESTADO.md) e [procedimento de atualização](FERRAMENTAS.md).

Anterior: [Manual](README.md) | Próximo: [Instalação](INSTALACAO.md)
