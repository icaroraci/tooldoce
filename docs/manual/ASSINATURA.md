# Certificado A1 e assinatura XML

[Manual](README.md) · [assinatura.h](api/assinatura.md) · [Transmissão](SERVICOS.md)

A assinatura implementada é XMLDSig enveloped, RSA-SHA1 e canonicalização C14N, conforme o padrão usado pelo leiaute conferido. O certificado suportado é A1, carregado de `.pfx`/`.p12` ou de memória. A3 não tem implementação nesta versão. Algoritmos de assinatura não devem ser trocados apenas por preferência da aplicação; precisam corresponder ao leiaute do documento.

## Carga e recursos

`nfe_certificado_pfx(caminho, senha, &rc)` ou `nfe_certificado_pfx_memoria(dados, tam, senha, &rc)` devolve um objeto ou NULL e um código de erro. A biblioteca não guarda a senha. `nfe_certificado_titular` fornece texto emprestado; `nfe_certificado_validade` fornece o fim de validade ou `(time_t)-1`. Libere o objeto com `nfe_certificado_free`, depois de destruir conexões que o estejam usando.

O [programa assinar_nfe.c](../../examples/assinar_nfe.c) monta uma nota, carrega o A1, assina e escreve XML. O certificado em `tests/certificados` é de teste, não possui validade fiscal. Para reproduzir o exemplo local:

```sh
make exemplos
./obj/assinar_nfe tests/certificados/teste.pfx teste
```

O [teste de assinatura](../../tests/test_assinatura.c) verifica também documento alterado, senha incorreta e assinatura de outro documento. Não há transmissão nesse exemplo.

## Elemento coberto e posição

| Raiz | Elemento assinado | Atributo de identificação |
|---|---|---|
| NFe | infNFe | Id, com prefixo NFe e chave |
| evento | infEvento | Id do evento |
| inutNFe | infInut | Id da inutilização |

`nfe_assinar_xml` reconhece esses documentos e acrescenta Signature ao fim da raiz. `nfe_assinar_elemento` recebe o nome do filho com Id para outro documento que siga o mesmo padrão; seu contrato não confere namespace/nome da raiz. As funções devolvem um novo XML alocado, com tamanho em bytes. Não altere infNFe ou sua representação depois de assinar; transmita e arquive o documento assinado preservado.

## Estrutura de Signature

A assinatura usa o namespace `http://www.w3.org/2000/09/xmldsig#`, distinto do namespace da NF-e. O [XSD XMLDSig](../../tests/schemas/nfe/xmldsig-core-schema_v1.01.xsd) é a referência da estrutura. Os elementos produzidos pela assinatura da biblioteca cumprem estes papéis:

| Nó | Finalidade |
|---|---|
| Signature/SignedInfo | Informações efetivamente assinadas |
| CanonicalizationMethod | Identifica a canonicalização de SignedInfo |
| SignatureMethod | Identifica RSA-SHA1 |
| Reference/@URI | Referencia o Id do elemento coberto, com `#` |
| Transforms/Transform | Transformação enveloped e canonicalização do conteúdo |
| DigestMethod / DigestValue | Algoritmo e resumo do elemento referenciado |
| SignatureValue | Resultado criptográfico em base64 |
| KeyInfo/X509Data/X509Certificate | Certificado do signatário em base64 |

Signature é criada pelo módulo de assinatura, não pelo motor de preenchimento dos grupos. Um valor base64 que passa no XSD não constitui uma assinatura válida. Isso também vale para valores sintéticos de [infPAA](NFe/infNFe/infPAA.md), cujo conteúdo é outro contexto documental.

## Conferir a assinatura

`nfe_verificar_assinatura` devolve 0 se a assinatura confere com o certificado incluído no documento; `E_VALOR` indica assinatura inválida ou alteração. A verificação não estabelece confiança na cadeia ICP-Brasil nem consulta revogação. `nfe_verificar_assinatura_elemento` exige que a assinatura cubra o filho indicado, para outro documento.

`nfe_certificado_assinar` assina dados binários para usos fora do XML previstos no leiaute, como o QR Code de outra biblioteca. Não substituir uma assinatura XML por essa operação. O retorno binário também é alocado e deve ser liberado.

Fontes: [contrato público](../../include/libnfe/assinatura.h), [implementação](../../src/libnfe/assinatura.c), [XSD oficial](../../tests/schemas/nfe/xmldsig-core-schema_v1.01.xsd) e [MOC/base normativa](BASES.md). A confiança no certificado TLS do servidor é tratada separadamente em [TLS](../TLS.md).

Anterior: [Validação](VALIDACAO.md) | Próximo: [Serviços](SERVICOS.md)
