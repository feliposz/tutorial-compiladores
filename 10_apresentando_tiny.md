Parte 10: Apresentando "Tiny"
=============================

> **Autor:** Jack W. Crenshaw, Ph.D. (21/05/1989)<br>
> **Tradução e adaptação:** Felipo Soranz (23/05/2002)

Na [última seção](09_uma_visao_do_alto.md), eu lhes mostrei a idéia geral para o desenvolvimento top-down de um compilador. Eu lhe mostrei os primeiros passos no processo para compiladores do estilo "Pascal" e do estilo "C", mas eu parei muito antes de chegarmos a completá-los. A razão é simples: se vamos produzir um compilador real, funcional para alguma linguagem, eu prefiro que seja para KISS, a linguagem que eu venho definindo nesta série de tutoriais.

Nesta seção, nós vamos fazer exatemente isto, para um subconjunto de KISS que eu decidi chamar de TINY.

O processo vai ser essencialmente aquele mostrado no [capítulo 9](09_uma_visao_do_alto.md), exceto por uma diferença notável. Naquele capítulo eu sugeri que você começasse com uma descrição BNF completa da linguagem. Isto é bom para algo como Pascal ou C, para as quais a definição da linguagem é sólida. No caso de TINY, porém, nós ainda não temos uma descrição completa... pois até agora estivemos definindo a linguagem conforme seguimos. Mas não há nada de errado com isso. Na verdade, é preferível, já que podemos adaptar ligeiramente a linguagem conforme avançamos, para manter a análise sintática fácil.

Então, no desenvolvimento a seguir, nós vamos na verdade desenvolver tanto a linguagem quanto o compilador numa abordagem top-down. A descrição BNF vai crescer com o compilador.

Neste processo, haverá um certo número de decisões a serem tomada, cada uma influenciando a BNF e portanto a natureza da linguagem. A cada ponto de decisão eu vou tentar explicar os motivos por trás da minha escolha. Deste modo, se você tiver uma opinião diferente e preferir uma outra opção, você pode escolhê-la. Agora você tem o conhecimento para isto. Eu acho que é importante notar que nada do que fazemos aqui é totalmente definitivo. Quando VOCÊ estiver projetando a SUA linguagem, você deve se sentir livre para fazê-la do SEU jeito.

Muitos de vocês podem perguntar neste ponto: Por que começar tudo de novo? Nós já temos um subconjunto de KISS como produto do [capítulo 7 (análise léxica)](07_analise_lexica.md). Por que não simplesmente estendê-lo conforme necessário? A resposta é tripla. Primeiro, eu estive fazendo algumas mudanças para simplificar ainda mais o programa... mudanças como encapsular as rotinas de geração de código, de forma que possamos converter o programa para outras plataformas alvo mais facilmente. Segundo, eu quero que você veja como o desenvolvimento pode realmente ser feito partindo numa abordagem top-down como foi discutido no último capítulo. Finalmente, nós dois precisamos praticar. Cada vez que eu passo por este exercício eu fico um pouco melhor, e você também ficará.

Começando
---------

Há muitos anos houveram linguagens chamadas Tiny BASIC, Tiny Pascal e Tiny C. Cada uma das quais era um subconjunto de sua língua-mãe completa. Tiny BASIC, por exemplo, possuia apenas variáveis globais cujo nome era definido por uma única letra. Só suportava um único tipo de dados. Soa familiar? Neste ponto nós já temos quase todas as ferramentas necessárias pra construir uma linguagem como esta.

Porém, qualquer linguagem Tiny-alguma-coisa ainda carrega alguma bagagem herdada de sua língua-mãe. Eu sempre fiquei imaginando se isso é uma boa idéia. Certamente, uma linguagem baseada em alguma outra terá a vantagem de ter certa familiaridade, mas pode haver também algum tipo de sintaxe peculiar trazida da linguagem de origem que pode adicionar complexidade desnecessária ao compilador. (Em nenhum lugar isto é mais verdadeiro que em Small C.)

Eu sempre imaginei quão pequeno e simples um compilador poderia ser e ainda assim ser útil, se for projetado para ser ao mesmo tempo fácil de usar e simples de processar. Vamos verificar. Esta linguagem será chamada simplesmente "TINY". É um subconjunto de KISS, a qual eu mesmo não defini completamente, que pelo menos vai nos permitir consistência (!). Eu acho que você pode chamá-la de TINY KISS se quiser, mas eu vou chamá-la simplesmente de TINY. 

As limitações principais de TINY vão ser por causa de coisas que ainda não cobrimos, como tipos de dados. Como suas primas Tiny C e Tiny BASIC, TINY só vai ter um tipo de dados, inteiros de 16-bits. A primeira versão que desenvolvermos também não vai possuir chamadas de rotinas e vai usar nomes de variáveis de uma só letra, embora como você verá podemos remover estas restrições sem muito esforço.

A linguagem que eu tenho em mente vai compartilhar algumas das boas características de Pascal, C, e Ada. Porém, tendo aprendido uma lição na comparação entre compiladores Pascal e C no capítulo anterior, TINY vai ter uma aparência decididamente próxima de Pascal. Sempre que apropriado, uma estrutura de linguagem vai ser delimitada por palavras-chave ou símbolos, para que o analisador saiba para onde está indo sem precisar adivinhar.

Outra regra fundamental: conforme seguimos, eu gostaria de manter o compilador produzindo código real e executável. Mesmo que ele não faça muita coisa no começo, pelo menos ele vai fazer corretamente.

Finalmente, eu vou usar algumas restrições de Pascal que fazem sentido: todos os dados e rotinas devem ser declarados antes de serem usados. Isto faz certo sentido, mesmo que por enquanto o único tipo de dados usado seja o inteiro. Esta regra por sua vez diz que o único lugar em que podemos colocar o código executável para o programa principal é no fim da listagem.

A definição de mais alto nível é similar à de Pascal:

~~~ebnf
    <program> ::= PROGRAM <top-level-decl> <main> '.'
~~~

Nós já chegamos num ponto de decisão. Minha primeira idéia era fazer do bloco principal opcional. Não faz sentido criar um "programa" sem uma rotina principal, mas faz sentido se permitirmos múltiplos módulos, que podem ser combinados depois. De fato, eu pretendo permitir insto em KISS. Mas então estaríamos começando com algo que eu prefiro deixar para depois. O termo PROGRAM não é realmente um nome muito bom. MODULE de Modula-2 ou Unit do Turbo Pascal seria mais apropriado. Em segundo lugar, o que dizer a respeito de regras de escopo? Nós precisaríamos de uma convenção para lidar com a visibilidade dos nomes através dos módulos. Por enquanto, é melhor manter as coisas simples e ignorar esta idéia.

Há também uma decisão com relação a permitir que o programa principal fique somente no fim. Eu trabalhei com a idéia de permitir que sua posição fosse opcional, como em C. A natureza dos montadores como o NASM (que é o que eu estou usando) permitem que isto seja realmente fácil de fazer. Mas isto não faz muito sentido do ponto de vista da regra que estamos usando. Como em Pascal todos os dados e rotinas devem ser declarados antes de referenciados. Como o programa principal só pode chamar subrotinas que já foram declaradas, a única posição que faz sentido é no final, como em Pascal.

Dada a BNF acima, vamos criar o analisador que reconhece apenas os limitadores.

~~~c
/* Analisa e traduz um programa completo */
void Program()
{
    Match('p');
    AsmHeader();
    AsmProlog();
    Match('.');
    AsmEpilog();
}
~~~

A rotina `AsmHeader()` apenas emite o código inicial necessário para o montador:

~~~c
/* Cabeçalho inicial para o montador */
void AsmHeader()
{
    printf("org 100h\n");
    printf("section .data\n");
}
~~~

As rotinas `AsmProlog()` e `AsmEpilog()` emitem o código que identifica o programa principal. O epilogo pode conter alguma inicialização e o prólogo o necessário para retornar ao sistema operacional:

~~~c
/* Emite código para o prólogo de um programa */
void AsmProlog()
{
    printf("section .text\n");
    printf("_start:\n");
}

/* Emite código para o epílogo de um programa */
void AsmEpilog()
{
    EmitLn("MOV AX, 4C00h");
    EmitLn("INT 21h");
}
~~~

O programa principal apenas chama `Program()`, e verifica se o final está correto:

~~~c
/* Programa principal */
int main()
{
    Init();
    Program();

    if (Look != '\n')
        Abort("Unexpected data after \'.\'");

    return 0;
}
~~~

Neste ponto, TINY vai aceitar apenas uma entrada, um programa vazio:

    PROGRAM . (ou "p." no nosso caso.)

Note, porém, que o compilador gera código correto para este programa. Ele pode ser executado, e fazer o que se espera que um programa vazio faça, ou seja, nada a não ser voltar para o sistema operacional.

Apenas por curiosidade, um dos meus testes de eficiência de compiladores favoritos é compilar, ligar (*link edit*), e executar um programa vazio na linguagem envolvida. É possível aprender muito a respeito da implementação medindo o *overhead* de tempo necessário para compilar o que deveria ser um caso trivial. Também é interessante analisar a quantidade de código produzido. Em muitos compiladores, o código pode ser bem grande, porque é incluída sempre a biblioteca de tempo de execução (*run-time library*) sendo ela necessária ou não. As primeiras versões do Turbo Pascal produziam arquivos objeto de cerca de 12K. VAX C gerava 50K, e há casos em que o espaço é ainda maior!

Um dos menores programas vazíos que eu já vi eram produzidos por compiladores de Modula-2, e eles possuiam algo entre 200-800 bytes.

No caso de TINY, não temos uma biblioteca run-time ainda, então o código objeto é de fato minúsculo: não muito mais do que 1K dependendo do montador e do linker que você estiver usando. (Se mudarmos um pouco o código Assembly para produzir programas .COM do DOS, é possível chegar a apenas alguns bytes, o resto é por causa do "overhead" necessário para programas do tipo .EXE). Isto é quase um recorde, e provavelmente será mantido pois é o código mínimo requerido pelo sistema operacional.

O próximo passo é processar o código para o programa principal. Eu vou usar o bloco "BEGIN" do Pascal:

~~~ebnf
    <main> ::= BEGIN <block> END
~~~

Aqui, novamente, tivemos que fazer uma decisão. Poderíamos ter escolhido algum tipo de declaração como "PROCEDURE MAIN", assim como em C. Eu devo admitir que esta não é uma idéia ruim... eu particularmente não gosto da abordagem Pascal pois eu geralmente tenho problemas para encontrar o programa principal em uma listagem Pascal. Mas a alternativa é um pouco esquisita também, já que teríamos que lidar com a ocasião em que o usuário omite o a rotina "main" ou escreve seu nome errado. Eu vou escolher a saída mais fácil neste caso.

Outra solução para o problema de "onde está a rotina principal" pode ser obrigar que o programa tenha um nome, e envolver o programa principal assim:

    BEGIN <name>
    END <name>

similar à convenção de Modula 2. Isto adiciona um pouco de "açúcar sintático" à linguagem. Coisas como esta são fáceis de adicionar ou alterar da forma que você preferir, se o projeto da linguagem está em suas mãos.

Para processar a definição de um bloco principal, altere a rotina `Program()`:

~~~c
/* Analisa e traduz um programa completo */
void Program()
{
    Match('p');
    AsmHeader();
    MainBlock();
    Match('.');
}
~~~

Depois adicione a nova rotina:

~~~c
/* Analisa e traduz o bloco principal */
void MainBlock()
{
    Match('b');
    AsmProlog();
    Match('e');
    AsmEpilog();
}
~~~

Agora, o único program válido é:

    PROGRAM BEGIN END . (ou "pbe.")

Não estamos fazendo progresso??? Bom, como de costume a coisa vai melhorando. Você pode testar alguns erros aqui, como omitir o "b" ou o "e", e ver o que acontece. Como sempre, o compilador deve indicar as entradas ilegais.

Declarações
-----------

O próximo passo é obviamente decidir o que entendemos por uma declaração. Minha intenção aqui é ter dois tipos de declaração: variáveis e procedimentos/funções. No nível mais alto, apenas declarações globais são permitidas, como em C.

Por enquanto só podem haver declarações de variáveis, identificadas pela palavra-chave VAR (abreviado "v"):

~~~ebnf
    <top-level-decls> ::= ( <data-declaration> )*
    <data-declaration> ::= VAR <var-list>
~~~

Note que, como só há um tipo de variável, não há necessidade de declarar o tipo. Mais tarde, para a versão completa de KISS, podemos facilmente adicionar uma declaração de tipo.

A rotina `Program()` fica:

~~~c
/* Analisa e traduz um programa completo */
void Program()
{
    Match('p');
    AsmHeader();
    TopDeclarations();
    MainBlock();
    Match('.');
}
~~~

Agora adicione estas duas rotinas:

~~~c
/* Analisa uma declaração de variável */
void Declaration()
{
    Match('v');
    NextChar();
}

/* Analisa e traduz declarações globais */
void TopDeclarations()
{
    while (Look != 'b') {
        switch (Look) {
            case 'v':
                Declaration();
                break;
            default:
                Error("Unrecognized keyword.");
                Expected("BEGIN");
                break;
        }
    }
}
~~~

Repare que por enquanto, `Declaration()` não faz muita coisa. Não gera nenhum código, e não processa uma lista... cada variável deve ocorrer num comando VAR separado.

Certo, agora podemos ter qualquer número de declarações de dados, cada uma começando com um "v" de VAR, antes do bloco principal. Tente alguns casos e veja o que acontece.

Declarações e Símbolos
----------------------

Isto parece interessante, mas continuamos gerando apenas o programa vazio como saída. Um compilador real iria gerar diretivas assembly para alocar memória para as variáveis. Já é hora de fazermos o mesmo.

Com algum código extra, é algo fácil de fazer na rotina `Declaration()`. Modifique-a como segue:

~~~c
/* Analisa uma declaração de variável */
void Declaration()
{
    Match('v');
    AllocVar(GetName());
}
~~~

A rotina `AllocVar()` simplesmente emite o comando assembly para alocar memória:

~~~c
/* Emite código de alocação de memória para uma variável */
void AllocVar(char name)
{
    printf("%c\tdw 0\n", name);
}
~~~

Faça o teste agora. Tente uma entrada que declara algumas variáveis, como:

    pvxvyvzbe.

Viu como o armazenamento é alocado? Simples, não? Note também que o ponto de entrada `_start`, se encaixa no lugar certo.

Para constar, um compilador "real" também teria uma tabela de símbolos para armazenar as variáveis usadas. Normalmente, a tabela de símbolos é necessária para armazenar o tipo de cada variável. Mas como neste caso todas as variáveis tem o mesmo tipo, não precisamos de uma tabela de símbolos por este motivo. Mas como você vai ver, vamos constatar que um símbolo é necessário mesmo sem a diferença dos tipos, mas vamos por esta questão de lado até que seja necessário.

É claro que ainda não processamos a sintaxe correta para uma declaração de dados, já que ela envolve uma lista de variáveis. Nossa versão permite apenas uma única variável. Isto é fácil de arrumar, também.

A BNF para a lista de variáveis é:

~~~ebnf
    <var-list> ::= <ident> (, <ident>)*
~~~

Adicionando esta sintaxe a `Declaration()` temos sua nova versão:

~~~c
/* Analisa uma lista de declaração de variáveis */
void Declaration()
{
    Match('v');
    for (;;) {
        AllocVar(GetName());
        if (Look != ',')
            break;
        Match(',');
    }
}
~~~

Certo, agora compile este código e faça um teste. Tente uma série de declarações VAR, tente uma lista de diversas variáveis numa só, e tente combinar os dois tipos. Funcionou?

Inicializadores
---------------

Como estamos tratando de declarações de dados, uma coisa que sempre me incomodou em Pascal é que não é possível inicializar os itens de dados na declaração. Esta característica seria uma certo capricho em uma linguagem que é supostamente uma linguagem mínima. Mas em compensação é tão fácil de adicionar que pareceria uma vergonha não fazê-lo. A BNF passa a ser:

~~~ebnf
    <var-list> ::= <var> ( <var> ) *
    <var> ::= <ident> [ = <integer> ]
~~~

Altere `AllocVar()` desta forma:

~~~c
/* Alocação de memória para uma variável global */
void AllocVar(char name)
{
    char value = '0';

    if (Look == '=') {
        Match('=');
        value = GetNum();
    }

    printf("%c\tdw %c\n", name, value);
}
~~~

Aí está: um inicializador com 5 novas linhas de C.

Ok, tente esta versão de TINY e verifique que é possível, de fato, dar às variáveis valores iniciais.

Isto está começando a parecer real! É claro que ainda não faz nada, mas parece bom, não parece?

Antes de deixar esta seção, eu devo lembrá-lo que já usamos duas versões de `GetNum()`. Uma, a primeira, retorna um valor em caracter, um dígito único. A outra aceita valores inteiros multi-dígitos e retorna um valor inteiro. Qualquer uma funcionaria aqui, bastaria alterar "%c" para "%d" em `printf()`. Mas não há razão para nos limitarmos ao dígito único aqui, então vamos corrigir esta versão e retornar inteiros. Aqui está:

~~~c
/* Recebe um número inteiro */
int GetNum()
{
    int num;

    num = 0;

    if (!isdigit(Look))
        Expected("Integer");

    while (isdigit(Look)) {
        num *= 10;
        num += Look - '0';
        NextChar();
    }

    return num;
}
~~~

De fato, deveríamos permitir expressões completas no lugar do analisador, ou pelo menos valores negativos. Por enquanto, vamos permitir apenas valores negativos alterando `AllocVar()` como segue (repare também nas pequenas alterações para a nova versão de `GetNum()`):

~~~c
/* Alocação de memória para uma variável global */
void AllocVar(char name)
{
    int value = 0, signal = 1;

    if (Look == '=') {
        Match('=');
        if (Look == '-') {
            Match('-');
            signal = -1;
        }
        value = signal * GetNum();
    }    

    printf("%c\tdw %d\n", name, value);
}
~~~

Agora, deve ser possível inicializar variáveis com valores negativos e/ou com vários dígitos.

A Tabela de Símbolos
--------------------

Há um problema com o compilador como ele está agora: ele não faz nada para marcar uma variável quando a declaramos. Então o compilador aloca perfeitamente espaço para diversas variáveis com o mesmo nome. Você pode facilmente verificar isto com uma entrada assim:

    pvavavabe.

Aqui declaramos a variável "A" três vezes. Como você pode ver, o compilador aceita sem reclamar, e gera três rótulos idênticos. Nada bom!

Mais tarde, quando começarmos a referenciar variáveis, o compilador também vai permitir que referenciemos variáveis que não existem. O montador vai capturar ambas condições de erro, mas não parece muito amigável passar estes erros para o montador. O compilador deveria identificar estes erros no nível da linguagem fonte.

Portanto, mesmo não possuindo uma tabela de símbolos para armazenar os tipos de dados, precisamos instalar uma para checar estas duas condições. Como neste ponto estamos restritos ainda a nomes de variáveis a tabela de símbolos pode ser trivial. Para provê-la, primeiro adicione as seguintes declarações ao início do programa:

~~~c
/* Tabela de símbolos */
#define VARTABLE_SIZE 26
char VarTable[VARTABLE_SIZE];
~~~

E adicione a seguinte função:

~~~c
/* Verifica se símbolo está na tabela */
int InTable(char name)
{
    return (VarTable[name - 'A'] != ' ');
}
~~~

Também temos que inicializar a tabela com espaços. Adicione a inicialização em `Init()`:

~~~c
/* Inicialização do compilador */
void Init()
{
    int i = 0;

    for (i = 0; i < VARTABLE_SIZE; i++)
        VarTable[i] = ' ';

    NextChar();
}
~~~

Finalmente, insira estas linhas no começo de `AllocVar()`:

~~~c
    if (InTable(name))
        Abort("Duplicate variable name: %c", name);
    else
        VarTable[name - 'A'] = 'v';
~~~

Isto deve bastar. O compilador agora vai reconhecer declarações duplicadas. Mais tarde, também podemos usar `InTable()` quando gerar referências às variáveis.

Comandos executáveis
--------------------

Neste ponto, somos capazes de gerar um programa vazio que tem algumas variáveis de dados declaradas e possivelmente inicializadas. Mas até agora não fizemos nada para começar a gerar a primeira linha de código executável.

Acredite ou não, nós quase temos uma linguagem usável! O que está faltando é o código executável que vai no programa principal. Mas este código consiste em comandos de atribuição e de controle... coisas que já fizemos antes. Portanto, não deve levar muito tempo para adicioná-las também.

A definição BNF dada anteriormente para o programa principal incluia o bloco de comandos, que temos ignorado até então:

~~~ebnf
    <main> ::= BEGIN <block> END
~~~

Por enquanto, podemos considerar o bloco como apenas uma série de comandos de atribuição:

~~~ebnf
    <block> ::= (assignment)*
~~~

Vamos começar as coisas adicionando um analisador para o bloco. Vamos começar com uma rotina para o comando de atribuição vazia:

~~~c
/* Avalia um comando de atribuição */
void Assignment()
{
    NextChar();
}

/* Analisa e traduz um bloco de comandos */
void Block()
{
    while (Look != 'e')
        Assignment();
}
~~~

Modifique a rotina "mainBlock" para chamar "block" conforme abaixo:

~~~c
/* Analisa e traduz o bloco principal */
void MainBlock()
{
    Match('b');
    AsmProlog();
    Block();
    Match('e');
    AsmEpilog();
}
~~~

Esta versão ainda não gera nenhum código para os "comandos de atribuição"... tudo o que ela faz é "engolir" os caracteres até encontrar o "e" de END. Mas ela já é uma preparação para o que segue.

O próximo passo  logicamente é inserir código para o comando de atribuição. Isto é algo que fizemos muitas vezes antes, então eu não vou atrasar as coisas. Desta vez porém, eu gostaria de tratar da geração de código de forma um pouco diferente. Até agora, nós simplesmente emitimos o código para gerar a saída dentro das rotinas do analisador. Um pouco desestruturado, porém, parecia ser a abordagem mais fácil, e tornou fácil ver que tipo de código seria emitido para cada construção.

De qualquer forma, eu percebi que a maioria de vocês gostaria de saber se é possível que o código dependente de CPU poderia ser colocado em outro ponto onde seria mais fácil de portar o código para outra CPU-alvo. A resposta é: **sim, claro!**

Para fazer isto, insira as seguintes rotinas de "geração de código":

~~~c
/* Zera o registrador primário */
void AsmClear()
{
    EmitLn("XOR AX, AX");
}

/* Negativa o registrador primário */
void AsmNegate()
{
    EmitLn("NEG AX");
}

/* Carrega uma constante numérica no registrador primário */
void AsmLoadConst(int i)
{
    EmitLn("MOV AX, %d", i);
}

/* Carrega uma variável no registrador primário */
void AsmLoadVar(char name)
{
    if (!InTable(name))
        Undefined(name);
    EmitLn("MOV AX, [%c]", name);
}

/* Armazena registrador primário em variável */
void AsmStore(char name)
{
    if (!InTable(name))
        Undefined(name);
    EmitLn("MOV [%c], AX", name);
}

/* Coloca registrador primário na pilha */
void AsmPush()
{
    EmitLn("PUSH AX");
}

/* Adiciona topo da pilha ao registrador primário */
void AsmPopAdd()
{
    EmitLn("POP BX");
    EmitLn("ADD AX, BX");
}

/* Subtrai o registrador primário do topo da pilha */
void AsmPopSub()
{
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    EmitLn("NEG AX");
}

/* Multiplica o topo da pilha pelo registrador primário */
void AsmPopMul()
{
    EmitLn("POP BX");
    EmitLn("IMUL BX");
}

/* Divide o topo da pilha pelo registrador primário */
void AsmPopDiv()
{
    EmitLn("POP BX");
    EmitLn("XCHG AX, BX");
    EmitLn("CWD");
    EmitLn("IDIV BX");
}
~~~

A parte boa desta abordagem, logicamente, é que podemos redirecionar a saída do compilador para uma nova CPU simplesmente reescrevendo estas rotinas de geração de código. Além disso, vamos descobrir mais tarde que podemos melhor a qualidade do código simplesmente melhorando estas rotinas um pouco, sem ter que alterar o próprio compilador.

Repare que tanto `AsmLoadVar()` quanto `AsmStore()` verificam a tabela de símbolos para ter certeza de que a variável está definida. O tratador de erros "undefined" simplesmente mostra o erro e saí, da mesma forma que "expect":

~~~c
/* Avisa a respeito de um identificador desconhecido */
void Undefined(char name)
{
    Abort("Undefined identifier %c\n", name);
}
~~~

Certo, finalmente estamos prontos para começar a adicionar código executável. Faremos isto substituindo a versão vazia de `Assignment()`.

Isto deve ser familiar pra você a esta altura, pois é algo que já fizemos várias vezes. Na verdade, exceto pelas alterações associadas ao código gerado, poderíamos simplesmente copiar as rotinas da parte 7. Como estamos fazendo algumas alterações, eu não vou simplesmente copiá-las, mas vamos seguir um pouco mais rápido que o normal.

A BNF para o comando de atribuição é:

~~~ebnf
    <assignment> ::= <ident> '=' <expression>
    <expression> ::= <first-term> ( <addop> <term> )*
    <first-term> ::= <first-factor> <rest>
    <term> ::= <factor> <rest>
    <rest> ::= ( <mulop> <factor> )*
    <first-factor> ::= [ <addop> ] <factor>
    <factor> ::= <var> | <number> | ( <expression> )
~~~

Esta versão da BNF é também um pouco diferente da que usamos anteriormente... mais uma "variação no tema de uma expressão". Esta versão em particular tem o que eu considero ser o melhor tratamento para o menos unário. Como você verá, ela permite que tratemos de constantes negatives de forma eficiente. Vale a pena mencionar aqui que vimos frequentemente as vantagens de "lapidar" a BNF conforme avançamos, para fazer a linguagem ser fácil de processar. O que você viu agora é um pouco diferente, nós alteramos a BNF para fazer a GERAÇÃO DE CÓDIGO mais eficiente! Isto é novidade nesta série.

De qualquer forma, o código a seguir implementa a BNF:

~~~c
/* Analisa e traduz um fator matemático */
void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (isalpha(Look))
        AsmLoadVar(GetName());
    else
        AsmLoadConst(GetNum());
}

/* Analisa e traduz um fator negativo */
void NegFactor()
{
    Match('-');
    if (isdigit(Look))
        AsmLoadConst(-GetNum());
    else {
        Factor();
        AsmNegate();
    }
}

/* Analisa e traduz um fator inicial */
void FirstFactor()
{
    switch (Look) {
        case '+':
            Match('+');
            Factor();
            break;
        case '-':
            NegFactor();
            break;
        default:
            Factor();
            break;
    }
}

/* Reconhece e traduz uma multiplicação */
void Multiply()
{
    Match('*');
    Factor();
    AsmPopMul();
}

/* Reconhece e traduz uma divisão */
void Divide()
{
    Match('/');
    Factor();
    AsmPopDiv();
}

/* Código comum usado por "term" e "firstTerm" */
void TermCommon()
{
    while (IsMulOp(Look)) {
        AsmPush();
        switch (Look) {
          case '*':
            Multiply();
            break;
          case '/':
            Divide();
            break;
        }
    }
}

/* Analisa e traduz um termo matemático */
void Term()
{
    Factor();
    TermCommon();
}

/* Analisa e traduz um termo inicial */
void FirstTerm()
{
    FirstFactor();
    TermCommon();
}

/* Reconhece e traduz uma adição */
void Add()
{
    Match('+');
    Term();
    AsmPopAdd();
}

/* Reconhece e traduz uma subtração*/
void Subtract()
{
    Match('-');
    Term();
    AsmPopSub();
}

/* Analisa e traduz uma expressão matemática */
void Expression()
{
    FirstTerm();
    while (IsAddOp(Look)) {
        AsmPush();
        switch (Look) {
          case '+':
            Add();
            break;
          case '-':
            Subtract();
            break;
        }
    }
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    Expression();
    AsmStore(name);
}
~~~

Agora que você já inseriu todo este código, compile-o e verifique o resultado. Você deve ter agora um código que parece razoável, representando um program completo que vai ser montado corretamento e executado. Temos um compilador!

Expressões Booleanas
--------------------

O próximo passo também deve ser familiar pra você. Temos que incluir expressões booleanas e operadores relacionais. Novamente, como já lidamos com isto mais de uma vez, eu não vou elaborar muito em cima deles, exceto onde existirem diferenças do que já fizemos. Novamente, nós não vamos simplesmente copiar de outros arquivos pois eu mudei um pouco as coisas. A maioria da mudanças envolveram apenas encapsular a parte dependente de máquina como fizemos para as operações aritméticas. Eu também modifiquei a rotina `NotFactor()`, para ficar semelhante à estrutura de `FirstFactor()`.

Para começar, vamos precisar de mais alguns reconhecedores:

~~~c
/* Reconhece um operador OU */
int IsOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* Reconhece operadores relacionais */
int IsRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}
~~~

Também vamos precisar de mais rotinas de geração de código:

~~~c
/* Inverte registrador primário */
void AsmNot()
{
    EmitLn("NOT AX");
}

/* Aplica "E" binário ao topo da pilha com registrador primário */
void AsmPopAnd()
{
    EmitLn("POP BX");
    EmitLn("AND AX, BX");
}

/* Aplica "OU" binário ao topo da pilha com registrador primário */
void AsmPopOr()
{
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* Aplica "OU-exclusivo" binário ao topo da pilha com registrador primário */
void AsmPopXor()
{
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}

/* Compara topo da pilha com registrador primário */
void AsmPopCompare()
{
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
}

/* Altera registrador primário (e flags, indiretamente) conforme a comparação */
void AsmRelOp(char op)
{
    char *jump;
    int l1, l2;

    l1 = NewLabel();
    l2 = NewLabel();

    switch (op) {
        case '=': jump = "JE"; break;
        case '#': jump = "JNE"; break;
        case '<': jump = "JL"; break;
        case '>': jump = "JG"; break;
    }

    EmitLn("%s L%d", jump, l1);
    EmitLn("XOR AX, AX");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}
~~~

Estas são as ferramentas de que precisamos. Repare que eu substitui as rotinas dos operadores relacionais por uma só rotina para todos. Desta forma evitamos duplicar o código. Insira também a declaração de `LabelCount`, `NewLabel()`, `PostLabel()` das seções anteriores já que são necessárias nesta rotina.

A BNF para expressões booleanas é:

~~~ebnf
    <bool-expr> ::= <bool-term> ( <orop> <bool-term> )*
    <bool-term> ::= <not-factor> ( <andop> <not-factor> )*
    <not-factor> ::= [ '!' ] <relation>
    <relation> ::= <expression> [ <relop> <expression> ]
~~~

Leitores atentos devem notar que esta sintaxe não inclui o não-terminal "bool-factor" usado nas versões anteriores. Ele foi necessário porque eu permitia as constantes booleanas VERDADEIRO e FALSO. Mas lembre que em TINY não há distinção feita entre tipos booleanos e aritméticos... eles podem ser livremente misturados. Então não há uma necessidade real para estes valores pré-definidos... podemos simplesmente usar -1 e 0, respectivamente.

Em terminologia de C, poderíamos usar os "defines":

~~~c
#define TRUE -1
#define FALSE 0
~~~

(Isto é, se TINY tivesse um pré-processador.) Mais tarde, quando permitirmos declaração de constantes, estes valores serão pré-definidos pela linguagem.

A razão pela qual eu estou falando disso é por que eu já tentei a alternativa, que é incluir TRUE e FALSE como palavras-chave. O problema com esta abordagem é que ela requer análise léxica para CADA nome de variável em CADA expressão. Se você recordar, eu demonstrei no capítulo 7 que isto pode tornar o compilador consideravelmente mais lento. Como palavras-chave não podem estar em expressões, precisamos fazer a análise léxica apenas no começo de cada novo comando... uma grande melhoria. Portanto, usando a sintaxe acima, não apenas simplificamos a análise sintática, mas melhoramos a própria análise léxica também.

Certo, assumindo que estamos satisfeitos com a sintaxe acima, o código correspondente é o mostrado abaixo:

~~~c
/* Analisa e traduz uma relação */
void Relation()
{
    char op;

    Expression();
    if (IsRelOp(Look)) {
        op = Look;
        Match(op); /* Só para remover o operador do caminho */
        AsmPush();
        Expression();
        AsmPopCompare();
        AsmRelOp(op);
    }
}

/* Analisa e traduz um fator booleano com NOT inicial */
void NotFactor()
{
    if (Look == '!') {
        Match('!');
        Relation();
        AsmNot();
    } else
        Relation();
}

/* Analisa e traduz um termo booleano */
void BoolTerm()
{
    NotFactor();
    while (Look == '&') {
        AsmPush();
        Match('&');
        NotFactor();
        AsmPopAnd();
    }
}

/* Reconhece e traduz um operador OR */
void BoolOr()
{
    Match('|');
    BoolTerm();
    AsmPopOr();
}

/* Reconhece e traduz um operador XOR */
void BoolXor()
{
    Match('~');
    BoolTerm();
    AsmPopXor();
}

/* Analisa e traduz uma expressão booleana */
void BoolExpression()
{
    BoolTerm();
    while (IsOrOp(Look)) {
        AsmPush();
        switch (Look) {
          case '|':
              BoolOr();
              break;
          case '~':
              BoolXor();
              break;
        }
    }
}
~~~

Repare que eu alterei `Relation()` para refletir a estrutura usada por `AsmRelOp()`. No fim acabamos economizando 8 rotinas e não duplicamos código.

Para juntar tudo, não esqueça de alterar as referências para `Expression()` nas rotinas `Factor()` e `Assignment()` para que elas chamem `BoolExpression()` agora.

OK, se você já entrou com tudo isto, compile e faça um teste. Primeiro, certifique-se que ainda é possível usar expressões aritméticas. Então, teste uma booleana. Finalmente, certifique-se que é possível atribuir o resultado de relações. Tente, por exemplo:

    pvx,y,zbx=z>ye.

Que significa:

    PROGRAM
    VAR X, Y, Z
    BEGIN
    X = Z > Y
    END.

Viu como isto atribui um valor booleano a X?

Construções de Controle
-----------------------

Estamos quase em casa. Com expressões booleanas no lugar, basta adicionar as estruturas de controle. Para TINY, só vamos permitir dois tipos, o IF e o WHILE:

~~~ebnf
    <if> ::= IF <bool-expression> <block> [ ELSE <block> ] ENDIF
    <while> ::= WHILE <bool-expression> <block> ENDWHILE
~~~

Mais uma vez, deixe-me falar das decisões implícitas nesta sintaxe, que partem fortemente de C ou Pascal. Em ambas linguagens, o "corpo" do IF ou WHILE é considerado como um único comando. Se você pretende usar um bloco de mais de um comando, é preciso criar um comando composto usando BEGIN-END (em Pascal) ou "{}" (em C). Em TINY (e KISS) não há nada parecido com comandos compostos... simples ou múltiplos eles não passam de blocos para nossas linguagens.

Em KISS, todas as estruturas de controle possuem palavras-chave únicas e explícitas delimitando o bloco de comandos, de forma que não há confusão em relação a onde as coisas começam e terminam. Esta é a abordagem moderna, usada em linguagens respeitadas como Ada ou Modula-2, e ela elimina por completo o problema do "ELSE perdido" (*dangling else*).

Repare que eu poderia ter escolhido usar a mesma palavra-chave END para terminar todas as construções, como é feito em Pascal. (O "}" final em C serve para a mesma coisa.) Mas isto sempre nos leva a confusões, e é por isto que alguns programadores Pascal escrevem coisas como:

~~~pascal
    end { loop }
    end { if }
~~~

Como eu expliquei na [parte 5](05_estruturas_controle.md), usando a técnica de terminais únicos como palavras-chave aumenta o tamanho da lista de palavras-chave e portanto deixa mais lenta a análise léxica, mas neste caso parece um preço pequeno a ser pago por segurança garantida. É melhor achar erros em tempo de compilação do que em tempo de execução.

Um último pensamento, as duas construções acima tem os não-terminais:

    <bool-expression> e <block>

justapostas sem separação por palavras-chave. Em Pascal esperaríamos as palavras-chave THEN e DO nestes locais. Em C, as condições de IF e WHILE são colocadas sempre entre parênteses.

Porém, eu não vejo problemas em deixar fora estas palavras-chave, e o analisador também não tem problema com isso, CONTANDO QUE não haja erros na parte da expressão booleana. Por outro lado, se fossemos incluir estas palavras-chave teríamos mais um nível de segurança com um custo muito baixo, e eu também não vejo problemas nisto. Tome a decisão que você acha mais correta de como proceder.

Certo, com esta explicação, vamos prosseguir. Como de costume, vamos precisar de mais algumas rotinas de geração de código. Estas geram código para desvio condicional e incondicional:

~~~c
/* Desvio incondicional */
void AsmBranch(int label)
{
    EmitLn("JMP L%d", label);
}

/* Desvio se falso (0) */
void AsmBranchFalse(int label)
{
    EmitLn("JZ L%d", label);
}
~~~

Exceto pela encapsulação da geração de código, as rotinas para analisar as construções de controle são as mesmas vistas anteriormente:

~~~c
/* Analisa e traduz um comando IF */
void DoIf()
{
    int l1, l2;

    Match('i');
    BoolExpression();
    l1 = NewLabel();
    l2 = l1;
    AsmBranchFalse(l1);
    Block();
    if (Look == 'l') {
        Match('l');
        l2 = NewLabel();
        AsmBranch(l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    Match('e');
}

/* Analisa e traduz um comando WHILE */
void DoWhile()
{
    int l1, l2;

    Match('w');
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    BoolExpression();
    AsmBranchFalse(l2)
    Block();
    Match('e');
    AsmBranch(l1);
    PostLabel(l2);
}
~~~

Para juntar as coisas agora, só temos que modificar "block" para reconhecer as palavras-chave IF e WHILE. Como de costume, expandimos a definição de um bloco como:

~~~ebnf
    <block> ::= ( <statement> )*
    <statement> ::= <if> | <while> | <assignment>
~~~

O código correspondente é:

~~~c
/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    while (!follow) {
        switch (Look) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                Assignment();
                break;
        }
    }
}
~~~

Certo, adicione as rotinas dadas, compile e teste. Agora já é possível compilar versões de um caracter de qualquer uma das construções de controle. Está ficando muito bom!

De fato, exceto pela limitação de um único caracter, temos uma versão completa de TINY. Eu a chamo, com muito orgulho de: **TINY Versão 0.1**!

Para referência, a listagem completa de TINY Versão 0.1 é mostrada abaixo:

~~~c
{% include_relative src/cap10-tiny01.c %}
~~~

> Download do [compilador "Tiny 0.1"](src/cap10-tiny01.c).

Análise Léxica
--------------

É claro que você sabe o que vem depois: Temos que converter o programa para que ele trate de palavras-chave multi-caracter, quebras de linha, e espaços em branco. Nós já passamos por tudo isto na [parte 7](07_analise_lexica.md). Vamos usar a técnica de análise distribuída que eu mostrei naquele capítulo. A implementação atual é um pouco diferente por causa da forma que eu vou tratar das quebras de linha.

Para começar, vamos simplesmente permitir espaços em branco. Isto envolve apenas adicionar chamadas a `SkipWhite()` no fim das três rotinas, `GetName()`, `GetNum()` e `Match()`. Um chamada a `SkipWhite()` em `Init()` remove os espaços em branco iniciais. Podemos então entrar com um programa que é mais compreensível, como: 

    p vx b x=1 e.

Depois, temos que tratar de quebras de linha. Isto é na verdade um processo de dois passos, já que o tratamento das quebras de linha é diferente nas versões de token de um só caracter e multi-caracter. Podemos eliminar algum trabalho fazendo os dois passos de uma só vez, mas eu acho que é mais garantido fazer uma coisa de cada vez.

Insira a rotina:

~~~c
/* Captura caracteres de nova linha */
void NewLine()
{
    while (Look == '\n') {
        NextChar();
        SkipWhite();
    }
}
~~~

Note que já vimos esta rotina antes mas numa forma diferente. Eu agora alterei o código para permitir múltiplas quebras de linha e linhas que só possuem espaços.

O próximo passo é inserir a chamada a `NewLine()` onde quer que seja permitido uma nova linha. Como eu já disse antes, isto pode ser muito diferente dependendo da linguagem. Em TINY, eu decidi que é possível colocar quebras de linha virtualmente em qualquer lugar. Isto significa que precisamos de chamadas a `NewLine()` no INÍCIO (não no fim, como `SkipWhite()` das rotinas `GetName()`, `GetNum()` e `Match()`.

Para rotinas que possuem laços while, como `TopDeclarations()`, precisamos de uma chamada a `NewLine()` no início da rotina e no fim de cada repetição. Desta forma podemos garantir que `NewLine()` foi chamada no início de cada passagem do laço. Também é necessário adicionar `NewLine()` antes dos testes diretos de `Look`, como em `Factor()` e `AllocVar()`.

Se você já fez tudo isto, teste o programa e verifique que ele realmente trata de espaços em branco e quebras de linha. Tente todas as possibilidades que vierem à sua mente. Se alguma delas não funcionar é possível que você tenha esquecido de algum `NewLine()`. Basta verificar em que construção ocorrereu o problema e procurar a mesma no código.

Se tudo estiver correto, estamos prontos para tratar dos tokens multi-caracter e palavras-chave. Para começar, adicione as declarações adicionais (cópias quase idênticas da [parte 7](07_analise_lexica.md)):

~~~c
#define MAXTOKEN 16
#define KEYWORDLIST_SIZE 9

/* Lista de palavras-chave */
char *KeywordList[KEYWORDLIST_SIZE] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
               "VAR", "BEGIN", "END", "PROGRAM"};

/* A ordem deve obedecer a lista de palavras-chave */
char *KeywordCode = "ilewevbep";

char Token; /* Código do token atual */
char TokenText[MAXTOKEN+1]; /* Texto do token atual */
~~~

Em seguida, adicione as três rotinas, também da [parte 7](07_analise_lexica.md):

~~~c
/* Se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int Lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}

/* Analisador léxico */
void Scan()
{
    int kw;

    GetName();
    kw = Lookup(TokenText, KeywordList, KEYWORDLIST_SIZE);
    if (kw == -1)
        Token = 'x';
    else
        Token = KeywordCode[kw];
}

/* Verifica se a string combina com o esperado */
void MatchString(char *s)
{
    if (strcmp(TokenText, s) != 0)
        Expected(s);
}
~~~

Agora, nós temos um número grande de pequenas mudanças a serem feitas às rotinas restantes. Primeiro, temos que alterar a rotina `GetName()` para ser um procedimento, como fizemos na parte 7:

~~~c
/* Recebe o nome de um identificador */
void GetName()
{
    int i;

    NewLine();
    if (!isalpha(Look))
        Expected("Name");
    for (i = 0; isalnum(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = toupper(Look);
        NextChar();
    }
    TokenText[i] = '\0';
    Token = 'x';
    SkipWhite();
}
~~~

Repare que esta rotina deixa seu resultado na variável global `TokenText`.

Depois, temos que alterar cada referência a `GetName()` para refletir sua nova forma. Elas ocorrem em `Factor()`, `Assignment()` e `Declaration()`:

~~~c
/* Analisa e traduz um fator matemático */
void Factor()
{
    NewLine();
    if (Look == '(') {
        Match('(');
        BoolExpression();
        Match(')');
    } else if (isalpha(Look)) {
        GetName();
        AsmLoadVar(TokenText[0]);
    } else
        AsmLoadConst(GetNum());
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

    name = TokenText[0];
    Match('=');
    BoolExpression();
    AsmStore(name);
}

/* Analisa uma lista de declaração de variáveis */
void Declaration()
{
    NewLine();
    for (;;) {
        GetName();
        AllocVar(TokenText[0]);
        NewLine();
        if (Look != ',')
            break;
        Match(',');
        NewLine();
    }
}
~~~

(Repare que ainda estamos nos limitando a variáveis com nomes de uma só letra, então vamos usar apenas o primeiro caracter da string como uma saída fácil por enquanto.)

Finalmente, temos que fazer as alterações para usar "token" ao invés de `Look` como caracter de teste e chamar `Scan()` nos lugares apropriados. Na maioria, isto envolve remover chamadas a `Match()`, ocasionalmente trocando chamadas de `Match()` por chamadas a `MatchString()`, e trocando chamadas a `NewLine()` por chamadas a `Scan()`. Aqui estão as rotinas afetadas:

~~~c
/* Analisa e traduz um comando IF */
void DoIf()
{
    int l1, l2;

    BoolExpression();
    l1 = NewLabel();
    l2 = l1;
    AsmBranchFalse(l1);
    Block();
    if (Token == 'l') {
        l2 = NewLabel();
        AsmBranch(l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    MatchString("ENDIF");
}

/* Analisa e traduz um comando WHILE */
void DoWhile()
{
    int l1, l2;

    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    BoolExpression();
    AsmBranchFalse(l2);
    Block();
    MatchString("ENDWHILE");
    AsmBranch(l1);
    PostLabel(l2);
}

/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    do {
        Scan();
        switch (Token) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                Assignment();
                break;
        }
    } while (!follow);
}

/* Analisa e traduz declarações globais */
void TopDeclarations()
{
    Scan();
    while (token != 'b') {
        switch (Token) {
            case 'v':
                Declaration();
                break;
            default:
                Error("Unrecognized keyword.");
                Expected("BEGIN");
                break;
        }
        Scan();
    }
}

/* Analisa e traduz o bloco principal */
void MainBlock()
{
    MatchString("BEGIN");
    AsmProlog();
    Block();
    MatchString("END");
    AsmEpilog();
}

/* Analisa e traduz um programa completo */
void Program()
{
    MatchString("PROGRAM");
    AsmHeader();
    TopDeclarations();
    MainBlock();
    Match('.');
}

/* Inicialização do compilador */
void Init()
{
    int i = 0;

    for (i = 0; i < VARTABLE_SIZE; i++)
        VarTable[i] = ' ';

    NextChar();
    Scan();
}
~~~

Isto deve bastar. Se todas as alterações forem feitas corretamente, você deve agora estar compilando programas que parecem com programas mesmo! (Se você não fez todas as mudanças você mesmo, não se desespere. Uma listagem completa da forma final será dada depois.)

Deu certo? Se deu, então já estamos em casa. De fato, com algumas pequenas exceções já temos um compilador usável. Ainda há algumas áreas que podem ser melhoradas.

Nomes de Variáveis Multi-caracter
---------------------------------

Uma das coisas a melhorar é com relação à restrição de nomes de variáveis de apenas um caracter. Agora que podemos tratar de palavras-chave multi-caracter, isto começa a parecer uma restrição arbitrário e desnecessária. E na verdade é. Basicamente, a sua única virtude é que ela permite uma implementação trivial da tabela de símbolos. Mas isto é uma conveniência para o desenvolvedor do compilador, e deve ser eliminada.

Já fizemos isto antes. Desta vez, como de costume, eu vou fazê-lo de uma forma um pouco diferente. Eu acho que a abordagem aqui mantém as coisas tão simples quanto possível.

A forma natural de implementar uma tabela de símbolos é declarar uma estrutura (struct), e fazer da tabela de símbolos um vetor destas estruturas. Porém, aqui não precisamos realmente de um campo de tipo (afinal, só existe um tipo de entrada até aqui), portanto só precisamos de um vetor de símbolos. A vantagem é que podemos usar a rotina existente `Lookup()` para procurar na tabela de símbolos e também na lista de palavras-chave. No entando, mesmo quando precisarmos de mais campos poderíamos usar a mesma abordagem, simplesmente armazenando os outros campos em vetores separados.

Certo, aqui estão as mudanças necessárias.  Primeiro adicione:

~~~c
int SymbolCount; /* Número de entradas na tabela de símbolos */
~~~

Então remova a definição de "VarTable" e acrescente a nova tabela de símbolos:

~~~c
#define SYMBOLTABLE_SIZE 1000
char *SymbolTable[SYMBOLTABLE_SIZE]; /* Tabela de símbolos */
~~~

Em seguida, alteramos a definição de `InTable()`:

~~~c
/* Verifica se símbolo está na tabela */
int InTable(char *name)
{
    return (Lookup(name, SymbolTable, SymbolCount) >= 0);
}
~~~

Também precisamos de uma nova rotina, `AddEntry()`, que adiciona uma nova entrada à tabela de símbolos:

~~~c
/* Adiciona uma nova entrada à tabela de símbolos */
void AddEntry(char *name)
{
    char *newSymbol;

    if (InTable(name)) {
        Abort("Duplicated variable name: %s", name);
    }

    if (SymbolCount >= SYMBOLTABLE_SIZE) {
        Abort("Symbol table full!");
    }

    newSymbol = (char *) malloc(sizeof(char) * (strlen(name) + 1));
    if (newSymbol == NULL)
        Abort("Out of memory!");
    strcpy(newSymbol, name);

    SymbolTable[SymbolCount++] = newSymbol;
}
~~~

Esta rotina é chamada por `AllocVar()`, repare também nas outras alterações:

~~~c
/* Analisa e traduz uma declaração */
void AllocVar(char *name)
{
    int value = 0, signal = 1;

    AddEntry(name);

    NewLine();
    if (Look == '=') {
        Match('=');
        NewLine();
        if (Look == '-') {
                Match('-');
                signal = -1;
        }
        value = signal * GetNum();
    }    

    printf("%s\tdw %d\n", name, value);
}
~~~

Finalmente, temos que alterar todas as rotinas que tratam o nome da variável como um caracter único. Elas incluem `AsmLoadVar()`, `AsmStore()`, `Undefined()` (apenas alteramos o tipo de `char` para `char *` e `%c` para `%s` nos `printf`'s), `Factor()`, `Declaration()` (alteramos `TokenText[0]` para `TokenText`). Em `Assignment()` a mudança é um pouco diferente, mas nada complicada:

~~~c
/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name[MAXTOKEN+1];

    strcpy(name, TokenText);
    Match('=');
    BoolExpression();
    AsmStore(name);
}
~~~

Uma última alteração na rotina `Init()`:

~~~c
/* Inicialização do compilador */
void Init()
{
    SymbolCount = 0;

    NextChar();
    Scan();
}
~~~

Isto deve bastar. Teste o compilador agora e verifique que é possível adicionar nomes de variáveis multi-caracter.

Mais Operadores Relacionais
---------------------------

Ainda temos mais uma restrição de "caracter simples" para eliminar: a dos operadores relacionais. Alguns operadores relacionais são realmente caracteres únicos, mas outros requerem dois. Eles são "<=" e ">=". Eu também prefiro "<>" para diferente, ao invés de "#".

Se você se lembrar, na [parte 7](07_analise_lexica.md) eu disse que a maneira convencional de lidar de operadores relacionais é incluí-los na lista de palavras-chave, e permitir que o analisador léxico os encontre. Mas novamente, para fazer isto teriamos que analisar léxicamente a expressão toda no processo, sendo que até aqui fomos capazes de limitar a análise léxica apenas no começo do comando.

Eu mencionei então que ainda podemos nos livrar disto, já que os operadores relacionais são tão poucos e tão limitados em seu uso. É fácil tratar deles apenas como casos especiais e lidar com eles de uma forma **ad hoc**.

As mudanças necessárias afetam apenas as rotina de geração de códigos `AsmRelOp()` e `Relation()`:

~~~c
/* Analisa e traduz uma relação */
void Relation()
{
    char op;

    Expression();
    if (IsRelOp(Look)) {
        op = Look;
        Match(op); /* Só para remover o operador do caminho */
        if (op == '<') {
            if (Look == '>') { /* Trata operador <> */
                Match('>');
                op = '#';
            } else if (Look == '=') { /* Trata operador <= */
                Match('=');
                op = 'L';
            }
        } else if (op == '>' && Look == '=') { /* Trata operador >= */
            Match('=');
            op = 'G';
        }
        AsmPush();
        Expression();
        AsmPopCompare();
        AsmRelOp(op);
    }
}

/* Altera registrador primário (e flags, indiretamente) conforme a comparação */
void AsmRelOp(char op)
{
    char *jump;
    int l1, l2;

    l1 = NewLabel();
    l2 = NewLabel();

    switch (op) {
        case '=': jump = "JE"; break;
        case '#': jump = "JNE"; break;
        case '<': jump = "JL"; break;
        case '>': jump = "JG"; break;
        case 'L': jump = "JLE"; break;
        case 'G': jump = "JGE"; break;
    }

    EmitLn("%s L%d", jump, l1);
    EmitLn("XOR AX, AX");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}
~~~

(Repare que estou utilizando "G" e "L" para indicar os operadores ">=" e "<=" na passagem para `AsmRelOp()`. Repare também como é tratado o operador "<>".)

Isto deve bastar. Agora é possível processar todo tipo de operador relacional. Faça alguns testes.

Entrada/Saída
-------------

Agora nós temos uma linguagem completa e funcional, exceto por uma pequena omissão: não há como obter dados ou mostrá-los. Precisamos de entrada/saída.

A convenção nos dias de hoje, estabelecida em C e continuada em Ada e Modula-2, é deixar os comandos de E/S fora da linguagem em si, e incluí-los na biblioteca de rotinas. Isto seria bom, exceto pelo fato de que até aqui, não temos como tratar de subrotinas. De qualquer forma, com esta abordagem você acaba encontrando o problema de listas de parâmetros de tamanho variável. Em Pascal, os comandos de E/S estão embutidos na linguagem porque eles são os únicos em que a lista de parâmetros pode ter um número variado de entradas. Em C, as funções `scanf()` e `printf()` exigem que uma string com o formato da entrada seja passada e a partir desta string é determinada a quantidade de parâmetros. Em Ada e Modula-2 precisamos usar a estranha (e LENTA!) abordagem de uma chamada separada para cada parâmetro.

Eu prefiro a abordagem Pascal de colocar a E/S na própria linguagem, mesmo que não tenhamos que fazer isto.

Como de costume, para isto precisamos de mais rotinas de geração de código. Por sinal estas são as mais fáceis, pois tudo o que temos que fazer é chamar as rotinas da biblioteca para fazer o trabalho:

~~~c
/* Lê um valor a partir da entrada e armazena-o no registrador primário */
void AsmRead()
{
    EmitLn("CALL READ");
}

/* Mostra valor do registrador primário */
void AsmWrite()
{
    EmitLn("CALL WRITE");
}
~~~

A idéia é que `READ` carrega o valor de entrada em AX, e `WRITE` exibe o valor dele.

Estas duas rotinas representam nosso primeiro encontro com a necessidade de rotinas de biblioteca... os componentes de uma biblioteca de tempo de execução (*RTL - Run Time Library*). É claro, alguém (no caso, nós mesmos) deve escrever estas rotinas, mas elas não fazem parte do compilador em si. Repare que estas rotinas podem ser MUITO dependentes do sistema operacional. Eu vou colocar UMA versão destas rotinas para que você possa testar. É claro que não será a melhor das versões. É possível criar todo tipo de fantasia para estas coisas, por exemplo, exibir um prompt em `READ` para as entradas, e dar ao usuário uma nova chance se ele entrar com um valor inválido.

Mas isto é certamente separado do projeto do compilador. Por enquanto vamos fazer de conta que possuimos uma biblioteca chamada `TINYRTL`. Eu vou explicar como criar a biblioteca, suas rotinas e como importá-la para uso nos nossos programas nos apêndices.

Isto deve ser o suficiente. Agora, também devemos reconhecer os comandos de leitura e escrita. Podemos fazer isto adicionando mais duas palavras-chave à nossa lista:

~~~c
#define KEYWORDLIST_SIZE 11

/* Lista de palavras-chave */
char *KeywordList[KEYWORDLIST_SIZE] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
               "READ", "WRITE", "VAR", "BEGIN", "END", "PROGRAM"};

/* A ordem deve obedecer a lista de palavras-chave */
char *KeywordCode = "ileweRWvbep";
~~~

(Repare como estou usando letras maiúsculas nos códigos para evitar conflitos com o "w" de "WHILE".)

Em seguida, precisamos das rotinas para processar os comandos em si e sua lista de parâmetros:

~~~c
/* Processa um comando READ */
void DoRead()
{
    Match('(');
    for (;;) {
        GetName();
        AsmRead();
        AsmStore(TokenText);
        NewLine();
        if (Look != ',')
            break;
        Match(',');
    }
    Match(')');
}

/* Processa um comando WRITE */
void DoWrite()
{
    Match('(');
    for (;;) {
        Expression();
        AsmWrite();
        NewLine();
        if (Look != ',')
            break;
        Match(',');
    }
    Match(')');
}
~~~

Finalmente, temos que expandir "block" para tratar dos novos comandos:

~~~c
/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    do {
        Scan();
        switch (Token) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'R':
                DoRead();
                break;
            case 'W':
                DoWrite();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                Assignment();
                break;
        }
    } while (!follow);
}
~~~

Se você tentar montar o código gerado agora com as rotinas `READ` e `WRITE` vai obter uma mensagem de erro, dizendo que não encontrou tais rotinas no código. Como elas são rotinas externas (da biblioteca), precisamos avisar o montador a respeito disso, altere `AsmEpilog()`:

~~~c
/* Emite código para o epílogo de um programa */
void AsmEpilog()
{
    EmitLn("MOV AX, 4C00h");
    EmitLn("INT 21h");
    printf("\n%%include \"tinyrtl_dos.inc\"\n");
}
~~~

Tudo que isto faz, é incluir a linha abaixo no código gerado para importar as rotinas necessárias (para ver a implementação das rotinas consulte os apêndices):

~~~
    %include "tinyrtl_dos.inc"
~~~

Isto é tudo o que precisa ser feito. **AGORA temos uma linguagem!**

Conclusão
---------

Neste ponto, temos TINY completamente definida. Não é muita coisa... na verdade um compilador "de brinquedo". TINY só possui um tipo de dados e nenhuma subrotina... mas é uma linguagem completa e usável. Embora você não seja capaz de escrever um compilador usando TINY, ou fazer qualquer outra coisa seriamente, é possível escrever programas para ler algumas entradas, processar cálculos, e emitir algum resultado. Nada mal para um brinquedo.

Mais importante, temos uma base firme na qual podemos construir outras extensões. Eu acho que você vai ficar feliz em saber isto: esta é a última vez que eu vou começar a construir um analisador do zero... de agora em diante eu pretendo apenas adicionar características a TINY até que ele se torne KISS. Bem, no futuro vamos precisar testar outras coisas com novas cópias do ["berço"](src/cap01-craddle.c), mas uma vez que tenhamos descoberto como fazer estas coisas, elas serão incorporadas em TINY.

Quais características serão estas? Bem, pra começar precisaremos de procedimentos e funções. Então precisaremos tratar de tipos diferentes, incluindo matrizes, strings, e outras estruturas. Então temos que lidar com a idéia de ponteiros. Tudo isto será visto em capítulos posteriores.

Até lá!

Para referência, a listagem completa de **TINY Versão 1.0** é mostrada abaixo:

~~~c
{% include_relative src/cap10-tiny10.c %}
~~~

> Download do [compilador "Tiny 1.0"](src/cap10-tiny10.c).

{% include footer.md %}