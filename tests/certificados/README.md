# Certificado de teste

`teste.pfx` é um certificado **autoassinado, só para os testes**: não foi
emitido pela ICP-Brasil e não tem valor jurídico. A SEFAZ não aceita notas
assinadas com ele.

- Titular: `EMPRESA DE TESTE LTDA:12345678000195` (CNPJ fictício)
- Senha: `teste`
- Validade: até 2056

Gerado com:

    openssl req -x509 -newkey rsa:2048 -keyout chave.pem -out cert.pem \
        -days 10950 -nodes -subj "/C=BR/O=ICP-Brasil/OU=Certificado de TESTE da tooldoce/CN=EMPRESA DE TESTE LTDA:12345678000195"
    openssl pkcs12 -export -in cert.pem -inkey chave.pem -out teste.pfx \
        -passout pass:teste -name teste

**Nunca coloque um certificado real no repositório.**
