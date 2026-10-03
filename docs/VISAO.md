# Visão e requisitos do tooldoce

> Documento de referência para decisões técnicas. Mudanças de escopo devem ser registradas aqui antes de virarem código.

## 1. O que é

O tooldoce é uma **biblioteca livre, escrita em C, para emissão de documentos fiscais eletrônicos brasileiros**. Ela é feita para ser usada por outros programas, principalmente ERPs. Não é um programa para o usuário final.

A referência de escopo é o [ACBr](https://projetoacbr.com.br/), mas com uma biblioteca nativa em C, pensada primeiro para Linux e sem dependências proprietárias.

## 2. Para quem

- Desenvolvedores de ERPs e sistemas comerciais, livres ou proprietários.
- Qualquer projeto que precise emitir documentos fiscais eletrônicos e possa ligar (*link*) uma biblioteca C.

O uso é livre, inclusive em software comercial (ver [Licença](#6-licença)).

## 3. O que a biblioteca faz

O escopo cobre o ciclo completo de um documento fiscal eletrônico:

| Etapa | Descrição |
|---|---|
| **Gerar** | Montar o XML do documento a partir dos dados informados pelo programa |
| **Validar** | Conferir o XML contra os schemas (XSD) oficiais e as regras de validação publicadas |
| **Assinar** | Assinar digitalmente (XMLDSig) com certificado ICP-Brasil |
| **Transmitir** | Enviar aos webservices da SEFAZ ou do órgão autorizador (SOAP sobre TLS com certificado) |
| **Tratar o retorno** | Interpretar a resposta (autorização, rejeição, protocolo) e devolvê-la ao programa de forma estruturada |

Também fazem parte do escopo as operações associadas a cada documento, como consulta de situação, eventos (cancelamento, carta de correção) e inutilização de numeração.

### Fora do escopo
- Interface gráfica, telas ou programa para o usuário final.
- Armazenamento dos documentos (banco de dados, arquivamento). A biblioteca entrega o XML; guardá-lo é responsabilidade do programa.
- Regras de negócio do ERP (cálculo de preço, estoque etc.).

### A definir
- **Impressão da DANFE / DACTE / DAMDFE** (representação gráfica dos documentos): pode ser um módulo separado no futuro.

## 4. Documentos suportados e prioridade

Regra: **os documentos mais usados primeiro**. A NF-e vem à frente.

| Prioridade | Documento | Modelo | Situação |
|---|---|---|---|
| 1 | NF-e — Nota Fiscal Eletrônica | 55 | Em desenvolvimento |
| 2 | NFC-e — Nota Fiscal de Consumidor Eletrônica | 65 | Planejado (compartilha a maior parte do layout com a NF-e) |
| 3 | NFS-e — Nota Fiscal de Serviço Eletrônica (padrão nacional) | — | Planejado |
| 4 | CT-e — Conhecimento de Transporte Eletrônico | 57 | Planejado |
| 5 | MDF-e — Manifesto Eletrônico de Documentos Fiscais | 58 | Planejado |
| 6+ | Demais documentos de uso comercial (CT-e OS, BP-e, NF3e, NFCom etc.) | — | Futuro |

A ordem a partir do item 2 é uma proposta e pode ser revista conforme a demanda dos usuários.

## 5. Plataforma e dependências

- **Plataforma nativa: Linux.** Funcionar em outros sistemas (BSD, macOS, Windows) é desejável, mas não é requisito.
- **Linguagem:** C99, sem extensões de compilador obrigatórias. Código específico de sistema operacional deve ficar isolado para facilitar portes.
- **Sem ferramentas ou código proprietário.** Todas as dependências devem ser software livre.

| Dependência | Uso | Licença |
|---|---|---|
| [libxml2](https://gitlab.gnome.org/GNOME/libxml2) | Geração, leitura e validação de XML (XSD) | MIT |
| [xmlsec1](https://www.aleksey.com/xmlsec/) | Assinatura XMLDSig | MIT |
| [OpenSSL](https://www.openssl.org/) 3.x | Certificados e criptografia | Apache 2.0 |
| [libcurl](https://curl.se/libcurl/) (previsto) | Comunicação HTTPS com os webservices | curl (estilo MIT) |

## 6. Licença

Decisão: **LGPL** (GNU Lesser General Public License), para permitir o uso da biblioteca em programas de qualquer licença, inclusive proprietários. Alterações *na própria biblioteca* continuam devendo ser compartilhadas.

Situação: **em transição.** O código atual é GPLv3, e a troca depende da autorização de quem contribuiu com código. Ver a issue [#59](https://github.com/icaroraci/tooldoce/issues/59).

## 7. Requisitos de qualidade

- **API estável e documentada**, com tipos opacos e funções com prefixo próprio.
- **A biblioteca não imprime nada** na tela; erros são devolvidos por códigos e mensagens consultáveis pelo programa.
- **Sem estouro de memória ou de buffer:** todo texto é validado contra o tamanho do campo no layout.
- **Layouts versionados:** os leiautes fiscais mudam com frequência (notas técnicas, Reforma Tributária com IBS/CBS). A estrutura deve permitir atualizar o layout sem reescrever a biblioteca.
- **Testes automáticos** com validação contra os XSDs oficiais e execução no CI a cada mudança.
- **Versionamento semântico** da biblioteca (soname).

## 8. Arquitetura (proposta)

```
programa (ERP)
      │
      ▼
┌──────────────────────────────────────────────┐
│ Módulos por documento: NF-e, NFC-e, NFS-e... │  ← montagem e leitura de cada leiaute
├──────────────────────────────────────────────┤
│ Núcleo comum: XML, validação, assinatura,    │
│ certificados, transporte, erros, utilitários │
└──────────────────────────────────────────────┘
```

O núcleo comum evita reescrever assinatura, transmissão e validação em cada documento. A forma de empacotar (uma biblioteca única ou uma por documento) será decidida quando o segundo documento começar.

## 9. Roteiro

1. **NF-e:** corrigir a base atual (fases 1 a 6 das issues), depois completar os grupos do leiaute, a assinatura e a transmissão (fase 7).
2. Extrair o núcleo comum à medida que a NF-e ficar pronta.
3. Seguir para os demais documentos na ordem da seção 4.

## 10. Decisões pendentes

- [x] Impressão da DANFE: **fora** do escopo (projeto à parte; decisão de 03/10/2026).
- [ ] Suporte a certificado A3 (token/cartão, via PKCS#11) além do A1 (arquivo). Proposta: começar com A1 e deixar a fonte da chave trocável (ver `docs/DUVIDAS.md`).
- [ ] Nome final da biblioteca quando houver mais de um documento (hoje `libnfe`).
- [ ] Ordem de prioridade dos documentos a partir da NFC-e.
