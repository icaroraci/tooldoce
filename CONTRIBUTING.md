# Como contribuir

Estou muito feliz por você estar lendo isso, porque precisamos de desenvolvedores voluntários para ajudar este projeto a se concretizar.

Se você ainda não o fez, venha nos encontrar no [Telegram](https://t.me/LivreDocE). Nós queremos que você trabalhe em coisas que você está animado.
# Contribuindo

Ao contribuir com este repositório, por favor, primeiro discuta a mudança que você deseja fazer através do [issues](https://github.com/icaroraci/tooldoce/issues),
<a href="mailto:gabriellampa@gmail.com">e-mail</a>, Telegram (@GabrielLampa) ou qualquer outro método com os proprietários deste repositório antes de fazer uma alteração.

Por favor, note que temos um [código de conduta](https://github.com/icaroraci/tooldoce/blob/master/CODE_OF_CONDUCT.md), por favor, siga-o em todas as suas interações com o projeto.

## Pull Request Process
## Enviando alterações

Por favor, ao enviar PR's com mudanças significativas, envie com uma lista clara do que você fez. 
Sempre escreva uma mensagem de log clara para seus commits. Mensagens de uma linha são boas para pequenas mudanças, mas mudanças maiores devem ser assim:

    $ git commit -m "Um breve resumo do commit
    >
    > Um parágrafo descrevendo o que mudou e seu impacto. "

## Convenções de codificação
0. [code style](https://github.com/icaroraci/tooldoce/wiki/Code-style)

1. Certifique-se de que quaisquer dependências de instalação ou construção sejam removidas antes do final da camada ao executar um
   builder.
2. Atualize o README.md com detalhes das mudanças na interface, isso inclui novo ambiente
   variáveis, portas expostas, locais de arquivos úteis e parâmetros.

## Fluxo com fork

As alterações chegam ao projeto por pull request a partir de um fork na sua conta do GitHub.

1. Use o botão *Fork* na página do [repositório](https://github.com/icaroraci/tooldoce) para criar a sua cópia.
2. Clone a sua cópia e cadastre o repositório do projeto como um segundo remoto, aqui chamado `upstream`:

   ```
   $ git clone git@github.com:SEU_USUARIO/tooldoce.git
   $ cd tooldoce
   $ git remote add upstream https://github.com/icaroraci/tooldoce.git
   ```

   `git remote -v` deve listar `origin` (a sua cópia) e `upstream` (o projeto).

3. Antes de começar algo novo, traga a `master` do projeto para a sua:

   ```
   $ git switch master
   $ git pull --ff-only upstream master
   $ git push origin master
   ```

4. Trabalhe numa branch com nome descritivo, nunca direto na `master`. Assim você pode ter várias alterações abertas ao mesmo tempo:

   ```
   $ git switch -c corrige-calculo-icms
   ```

5. Faça os commits, rode `make test` e `make verificar-formato`, envie a branch e abra o pull request pelo GitHub:

   ```
   $ git push -u origin corrige-calculo-icms
   ```

Se a `master` do projeto avançar enquanto o pull request está aberto, repita o passo 3 e faça `git merge master` na sua branch. Em caso de dúvida, `git status` mostra em que branch você está e o que falta fazer.

## Código de conduta

### Nosso compromisso

No interesse de promover um ambiente aberto e acolhedor, nós
contribuintes e mantenedores nos comprometemos a fazer a participação de todos em nosso projeto e em
nossa comunidade uma experiência livre de assédios para todos, independentemente da idade, aparência física, deficiência, etnia, identidade e expressão de gênero, nível de experiência,
nacionalidade, aparência pessoal, raça, religião ou identidade sexual e
orientação.

### Nossos Padrões

Exemplos de comportamento que contribuem para criar um ambiente saudável incluem:

* Uso de linguagem acolhedora e inclusiva
* Ser respeitoso com diferentes pontos de vista e experiências
* Graciosamente aceitando críticas construtivas
* Focando no que é melhor para a comunidade
* Mostrando empatia para com outros membros da comunidade

Exemplos de comportamento inaceitável pelos participantes incluem:

* O uso de linguagem ou imagens sexualizadas e atenção sexual indesejada ou
afins
* Trolling/insultos, comentários depreciativos e ataques pessoais ou políticos
Assédio público ou privado
* Publicação de informações privadas de outras pessoas, tais como informações físicas ou eletrônicas.
  endereço, sem permissão explícita
* Outra conduta que poderia razoavelmente ser considerada inadequada em um
  ambiente profissional

### Nossas Responsabilidades

Os mantenedores do projeto são responsáveis ​​por esclarecer os padrões aceitáveis de
comportamento e espera-se que tomem medidas corretivas apropriadas e justas
resposta a quaisquer ocorrências de comportamento inaceitável.

Os mantenedores do projeto têm o direito e a responsabilidade de remover, editar ou
rejeitar comentários, confirmações, códigos, edições do wiki, problemas e outras contribuições
que não estão alinhados com este Código de Conduta, ou banir temporariamente ou
permanentemente qualquer contribuinte para outros comportamentos que eles considerem inadequados,
ameaçador, ofensivo ou prejudicial.

### Escopo

Este Código de Conduta se aplica tanto nos espaços do projeto quanto nos espaços públicos
quando um indivíduo está representando o projeto ou sua comunidade. Exemplos de
representando um projeto ou comunidade incluem o uso de um e-mail oficial do projeto
endereço, postagem por meio de uma conta oficial da mídia social ou agindo como
representante em um evento on-line ou off-line. A representação de um projeto pode ser
definidos e esclarecidos pelos mantenedores do projeto.

### Aplicações

Instâncias de comportamento abusivo, de assédio ou inaceitável podem ser
informados entrando em contato com a equipe do projeto em [gabriellampa@gmail.com]. Todas
reclamações serão analisadas e investigadas e resultarão em uma resposta que
é considerado necessário e apropriado às circunstâncias. A equipe do projeto é
obrigados a manter a confidencialidade em relação ao relator de um incidente.
Detalhes adicionais sobre políticas específicas de execução podem ser publicados separadamente.

Os mantenedores do projeto que não seguem ou aplicam o Código de Conduta em boa
fé pode enfrentar repercussões temporárias ou permanentes, conforme
membros da liderança do projeto.

### Atribuição

Este Código de Conduta é adaptado do [Pacto do Colaborador] [homepage], versão 1.4,
disponível em [http://contributor-covenant.org/version/1/4][version]

[homepage]: http://contributor-covenant.org
[versão]: http://contributor-covenant.org/version/1/4/
