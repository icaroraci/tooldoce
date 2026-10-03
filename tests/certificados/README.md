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

## Servidor de teste da SEFAZ

`servidor.pem` e `servidor_chave.pem` são o certificado e a chave do
servidor HTTPS falso usado em `tests/test_sefaz.c` (`tests/servidor_sefaz.py`,
em 127.0.0.1), também autoassinados e só para os testes. `teste_cert.pem` é a
parte pública de `teste.pfx`, para o servidor aceitar o cliente.

    openssl req -x509 -newkey rsa:2048 -nodes -days 10950 -subj "/CN=127.0.0.1" \
        -addext "subjectAltName=IP:127.0.0.1,DNS:localhost" \
        -keyout servidor_chave.pem -out servidor.pem
    openssl pkcs12 -in teste.pfx -passin pass:teste -nokeys -clcerts | \
        openssl x509 -out teste_cert.pem
