Parte 9: Uma visão do alto
==========================

> **Autor:** Jack W. Crenshaw, Ph.D. (16/04/1989)<br>
> **Tradução e adaptação:** Felipo Soranz (22/05/2002)

Nos capítulos anteriores, aprendemos muitas das técnicas necessárias para construir um compilador completo. Nós já fizemos comandos de atribuição (com [expressões](02_analise_expressoes.md) [aritméticas](03_mais_expressoes.md) e [booleanas](06_expressoes_booleanas.md)), operadores relacionais, e [estruturas de controle](05_estruturas_controle.md). Ainda não tratamos de chamadas de procedimentos e funções, mas mesmo assim podemos construir uma mini-linguagem sem elas. Eu sempre achei interessante ver quão pequena pode ser uma linguagem e ainda assim ser útil. Estamos QUASE em uma posição para fazer isto agora. O problema é: apesar de sabermos como analisar e traduzir as construções, ainda não sabemos muito bem como colocá-las juntas em uma linguagem.

Nestas seções anteriores, o desenvolvimento dos nossos programas tive decididamente um comportamento "bottom-up". No caso de expressões por exemplo, começamos com o nível mais baixo das construções, as constantes e variáveis individuais, e trabalhamos até chegar em expressões mais complexas.

A maioria das pessoas ao contrário, acha que o projeto usando uma abordagem "top-down" é bem melhor que o "bottom-up". Eu também acho, mas a maneira como fizemos certamente pareceu natural o bastante para os tipos de coisas que estávamos analisando e traduzindo.

Mas talvez você não tenha captado a idéia, de que o processo incremental que usamos durante todo o tutorial é em si "bottom-up". Neste capítulo eu gostaria de mostrar que a abordagem pode funcionar da mesma forma quando aplicada da forma inversa... talvez até melhor. Vamos considerar linguagens como C e Pascal, e ver como compiladores completos podem ser construídos começando do "topo".

Nos próximos capítulos, vamos usar a mesma técnica para construir um tradutor completo para um subconjunto da minha linguagem KISS, que camos chamar de TINY. Mas um dos meus objetivos para esta série é que você não só veja como um compilador para TINY ou KISS funciona, mas que você possa também projetar e construir compiladores para suas próprias linguagens. Os exemplos de C e Pascal vão ajudar. Uma coisa que eu gostaria que você visse é que a estrutura natual do compilador depende muito da linguagem sendo traduzida, então muito da simplicidade e facilidade da construção do compilador depende muito deixar a linguagem definir a estrutura do programa.

É um pouco demais produzir um compilador C ou Pascal completo aqui, e nós não vamos nem tentar. Mas podemos ver os níveis mais superficiais da linguagem o suficiente para que você entenda como funciona.

Vamos começar.

O Nível do Topo
---------------

Um dos grandes erros que as pessoas cometem em um projeto "top-down" é não começar do verdadeiro "topo"! Eles acham que sabem como a estrutura geral do projeto deve ser, então eles vão em frente e a escrevem.

Sempre que eu começo um novo projeto, eu sempre gosto de começar desde as coisas mais preliminares. No linguagem de projeto de programa (program design language, PDL), este nível do topo é algo assim:

    início
        resolva o problema
    fim

Certo, eu garanto que isto não dá uma dica muito boa a respeito do que representa o próximo nível, mas eu gosto de escrevê-lo da mesma forma, apenas para me dar a confortável sensação de que eu estou de fato começando do topo.

Para o nosso problema, a função geral do compilador é compilar um programa completo. Qualquer definição de uma linguagem, escrita em BNF, começa por aqui. Com o que o nível superior da BNF se parece? Bem, isto depende um tanto da linguagem a ser traduzida. Vamos dar uma olhada em Pascal.

A Estrutura em Pascal
---------------------

Alguns dos textos sobre Pascal incluem uma definição da linguagem em sintaxe BNF ou um diagrama. Aqui estão as primeiras linhas de um deles:

~~~ebnf
    <program> ::= <program-header> <block> '.'
    <program-header> ::= PROGRAM <ident>
    <block> ::= <declarations> <statements>
~~~

Podemos construir reconhecedores para tratar de cada um destes elementos, da mesma forma como fizemos antes. Para cada um, vamos usar nossos já familiares tokens de um só caracter para representar a entrada, e então melhor as coisas um pouco de cada vez. Vamos começar com o primeiro reconhecedor: o progama em si.

Para traduzir isto, vamos começar com uma nova cópia do ["berço"](src/cap01-craddle.c). Como voltamos ao esquema de caracteres únicos, vamos usar apenas "p" para simbolizar "PROGRAM".

Adicione o seguinte numa nova cópia do "berço":

~~~c
/* Analisa e traduz um programa Pascal */
void Program()
{
    char name;

    Match('p'); /* Trata do cabeçalho do programa */
    name = GetName();
    AsmProlog(name);
    Match('.');
    AsmEpilog(name);
}
~~~

Insira uma chamada no programa principal:

~~~c
/* Programa principal */
int main()
{
    Init();
    Program();

    return 0;
}
~~~

As rotinas `AsmProlog()` e `AsmEpilog()` executam ações necessárias para permitir que o programa se comunique com o sistema operacional, de forma que ele possa ser executado como um programa. É desnecessário dizer, **que esta parte é MUITO dependente do sistema operacional.** Lembre-se, estamos gerando código para uma CPU 80x86. Eu estou usando um sistema compatível com DOS e vou usar um montador compatível com TASM para gerar o programa executável. Eu compreendo que a maioria possa estar usando outro tipo de CPU e um sistema operacional mais moderno, mas já chegamos longe demais para mudar agora!

O DOS em particular é de certa forma fácil de lidar se você quer algo simples. É possível brincar um pouco mais com modelos de memória, organização dos segmentos, etc. Mas eu preferi usar algo mais simples e que funciona! Você pode adaptar os códigos da forma que preferir. Estes são os códigos para o `AsmProlog()` e `AsmEpilog()`:

~~~c
/* Emite código para o prólogo de um programa */
void AsmProlog()
{
    EmitLn(".model small");
    EmitLn(".stack");
    EmitLn(".code");
    printf("PROG segment byte public\n");
    EmitLn("assume cs:PROG,ds:PROG,es:PROG,ss:PROG");
}

/* Emite código para o epílogo de um programa */
void AsmEpilog(char name)
{
    printf("exit_prog:\n");
    EmitLn("MOV AX, 4C00h  ; AH=4C (termina execucao do programa) AL=00 (saida ok)");
    EmitLn("INT 21h       ; chamada de sistema DOS");
    printf("PROG ends\n");
    EmitLn("end %c", name);
}
~~~

>**Nota de tradução:** Esta parte do tutorial precisa desesperadamente de uma "modernização"! Peço desculpas ao leitor que precisará fazer adaptações para poder transformar o código assembly gerado em algo executável num computador moderno. O código gerado funciona, mas só consegui testá-lo (em 2022!) usando [DOSBOX](https://www.dosbox.com/).

Adicione este código e teste o "compilador". Neste ponto, a única entrada válida é:

    pX.

"X" é uma única letra representando o nome do programa.

Bem, como de costume nossa primeira tentativa não impressiona muito, mas neste ponto eu tenho certeza que as coisas vão ficar mais interessantes. Há algo importante a ser notado: A SAÍDA AGORA É UM PROGRAMA QUE FUNCIONA, COMPLETO E EXECUTÁVEL (pelo menos depois de ser montado).

Isto é muito importante. A melhor característica da abordagem "top-down" é que a qualquer estágio é possível compilar um subconjunto da linguagem completa e obter um programa que pode ser executado na máquina alvo. Daqui por diante, nós só precisamos adicionar características melhorando as construções da linguagem. Tudo isto é muito similar ao que já fizemos, exceto pelo fato da abordagem agora ser inversa.

Apenas para completar. Caso você queira transformar a saída do nosso programa em código executável, deve usar um montador (assembler) em um ligador (linker). Se você usar o [Turbo Assembler](https://winworldpc.com/product/turbo-assembler/1x) da Borland, pode fazer desta forma:

    TASM programa.asm
    TLINK programa.obj

Adicionando Código
------------------

Para adicionar código ao compilador, só precisamos tratar das características da linguagem uma a uma. Eu gosto de começar com uma rotina que não faz nada, e então adicionar detalhes de forma incremental. Vamos começar processando um bloco, de acordo com a PDL acima. Podemos fazer isto em dois estágios. Primeiro, adicione a rotina:

~~~c
/* Analisa e traduz um bloco Pascal */
void Block(char name)
{
}
~~~

Então altere `Program()`:

~~~c
/* Analisa e traduz um programa */
void Program()
{
    char name;
    
    Match('p'); /* Trata do cabeçalho do programa */
    name = GetName();
    AsmProlog();
    Block(name);
    Match('.');
    AsmEpilog(name);
}
~~~

Isto certamente não deve alterar o funcionamento do nosso programa, e não altera. Mas agora a definição de `Program()` está completa, e podemos prosseguir com `Block()`. Isto é feito diretamente da definição BNF:

~~~c
/* Analisa e traduz um bloco pascal */
void Block(char name)
{
    Declarations();
    printf("%c:\n", name);
    Statements();
}
~~~

Note que estamos emitindo um rótulo de desvio. Eu provavelmente devo explicar a razão de inserí-lo justamente ali. Tem a ver com a forma como o montador funciona. Ao contrário de outros montadores o TASM e seus compatíveis permitem que o ponto de entrada do programa seja em qualquer lugar. Tudo o que você deve fazer é dar um nome a este ponto. Nós colocamos o rótulo bem antes do primeiro comando executável no programa principal. Como o montador sabe qual dos rótulos é o ponto de entrada? É aquele que combina com o do comando END no fim do programa.

Certo, agora adicione as rotinas `Declaration()` e `Statements()`. Elas são rotinas vazias assim como fizemos antes.

O programa continua rodando da mesma forma? Então podemos mover para o próximo estágio.

Declarações
-----------

A BNF para declarações em pascal é:

~~~ebnf
    <declarations> ::= ( <label-list>    |
                         <constant-list> |
                         <type-list>     |
                         <variable-list> |
                         <procedure>     |
                         <function>         )*
~~~

(Note que eu estou usando uma definição mais liberal usada pelo Turbo Pascal. Na definição padrão de Pascal, cada parte deve ter uma ordem específica uma em relação à outra.)

Como de costume, vamos permitir que um simples caracter represente cada um destes tipos de declaração. A nova forma de `Declarations()` é:

~~~c
/* Analisa e traduz declarações pascal */
void Declarations()
{
    int valid;

    do {
        valid = 1;
        switch (Look) {
            case 'l': Labels(); break;
            case 'c': Constants(); break;
            case 't': Types(); break;
            case 'v': Variables(); break;
            case 'p': Procedure(); break;
            case 'f': Function(); break;
            default: valid = 0; break;
        }
    } while (valid);
}
~~~

É claro que vamos precisar de rotinas pra cada um destes tipos de declaração. Desta vez, elas não podem ser apenas rotinas vazias, pois senão teríamos um laço infinito. No mínimo, cada reconhecedor deve eliminar o caracter que o invocou. Insira as seguintes rotinas:

~~~c
/* Funções de declaração pascal */

void Labels()
{
    Match('l');
}

void Constants()
{
    Match('c');
}

void Types()
{
    Match('t');
}

void Variables()
{
    Match('v');
}

void Procedure()
{
    Match('p');
}

void Function()
{
    Match('f');
}
~~~

Agora teste o compilador com algumas entradas representativas. É possível misturar declarações da forma que você preferir, desde que o último caracter seja um "." para indicar o fim do programa. Certamente, nenhuma das declarações declara alguma coisa de verdade, então você não precisa (e não pode) usar outros caracteres além daqueles que indicam uma palavra-chave.

Podemos inserir a parte dos comandos de forma similar. A BNF é assim:

~~~ebnf
    <statements> ::= <compound_statement>
    <compound_statement> ::= BEGIN <statement> [ ';' <statement> ]* END
~~~

Note que os comandos podem começar com qualquer identificador exceto END. Então a primeira forma de `Statements()` é:

~~~c
/* Analisa e traduz um bloco de comandos pascal */
void Statements()
{
    Match('b');
    while (Look != 'e')
        NextChar();
    Match('e');
}
~~~

Neste ponto o compilador vai aceitar qualquer número de declarações, seguidos por um bloco iniciado por BEGIN no programa principal. O bloco em si pode conter qualquer caracter (exceto o "e" de END), mas deve estar presente.

A forma mais simples de entrada agora é:

    pxbe.

Teste isto. Tente outras combinações também. Faça algumas com erros e veja o que acontece.

Neste ponto você já deve ter começado a pegar o jeito. Começamos com um tradutor vazio para processar um programa, então inserimos as rotinas uma de cada vez, baseados na definição BNF. Da mesma forma que as definições BNF de nível mais baixo adicionam detalhes e elaboram as de alto nível, os reconhecedores de nível mais baixo vão processar mais detalhes da entrada. Quando o último reconhecedor vazio for expandido, o compilador vai estar completo. Isto é projeto e implementação "top-down" na sua forma mais pura.

Você pode notar que mesmo estando adicionando rotinas, a saída do programa não mudou. É como deveria ser. Neste nível superior não há código que deva ser emitido. Os reconhecedores estão funcionando apenas como meros reconhecedores e só! Eles aceitam sentenças de entrada, indicam as que estão erradas, e canalizam a entrada correta para os lugares corretos, portanto elas estão fazendo seu trabalho. Se começarmos a perseguir isto mais um pouco, o código vai aparecer em breve.

O próximo passo em nossa expansão deveria ser provavelmente na rotina `Statement()`. A definição de pascal é:

~~~ebnf
    <statement> ::= <simple-statement> | <structured-statement>
    
    <simple-statement> ::= <assignment> | <procedure-call> | null
    
    <structured-statement> ::= <compound-statement> |
                               <if-statement>       |
                               <case-statement>     |
                               <while-statement>    |
                               <repeat-statement>   |
                               <for-statement>      |
                               <with-statement>
~~~

Isto está começando a parecer familiar. Você já passou pelo processo de analisar e gerar código para comandos de atribuição e estruturas de controle. Este é o ponto em que o nível do topo encontra nossa abordagem "bottom-up" das seções anteriores. As construções vão ser um pouco diferentes daquelas que usamos em KISS, mas as diferenças não são nada que você não possa tratar.

Eu acho que você já conseguiu entender o procedimento. Começamos com uma descrição completa da linguagem em BNF. Começando do nível mais alto, codificamos os reconhecedores para os comandos BNF, usando rotinas vazias para os reconhecedores do próximo nível. Então vamos inserindo os comandos nos níveis mais baixos um a um.

A definição da linguagem Pascal é muito compatível com o uso da BNF, e descrições BNF da linguagem são abundantes. Armado com uma destas descrições, você vai achar que é de certa forma fácil continuar o processo que começamos.

Você deveria tentar inserir algumas destas construções você mesmo, apenas para entender a idéia. Eu não espero que você seja capaz de fazer um compilador Pascal completo... há muitas coisas como subrotinas e tipos que ainda não tratamos... mas pode ajudar tentar algumas das coisas mais familiares. Vai ser bom ver programas executáveis saindo do outro lado.

Eu tenho que cuidar das questões que ainda não cobrimos, mas prefiro fazê-lo no contexto da nossa linguagem KISS. Não estamos tentando construi um compilador Pascal completo ainda, portanto eu vou parar a expansão de Pascal aqui. Vamos dar uma olhada agora numa linguagem bem diferente.

O código completo do nosso compilador "Pascal":

~~~c
{% include_relative src/cap09-top-pascal.c %}
~~~

> Download do [compilador "Pascal"](src/cap09-top-pascal.c).


A Estrutura de C
----------------

A linguagem C é um problema bem diferente, como você vai ver. Textos sobre C raramente incluem uma definição BNF para a linguagem. Provavelmente porque é bem difícil de escrever BNF para a linguagem.

Uma razão para eu lhe mostra estas estruturas agora é para que eu posso lhe demonstrar estes dois fatos:

1. A definição de uma linguagem domina a estrutura do compilador. O que funciona para uma linguagem pode ser um desastre para outra. É uma idéia muito ruim tentar forçar uma determinada estrutura no compilador. Ao invés disso, você deve deixar a BNF dominar a estrutura, como já fizemos até aqui.

2. Uma linguagem que é difícil definir em BNF provavelmente também é complicada para compilar. C é uma linguagem popular, e tem toda esta reputação por deixar você fazer virtualmente qualquer coisa possível. Apesar do sucesso do compilador Small C, C NÃO é uma linguagem fácil de compilar.

Um programa em C tem menos estrutura que seu sósia em Pascal. No nível do topo, tudo em C é uma declaração estática, ou de dados ou de uma função. Podemos capturar isto desta forma:

~~~ebnf
    <program> ::= [ <global-declaration> ]*
    <global-declaration> ::= <data-declaration> | <function>
~~~

Em Small C, as funções só podem ter o tipo padrão `int`, que não é declarado. Isto faz com que a entrada seja fácil de analisar: o primeiro token é `int`, `char`, ou o nome de uma função. Em Small C, os comandos do pré-processador são sempre processados pelo próprio compilador, e a sintaxe torna-se:

~~~ebnf
    <global-declaration> ::= '#' <preprocessor-command>  |
                             'int' <data-list>           |
                             'char' <data-list>          |
                             <ident> <function-body>
~~~

Apesar de estarmos mais interessados em C completo aqui, eu vou mostrar o código correspondente a esta estrutura de nível mais alto de Small C. Este código é apenas para ilustração, não é necessário alterar seu programa.

~~~c
/* Analisa e traduz um programa Small C */
void Program()
{
    while (Look != EOF) {
        switch (Look) {
            case '#':
                PreProcessor(); break;
            case 'i':
                IntDeclaration(); break;
            case 'c':
                CharDeclaration(); break;
            default:
                FunctionDeclaration(); break;
        }
    }
}
~~~

Note que eu tive que usar `EOF` par indicar o fim do código. C não tem uma palavra-chave como `end` ou `.` para indicar o fim. REPARE porém que agora `Look` deve ser declarada como `int` e não mais como `char`.

Com a linguagem C completa, as coisas não são nem um pouco fáceis. O problema começa pois em C, as funções também podem ter tipos. Então quando o compilador vê a palavra chave `int`, ele ainda não sabe se deve esperar uma declaração de dados ou uma definição de função. As coisas ficam mais complicadas pois o próximo token pode não ser um nome... pode ser um "*" ou "(", ou uma combinações dos dois.

Mais especificamente, a BNF para C completa começa com:

~~~ebnf
    <program> ::= ( <top-level-decl> )*
    <top-level-decl> ::= <function-def> | <data-decl>
    <data-decl> ::= [<class>] <type> <decl-list>
    <function-def> ::= [<class>] [<type>] <function-decl>
~~~

Agora você começa a ver o problema: As primeiras duas partes da declaração para dados e funções podem ser iguais. Por causa da ambiguidade na gramática conforme escrito acima, ela não é uma gramática adequada para um analisador descendente-recursivo. Podemos transformá-la em uma que é? Sim, com um pouco de trabalho. Suponha que escrevamos assim:

~~~ebnf
    <top-level decl> ::= [<class>] <decl>
    <decl> ::= <type> <typed-decl> | <function-decl>
    <typed-decl> ::= <data-list> | <function-decl>
~~~

Podemos construir uma rotina de análise para as definições de classe e tipo, e nela guardamos o que encontramos e seguimos em frente, sem sequer saber se estamos processando uma declaração de função ou de dados.

Para começar, entre com a seguinte versão do programa principal:

~~~c
/* Programa principal */
int main()
{
    Init();

    while (Look != EOF && Look != '\n') {
        GetClass();
        GetType();
        TopDeclaration();
    }

    return 0;
}
~~~

Para o primeiro "round", apenas crie as 3 rotinas que por enquanto não fazem nada ALÉM de chamar `NextChar()`.

O programa funciona? Bem, seria difícil dizer que não, já que não estamos pedindo que ele faça nada. Foi dito que um compilador vai aceitar virtualmente qualquer entrada sem reclamar. Isto é certamente verdadeiro para ESTE compilador, já que de fato a única coisa que ele faz é ler caracteres até encontrar um fim de arquivo (`EOF`).

Agora, vamos fazer `GetClass()` fazer algo de valor. Declare a variável global:

~~~c
char CurrentClass; /* Classe atual lida por getClass */
~~~

E agora altere `GetClass()`:

~~~c
/* Recebe uma classe de armazenamento C */
void GetClass()
{
    if (Look == 'a' || Look == 'x' || Look == 's') {
        CurrentClass = Look;
        NextChar();
    } else
        CurrentClass = 'a';
}
~~~

Aqui, eu usei 3 caracteres para representar os 3 tipos de classe de armazenamento `auto`, `extern` e `static`. Estes não são os únicos tipos possíveis... há também `register` e `typedef`, mas isto deve dar uma idéia. Note que o padrão é `auto`.

Podemos fazer algo semelhante para tipos. Entre com a seguinte rotina:

~~~c
/* Recebe um tipo de dado C */
void GetType()
{
    CurrentType = ' ';
    if (Look == 'u') {
        CurrentSigned = 'u';
        CurrentType = 'i';
        NextChar();
    } else {
        CurrentSigned = 's';
    }
    if (Look == 'i' || Look == 'l' || Look == 'c') {
        CurrentType = Look;
        NextChar();
    }
}
~~~

Não esqueça de adicionar agora as variáveis globais:

~~~c
char CurrentType; /* Tipo atual lido por getType */
char CurrentSigned; /* Indica se tipo atual é com ou sem sinal */
~~~

Com estas duas rotinas, o compilador vai processar a classe de armazenamento a definição de tipo e armazenar o que encontrou. Podemos agora processar o resto da declaração.

Não estamos de forma alguma livres da complicação ainda, pois há ainda muitas complexidades apenas na definição do tipo, mesmo antes de chegarmos à definição do nome dos dados ou da função. Vamos fingir que neste ponto passamos por tudo isto, e que a próxima entrada é o nome. Se o nome for seguido de um parêntese esquerdo, temos uma declaração de função. Caso contrário, temos ao menos um item de dados, e possívelmente uma lista, e cada elemento pode ter um inicializador.

Insira agora `TopDeclaration()`:

~~~c
/* Analisa uma declaração global */
void TopDeclaration()
{
    char name;

    name = GetName();
    if (Look == '(')
        FunctionDeclaration(name);
    else
        DoData(name);
}
~~~

(Note que, como já lemos o nome, temos que passá-lo para a rotina apropriada.)

Finalmente, adicione as rotinas `functionDeclaration` e `dataDeclaration`:

~~~c
/* Analisa uma declaração de função */
void FunctionDeclaration(char name)
{
    Match('(');
    Match(')');
    Match('{');
    Match('}');
    if (CurrentType == ' ')
        CurrentType = 'i';
    printf("Class: %c, Sign: %c, Type: %c, Function: %c\n",
        CurrentClass, CurrentSigned, CurrentType, name);
}

/* Analisa uma declaração de variável */
void DoData(char name)
{
    if (CurrentType == ' ')
        Expected("Type declaration");
    for (;;) {
        printf("Class: %c, Sign: %c, Type: %c, Data: %c\n",
            CurrentClass, CurrentSigned, CurrentType, name);
        if (Look != ',')
            break;
        Match(',');
        name = GetName();
    }
    Match(';');
}
~~~

Como estamos um pouco distantes de produzir código executável, eu decidi que estas rotinas devem simplesmente nos dizer o que encontraram.

Certo, agora teste este programa um pouco. Para declarações, é correto entrar com uma lista separada por vírgulas. Não podemos processar inicializadores ainda. Também não há como processarmos ainda listas de parâmetros para as funções, mas os caracteres "(){}" devem estar lá. Por exemplo:

    iI,J;g(){}uU;iS;f(){}cC,D;

Ainda estamos BEM distantes de termos um compilador C, mas já começamos a processar os tipos corretos de entradas, e estamos diferenciando entradas corretas e inválidas. No processo, a estrutura natural do compilador está começando a tomar forma.

Podemos continuar com isto até termos algo que tenha um funcionamento mais parecido com o de um compilador. Claro que podemos. Mas devemos? Isto é outro problema. Eu não sei você, mas eu estou começando a ficar enjoado, e ainda temos uma longa estrada apenas para completar a parte da declaração.

O código completo do nosso compilador "C":

~~~c
{% include_relative src/cap09-top-c.c %}
~~~

> Download do [compilador "C"](src/cap09-top-c.c).

Neste ponto, eu acho que você consegue ver como a estrutura do compilador evolui a partir da definição da linguagem. As estruturas que vimos para os dois exemplos, Pascal e C, são tão diferentes como o dia e a noite. Pascal foi projetada ao menos parcialmente para ser fácil de analisar, e isto é refletido no compilador. Em geral, em Pascal há mais estrutura e temos uma idéia melhor de que tipo de construções esperar em cada ponto. Em C, por outro lado, o programa é essencialmente uma lista de declarações, terminados apenas pelo fim do arquivo.

Poderíamos continuar seguindo ambas estruturas bem mais longe, mas lembre-se que o objetivo aqui não é construir um compilador Pascal ou C, mas ao invés disso, estudar compiladores em geral. Para aqueles que querem realmente tratar de Pascal ou C, eu espero ter dado uma iniciação suficiente para que você possa continuar daqui (embora você vá precisar em breve de algumas das coisas que ainda não tratamos aqui, como tipos e chamadas de rotinas). Para o resto de vocês, sigam comigo pelo [próximo capítulo](10_apresentando_tiny.md). Lá, eu vou lhe apresentar o desenvolvimento de um compilador completo para TINY, um subconjunto de KISS.

Vejo vocês lá!

{% include footer.md %}