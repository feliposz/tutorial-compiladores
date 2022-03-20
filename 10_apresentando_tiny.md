Parte 10: Apresentando "Tiny"
=============================

- Autor: Jack W. Crenshaw, Ph.D. (21/05/1989)
- Tradução e adaptação: Felipo Soranz (23/05/2002)

Na [última seção](09_uma_visao_do_alto), eu lhes mostrei a idéia geral para o desenvolvimento top-down de um compilador. Eu lhe mostrei os primeiros passos no processo para compiladores Pascal e C, mas eu parei muito antes de chegarmos a completá-los. A razão é simples: se vamos produzir um compilador real, funcional para alguma linguagem, eu prefiro que seja para KISS, a linguagem que eu venho definindo nesta série de tutoriais.

Nesta seção, nós vamos fazer exatemente isto, para um subconjunto de KISS que eu decidi chamar de TINY.

O processo vai ser essencialmente aquele mostrado no [capítulo 9](09_uma_visao_do_alto), exceto por uma diferença notável. Naquele capítulo eu sugeri que você começasse com uma descrição BNF completa da linguagem. Isto é bom para algo como Pascal ou C, para as quais a definição da linguagem é sólida. No caso de TINY, porém, nós ainda não temos uma descrição completa... pois até agora estivemos definindo a linguagem conforme seguimos. Mas não há nada de errado com isso. Na verdade, é preferível, já que podemos adaptar ligeiramente a linguagem conforme avançamos, para manter a análise sintática fácil.

Então, no desenvolvimento a seguir, nós vamos na verdade desenvolver tanto a linguagem quanto o compilador numa abordagem top-down. A descrição BNF vai crescer com o compilador.

Neste processo, haverá um certo número de decisões a serem tomada, cada uma influenciando a BNF e portanto a natureza da linguagem. A cada ponto de decisão eu vou tentar explicar os motivos por trás da minha escolha. Deste modo, se você tiver uma opinião diferente e preferir uma outra opção, você pode escolhê-la. Agora você tem o conhecimento para isto. Eu acho que é importante notar que nada do que fazemos aqui é totalmente definitivo. Quando VOCÊ estiver projetando a SUA linguagem, você deve se sentir livre para fazê-la do SEU jeito.

Muitos de vocês podem perguntar neste ponto: Por que começar tudo de novo? Nós já temos um subconjunto de KISS como produto do [capítulo 7 (análise léxica)](07_analise_lexica). Por que não simplesmente estendê-lo conforme necessário? A resposta é tripla. Primeiro, eu estive fazendo algumas mudanças para simplificar ainda mais o programa... mudanças como encapsular as rotinas de geração de código, de forma que possamos converter o programa para outras plataformas alvo mais facilmente. Segundo, eu quero que você veja como o desenvolvimento pode realmente ser feito partindo numa abordagem top-down como foi discutido no último capítulo. Finalmente, nós dois precisamos praticar. Cada vez que eu passo por este exercício eu fico um pouco melhor, e você também ficará.

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

Há também uma decisão com relação a permitir que o programa principal fique somente no fim. Eu trabalhei com a idéia de permitir que sua posição fosse opcional, como em C. A natureza dos montadores como o TASM (que é o que eu estou usando) permitem que isto seja realmente fácil de fazer. Mas isto não faz muito sentido do ponto de vista da regra que estamos usando. Como em Pascal todos os dados e rotinas devem ser declarados antes de referenciados. Como o programa principal só pode chamar subrotinas que já foram declaradas, a única posição que faz sentido é no final, como em Pascal.

Dada a BNF acima, vamos criar o analisador que reconhece apenas os limitadores.

~~~c
/* analisa e traduz um programa completo */
void program()
{
    match('p');
    header();
    prolog();
    match('.');
    epilog();
}
~~~

A rotina `header()` apenas emite o código inicial necessário para o montador:

~~~c
/* cabeçalho inicial para o montador */
void header()
{
    emit(".model small");
    emit(".stack");
    emit(".code");
    printf("PROG segment byte public\n");
    emit("assume cs:PROG,ds:PROG,es:PROG,ss:PROG");
}
~~~

As rotinas `prolog()` e `epilog()` emitem código o código que identifica o programa principal. O epilogo contém algum código de inicialização e o prólogo código para retornar ao sistema operacional:

~~~c
/* emite código para o prólogo de um programa */
void prolog()
{
    printf("MAIN:\n");
    emit("MOV AX, PROG");
    emit("MOV DS, AX");
    emit("MOV ES, AX");
}

/* emite código para o epílogo de um programa */
void epilog()
{
    emit("MOV AX,4C00h");
    emit("INT 21h");
    printf("PROG ends\n");
    emit("end MAIN");
}
~~~

O programa principal apenas chama `program()`, e verifica se o final está correto:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    program();

    if (look != '\n')
        fatal("Unexpected data after \'.\'");

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

Para processar a definição de um bloco principal, altere a rotina `program()`:

~~~c
/* analisa e traduz um programa completo */
void program()
{
    match('p');
    header();
    mainBlock();
    match('.');
}
~~~

Depois adicione a nova rotina:

~~~c
/* analisa e traduz o bloco principal */
void mainBlock()
{
    match('b');
    prolog();
    match('e');
    epilog();
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

A rotina `program()` fica:

~~~c
/* analisa e traduz um programa completo */
void program()
{
    match('p');
    header();
    topDeclarations();
    mainBlock();
    match('.');
}
~~~

Agora adicione estas duas rotinas:

~~~c
/* analisa uma declaração de variável */
void declaration()
{
    match('v');
    nextChar();
}

/* analisa e traduz declarações globais */
void topDeclarations()
{
    while (look != 'b') {
        switch (look) {
            case 'v':
                declaration();
                break;
            default:
                error("Unrecognized keyword.");
                expected("BEGIN");
                break;
        }
    }
}
~~~

Repare que por enquanto, `declaration()` não faz muita coisa. Não gera nenhum código, e não processa uma lista... cada variável deve ocorrer num comando VAR separado.

Certo, agora podemos ter qualquer número de declarações de dados, cada uma começando com um "v" de VAR, antes do bloco principal. Tente alguns casos e veja o que acontece.

Declarações e Símbolos
----------------------

Isto parece interessante, mas continuamos gerando apenas o programa vazio como saída. Um compilador real iria gerar diretivas assembly para alocar memória para as variáveis. Já é hora de fazermos o mesmo.

Com algum código extra, é algo fácil de fazer na rotina `declaration()`. Modifique-a como segue:

~~~c
/* analisa uma declaração de variável */
void declaration()
{
    match('v');
    allocVar(getName());
}
~~~

A rotina `allocVar()` simplesmente emite o comando assembly para alocar memória:

~~~c
/* emite código de alocação de memória para uma variável */
void allocVar(char name)
{
    printf("%c:\tdw 0\n", name);
}
~~~

Faça o teste agora. Tente uma entrada que declara algumas variáveis, como:

    pvxvyvzbe.

Viu como o armazenamento é alocado? Simples, não? Note também que o ponto de entrada "MAIN", se encaixa no lugar certo.

Para constar, um compilador "real" também teria uma tabela de símbolos para armazenar as variáveis usadas. Normalmente, a tabela de símbolos é necessária para armazenar o tipo de cada variável. Mas como neste caso todas as variáveis tem o mesmo tipo, não precisamos de uma tabela de símbolos por este motivo. Mas como você vai ver, vamos constatar que um símbolo é necessário mesmo sem a diferença dos tipos, mas vamos por esta questão de lado até que seja necessário.

É claro que ainda não processamos a sintaxe correta para uma declaração de dados, já que ela envolve uma lista de variáveis. Nossa versão permite apenas uma única variável. Isto é fácil de arrumar, também.

A BNF para a lista de variáveis é:

~~~ebnf
    <var-list> ::= <ident> (, <ident>)*
~~~

Adicionando esta sintaxe a `declaration()` temos sua nova versão:

~~~c
/* analisa uma lista de declaração de variáveis */
void declaration()
{
    match('v');
    for (;;) {
        allocVar(getName());
        if (look != ',')
            break;
        match(',');
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

Altere `allocVar()` desta forma:

~~~c
/* alocação de memória para uma variável global */
void allocVar(char name)
{
    char value = '0';

    if (look == '=') {
        match('=');
        value = getNum();
    }

    printf("%c:\tdw %c\n", name, value);
}
~~~

Aí está: um inicializador com 5 novas linhas de C.

Ok, tente esta versão de TINY e verifique que é possível, de fato, dar às variáveis valores iniciais.

Isto está começando a parecer real! É claro que ainda não faz nada, mas parece bom, não parece?

Antes de deixar esta seção, eu devo lembrá-lo que já usamos duas versões de `getNum()`. Uma, a primeira, retorna um valor em caracter, um dígito único. A outra aceita valores inteiros multi-dígitos e retorna um valor inteiro. Qualquer uma funcionaria aqui, bastaria alterar "%c" para "%d" em "printf". Mas não há razão para nos limitarmos ao dígito único aqui, então vamos corrigir esta versão e retornar inteiros. Aqui está:

~~~c
/* recebe um número inteiro */
int getNum()
{
    int num;

    num = 0;

    if (!isdigit(look))
        expected("Integer");

    while (isdigit(look)) {
        num *= 10;
        num += look - '0';
        nextChar();
    }

    return num;
}
~~~

De fato, deveríamos permitir expressões completas no lugar do analisador, ou pelo menos valores negativos. Por enquanto, vamos permitir apenas valores negativos alterando `allocVar()` como segue (repare também nas pequenas alterações para a nova versão de `getNum()`):

~~~c
/* alocação de memória para uma variável global */
void allocVar(char name)
{
    int value = 0, signal = 1;

    if (look == '=') {
        match('=');
        if (look == '-') {
            match('-');
            signal = -1;
        }
        value = signal * getNum();
    }    

    printf("%c:\tdw %d\n", name, value);
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
/* tabela de símbolos */
#define VARTBL_SZ 26
char vartbl[VARTBL_SZ];
~~~

E adicione a seguinte função:

~~~c
/* verifica se símbolo está na tabela */
int inTable(char name)
{
    return (vartbl[name - 'A'] != ' ');
}
~~~

Também temos que inicializar a tabela com espaços. Adicione a inicialização em `init()`:

~~~c
/* inicialização do compilador */
void init()
{
    int i = 0;

    for (i = 0; i < VARTBL_SZ; i++)
        vartbl[i] = ' ';

    nextChar();
}
~~~

Finalmente, insira estas linhas no começo de `allocVar()`:

~~~c
    if (inTable(name))
        fatal("Duplicate variable name: %c", name);
    else
        vartbl[name - 'A'] = 'v';
~~~

Isto deve bastar. O compilador agora vai reconhecer declarações duplicadas. Mais tarde, também podemos usar `inTable()` quando gerar referências às variáveis.

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
/* avalia um comando de atribuição */
void assignment()
{
    nextChar();
}

/* analisa e traduz um bloco de comandos */
void block()
{
    while (look != 'e')
        assignment();
}
~~~

Modifique a rotina "mainBlock" para chamar "block" conforme abaixo:

~~~c
/* analisa e traduz o bloco principal */
void mainBlock()
{
    match('b');
    prolog();
    block();
    match('e');
    epilog();
}
~~~

Esta versão ainda não gera nenhum código para os "comandos de atribuição"... tudo o que ela faz é "engolir" os caracteres até encontrar o "e" de END. Mas ela já é uma preparação para o que segue.

O próximo passo  logicamente é inserir código para o comando de atribuição. Isto é algo que fizemos muitas vezes antes, então eu não vou atrasar as coisas. Desta vez porém, eu gostaria de tratar da geração de código de forma um pouco diferente. Até agora, nós simplesmente emitimos o código para gerar a saída dentro das rotinas do analisador. Um pouco desestruturado, porém, parecia ser a abordagem mais fácil, e tornou fácil ver que tipo de código seria emitido para cada construção.

De qualquer forma, eu percebi que a maioria de vocês gostaria de saber se é possível que o código dependente de CPU poderia ser colocado em outro ponto onde seria mais fácil de portar o código para outra CPU-alvo. A resposta é: **sim, claro!**

Para fazer isto, insira as seguintes rotinas de "geração de código":

~~~c
/* zera o registrador primário */
void asmClear()
{
    emit("XOR AX, AX");
}

/* negativa o registrador primário */
void asmNegative()
{
    emit("NEG AX");
}

/* carrega uma constante numérica no registrador primário */
void asmLoadConst(int i)
{
    emit("MOV AX, %d", i);
}

/* carrega uma variável no registrador primário */
void asmLoadVar(char name)
{
    if (!inTable(name))
        undefined(name);
    emit("MOV AX, WORD PTR %c", name);
}

/* armazena registrador primário em variável */
void asmStore(char name)
{
    if (!inTable(name))
        undefined(name);
    emit("MOV WORD PTR %c, AX", name);
}

/* coloca registrador primário na pilha */
void asmPush()
{
    emit("PUSH AX");
}

/* adiciona o topo da pilha ao registrador primário */
void asmPopAdd()
{
    emit("POP BX");
    emit("ADD AX, BX");
}

/* subtrai o registrador primário do topo da pilha */
void asmPopSub()
{
    emit("POP BX");
    emit("SUB AX, BX");
    emit("NEG AX");
}

/* multiplica o topo da pilha pelo registrador primário */
void asmPopMul()
{
    emit("POP BX");
    emit("IMUL BX");
}

/* divide o topo da pilha pelo registrador primário */
void asmPopDiv()
{
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}
~~~

A parte boa desta abordagem, logicamente, é que podemos redirecionar a saída do compilador para uma nova CPU simplesmente reescrevendo estas rotinas de geração de código. Além disso, vamos descobrir mais tarde que podemos melhor a qualidade do código simplesmente melhorando estas rotinas um pouco, sem ter que alterar o próprio compilador.

Repare que tanto `asmLoadVar()` quanto `asmStore()` verificam a tabela de símbolos para ter certeza de que a variável está definida. O tratador de erros "undefined" simplesmente mostra o erro e saí, da mesma forma que "expect":

~~~c
/* avisa a respeito de um identificador desconhecido */
void undefined(char name)
{
    fatal("Error: Undefined identifier %c\n", name);
}
~~~

Certo, finalmente estamos prontos para começar a adicionar código executável. Faremos isto substituindo a versão vazia de `assignment()`.

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
/* analisa e traduz um fator matemático */
void factor()
{
    if (look == '(') {
        match('(');
        expression();
        match(')');
    } else if (isalpha(look))
        asmLoadVar(getName());
    else
        asmLoadConst(getNum());
}

/* analisa e traduz um fator negativo */
void negFactor()
{
    match('-');
    if (isdigit(look))
        asmLoadConst(-getNum());
    else {
        factor();
        asmNegative();
    }
}

/* analisa e traduz um fator inicial */
void firstFactor()
{
    switch (look) {
        case '+':
            match('+');
            factor();
            break;
        case '-':
            negFactor();
            break;
        default:
            factor();
            break;
    }
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    asmPopMul();
}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    asmPopDiv();
}

/* código comum usado por "term" e "firstTerm" */
void termCommon()
{
    while (isMulOp(look)) {
        asmPush();
        switch (look) {
          case '*':
            multiply();
            break;
          case '/':
            divide();
            break;
        }
    }
}

/* analisa e traduz um termo matemático */
void term()
{
    factor();
    termCommon();
}

/* analisa e traduz um termo inicial */
void firstTerm()
{
    firstFactor();
    termCommon();
}

/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    asmPopAdd();
}

/* reconhece e traduz uma subtração*/
void subtract()
{
    match('-');
    term();
    asmPopSub();
}

/* analisa e traduz uma expressão matemática */
void expression()
{
    firstTerm();
    while (isAddOp(look)) {
        asmPush();
        switch (look) {
          case '+':
            add();
            break;
          case '-':
            subtract();
            break;
        }
    }
}

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name;

    name = getName();
    match('=');
    expression();
    asmStore(name);
}
~~~

Agora que você já inseriu todo este código, compile-o e verifique o resultado. Você deve ter agora um código que parece razoável, representando um program completo que vai ser montado corretamento e executado. Temos um compilador!

Expressões Booleanas
--------------------

O próximo passo também deve ser familiar pra você. Temos que incluir expressões booleanas e operadores relacionais. Novamente, como já lidamos com isto mais de uma vez, eu não vou elaborar muito em cima deles, exceto onde existirem diferenças do que já fizemos. Novamente, nós não vamos simplesmente copiar de outros arquivos pois eu mudei um pouco as coisas. A maioria da mudanças envolveram apenas encapsular a parte dependente de máquina como fizemos para as operações aritméticas. Eu também modifiquei a rotina `notFactor()`, para ficar semelhante à estrutura de `firstFactor()`.

Para começar, vamos precisar de mais alguns reconhecedores:

~~~c
/* reconhece um operador OU */
int isOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* reconhece operadores relacionais */
int isRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}
~~~

Também vamos precisar de mais rotinas de geração de código:

~~~c
/* inverte registrador primário */
void asmNot()
{
    emit("NOT AX");
}

/* aplica "E" binário ao topo da pilha com registrador primário */
void asmPopAnd()
{
    emit("POP BX");
    emit("AND AX, BX");
}

/* aplica "OU" binário ao topo da pilha com registrador primário */
void asmPopOr()
{
    emit("POP BX");
    emit("OR AX, BX");
}

/* aplica "OU-exclusivo" binário ao topo da pilha com registrador primário */
void asmPopXor()
{
    emit("POP BX");
    emit("XOR AX, BX");
}

/* compara topo da pilha com registrador primário */
void asmPopCompare()
{
    emit("POP BX");
    emit("CMP BX, AX");
}

/* altera registrador primário (e flags, indiretamente) conforme a comparação */
void asmRelOp(char op)
{
    char *jump;
    int l1, l2;

    l1 = newLabel();
    l2 = newLabel();

    switch (op) {
        case '=': jump = "JE"; break;
        case '#': jump = "JNE"; break;
        case '<': jump = "JL"; break;
        case '>': jump = "JG"; break;
    }

    emit("%s L%d", jump, l1);
    emit("XOR AX, AX");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}
~~~

Estas são as ferramentas de que precisamos. Repare que eu substitui as rotinas dos operadores relacionais por uma só rotina para todos. Desta forma evitamos duplicar o código. Insira também a declaração de `labelCount`, `newLabel()`, `postLabel()` das seções anteriores já que são necessárias nesta rotina.

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
/* analisa e traduz uma relação */
void relation()
{
    char op;

    expression();
    if (isRelOp(look)) {
        op = look;
        match(op); /* só para remover o operador do caminho */
        asmPush();
        expression();
        asmPopCompare();
        asmRelOp(op);
    }
}

/* analisa e traduz um fator booleano com NOT inicial */
void notFactor()
{
    if (look == '!') {
        match('!');
        relation();
        asmNot();
    } else
        relation();
}

/* analisa e traduz um termo booleano */
void boolTerm()
{
    notFactor();
    while (look == '&') {
        asmPush();
        match('&');
        notFactor();
        asmPopAnd();
    }
}

/* reconhece e traduz um "OR" */
void boolOr()
{
    match('|');
    boolTerm();
    asmPopOr();
}

/* reconhece e traduz um "xor" */
void boolXor()
{
    match('~');
    boolTerm();
    asmPopXor();
}

/* analisa e traduz uma expressão booleana */
void boolExpression()
{
    boolTerm();
    while (isOrOp(look)) {
        asmPush();
        switch (look) {
          case '|':
              boolOr();
              break;
          case '~':
              boolXor();
              break;
        }
    }
}
~~~

Repare que eu alterei `relation()` para refletir a estrutura usada por `asmRelOp()`. No fim acabamos economizando 8 rotinas e não duplicamos código.

Para juntar tudo, não esqueça de alterar as referências para `expression()` nas rotinas `factor()` e `assignment()` para que elas chamem `boolExpression()` agora.

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

Como eu expliquei na [parte 5](05_estruturas_controle), usando a técnica de terminais únicos como palavras-chave aumenta o tamanho da lista de palavras-chave e portanto deixa mais lenta a análise léxica, mas neste caso parece um preço pequeno a ser pago por segurança garantida. É melhor achar erros em tempo de compilação do que em tempo de execução.

Um último pensamento, as duas construções acima tem os não-terminais:

    <bool-expression> e <block>

justapostas sem separação por palavras-chave. Em Pascal esperaríamos as palavras-chave THEN e DO nestes locais. Em C, as condições de IF e WHILE são colocadas sempre entre parênteses.

Porém, eu não vejo problemas em deixar fora estas palavras-chave, e o analisador também não tem problema com isso, CONTANDO QUE não haja erros na parte da expressão booleana. Por outro lado, se fossemos incluir estas palavras-chave teríamos mais um nível de segurança com um custo muito baixo, e eu também não vejo problemas nisto. Tome a decisão que você acha mais correta de como proceder.

Certo, com esta explicação, vamos prosseguir. Como de costume, vamos precisar de mais algumas rotinas de geração de código. Estas geram código para desvio condicional e incondicional:

~~~c
/* desvio incondicional */
void asmJmp(int label)
{
    emit("JMP L%d", label);
}

/* desvio se falso (0) */
void asmJmpFalse(int label)
{
    emit("JZ L%d", label);
}
~~~

Exceto pela encapsulação da geração de código, as rotinas para analisar as construções de controle são as mesmas vistas anteriormente:

~~~c
/* analisa e traduz um comando IF */
void doIf()
{
    int l1, l2;

    match('i');
    boolExpression();
    l1 = newLabel();
    l2 = l1;
    asmJmpFalse(l1);
    block();
    if (look == 'l') {
        match('l');
        l2 = newLabel();
        asmJmp(l2);
        postLabel(l1);
        block();
    }
    postLabel(l2);
    match('e');
}

/* analisa e traduz um comando WHILE */
void doWhile()
{
    int l1, l2;

    match('w');
    l1 = newLabel();
    l2 = newLabel();
    postLabel(l1);
    boolExpression();
    asmJmpFalse(l2)
    block();
    match('e');
    asmJmp(l1);
    postLabel(l2);
}
~~~

Para juntar as coisas agora, só temos que modificar "block" para reconhecer as palavras-chave IF e WHILE. Como de costume, expandimos a definição de um bloco como:

~~~ebnf
    <block> ::= ( <statement> )*
    <statement> ::= <if> | <while> | <assignment>
~~~

O código correspondente é:

~~~c
/* analisa e traduz um bloco de comandos */
void block()
{
    int follow = 0;

    while (!follow) {
        switch (look) {
            case 'i':
                doIf();
                break;
            case 'w':
                doWhile();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                assignment();
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

Download do [compilador "Tiny 0.1"](src/cap10-tiny01.c).

Análise Léxica
--------------

É claro que você sabe o que vem depois: Temos que converter o programa para que ele trate de palavras-chave multi-caracter, quebras de linha, e espaços em branco. Nós já passamos por tudo isto na [parte 7](07_analise_lexica). Vamos usar a técnica de análise distribuída que eu mostrei naquele capítulo. A implementação atual é um pouco diferente por causa da forma que eu vou tratar das quebras de linha.

Para começar, vamos simplesmente permitir espaços em branco. Isto envolve apenas adicionar chamadas a `skipWhite()` no fim das três rotinas, `getName()`, `getNum()` e `match()`. Um chamada a `skipWhite()` em `init()` remove os espaços em branco iniciais. Podemos então entrar com um programa que é mais compreensível, como: 

    p vx b x=1 e.

Depois, temos que tratar de quebras de linha. Isto é na verdade um processo de dois passos, já que o tratamento das quebras de linha é diferente nas versões de token de um só caracter e multi-caracter. Podemos eliminar algum trabalho fazendo os dois passos de uma só vez, mas eu acho que é mais garantido fazer uma coisa de cada vez.

Insira a rotina:

~~~c
/* captura caracteres de nova linha */
void newLine()
{
    while (look == '\n') {
        nextChar();
        skipWhite();
    }
}
~~~

Note que já vimos esta rotina antes mas numa forma diferente. Eu agora alterei o código para permitir múltiplas quebras de linha e linhas que só possuem espaços.

O próximo passo é inserir a chamada a `newLine()` onde quer que seja permitido uma nova linha. Como eu já disse antes, isto pode ser muito diferente dependendo da linguagem. Em TINY, eu decidi que é possível colocar quebras de linha virtualmente em qualquer lugar. Isto significa que precisamos de chamadas a `newLine()` no INÍCIO (não no fim, como `skipWhite()` das rotinas `getName()`, `getNum()` e `match()`.

Para rotinas que possuem laços while, como `topDeclarations()`, precisamos de uma chamada a `newLine()` no início da rotina e no fim de cada repetição. Desta forma podemos garantir que `newLine()` foi chamada no início de cada passagem do laço. Também é necessário adicionar `newLine()` antes dos testes diretos de `look`, como em `factor()` e `allocVar()`.

Se você já fez tudo isto, teste o programa e verifique que ele realmente trata de espaços em branco e quebras de linha. Tente todas as possibilidades que vierem à sua mente. Se alguma delas não funcionar é possível que você tenha esquecido de algum `newLine()`. Basta verificar em que construção ocorrereu o problema e procurar a mesma no código.

Se tudo estiver correto, estamos prontos para tratar dos tokens multi-caracter e palavras-chave. Para começar, adicione as declarações adicionais (cópias quase idênticas da [parte 7](07_analise_lexica)):

~~~c
#define MAXTOKEN 16
#define KWLIST_SZ 9

/* lista de palavras-chave */
char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
               "VAR", "BEGIN", "END", "PROGRAM"};

/* a ordem deve obedecer a lista de palavras-chave */
char *kwcode = "ilewevbep";

char token; /* código do token atual */
char value[MAXTOKEN+1]; /* texto do token atual */
~~~

Em seguida, adicione as três rotinas, também da [parte 7](07_analise_lexica):

~~~c
/* se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}

/* analisador léxico */
void scan()
{
    int kw;

    getName();
    kw = lookup(value, kwlist, KWLIST_SZ);
    if (kw == -1)
        token = 'x';
    else
        token = kwcode[kw];
}

/* verifica se a string combina com o esperado */
void matchString(char *s)
{
    if (strcmp(value, s) != 0)
        expected(s);
}
~~~

Agora, nós temos um número grande de pequenas mudanças a serem feitas às rotinas restantes. Primeiro, temos que alterar a rotina `getName()` para ser um procedimento, como fizemos na parte 7:

~~~c
/* recebe o nome de um identificador */
void getName()
{
    int i;

    newLine();
    if (!isalpha(look))
        expected("Name");
    for (i = 0; isalnum(look) && i < MAXTOKEN; i++) {
        value[i] = toupper(look);
        nextChar();
    }
    value[i] = '\0';
    token = 'x';
    skipWhite();
}
~~~

Repare que esta rotina deixa seu resultado na variável global `value`.

Depois, temos que alterar cada referência a `getName()` para refletir sua nova forma. Elas ocorrem em `factor()`, `assignment()` e `declaration()`:

~~~c
/* analisa e traduz um fator matemático */
void factor()
{
    newLine();
    if (look == '(') {
        match('(');
        boolExpression();
        match(')');
    } else if (isalpha(look)) {
        getName();
        asmLoadVar(value[0]);
    } else
        asmLoadConst(getNum());
}

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name;

    name = value[0];
    match('=');
    boolExpression();
    asmStore(name);
}

/* analisa uma lista de declaração de variáveis */
void declaration()
{
    newLine();
    for (;;) {
        getName();
        allocVar(value[0]);
        newLine();
        if (look != ',')
            break;
        match(',');
        newLine();
    }
}
~~~

(Repare que ainda estamos nos limitando a variáveis com nomes de uma só letra, então vamos usar apenas o primeiro caracter da string como uma saída fácil por enquanto.)

Finalmente, temos que fazer as alterações para usar "token" ao invés de `look` como caracter de teste e chamar `scan()` nos lugares apropriados. Na maioria, isto envolve remover chamadas a `match()`, ocasionalmente trocando chamadas de `match()` por chamadas a `matchString()`, e trocando chamadas a `newLine()` por chamadas a `scan()`. Aqui estão as rotinas afetadas:

~~~c
/* analisa e traduz um comando IF */
void doIf()
{
    int l1, l2;

    boolExpression();
    l1 = newLabel();
    l2 = l1;
    asmJmpFalse(l1);
    block();
    if (token == 'l') {
        l2 = newLabel();
        asmJmp(l2);
        postLabel(l1);
        block();
    }
    postLabel(l2);
    matchString("ENDIF");
}

/* analisa e traduz um comando WHILE */
void doWhile()
{
    int l1, l2;

    l1 = newLabel();
    l2 = newLabel();
    postLabel(l1);
    boolExpression();
    asmJmpFalse(l2);
    block();
    matchString("ENDWHILE");
    asmJmp(l1);
    postLabel(l2);
}

/* analisa e traduz um bloco de comandos */
void block()
{
    int follow = 0;

    do {
        scan();
        switch (token) {
            case 'i':
                doIf();
                break;
            case 'w':
                doWhile();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                assignment();
                break;
        }
    } while (!follow);
}

/* analisa e traduz declarações globais */
void topDeclarations()
{
    scan();
    while (token != 'b') {
        switch (token) {
            case 'v':
                declaration();
                break;
            default:
                error("Unrecognized keyword.");
                expected("BEGIN");
                break;
        }
        scan();
    }
}

/* analisa e traduz o bloco principal */
void mainBlock()
{
    matchString("BEGIN");
    prolog();
    block();
    matchString("END");
    epilog();
}

/* analisa e traduz um programa completo */
void program()
{
    matchString("PROGRAM");
    header();
    topDeclarations();
    mainBlock();
    match('.');
}

/* inicialização do compilador */
void init()
{
    int i = 0;

    for (i = 0; i < VARTBL_SZ; i++)
        vartbl[i] = ' ';

    nextChar();
    scan();
}
~~~

Isto deve bastar. Se todas as alterações forem feitas corretamente, você deve agora estar compilando programas que parecem com programas mesmo! (Se você não fez todas as mudanças você mesmo, não se desespere. Uma listagem completa da forma final será dada depois.)

Deu certo? Se deu, então já estamos em casa. De fato, com algumas pequenas exceções já temos um compilador usável. Ainda há algumas áreas que podem ser melhoradas.

Nomes de Variáveis Multi-caracter
---------------------------------

Uma das coisas a melhorar é com relação à restrição de nomes de variáveis de apenas um caracter. Agora que podemos tratar de palavras-chave multi-caracter, isto começa a parecer uma restrição arbitrário e desnecessária. E na verdade é. Basicamente, a sua única virtude é que ela permite uma implementação trivial da tabela de símbolos. Mas isto é uma conveniência para o desenvolvedor do compilador, e deve ser eliminada.

Já fizemos isto antes. Desta vez, como de costume, eu vou fazê-lo de uma forma um pouco diferente. Eu acho que a abordagem aqui mantém as coisas tão simples quanto possível.

A forma natural de implementar uma tabela de símbolos é declarar uma estrutura (struct), e fazer da tabela de símbolos um vetor destas estruturas. Porém, aqui não precisamos realmente de um campo de tipo (afinal, só existe um tipo de entrada até aqui), portanto só precisamos de um vetor de símbolos. A vantagem é que podemos usar a rotina existente `lookup()` para procurar na tabela de símbolos e também na lista de palavras-chave. No entando, mesmo quando precisarmos de mais campos poderíamos usar a mesma abordagem, simplesmente armazenando os outros campos em vetores separados.

Certo, aqui estão as mudanças necessárias.  Primeiro adicione:

~~~c
int symbolCount; /* número de entradas na tabela de símbolos */
~~~

Então remova a definição de "vartbl" e acrescente a nova tabela de símbolos:

~~~c
#define SYMTBL_SZ 1000
char *symbolTable[SYMTBL_SZ]; /* tabela de símbolos */
~~~

Em seguida, alteramos a definição de `inTable()`:

~~~c
/* verifica se símbolo está na tabela */
int inTable(char *name)
{
    return (lookup(name, symbolTable, symbolCount) >= 0);
}
~~~

Também precisamos de uma nova rotina, `addSymbol()`, que adiciona uma nova entrada à tabela de símbolos:

~~~c
/* adiciona uma nova entrada à tabela de símbolos */
void addSymbol(char *name)
{
    char *newSymbol;

    if (inTable(name)) {
        fatal("Duplicated variable name: %s", name);
    }

    if (symbolCount >= SYMTBL_SZ) {
        fatal("Symbol table full!");
    }

    newSymbol = (char *) malloc(sizeof(char) * (strlen(name) + 1));
    if (newSymbol == NULL)
        fatal("Out of memory!");
    strcpy(newSymbol, name);

    symbolTable[symbolCount++] = newSymbol;
}
~~~

Esta rotina é chamada por `allocVar()`, repare também nas outras alterações:

~~~c
/* analisa e traduz uma declaração */
void allocVar(char *name)
{
    int value = 0, signal = 1;

    addSymbol(name);

    newLine();
    if (look == '=') {
        match('=');
        newLine();
        if (look == '-') {
                match('-');
                signal = -1;
        }
        value = signal * getNum();
    }    

    printf("%s:\tdw %d\n", name, value);
}
~~~

Finalmente, temos que alterar todas as rotinas que tratam o nome da variável como um caracter único. Elas incluem `asmLoadVar()`, `asmStore()`, `undefined()` (apenas alteramos o tipo de "char" para "char *" e "%c" para "%s" nos `printf`'s), `factor()`, `declaration()` (alteramos "value[0]" para "value"). Em `assignment()` a mudança é um pouco diferente, mas nada complicada:

~~~c
/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name[MAXTOKEN+1];

    strcpy(name, value);
    match('=');
    boolExpression();
    asmStore(name);
}
~~~

Uma última alteração na rotina `init()`:

~~~c
/* inicialização do compilador */
void init()
{
    symbolCount = 0;

    nextChar();
    scan();
}
~~~

Isto deve bastar. Teste o compilador agora e verifique que é possível adicionar nomes de variáveis multi-caracter.

Mais Operadores Relacionais
---------------------------

Ainda temos mais uma restrição de "caracter simples" para eliminar: a dos operadores relacionais. Alguns operadores relacionais são realmente caracteres únicos, mas outros requerem dois. Eles são "<=" e ">=". Eu também prefiro "<>" para diferente, ao invés de "#".

Se você se lembrar, na parte 7 eu disse que a maneira convencional de lidar de operadores relacionais é incluí-los na lista de palavras-chave, e permitir que o analisador léxico os encontre. Mas novamente, para fazer isto teriamos que analisar léxicamente a expressão toda no processo, sendo que até aqui fomos capazes de limitar a análise léxica apenas no começo do comando.

Eu mencionei então que ainda podemos nos livrar disto, já que os operadores relacionais são tão poucos e tão limitados em seu uso. É fácil tratar deles apenas como casos especiais e lidar com eles de uma forma **ad hoc**.

As mudanças necessárias afetam apenas as rotina de geração de códigos `asmRelOp()` e `relation()`:

~~~c
/* analisa e traduz uma relação */
void relation()
{
    char op;

    expression();
    if (isRelOp(look)) {
        op = look;
        match(op); /* só para remover o operador do caminho */
        if (op == '<') {
            if (look == '>') { /* trata operador <> */
                match('>');
                op = '#';
            } else if (look == '=') { /* trata operador <= */
                match('=');
                op = 'L';
            }
        } else if (op == '>' && look == '=') { /* trata operador >= */
            match('=');
            op = 'G';
        }
        asmPush();
        expression();
        asmPopCompare();
        asmRelOp(op);
    }
}

/* altera registrador primário (e flags, indiretamente) conforme a comparação */
void asmRelOp(char op)
{
    char *jump;
    int l1, l2;

    l1 = newLabel();
    l2 = newLabel();

    switch (op) {
        case '=': jump = "JE"; break;
        case '#': jump = "JNE"; break;
        case '<': jump = "JL"; break;
        case '>': jump = "JG"; break;
        case 'L': jump = "JLE"; break;
        case 'G': jump = "JGE"; break;
    }

    emit("%s L%d", jump, l1);
    emit("XOR AX, AX");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}
~~~

(Repare que estou utilizando "G" e "L" para indicar os operadores ">=" e "<=" na passagem para `asmRelOp()`. Repare também como é tratado o operador "<>".)

Isto deve bastar. Agora é possível processar todo tipo de operador relacional. Faça alguns testes.

Entrada/Saída
-------------

Agora nós temos uma linguagem completa e funcional, exceto por uma pequena omissão: não há como obter dados ou mostrá-los. Precisamos de entrada/saída.

A convenção nos dias de hoje, estabelecida em C e continuada em Ada e Modula-2, é deixar os comandos de E/S fora da linguagem em si, e incluí-los na biblioteca de rotinas. Isto seria bom, exceto pelo fato de que até aqui, não temos como tratar de subrotinas. De qualquer forma, com esta abordagem você acaba encontrando o problema de listas de parâmetros de tamanho variável. Em Pascal, os comandos de E/S estão embutidos na linguagem porque eles são os únicos em que a lista de parâmetros pode ter um número variado de entradas. Em C, as funções `scanf` e `printf` requerem que uma string com o formato da entrada seja passada e a partir desta string é determinada a quantidade de parâmetros. Em Ada e Modula-2 precisamos usar a estranha (e LENTA!) abordagem de uma chamada separada para cada parâmetro.

Eu prefiro a abordagem Pascal de colocar a E/S na própria linguagem, mesmo que não tenhamos que fazer isto.

Como de costume, para isto precisamos de mais rotinas de geração de código. Por sinal estas são as mais fáceis, pois tudo o que temos que fazer é chamar as rotinas da biblioteca para fazer o trabalho:

~~~c
/* lê um valor a partir da entrada e armazena-o no registrador primário */
void asmRead()
{
    emit("CALL READ");
}

/* mostra valor do registrador primário */
void asmWrite()
{
    emit("CALL WRITE");
}
~~~

A idéia é que `READ` carrega o valor de entrada em AX, e `WRITE` exibe o valor dele.

Estas duas rotinas representam nosso primeiro encontro com a necessidade de rotinas de biblioteca... os componentes de uma biblioteca de tempo de execução (*RTL - Run Time Library*). É claro, alguém (no caso, nós mesmos) deve escrever estas rotinas, mas elas não fazem parte do compilador em si. Repare que estas rotinas podem ser MUITO dependentes do sistema operacional. Eu vou colocar UMA versão destas rotinas para que você possa testar, se você estiver usando o DOS. É claro que não será a melhor das versões. É possível criar todo tipo de fantasia para estas coisas, por exemplo, exibir um prompt em READ para as entradas, e dar ao usuário uma nova chance se ele entrar com um valor inválido.

Mas isto é certamente separado do projeto do compilador. Por enquanto vamos fazer de conta que possuimos uma biblioteca chamada `TINYRTL.LIB` (eu vou explicar como criar a biblioteca e suas rotinas num texto separado).  Para fazer uso dela, faça o seguinte quando for gerar o executável do programa:

    TASM prog.asm
    TLINK prog.obj,,,tinyrtl.lib

Isto deve ser o suficiente. Agora, também devemos reconhecer os comandos de leitura e escrita. Podemos fazer isto adicionando mais duas palavras-chave à nossa lista:

~~~c
#define KWLIST_SZ 11

/* lista de palavras-chave */
char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
               "READ", "WRITE", "VAR", "BEGIN", "END", "PROGRAM"};

/* a ordem deve obedecer a lista de palavras-chave */
char *kwcode = "ileweRWvbep";
~~~

(Repare como estou usando letras maiúsculas nos códigos para evitar conflitos com o "w" de "WHILE".)

Em seguida, precisamos das rotinas para processar os comandos em si e sua lista de parâmetros:

~~~c
/* processa um comando READ */
void doRead()
{
    match('(');
    for (;;) {
        getName();
        asmRead();
        asmStore(value);
        newLine();
        if (look != ',')
            break;
        match(',');
    }
    match(')');
}

/* processa um comando WRITE */
void doWrite()
{
    match('(');
    for (;;) {
        expression();
        asmWrite();
        newLine();
        if (look != ',')
            break;
        match(',');
    }
    match(')');
}
~~~

Finalmente, temos que expandir "block" para tratar dos novos comandos:

~~~c
/* analisa e traduz um bloco de comandos */
void block()
{
    int follow = 0;

    do {
        scan();
        switch (token) {
            case 'i':
                doIf();
                break;
            case 'w':
                doWhile();
                break;
            case 'R':
                doRead();
                break;
            case 'W':
                doWrite();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                assignment();
                break;
        }
    } while (!follow);
}
~~~

Se você tentar montar o código gerado agora com as rotinas READ e WRITE vai obter uma mensagem de erro, dizendo que não encontrou tais rotinas no código. Como elas são rotinas externas (da biblioteca), precisamos avisar o montador a respeito disso, altere `header()`:

~~~c
/* cabeçalho inicial para o montador */
void header()
{
    emit(".model small");
    emit(".stack");
    emit(".code");
    printf("extrn READ:near, WRITE:near\n");
    printf("PROG segment byte public\n");
    emit("assume cs:PROG,ds:PROG,es:PROG,ss:PROG");
}
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

Download do [compilador "Tiny 1.0"](src/cap10-tiny10.c).

{% include footer.md %}