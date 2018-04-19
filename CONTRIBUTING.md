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

## Mantendo um fork

Para contribuir com este projeto enviando alterações, é necessário que você mantenha um fork do mesmo na sua conta pessoal do GitHub. Caso não saiba como fazê-lo, você pode seguir este guia.

0. Crie uma conta no [GitHub](https://github.com/), caso ainda não tenha, e faça login na mesma.
1. Crie seu fork, acessando o [repositório do projeto](https://github.com/icaroraci/tooldoce) e clicando no botão *Fork* no canto superior direito.
2. No seu computador, faça o clone do fork que você acabou de criar com o seguinte comando:

   ```
   $ git clone https://github.com/seu_usuario/tooldoce.git
   ```

   Lembre de usar o caminho correto para o seu repositório, que irá incluir o seu nome de usuário e pode variar um pouco conforme você opte por HTTPS ou SSH.
3. Para facilitar algumas operações, como receber as atualizações do repositório principal para seu fork, adicione um romote para o repositório principal:

   ```
   $ git remote add principal https://github.com/icaroraci/tooldoce.git
   ```

   A configuração do mesmo pode ser verificado como a baixo:

   ```
   $ git remote -v
   origin	git@github.com:seu_usuario/tooldoce.git (fetch)
   origin	git@github.com:seu_usuario/tooldoce.git (push)
   principal	https://github.com/icaroraci/tooldoce.git (fetch)
   principal	https://github.com/icaroraci/tooldoce.git (push)
   ```

   Observe que a saída deste comando pode estar levemente diferente do apresentado aqui, porém se configurado corretamente irá funcionar da mesma forma.

   Outro ponto importante é que para esse repositório que você está configurando agora, `origin` é o nome dado ao seu fork e `principal` é o nome dado ao repositório principal ao qual você fez o fork.


### Atualizando seu fork

Para atualizar o seu fork, você deve baixar os commits do repositório principal, unindo-os ao seu repositório, o que pode ser feito com:

```
$ git checkout master  # Vai para a branch principal do projeto
$ git pull origin master  # Atualiza sua branch `master` para ter certeza de ter baixado todos os commits do seu fork no GitHub
$ git fetch principal master  # Busca os commits que estão na `master` do repositório principal
$ git merge principal/master  # Une os commits baixados com a sua branch `master` local
$ git push origin master  # Envia esses commits ao seu fork no GitHub
```

Caso algum desses comandos apresente algum problema, leia sua mensagem de erro, ou verifique com `git status`, que será explicado o que ocorreu e muitas vezes sugerindo dicas ou até mesmo o comando necessário para a correção do problema.

### Criando uma branch para suas alterações

É interessante fazer suas alterações em uma branch própria para isso (não usar a `master`), desta forma, permite fazer uma segunda alteração, enquanto ainda espera a alteração anterior ser aceita.

Para criar uma branch nova, primeiramente atualize seu fork como descrito anteriormente para evitar ao máximo que ocorram conflitos e ter certeza de que você está trabalhando na versão mais nova do código.

Estando na branch `master` (o que pode ser verificado com o comando `git status` na primeira linha `On branch ...`), escolha um nome descritivo para essa alteração e execute o comando a baixo:

```
$ git checkout -b ajute-do-titulo
```

De proceguimento no desenvolvimento, fazendo seus commits. Quando terminado, envie as alterações para o seu fork:

```
$ git push origin ajute-do-titulo
```

E abra o Pull Request na interface do GitHub (Obrigado por contribuir).

Caso queira testar a versão sem suas modificações, ou queira ir para outra modificação (branch), execute:

```
$ git checkout master  # Sem suas modificações
$ git checkout ajute-do-titulo  # Voltar as suas modificações ou ir para outra modificação
```

Lembrando que para isso você não pode ter feito modificações nos arquivos ou ter já ter feito o commit dos mesmos para que essa operação seja corretamente executada (use `git status` para verificar isso).

Você ainda pode listar suas branches com `git branch`.

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
