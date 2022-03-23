Parte 7: Análise Léxica
=======================

> **Autor:** Jack W. Crenshaw, Ph.D. (07/11/1988)<br>
> **Tradução e adaptação:** Felipo Soranz (20/05/2002)

No [último capítulo](06_expressoes_booleanas.md), criamos um compilador que QUASE funciona, exceto pelo fato de estarmos limitados a um caracter por token. O propósito deste seção é nos livrarmos desta restrição, de uma vez por todas. Isto significa que temos que lidar com o conceito de analisador léxico (*lexical scanner*).

Talvez eu deva mencionar porque precisamos de um analisador léxico, afinal conseguimos gerenciar as coisas sem precisar de um até agora. Mesmo quando fizemos uso de tokens de vários caracteres.

A ÚNICA razão, realmente, tem a ver com as palavras-chave. É um fato na vida computacional que a sintaxe para uma palavra-chave tem a mesma forma que a de qualquer outro identificador. Não conseguimos saber até ter a palavra por completo se ela é ou não uma palavra-chave. Por exemplo, a variável IFILE e a palavra chave IF parecem ser exatamente as mesmas, até você chegar ao terceiro caracter. Nos exemplos até agora, fomos sempre capazes de fazer uma decisão, baseados no primeiro caracter do token, mas isto não será possível agora que temos palavras-chaves completas. Temos que saber se uma dada sequência é uma palavra chave antes de começar a processá-la. E é por isto que precisamos de um analisador léxico.

Na última seção, eu também prometi que poderíamos prover tokens normais sem fazer grandes mudanças ao que fizemos até agora. Eu não menti... nós podemos, como você verá em breve. Mas toda vez que eu começo a instalar estes elementos novos no analisador que nós já fizemos, eu tenho maus pressentimentos. A coisa toda começa a parecer como um grande curativo. Eu finalmente descobri o que causava o problema: Eu estava instalando o analisador léxico sem antes explicar qual o significado da análise léxica em si, e quais são as alternativas. Até agora, eu tenho evitado ao máximo lhe passar um monte de teoria, e certamente alternativas também não. Em geral eu não me dou bem com livros que lhe dão 25 maneiras diferentes de fazer algo, mas nenhuma diga de qual jeito é o melhor pras suas necessidades. Eu procurei evitar isto mostrando-lhe apenas UM método, que FUNCIONA.

Mas esta é uma área importante. Enquanto o analisador léxico é dificilmente a parte mais excitante de um compilador, ele frequentemente tem um efeito profundo no efeito do estilo geral ("look & feel") da linguagem, afinal de contas, é a parte mais próxima do usuário. Eu tenho uma estrutura particular em mente para o analisador léxico a ser usada com a linguagem KISS. Ela combina com o estilo que eu quero que a linguagem tenha. Mas pode ser que ele não funcione para a linguagem que VOCÊ está projetando, portanto neste caso, eu acredito que é importante você saber quais são as opções.

Vamos partir novamente da forma usual. Nesta seção vamos nos aprofundar muito mais do que o normal na teoria básica de linguagens e gramáticas. Eu também vou tratar de outras áreas além de compiladores nas quais a análise léxica tem um papel importante. Finalmente, vou mostrar algumas alternativa para a estrutura de um analisador léxico. Então, e somente então, vamos voltar ao nosso analisador sintático do último capítulo. Confie em mim... eu creio que você vai achar que a espera vale a pena. De fato, como os analisadores léxicos tem muitas aplicações fora do mundo dos compiladores, talvez você ache que esta tenha sido a seção mais útil para você.

Análise Léxica
--------------

Análise léxica é o processo de analisar a entrada de caracteres separando-os em seqüências chamadas tokens. A maioria dos textos sobre compiladores começam aqui, e dedicam diversos capítulos discutindo diversas formas de construir analisadores léxicos. Esta abordagem tem seu lugar, mas como você já viu, há muita coisa que pode ser feita sem nunca se preocupar com isso, e no fim das contas o analisador que vamos construir não vai ser muito parecido com o que os livros descrevem. O motivo? A teoria dos compiladores e, consequentemente, os programas resultantes dela, devem lidar com os tipos mais gerais de regras de análise sintática. Nós não. No mundo real, é possível especificar a sintaxe de uma linguagem de forma que um analisador sintático bastante simples vai ser o suficiente. Em como sempre, nosso lema é KISS: Keep It Simple, Stupid!

Tipicamente, a análise léxica é feita em uma parte separada do compilador, então o analisador sintático só vê uma sequência de tokens. Agora, teoricamente não é necessário separar esta função do resto do analisador. Há apenas um conjunto de equações sintáticas que definem a linguagem como um todo, então em teoria podemos escrever o analisador completo em um só módulo.

Por que a separação? A resposta tem bases tanto práticas como teóricas.

Em 1956, Noam Chomsky definiu a "Hierarquia de Chomsky" de gramáticas. Consiste no seguinte:

- Tipo 0: Irrestrita (por exemplo: Português, Inglês, línguas naturais em geral).
- Tipo 1: Sensíveis a contexto
- Tipo 2: Livres de contexto
- Tipo 3: Regular

Algumas características de linguagens de programação (particularmente as mais antigas, como FORTRAN) são do tipo 1, mas a maior parte de todas as linguagens modernas pode ser descrita apenas com os últimos dois tipos, e estes são os que vamos usar aqui.

A parte boa destes dois tipos é que há formas muito específicas de analisá-los. Foi demonstrado que qualquer tipo de gramática regular pode ser analisada  usando um certo tipo de máquina abstrata chamada máquina de estados (ou autômato finito). Nós já implementamos autômatos finitos em alguns de nossos reconhecedores.

De forma similar, gramáticas de tipo 2 (gramáticas livres de contexto ou, *"context-free grammars"*) podem sempre ser analisadas usando-se um autômato de pilha (uma máquina de estados com uma pilha). Nós também já implementamos estas máquinas. Ao invés de implementar a pilha literalmente, nós confiamos na pilha implícita deixando o código recursivo fazer o trabalho, e de fato esta é a abordagem preferida para análise sintática top-down.

Agora, acontece que em gramáticas práticas reais as partes que se classificam como expressões regulares tendem a ser as partes mais "baixo nível", como a definição de um identificador:

~~~ebnf
    <ident> ::= <letter> [ <letter> | <digit> ]*
~~~

Como são necessários tipos diferentes de máquinas abstratas para analisar os dois tipos de gramática, faz sentido separar estas funções de "baixo nível" em um módulo separado, o analisador léxico, que é construído baseado na idéia de máquinas de estado. A idéia é usar a técnica mais simples de reconhecimento para fazer o trabalho.

Há outra razão, mais prática para separar os dois tipos de analisador. Gostamos de pensar no arquivo-fonte de entrada como uma sequência de caracteres, que processados da esquerda pra direita sem precisar de retrocesso ("backtracking"). Na prática isto não é possível. Toda linguagem tem certas palavras-chave como IF, WHILE e END. Como eu já mencionei antes, não há como saber realmente saber se uma certa sequência de caracteres é uma palavra-chave até que encontremos o seu fim, definido por um espaço ou outro delimitador. Então, neste sentido, temos que armazenar a sequência até saber se temos ou não uma palavra-chave. Isto é uma forma limitada de retrocesso.

Então a estrutura convencional de um compilador envolve separar as funções de baixo e alto nível do analisador. O analisador léxico trata das coisas do nível de caracter, coletando os caracteres em seqüências (strings), etc. e passando-as ao analisador léxico apropriadamente como tokens indivisíveis. É também considerado normal deixar que o analisador léxico identifique as palavras-chave.

Máquinas de Estado e Alternativas
---------------------------------

Eu mencionei anteriormente que expressões regulares podem ser analisadas usando uma máquina de estados. Na maioria dos livros sobre compiladores, e de fato na maioria dos compiladores, você vai encontrar isto literalmente. Há tipicamente uma implementação real de uma máquina de estados, com números inteiros usados para definir cada um dos estados, e uma tabela de ações a fazer para cada combinação de estado e caracter de entrada atuais. Se você escrever um "front end" de um compilador usando as populares ferramentas LEX e YACC (ou um de seus equivalentes), é isto que você vai obter. A saída de LEX é uma máquina de estados implementada em C, mais uma tabela de ações correspondentes à entrada da gramática dada a LEX. A saída de YACC é similar... um analisador sintático baseado em tabela, mais a tabela correspondente à sintaxe da linguagem.

Esta não é a única opção, porém. Em nossos capítulos anteriores, você viu repetidamente que é possível implementar analisadores sem lidar especificamente com tabelas, pilhas, ou variáveis de estado. Na verdade, eu avisei vocês no [capítulo 5](05_estruturas_controle.md) que se você acabar precisando destas coisas talvez esteja fazendo algo errado e não esteja tirando proveito do poder da linguagem em que está programando. Há basicamente duas formas de definir o estado de uma máquina de estados: explicitamente, com um código ou número de estado, e implicitamente, simplesmente pelo fato de se estar em um certo lugar no código (se é terça-feira, então aqui deve ser a Bélgica!). Nós confiamos totalmente na abordagem implícita anteriormente, e eu creio que você viu que elas funcionaram muito bem, também.

Na prática pode até ser desnecessário ter um analisador léxico bem definido. Esta não é nossa primeira experiência tratando de tokens de mais de um caracter. No [capítulo 3](03_mais_expressoes.md), nós estendemos nosso analisador para cuidar deles e nós nem sequer precisamos de um analisador léxico. Isto porque naquele contexto estreito, nós podíamos sempre saber, apenas olhando para o próximo caracter, se estávamos lidando com um número, uma variável ou um operador. Nós construímos, na verdade, um analisador léxico distribuído, com as rotinas `GetName()` e `GetNum()`.

Com a presença das palavras-chave, não podemos mais saber com o que estamos lidando até o token inteiro ser lido. Isto nos leva a um analisador mais localizado; porém, como você vai ver, a idéia de um analisador léxico distribuído ainda tem seus méritos.

Algumas Experiências em Análise Léxica
--------------------------------------

Antes de voltar ao nosso compilador, seria útil experimentar um pouco dos conceitos gerais.

Vamos começar com as duas definições mais frequentemente encontradas em linguagens de programação reais:

~~~ebnf
    <ident> ::= <letter> [ <letter> | <digit> ]*
    <number> ::= [<digit>]+
~~~

(Lembre-se, o "*" indica 0 ou mais ocorrências dos termos entre colchetes, e o "+", UMA ou mais.)

Nós já tratamos de itens similares no [capítulo 3](03_mais_expressoes.md). Vamos começar como sempre com o ["berço"](src/cap01-craddle.c).

Vamos escrever duas rotinas, que são muito similares à que usamos antes:


~~~c
/* recebe o nome de um identificador */
void GetName(char *name)
{
    int i;

    if (!isalpha(look))
        Expected("Name");
    for (i = 0; isalnum(look) && i < MAXNAME; i++) {
        name[i] = toupper(look);
        NextChar();
    }
    name[i] = '\0';
}

/* recebe um número inteiro */
void GetNum(char *num)
{
    int i;

    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look) && i < MAXNUM; i++) {
        num[i] = look;
        NextChar();
    }
    num[i] = '\0';
}
~~~



Não esqueça de definir um valor para MAXNAME e MAXNUM.

```c
#define MAXNAME 30
#define MAXNUM 5
```

Note também que `GetNum()` usa uma string não um inteiro.

Você pode verificar facilmente que estas rotinas funcionam fazendo uma chamada no programa principal, por exemplo:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    char name[MAXNAME+1];

    Init();
    GetName(name);
    printf("%s\n", name);

    return 0;
}
~~~

Este programa vai imprimir qualquer identificador válido digitado (no máximo MAXNAME caracteres, conforme limitado em `GetName()`). E vai rejeitar qualquer outra coisa. Teste a outra rotina de forma similar.

Espaço em Branco
----------------

Já tratamos também de espaços em branco antes, usando a rotina `SkipWhite()`:

~~~c
/* pula caracteres de espaço */
void SkipWhite()
{
    while (look == ' ' || look == '\t')
        NextChar();
}
~~~

Adicione `SkipWhite()` no final de `GetName()` e `GetNum()` e esta nova rotina:

~~~c
/* analisador léxico */
void Scan(char *token)
{
    if (isalpha(look))
        GetName(token);
    else if (isdigit(look))
        GetNum(token);
    else {
        token[0] = look;
        token[1] = '\0';
        NextChar();
    }
    SkipWhite();
}
~~~

Podemos chamar esta rotina do programa principal:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    do {
        Scan(token);
        printf("%s\n", token);
    } while (token[0] != '\n');

    return 0;
}
~~~

Você terá que declarar a variável `token` no começo do programa como uma variável global. Coloque em MAXTOKEN um tamanho conveniente.

```c
#define MAXTOKEN 30
char token[MAXTOKEN+1];
```

Agora, execute o programa. Note como a sequência de entrada é, de fato, separada em tokens.

Máquinas de Estados
-------------------

Apenas para constar, uma rotina de análise como `GetName()` já é em si uma implementação de máquina de estados. O estado está implícito na posição atual do código. Um truque bastante útil para visualizar o que está ocorrendo é o diagrama de sintaxe, ou diagrama de "ferrovia". A figura abaixo deve dar uma idéia:

~~~
           ┌──► outro ────────────────► erro
           │
 início ───┴──► letra ────────────┬───► outro ─────► fim
           ▲                      │
           │◄── letra  ◄──────────┤
           │                      │
           └─── dígito ◄──────────┘
~~~

Como você pode ver, este diagrama mostra como a lógica segue conforme os caracteres são lidos. As coisas começam, é claro, em um estado inicial, e terminam quando um outro caracter que não é um caracter alfanumérico é encontrado. Se o primeiro caracter não é alfanumérico, um erro ocorre. Caso contrário a máquina vai continuar repetindo até que um delimitador final seja encontrado (no nosso caso, pode ser o tamanho também).

Em qualquer ponto no fluxo, nossa posição é inteiramente dependente das entradas anteriores. Naquele ponto, a ação a ser tomada depende apenas do estado atual, mais o caracter de entrada atual. É por isso que é uma máquina de estados.

Por ser meio complicado ficar desenhando estes diagramas, vou continuar com as equações sintáticas de agora em diante. Mas eu recomendo fortemente os diagramas pra qualquer coisa que envolva análise sintática/léxica. Depois de um pouco de prática você poderá entender como escrever analisadores diretamente dos diagramas. Caminhos paralelos são codificados como condicionais (com IF ou estruturas CASE -- switch em C), caminhos em sequência são chamadas em sequência.

Nós nem sequer discutimos `SkipWhite()`, que foi introduzido anteriormente, mas é também outro tipo de máquina de estado simples, como também `GetNum()`. E é uma máquina de estados também o pai de todas elas, `Scan()`. Máquinas pequenas fazem máquinas grandes.

A coisa boa nisto tudo que eu quero que você repare é como é indolor esta abordagem implícita para criar máquinas de estado. Eu pessoalmente prefiro esta muito mais que a abordagem baseada em tabelas. Ela também resulta em código menor, mais compacto e rápido para analisadores léxicos.

Quebra de Linha
---------------

Continuando, vamos alterar a maneira como o analisador trata mais de uma linha. Como eu já mencionei da última vez, a maneira mais simples é tratar os caracteres de nova linha como um caracter em branco. Esta é a forma usada pela rotina da biblioteca padrão de C, "isspace". Nós não testamos isto anteriormente. Eu gostaria de testar isto agora, para que você tenha uma idéia dos resultados.

Altere a rotina `SkipWhite()` para:

~~~c
void SkipWhite()
{
    while (isspace(look))
        NextChar();
}
~~~


Precisamos dar ao programa principal uma nova condição de parada, pois ele nunca vai ver o retorno de linha. Vamos usar:

~~~c
    do {
        Scan(token);
        printf("%s\n", token);
    } while (token[0] != '.');
~~~


Compile e teste o programa. Teste uma série de linhas terminadas por um ponto. Eu usei:

~~~
    agora eh tempo
    para os homens de bom coracao.
~~~

Ei, o que aconteceu? Quando eu tentei isto, eu não obtive o último token, o ponto! O programa não parou. E o que é pior, quando eu pressionei ENTER mais algumas vezes, eu ainda não consegui aquele ponto.

Se você ainda estiver no seu programa, vai descobrir que digitar um ponto em uma nova linha vai terminá-lo.

O que está acontecendo aqui? A resposta é que estamos ficando travados em `SkipWhite()`. Uma olhada rápida na rotina vai mostrar que enquanto estivermos entrando com linhas nulas, vamos continuar no laço de repetição. Depois que `SkipWhite()` encontrar um "\n", ele tenta executar um `NextChar()`. Mas como o buffer de entrada está vazio, `NextChar()` insiste em ter uma outra linha. A rotina `Scan()` obtém o ponto, tudo bem, mas então ela chama `SkipWhite()` para finalizar, e `SkipWhite()` não vai retornar até que encontre uma linha que não esteja nula.

Este tipo de comportamento não é tão ruim quanto parece. Em um compilador real, vamos ler a entrada de um arquivo de entrada ao invés do console, e desde que tenhamos alguma rotina para tratar de fim-de-arquivo, tudo vai acabar dando certo. Mas para ler dados do console, este comportamento é muito bizarro. O fato é que a convenção C/Unix não é compatível com a estrutura do nosso compilador, que sempre busca um caracter "lookahead". O código que os magos da Bell implementaram não usa esta convenção, e é por isso que eles usam o `ungetc`.

Certo, vamos arrumar o problema. Para fazer isto, volte para a versão anterior de `SkipWhite()` e faça uso da rotina `NewLine()` que eu introduzi da última vez:

~~~c
/* reconhece uma linha em branco */
void NewLine()
{
    if (look == '\n')
        NextChar();
}
~~~

Se ela não está na versão atual do "berço" acrescente-a agora.

Modifique o programa principal para ficar assim:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    do {
        Scan(token);
        printf("%s\n", token);
        if (token[0] == '\n')
            NewLine();
    } while (token[0] != '.');

    return 0;
}
~~~

Repare no teste de precaução antes da chamada a `NewLine()`. E isto que faz a coisa toda funcionar, e certifica-se que não vamos tentar ler uma linha à frente.

Teste o código. Eu creio que você vai gostar mais agora.

Se você conferir ao código que nós fizemos no [último capítulo](src/cap06-bool2.c), vai descobrir que eu silenciosamente espalhei chamadas a `NewLine()` através do código, onde quer que uma quebra de linha fosse apropriada. Esta é uma das áreas que realmente afetam a aparência e o estilo que eu mencionei anteriormente. Neste ponto eu devo incentivá-lo a experimentar com diferentes arranjos e ver como você prefere. Se você pretende que sua linguagem seja realmente de forma livre, então as quebras de linha devem ser transparentes. Neste caso é melhor colocar estas linhas no começo de `Scan()`:

Se, por outro lado, você quer uma linguagem orientada a linha como assembly, BASIC ou FORTRAN (ou mesmo Ada... note que ela tem comentários terminados por quebra de linha), então você precisa que `Scan()` retorne as quebras de linha como tokens. E também precisa que as quebras de linha finais sejam removidas. A melhor forma de fazer isto é assim, novamente no início de `Scan()`:

~~~c
    if (look == '\n')
        NewLine();
~~~

Para outras convenções, você deve usar outros arranjos. No meu exemplo da última seção, eu permiti quebra de linha apenas em locais específicos, então eu fiquei no território intermediário. No resto destas seções, vou usar modos de tratar de quebras de linha da maneira que achar apropriada, mas eu quero que vocês saibam como escolher outras forma vocês mesmos.

Operadores
----------

Poderíamos para agora e ter um analisador bastante útil para nossos propósitos. Nos fragmentos da linguagem KISS que construímos até agora, os únicos tokens que tem múltiplos caracteres são os identificadores e números. Todos os operadores são de um único caracter. As únicas exceções que consigo pensar são os operadores relacionais, <=, >= e <>, mas eles podem ser tratados como casos especiais.

Além disso, outras linguagens tem operadores multi-caracter. como o ":=" de Pascal ou o "++" e ">>" de C. Então, mesmo não precisando de operadores multi-caracter, é bom saber como tratá-los se necessário.

Não é necessário dizer, podemos tratar de operadores de forma muito similar aos outros tokens. Vamos começar com um reconhecedor, entre com estas rotinas:

~~~c
/* testa se caracter é um operador */
int IsOp(char c)
{
    return (strchr("+-*/<>:=", c) != NULL);
}

/* recebe um operador */
void GetOp(char *op)
{
    int i;

    if (!IsOp(look))
        Expected("Operator");
    for (i = 0; IsOp(look) && i < MAXOP; i++) {
        op[i] = look;
        NextChar();
    }
    op[i] = '\0';
    SkipWhite();
}
~~~

É importante notar que não precisamos incluir todo operador possível nesta lista. Por exemplo, os parênteses não estão incluídos, nem o ponto terminador. A versão atual de `Scan()` trata de operadores de um caracter muito bem. A lista acima inclui apenas os caracteres que podem aparecer em operadores multi-caracter. (Para linguagens específicas a lista pode ser mudada, é claro.)

Agora vamos alterar `Scan()`:

~~~c
void Scan(char *token)
{
    while (look == '\n')
        NewLine();
    if (isalpha(look))
        GetName(token);
    else if (isdigit(look))
        GetNum(token);
    else if (IsOp(look))
        GetOp(token);
    else {
        token[0] = look;
        token[1] = '\0';
        NextChar();
    }
    SkipWhite();
}
~~~

Teste o programa agora. Você vai descobrir que qualquer fragmento que você testar vai ser corretamente separado em tokens individuais.

Listas, Vírgulas e Linhas de Comando
------------------------------------

Antes de voltar ao fluxo principal de nosso estudo, eu gostaria de falar um pouco de algumas idéias.

Quantas vezes você teve que trabalhar com um programa ou sistema operacional que tinha regras muito rígidas a respeito de como separar itens em uma lista? Alguns programas exigem espaços como delimitadores, e outros vírgula. O pior de tudo, alguns requerem os dois, em locais diferentes. A maioria é bem intolerante a respeito de violações de suas regras.

Eu acho que isto é indesculpável. É muito fácil escrever um analisador que é capaz de tratar de espaços e vírgulas de forma flexível. Considere a seguinte rotina:

~~~c

void SkipComma()
{
    SkipWhite();
    if (look == ',') {
        NextChar();
        SkipWhite();
    }
}
~~~

Esta rotina de 8 linhas vai pular um delimitador que consiste de qualquer número (inclusive zero) de espaços, e zero ou uma vírgula em uma sequência de caracteres.

TEMPORARIAMENTE, altere a chamada a `SkipWhite()` em `Scan()` para uma chamada a `SkipComma()`, e experimente entrar com algumas listas. Funciona bem, não é? Você não desejaria que mais autores de software soubessem sobre esse `SkipComma()`?

Apenas para constar, eu descobri que adicionar o equivalente a `SkipComma()` aos meus programas assembly Z80 consumiu apenas 6 bytes extras de código. Mesmo numa máquina de 64k de memória, não é um preço muito alto a pagar em troca de programas mais amigáveis ao usuário!

Eu acho que você já adivinhou aonde eu quero chegar. Mesmo que você nunca escreva uma linha de código de compilador na vida, há lugares em todo programa onde é possível usar os conceitos de análise léxica e sintática. Qualquer programa que processa uma linha de comando precisa deles. De fato, se você pensar nisso um pouco, vai concluir que toda vez que estiver escrevendo um programa que processa entradas de usuários, estará definindo uma linguagem. Pessoas comunicam-se com linguagens, e a sintaxe implícita no seu programa define a linguagem. A verdadeira questão é: você vai definí-la deliberadamente e explicitamente, ou vai apenas deixá-la terminar como o que quer que o programa analíse?

Eu digo que você vai ter um programa melhor e mais amigável se você levar um tempo para definir a sintaxe explicitamente. Escreva as equações sintáticas ou desenhe diagramas, e codifique o analisador usando as técnicas que eu mostrei aqui. Você vai acabar com um programa melhor, e mais fácil de escrever.

Começando a Fantasiar
---------------------

Certo, neste ponto temos um excelente analisador léxico que vai separar a entrada em tokens. Podemos usá-lo como ele está e ter um compilador usável. Mas há outros aspectos de análise léxica que precisamos cobrir.

A principal consideração é eficiência. Lembre-se que quando estávamos tratando de um único caracter como token, todo teste era uma comparação de um único caracter, `look`, com uma constante de caracter. Também usamos bastante o comando "switch".

Com os tokens multi-caracter sendo retornados por `Scan()`, todos aqueles testes vão requerer comparações de strings. Muito mais lento. E não só lento, mas também esquisito, pois não há equivalente do comando "switch" para strings em C. Parece especialmente dispendioso testar o que antes eram meros caracteres... o "=", "+", e outros operadores... usando comparação de strings.

Usar comparação de strings não é impossível... Ron Cain usou esta abordagem escrevendo Small C. Como estamos mantendo o princípio KISS, estaríamos verdadeiramente justificados se mantivéssemos esta abordagem. Mas então eu teria falhado em mostrar uma das abordagens principais em compiladores "reais".

Você deve lembra-se: o analisador léxico vai ser executado MUITAS vezes! Uma vez para cada token no programa-fonte todo. Experimentos indicaram que o compilador comum gasta algo em torno de 20% a 40% de seu tempo em rotinas léxicas. Se há um lugar em que eficiência requer atenção real é aqui.

Por esta razão, a maioria dos programadores de compiladores depositam no analisador léxico um pouco mais de trabalho, "tokenizando " a entrada. A idéia é fazer com que cada token seja comparada com uma lista de palavras-chave e operadores aceitáveis, e retornar um código único para cada um reconhecido. No caso de nomes de variáveis comuns ou números, simplesmente retornamos um código que diz qual tipo de token ele é e colocamos a string real em algum outro lugar.

Uma das primeiras coisas que temos que fazer é arranjar um modo de identificar as palavras-chave. Podemos sempre fazer isto com testes IF, mas certamente isto seria bom se tivéssemos uma rotina de uso geral que compararia uma dada string com uma tabela de palavras-chave. (A propósito, também vamos precisar deste tipo de rotina mais tarde, para lidar com tabelas de símbolos.)

Adicione as seguintes declarações ao programa junto com as outras variáveis globais:

~~~c
/* tabela de definições de símbolos */
#define SYMBOLTABLE_SIZE 1000
char *SymbolTable[SYMBOLTABLE_SIZE];

/* definição de palavras-chave e tipos de token */
#define KEYWORDLIST_SIZE 4
char *KeywordList[KEYWORDLIST_SIZE] = {"IF", "ELSE", "ENDIF", "END"};
~~~

A tabela de símbolos vai ter um tamanho limitado por enquanto. Só pra facilitar as coisas.

Depois insira a seguinte função nova:

~~~c
/* se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int Lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}
~~~


Para testá-la, você pode alterar temporariamente o programa principal como segue:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    GetName(token);
    printf("%d\n", Lookup(token, KeywordList, KEYWORDLIST_SIZE));

    return 0;
}
~~~

Repare como `Lookup()` é chamada: é passado o ponteiro para `token`, `KeywordList` e o tamanho da lista de palavras-chave.

Agora que podemos reconhecer palavras-chave, a próxima coisa a fazer é retornar códigos para elas.

Que tipo de código deveríamos retornar? Há apenas duas escolhas razoáveis. Uma delas parece ser ideal para a aplicação do tipo enumerado. Por exemplo, é possível definir algo assim:

~~~c
/* a ordem deve obedecer a lista de palavras-chave */
enum {
    KW_IF, KW_ELSE, KW_ENDIF, KW_END, 
    TK_IDENT, TK_NUMBER, TK_OPERATOR
};
~~~

e fazer retornar uma destas constantes. Vamos tentar. Insira a linha acima nas suas declarações.

Altere a declaração de `token` anterior para "value" e adicione uma nova:

~~~c
int token;
char value[MAXTOKEN+1];
~~~

Modifique o analisador desta forma:

~~~c
void Scan()
{
    int kw;

    while (look == '\n')
        NewLine();
    if (isalpha(look)) {
        GetName(value);
        kw = Lookup(value, KeywordList, KEYWORDLIST_SIZE);
        if (kw == -1)
            token = TK_IDENT;
        else
            token = kw;
    } else if (isdigit(look)) {
        GetNum(value);
        token = TK_NUMBER;
    } else if (IsOp(look)) {
        GetOp(value);
        token = TK_OPERATOR;
    } else {
        value[0] = look;
        value[1] = '\0';
        token = TK_OPERATOR;
        NextChar();
    }
    SkipWhite();
}
~~~

(Note que agora `Scan()` não tem parâmetros e também, não retorna valor.)

Finalmente altere o programa principal:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    do {
        Scan();
        switch (token) {
            case TK_IDENT:
                printf("Ident: ");
                break;
            case TK_NUMBER:
                printf("Number: ");
                break;
            case TK_OPERATOR:
                printf("Operator: ");
                break;
            case KW_IF:
            case KW_ELSE:
            case KW_ENDIF:
            case KW_END:
                printf("Keyword: ");
                break;
        }
        printf("%s\n", value);
        if (value[0] == '\n')
            NewLine();
    } while (token != KW_END);

    return 0;
}
~~~

O que fizemos foi substituir a string `token` usada anteriormente com um variável inteira com valores enumerados. `Scan()` retorna o tipo na variável `token`, e retorna a string em si na nova variável `value`.

Certo, compile e entre com algumas seqüências. Se tudo der certo, você vai perceber que agora estamos reconhecendo palavras-chave.

O que temos agora, está funcionando perfeitamente, e foi fácil de gerar a partir do que tinhamos anteriormente. De qualquer forma, ainda está um pouco esquisito pra mim. Podemos simplificar as coisas um pouco permitindo que `GetName()`, `GetNum()`, `GetOp()` e `Scan()` trabalharem com as variáveis globais `token` e "value", eliminando portando as cópias locais. E parece também mais "limpo" mover o teste em `Lookup()` dentro de `GetName()`. A nova forma para as rotinas é:

~~~c
/* recebe o nome de um identificador */
void GetName()
{
    int i, kw;

    if (!isalpha(look))
        Expected("Name");
    for (i = 0; isalnum(look) && i < MAXNAME; i++) {
        value[i] = toupper(look);
        NextChar();
    }
    value[i] = '\0';
    kw = Lookup(value, KeywordList, KEYWORDLIST_SIZE);
    if (kw == -1)
        token = TK_IDENT;
    else
        token = kw;
}

/* recebe um número inteiro */
void GetNum()
{
    int i;

    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look) && i < MAXNUM; i++) {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    token = TK_NUMBER;
}

/* recebe um operador */
void GetOp()
{
    int i;

    if (!IsOp(look))
        Expected("Operator");
    for (i = 0; IsOp(look) && i < MAXOP; i++) {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    token = TK_OPERATOR;
}

/* analisador léxico */
void Scan()
{
    int kw;

    while (look == '\n')
        NewLine();
    if (isalpha(look))
        GetName();
    else if (isdigit(look))
        GetNum();
    else if (IsOp(look))
        GetOp();
    else {
        value[0] = look;
        value[1] = '\0';
        token = TK_OPERATOR;
        NextChar();
    }
    SkipWhite();
}
~~~

> Download do analisador léxico [usando constantes](src/cap07-lex-enum.c).

Retornando um caracter
----------------------

Essencialmente todo analisador léxico que eu já vi usou um mecanismo de constantes enumeradas que eu acabei de descrever. Certamente é um mecanismo funcional, mas não parece ser a abordagem mais simples pra mim.

Por um motivo, a lista de possíveis símbolos pode ficar muito grande. Aqui, eu usei apenas um símbolo, "TK_OPERATOR", para se referir a todos os operadores, mas eu já vi outros projetos que usam códigos diferentes para cada um.

Existe, é claro, outro tipo simples que pode ser retornado pelo código: o caracter. Ao invés de retornar uma constante "TK_OPERATOR", o que há de errado em retornar o próprio caracter? Um caracter é uma boa variável também para codificar os diferentes tipos de tokens, ele pode ser usado em comandos switch facilmente, e também é bem mais fácil de digitar. O que seria mais simples?

Além disso, já temos experiência com a idéia de codificar palavras-chave como caracteres simples. Nossos programas anteriores já são escritos desta forma, então usar esta abordagem vai minimizar as mudanças que já fizemos.

Alguns de vocês podem ter o sentimento que essa idéia de retornar caracteres como código é muito "mickey mouse". Eu devo admitir que é um pouco esquisito para operadores multi-caracter como "<=". Se preferir ficar com as constantes enumeradas, tudo bem. Para o resto do pessoal, eu gostaria de mostrar como alterar o que já fizemos acima para suportar esta abordagem.

Primeiro, você pode apagar a declaração das constantes agora... não vamos precisar delas. E pode também alterar o tipo de `token` para caracter.

No lugar das constantes enumeradas, adicione a seguinte constante string:

~~~c
/* a ordem deve obedecer a lista de palavras-chave */
const char *KeywordCode = "ilee";
~~~

Por último modifique `Scan()` e seus parentes, como segue:

~~~c
/* recebe o nome de um identificador */
void GetName()
{
    int i, kw;

    if (!isalpha(look))
        Expected("Name");
    for (i = 0; isalnum(look) && i < MAXNAME; i++) {
        value[i] = toupper(look);
        NextChar();
    }
    value[i] = '\0';
    kw = Lookup(value, KeywordList, KEYWORDLIST_SIZE);
    if (kw == -1)
        token = 'x';
    else
        token = KeywordCode[kw];
}

/* recebe um número inteiro */
void GetNum()
{
    int i;

    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look) && i < MAXNUM; i++) {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    token = '#';
}

/* recebe um operador */
void GetOp()
{
    int i;

    if (!IsOp(look))
        Expected("Operator");
    for (i = 0; IsOp(look) && i < MAXOP; i++) {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    if (strlen(value) == 1)
        token = value[0];
    else
        token = '?';
}

/* analisador léxico */
void Scan()
{
    while (look == '\n')
            NewLine();
    if (isalpha(look))
        GetName();
    else if (isdigit(look))
        GetNum();
    else if (IsOp(look))
        GetOp();
    else {
        value[0] = look;
        value[1] = '\0';
        token = '?';
        NextChar();
    }
    SkipWhite();
}
~~~

Por último, modifique também o programa principal:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    do {
        Scan();
        switch (token) {
            case 'x':
                printf("Ident: ");
                break;
            case '#':
                printf("Number: ");
                break;
            case 'i':
            case 'l':
            case 'e':
                printf("Keyword: ");
                break;
            default:
                printf("Operator: ");
                break;
        }
        printf("%s\n", value);
        if (value[0] == '\n')
            NewLine();
    } while (strcmp(value, "END") != 0);

    return 0;
}
~~~

> Download do analisador léxico [usando caracteres como token](src/cap07-lex-char.c).

Este programa deve funcionar da mesma forma que o anterior. Com uma diferença mínima na estrutura, talvez, mas parece mais simplificado pra mim.

Analisadores Distribuídos X Centralizados
-----------------------------------------

A estrutura para o analisador léxico que eu mostrei é muito convencional, e cerca de 99% de todos os compiladores usam alguma coisa muito parecida com isso. Esta não é, de qualquer forma, a única estrutura possível, nem sequer é a melhor delas.

O problema com esta abordagem convencional é que o analisador não tem noção de contexto. Por exemplo, ele não tem como diferenciar entre o operador de atribuição "=" e o operador relacional "=" (talvez por isso C e Pascal usem operadores diferentes para os dois tipos). Tudo o que o analisador léxico pode fazer é passar o operador ao analisador sintático, o qual esperamos possa resolver em qual contexto o operador está. De forma similar, uma palavra chave como IF não tem lugar no meio de uma expressão matemática, mas se aparecer algum ali, o analisador léxico não vê problema algum, e vai passá-lo para o analisador sintático, propriamente identificado como um "IF".

Com este tipo de abordagem, não estamos fazendo todo uso da informação disponível. No meio de uma expressão, por exemplo, o analisador "sabe" que não tem porque procurar por palavras chave, mas não há como dizer isto ao analisador léxico. Então a análise léxica continua. Isto, logicamente, torna a compilação mais lenta.

Em compiladores "de verdade", os projetistas frequentemente tratam de passar mais informação entre os dois analisadores, para evitar este tipo de problema. Mas isto pode se tornar estranho, e certamente destrói muito da modularidade da estrutura.

A alternativa é procurar uma forma de usar a informação contextual que consiste em saber onde estamos no analisador sintático. Isto nos leva de volta à noção do analisador léxico distribuído, em que várias porções do analisador são chamadas dependendo do contexto.

Na linguagem KISS, como na maioria das linguagens, palavras-chave aparecem apenas no início de um comando. Em lugares como expressões, elas não são permitidas. Com apenas uma pequena exceção (os operadores relacionais multi-caracter) isto é facilmente tratável, todos os operadores são caracteres simples, o que significa que praticamente não precisamos de `GetOp()`.

Então concluímos que mesmo com tokens multi-caracter, podemos sempre saber a partir do caracter lookahead atual exatamente qual tipo de token vem depois, exceto no começo do comando.

Mesmo neste ponto, o ÚNICO tipo de token que podemos aceitar é um identificador. Só precisamos determinar se este identificador é uma palavra-chave ou o alvo de um comando de atribuição.

Acabamos, portanto, precisando apenas de `GetName()` e `GetNum()`, que será usado quase da mesma forma como os usamos nos capítulos anteriores.

Pode parecer a princípio que isto é um passo para trás, e uma abordagem muito primitiva. Na verdade, é uma melhora do analisador léxico clássico, pois estamos usando as rotinas de análise apenas onde elas são realmente necessárias. Em lugares onde palavras-chave não são permitidas, nós não vamos atrasar as coisas procurando por elas.

Juntando os Analisadores Léxico e Sintático
-------------------------------------------

Agora que já cobrimos toda a teoria e aspectos gerais de análise léxica que iríamos precisar, eu finalmente estou pronto para cumprir a promessa que eu fiz de acomodar tokens multi-caracter com mudanças mínimas ao nosso trabalho anterior. Para manter as coisas curtas e simples eu vou me restringir a um subconjunto do que nós já vimos; eu vou permitir apenas uma construção de controle (o IF) e nenhuma expressão booleana. Isto é o suficiente para demonstrar a análise de palavras-chaves e expressões. A extensão para o conjunto completo de construção deve ser bem próxima daquilo que já fizemos.

Todos os elementos do programa para analisar este subconjunto, usando tokens de um caracter, já existem em nossos programas anteriores. Eu o construi copiando cuidadosamente destes programas, mas eu não vou forçar você a passar por todo este processo. Ao invés disso, para evitar confusão, o programa completo está aqui:

~~~c
{% include_relative src/cap07-subset.c %}
~~~

> Download do [subconjunto](src/cap07-subset.c).

Alguns comentários:

- A forma do analisador de expressões, usando `FirstTerm()`, etc, está um pouco diferente do que vimos anteriormente. É uma outra variação no mesmo tema. Mas não deixe que isto atrapalhe você... a mudança não é necessária para o que segue.

- Repare que, como de costume, eu tive que adicionar chamadas a `NewLine()` em pontos estratégicos para permitir múltiplas linhas.

Antes de adicionarmos o analisador léxico, copie o arquivo e verifique que ele analisa as coisas corretamente. Não esqueça dos códigos: "i" para IF, "l" para ELSE, "e" para END ou ENDIF.

Se o programa funcionar, vamos continuar. Quando adicionarmos o módulo do analisador léxico ao programa, ajudaria se tivéssemos um plano sistemático. Em todos os analisadores sintáticos que criamos até agora, ficamos com a convenção de o caracter lookahead atual deveria sempre ser um caracter não-branco. Nós pré-carregamos o caracter em `Init()` e depois acertamos a entrada depois disso. Pra manter as coisas funcionando corretamente, tivemos que tratar as quebras de linha como um token válido.

Na versão multi-caracter, a regra é similar: o caracter lookahead corrente deve sempre ser deixado no começo do próximo token, ou numa nova linha.

A versão multi-caracter é mostrada abaixo. Para chegar nela, tive que fazer as seguintes alterações:

- Adicionar as variáveis `token` e `value`, e as definições usadas por `Lookup()`.
- Adicionar as definições de `KeywordList` e `KeywordCode`.
- Adicionar `Lookup()`.
- Trocar `GetName()` e `GetNum()` por suas versões multi-caracter. (Repare que a chamada a `Lookup()` foi retirada de `GetName()`, para que ela não seja executada em chamadas dentro de `Expression()`.)
- Criar uma nova versão de `Scan()` que chama `GetName()` e testa palavras-chave.
- Criar uma nova rotina, `MatchString()`, que testa uma palavra-chave específica. Repare que, ao contrário de `Match()`, `MatchString()` NÃO lê a próxima palavra-chave.
- Modificar `Block()` para chamar `Scan()`.
- Modificar ligeiramente as chamadas a `NewLine()`. `NewLine()` agora é chamada dentro de `GetName()`.
- Modificar as rotinas para se ajustar às novas versões de `GetName()` e `GetNum()`. Repare também nas pequenas mudanças em `DoIf()`, `Assignment()`, `Block()`, etc.

Aqui está o programa completo:

~~~c
{% include_relative src/cap07-lex-subset.c %}
~~~

> Download do [programa completo](src/cap07-lex-subset.c).

Compare este programa com sua versão de um só caracter. Mesmo com as alterações que fizemos, perceba que as diferenças na estrutura são mínimas.

Conclusão
---------

Neste ponto, você deve ter aprendido a analisar sintaticamente expressões, expressões booleanas, estruturas de controle e aprendeu como gerar código para isto tudo. Aprendeu também como desenvolver analisadores léxicos, e como incorporar seus elementos em um tradutor. Você ainda não viu TODOS os elementos combinados em um único programa, mas baseado no que foi feito você vai descobrir que é fácil estender os programas anteriores para que eles tenham analisadores léxicos.

Estamos muito próximos de ter todos os elementos necessários para construir um compilador real e funcional. Ainda faltam alguns elementos, como chamadas de rotinas e definição de tipos. Nós vamos tratar deles nas próximas seções. Antes de fazer isto, porém, eu acho que seria interessante transformar o tradutor acima em um verdadeiro compilador. É isto que vamos fazer no próximo capítulo.

Até agora, nós usamos uma abordagem bastante "bottom-up" com relação à análise (mas não na estrutura do analisador, por favor, não confunda), começando com construções de baixo nível e melhorando. No próximo capítulo, vamos dar uma olhada num sentido mais "top-down" e vamos discutir como a estrutura do nosso tradutor é alterada por mudanças na definição da linguagem.

Até lá!

{% include footer.md %}