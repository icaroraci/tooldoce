# Conexão segura com a SEFAZ (TLS)

Na comunicação com os webservices (`sefaz.h`) entram **dois certificados
diferentes**:

| Certificado | De quem | Para quê | Na biblioteca |
|---|---|---|---|
| A1 do emitente (`.pfx`) | da empresa | a SEFAZ saber **quem** está enviando (autenticação do cliente) e assinar os documentos | `nfe_certificado_pfx`, `nfe_sefaz_new` |
| Autoridades certificadoras (AC) | públicos, da ICP-Brasil | a biblioteca conferir que está falando com a SEFAZ **verdadeira** (autenticação do servidor) | `nfe_sefaz_set_ca` |

O segundo não é segredo: são certificados públicos das autoridades
certificadoras. Nunca desative a verificação do servidor para "resolver" um
erro de conexão; a biblioteca nem oferece essa opção.

## O erro "unable to get local issuer certificate"

Vários servidores da SEFAZ usam certificados da ICP-Brasil cuja raiz não vem
instalada no Linux (nem na WSL). Foi o caso da SVRS em homologação
(`nfe-homologacao.svrs.rs.gov.br`) em 03/10/2026: o certificado do servidor é
emitido pela AC SERPRO SSLv1, cuja raiz é a **Autoridade Certificadora Raiz
Brasileira v10**. Sem ela, a conexão falha com:

    SSL certificate problem: unable to get local issuer certificate

## Como resolver

1. Baixe a raiz do repositório oficial do ITI
   (<https://www.gov.br/iti/pt-br/assuntos/repositorio/repositorio-ac-raiz>):

       wget https://acraiz.icpbrasil.gov.br/credenciadas/RAIZ/ICP-Brasilv10.crt

2. Confira a impressão digital (SHA-256) do arquivo baixado com a publicada
   pelo ITI. Em 03/10/2026 era:

       sha256sum ICP-Brasilv10.crt
       6e0bff069a26994c15de2c4888cc54af84882e5495b7fbf66be9ccffec7489f6

3. Converta para PEM:

       openssl x509 -inform DER -in ICP-Brasilv10.crt -out icp-brasil-v10.pem

4. Informe o arquivo à biblioteca, com a verificação ligada:

   ```c
   nfe_sefaz_set_ca(s, "icp-brasil-v10.pem");
   ```

   ou, no exemplo:

       ./obj/status_sefaz empresa.pfx senha <url> 33 icp-brasil-v10.pem

   Se vários servidores usarem raízes diferentes, junte os PEM num só
   arquivo (`cat raiz1.pem raiz2.pem > icp-brasil.pem`).

Para descobrir qual raiz um servidor usa:

    openssl s_client -connect nfe-homologacao.svrs.rs.gov.br:443 -showcerts </dev/null

A última linha `i:` da cadeia mostra a AC raiz. Com a raiz certa,
`openssl s_client ... -CAfile icp-brasil-v10.pem` termina com
`Verify return code: 0 (ok)`.

## Manutenção

As autoridades e os certificados dos servidores mudam com o tempo. Se a
conexão voltar a falhar com o mesmo erro, repita a consulta acima, baixe a
raiz indicada no repositório do ITI e confira de novo a impressão digital.
Não instale certificados obtidos de outras fontes.
