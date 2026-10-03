# Dúvidas para o Gabriel

Perguntas anotadas durante o trabalho autônomo. Responda aqui mesmo (ou na
conversa) e eu sigo a resposta. Enquanto não houver resposta, sigo o
caminho marcado como **provisório**.

## Em aberto

1. **Certificado A3.** Proposta: começar só com o A1 (arquivo .pfx) e
   desenhar a assinatura com uma "fonte da chave" trocável, para o A3
   (token/cartão via PKCS#11, com libp11) entrar depois sem mudar o resto.
   Implicações do A3: o token precisa estar conectado e o PIN digitado na
   hora de assinar (difícil em servidores/nuvem), cada fabricante tem seu
   driver PKCS#11, e os testes precisam de token real (no CI dá para
   simular com SoftHSM). **Provisório:** só A1, com o ponto de troca
   preparado. Confirma?
2. **Totais automáticos.** `nfe_nfe_calcular_totais` soma os itens e
   preenche o `ICMSTot`, como função opcional; quem preferir continua
   informando os totais. **Decisão (03/10):** deixar assim por enquanto e
   manter o assunto aqui.
3. **Motor genérico de grupos.** Para os grupos com muitos campos, em vez
   de um setter para cada campo, gerei do XSD uma tabela com a estrutura de
   cada grupo (`tools/gerar_esquemas.py`) e um motor que valida e escreve
   qualquer campo pelo caminho (`grupo.h`):
   `nfe_grupo_set(nfe_imposto_grupo(imp), "ICMS10/vBC", "100.00")`, com
   listas (`nfe_grupo_add`) e atributos. Os setters tipados dos campos mais
   usados continuam. **Provisório (feito):** uso o motor nos tributos, no
   produto, no transporte, nos totais e nos demais grupos grandes; se
   preferir setters tipados para algum grupo, dá para acrescentar por cima.
4. **Licença LGPL** (#59): continua esperando a autorização do Marcelo.
   Não vou mexer.
5. **Schemas dos webservices (para a transmissão, #57).** Obrigado pelo
   pacote dos eventos da Reforma Tributária (RTC): guardei em
   `tests/schemas/eventos_rtc`. Ele traz só o detalhe de cada evento novo;
   ainda faltam os schemas das mensagens. Pela lista que você mandou do
   portal, devem estar nestes pacotes (pode baixar e mandar os .zip?):
   - "Pacote de Liberação 010d_v1.03 (ZIP) CNPJ Alfanumérico", que deve
     ser o pacote completo, com enviNFe, retEnviNFe, consReciNFe,
     consSitNFe, consStatServ e inutNFe (o 010f parece trazer só os
     arquivos que mudaram);
   - "Evento Cancelamento (Atualizado em 21/12/2018)";
   - "Pacote de Liberação Evento CCe v1.01 (30/05/2014)";
   - "Pacote de Liberação Evento Generico v1.01 (30/05/2014)".
   **Provisório:** a comunicação (SOAP/TLS com o certificado A1), o envio
   do lote, as consultas e o nfeProc já estão feitos e testados com um
   servidor falso; os schemas servirão para validar as mensagens.
   Também não consigo acessar o portal daqui (o acesso é bloqueado), então
   a tabela de endereços dos webservices por UF fica para depois: por
   enquanto, quem usa a biblioteca informa o endereço.
6. **libcurl (para a transmissão).** Para conversar com a SEFAZ por HTTPS
   com o certificado, vou usar a libcurl, cuja licença (curl, no estilo
   MIT) permite uso em programas fechados, como a xmlsec1 e o OpenSSL que
   você já aprovou. **Provisório:** sigo com a libcurl.

## Respondidas

- **Assinatura digital (03/10):** pode usar xmlsec1 (licença MIT) e
  OpenSSL 3.x (Apache 2.0), ambas livres para programas fechados.
  Implementada com certificado A1 (`assinatura.h`).
- **Certificado A1 do Gabriel (03/10):** não precisa ser enviado; os testes
  usam um certificado autoassinado (`tests/certificados`). Para conferir
  com o certificado real, rode `./obj/assinar_nfe empresa.pfx senha` na
  sua máquina e valide a nota num validador de assinatura de NF-e.
- **DANFE (03/10):** fora da biblioteca; é um projeto à parte.
