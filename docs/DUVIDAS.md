# Dúvidas para o Gabriel

Perguntas anotadas durante o trabalho autônomo. Responda aqui mesmo (ou na
conversa) e eu sigo a resposta. Enquanto não houver resposta, sigo o
caminho marcado como **provisório**.

## Em aberto

1. **Assinatura digital.** A nota só é aceita pela SEFAZ assinada (XMLDSig),
   com certificado A1 (arquivo .pfx) ou A3 (token/cartão). O `VISAO.md`
   prevê xmlsec1 + OpenSSL. Posso adicionar essas dependências?
   **Provisório:** sigo com as partes que não dependem da assinatura e deixo
   a assinatura (e a transmissão, #57) para depois da sua resposta.
2. **Certificado A3** (decisão pendente no `VISAO.md`): suportar já, ou só
   A1 no começo? **Provisório:** só A1.
3. **DANFE** (decisão pendente no `VISAO.md`): a impressão fica dentro ou
   fora da biblioteca? **Provisório:** fora; não vou mexer nisso.
4. **Totais automáticos.** Hoje quem usa a biblioteca informa os totais da
   nota. Posso acrescentar uma função que some os itens e preencha o
   `ICMSTot`? **Provisório (feito):** sim, como função opcional
   (`nfe_nfe_calcular_totais`), sem mudar o comportamento atual.
5. **Licença LGPL** (#59): continua esperando a autorização do Marcelo.
   Não vou mexer.
