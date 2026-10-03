# Endereços dos webservices da NF-e

`nfe_sefaz_endereco`, declarada em `libnfe/sefaz.h`, consulta a tabela local
da **NF-e modelo 55, versão 4.00**, por UF, ambiente, tipo de emissão e
serviço. Não faz requisições nem precisa de certificado. A tabela abrange
as 27 UFs e os seis serviços de `nfe_servico`.

```c
const char *url = NULL;
int rc = nfe_sefaz_endereco(NFE_UF_RJ, NFE_AMBIENTE_HOMOLOGACAO,
                           NFE_EMISSAO_NORMAL, NFE_SERVICO_AUTORIZACAO,
                           &url);
if (rc == 0) {
        /* Use url em nfe_sefaz_enviar; não libere nem altere a string. */
}
```

Nesse caso, a URL pertence à SVRS. `NFE_EMISSAO_NORMAL` seleciona o
autorizador normal da UF (próprio, SVAN ou SVRS).
`NFE_EMISSAO_CONTINGENCIA_SVC_AN` e `NFE_EMISSAO_CONTINGENCIA_SVC_RS`
selecionam apenas a contingência publicada para aquela UF e ambiente.
A consulta não informa se a contingência está ativada e não troca o
tipo de emissão automaticamente. Tipos FS-DA, EPEC, emissão offline e
outros sem endpoint próprio não são aceitos por esta consulta.

A função retorna `0`, `E_ISNULL` para uma saída nula ou `E_VALOR` para
parâmetros inválidos, contingência incompatível ou serviço não listado.
Em erro, o ponteiro de saída permanece inalterado. Por exemplo, a tabela
oficial não lista inutilização na SVC-RS; a função não inventa uma URL.

`nfe_sefaz_enviar` continua recebendo uma URL explícita. Isso permite
endereços atualizados ou personalizados sem alterar a biblioteca.
Esta tabela não cobre NFC-e (modelo 65), consulta cadastro, distribuição
de DF-e ou eventos destinados ao Ambiente Nacional.

## Fonte e captura

Captura em **3 de outubro de 2026**, da página **Relação de Serviços Web**
do Portal Nacional da NF-e:

- [Produção](https://www.nfe.fazenda.gov.br/portal/webServices.aspx?tipoConteudo=OUC%2FYVNWZfo%3D): [cópia em texto](webservices/producao.txt).
- [Homologação](https://hom.nfe.fazenda.gov.br/portal/webServices.aspx?tipoConteudo=OUC%2FYVNWZfo%3D): [cópia em texto](webservices/homologacao.txt).

As cópias preservam os mapeamentos de UFs, todas as tabelas de serviços,
versões e URLs (inclusive serviços fora do escopo da API). Não incluem
menus e outros elementos de navegação da página.

O gerador remove somente o sufixo `?wsdl` dos links que apontam para a
descrição do serviço, para fornecer o endpoint de envio SOAP. As cópias
em texto preservam os links originais.

**Diferença publicada para PI:** a página de produção o inclui na SVC-AN,
enquanto a página de homologação o inclui na SVC-RS. A tabela preserva
cada ambiente separadamente; não presume que essa diferença seja um
erro nem aplica o mapeamento de produção à homologação. Deve ser
reconferida na fonte ao atualizar os dados.

## Atualização

1. Consulte as duas páginas oficiais e atualize os arquivos em
   `docs/webservices/`, incluindo a data da captura e a URL de origem.
   Cada seção `[AUTORIZADOR]` tem linhas separadas por tabulação:
   serviço, versão, URL. Preserve também os mapeamentos `Usuarios*`.
   Se houver redirecionamentos repetidos, use cookies na navegação.
2. Execute `python3 tools/gerar_enderecos.py`. Não edite manualmente
   `src/libnfe/enderecos_dados.h`.
3. Execute `python3 tools/gerar_enderecos.py --verificar`, `make` e
   `make test`. Revise também as diferenças entre as capturas oficiais
   e os casos de teste de endereços. A validação do gerador exige
   cobertura das 27 UFs, HTTPS e os serviços NF-e 4.00 esperados.
4. O CI confere se a tabela gerada está atualizada. Os testes de
   mapeamento são locais, sem chamadas à SEFAZ.

Nesta WSL, os sanitizers precisaram de executáveis sem PIE. Para forçar
a recompilação mantendo ASan e UBSan:

```sh
make -B -j2 test SANITIZE="-fsanitize=address,undefined -fno-omit-frame-pointer -fno-pie -no-pie"
```
