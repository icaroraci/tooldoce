# Contingência e escolha do autorizador

[Manual](README.md) · [ide](NFe/infNFe/ide.md) · [Serviços](SERVICOS.md)

Contingência altera a forma de emissão e pode alterar chave, campos e serviço. A aplicação precisa seguir o procedimento oficial aplicável ao modelo/UF e verificar a disponibilidade do autorizador. A biblioteca representa a opção escolhida e aplica algumas regras locais; não ativa contingência automaticamente.

## Modelo e dados

No [ide](NFe/infNFe/ide.md), tpEmis identifica a forma de emissão. dhCont/xJust têm restrições de presença conforme o tipo. Os setters de data usam instante/fuso; justificativas seguem o comprimento do contrato. A chave inclui tpEmis: recalcular a chave e assinar o XML final depois de definir essa condição.

O [validador local](VALIDACAO.md) cobre emissão normal com dados de contingência indevidos, falta desses dados nas modalidades previstas, SCAN extinta e incompatibilidades entre NF-e/NFC-e e modalidades. Isso não cobre todo o procedimento operacional de contingência.

## SVC e NFC-e offline

`nfe_sefaz_endereco` aceita SVC-AN/SVC-RS para NF-e 55 quando compatível com a UF, além da emissão normal. Consultar a tabela não comprova que a SVC está ativada. O [guia de webservices](../WEBSERVICES.md) identifica fontes e atualização dos endpoints.

A NFC-e 65 offline (`tpEmis=9`) pertence ao projeto [libnfc](https://github.com/icaroraci/libnfc). QR Code, contingência offline e transmissão posterior precisam ser tratados naquele fluxo; não usar a tabela de NF-e 55 como endpoint de NFC-e. O [cancelamento por substituição](EVENTOS.md) é uma operação própria para a nota normal substituída por uma offline, com as condições do contrato.

O contribuinte exclusivo do IBS/CBS da NT 2026.007 tem direcionamento específico à SVRS. Esse direcionamento não deve ser confundido com a decisão de entrar em contingência SVC; veja [bases e cronogramas](BASES.md).

## Resultado desconhecido

Quando a resposta de autorização não chegar, registre a chave e o estado do envio e use os serviços de recibo/consulta conforme o caso. Não deduzir rejeição de um timeout nem atribuir autorização ao XML apenas porque ele foi assinado. O [guia de serviços](SERVICOS.md) mostra consulta e leitura de protocolos; a aplicação controla repetição, conciliação e persistência.

Fontes: [ide.h](api/ide.md), [validar.h](api/validar.md), [tabela de endpoints](../WEBSERVICES.md) e [normas oficiais](BASES.md).

Anterior: [Inutilização](INUTILIZACAO.md) | Próximo: [Exemplos](EXEMPLOS.md)
