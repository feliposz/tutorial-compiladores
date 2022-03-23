Parte 5: Estruturas de controle
===============================

> **Autor:** Jack W. Crenshaw, Ph.D. (19/08/1988)<br>
> **Tradução e adaptação:** Felipo Soranz (16/05/2002)

Nos primeiros quatro capítulos desta série, nos concentramos na análise de expressões matemáticas e comandos de atribuição. Neste capítulo, vamos começar com uma tangente nova e excitante: a análise e tradução de construções de controle, como comandos IF e WHILE.

Eu particularmente gosto muito deste assunto, pois para mim ele representa uma grande virada. Eu brincava com análise de expressões, conforme estávamos fazendo nesta série, mas achava que estava muito longe de conseguir tratar de uma linguagem completa. Afinal, linguagens REAIS tem desvios, laços, sub-rotinas e tudo mais. Talvez você tenha compartilhado comigo alguns destes pensamentos. Porém, mais tarde, tive que produzir estruturas de controle para um pré-processador estruturado de um assembler que estava construindo. Imagine só a minha surpresa quando descobri que era bem mais fácil do que as expressões matemáticas que estávamos analisando até então. Eu lembro de ter pensado, "Ei, isto é FÁCIL!". Depois de terminarmos esta parte, aposto como você vai estar pensando o mesmo!

O Plano
-------

Em sequência, iremos começar mais uma vez com um "berço" novo, e vamos fazer da mesma forma que já fizemos duas vezes: vamos construir as coisas uma de cada vez. Vamos ainda manter o conceito de um caracter único que nos ajudou tanto até agora. Isto significa que o código vai parecer um pouco engraçado, apenas com um "i" no lugar de IF, "w" no lugar de WHILE, etc. Mas ajuda a manter os conceitos sem perder muito tempo com análise léxica. Não tema... eventualmente vamos nos deparar com algo que parece com código "real".

Eu também não quero ficar me prendendo a lidar com comandos que não sejam condições, como comandos de atribuição nos quais estivemos trabalhando. Já foi demonstrado que podemos tratar deles, então não há por que ficar carregando isto como bagagem em excesso pelo resto do exercício. Então, o que eu farei ao invés disso é usar um comando anônimo, "outro", para ficar no lugar dos comandos que não são de controle e servir como um suporte para eles. Temos que gerar algum tipo de código para eles (lembre-se, estamos novamente compilando, não interpretando), então eu vou simplesmente mostrar o caracter da entrada.

Começando com [uma nova cópia do "berço"](src/cap01-craddle.c), vamos definir a rotina:

~~~c
/* reconhece e traduz um comando qualquer */
void Other()
{
    EmitLn("; %c", GetName());
}
~~~

> O caracter ";" indica um comentário de linha em assembly.

Agora inclua uma chamada no programa principal:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    Other();

    return 0;
}
~~~

Execute o programa e veja o que você obtem. Não muito excitante, não é mesmo? Mas vamos com calma, é só um começo e as coisas vão melhorar.

A primeira coisa necessária é a habilidade para tratar de mais de uma linha de comando, uma vez que uma condição de uma só linha é meio limitada. Nós fizemos isto na última parte sobre interpretadores, mas desta vez vamos ser mais formais. Considere a seguinte BNF:

~~~ebnf
    <program> ::= <block> END
    <block> ::= [ <statement> ]*
~~~

Isto quer dizer que, para os nossos objetivos, um programa é definido como um bloco seguido de um comando END. Um bloco, por sua vez, consiste de zero ou mais comandos. Nós só temos um tipo de comando até aqui.

O que indica o fim do bloco? Simplesmente, qualquer construção que não seja um comando "other". Por enquanto, isto significa apenas o comando END.

Com estas idéias em mente, podemos continuar construindo nosso analisador. O código para o programa é:

~~~c
/* analisa e traduz um programa completo */
void Program()
{
    Block();
    if (Look != 'e')
        Expected("End");
    EmitLn("; END");
}
~~~

Note que eu estou emitindo um "; END". Podemos considerá-lo como uma instrução que interrompe a execução do programa, e faz sentido também, afinal estamos compilando um programa completo.

O código de "block" é:

~~~c
/* analisa e traduz um bloco de comandos */
void Block()
{
    while (Look != 'e') {
        Other();
    }
}
~~~

Coloque estas rotinas no seu programa. Troque a chamada de `Other()` no programa principal para `Program()`. Agora teste e veja como funciona. Bem, ainda não é grande coisa, mas estamos chegando lá.

Uma preparação
--------------

Antes de começar a definir diversas estruturas de controle, precisamos preparar mais algumas coisas. Antes de mais nada, um aviso: Eu não vou usar as mesmas sintaxes das construções que você está acostumado a encontrar em Pascal ou C. Por exemplo, a sintaxe Pascal para um IF é:

~~~
    IF <condição> THEN <comando>
~~~

(onde o comando, logicamente, pode ser um bloco)

A versão em C:

~~~
    if ( <condição> ) <comando>
~~~

Ao invés disso, vou usar algo que lembra um pouco a linguagem Ada:

~~~
    IF <condição> <bloco> ENDIF
~~~

Em outras palavras, a construção IF tem um símbolo de terminação específico. Isto evita problemas com "else" perdido como em Pascal e C e também elimina a necessidade de chaves {} ou begin/else. A sintaxe que estou apresentando, na verdade, é da linguagem "KISS" que eu estarei detalhando em capítulos posteriores. As outras construções vão ser um pouco diferentes. Isto não deve ser um problema real pra você. Uma vez que você tenha visto como é feito, vai perceber que não importa muito que sintaxe está sendo usada. Uma vez que ela esteja definida, transformá-la em código é fácil.

Agora, toda construção que tratarmos aqui vai envolver transferência de controle, que significa em assembly, desvios condicionais e incondicionais. Por exemplo, o comando IF abaixo:

~~~
    IF <condição> A ENDIF B
~~~

deve ser traduzido como:

~~~
        se NÃO <condição> vá para L
        A
    L:  B
        ...
~~~

Está claro portanto, que nós vamos precisar de uma rotina a mais para ajudar a lidar com estes desvios. Eu a defini abaixo. A rotina `NewLabel()` gera um rótulo único. Isto é feito simplesmente chamando todo rótulo como "Lxx", onde xx é um número começando com zero.

Aqui está:

~~~c
/* gera um novo rótulo único */
int NewLabel()
{
    return LabelCount++;
}
~~~

Precisamos também de um comando para emitir o rótulo:

~~~c
/* emite um rótulo */
void PostLabel(int lbl)
{
    printf("L%d:\n", lbl);
}
~~~

Note que foi adicionada uma nova variável global chamada `LabelCount`, então adicione mais uma declaração de variável abaixo da definição de `Look`:

~~~c
int LabelCount; /* Contador usado pelo gerador de rótulos */
~~~

Adicione também sua inicialização em `Init()`, colocando zero como seu valor.

Neste ponto, gostaria de mostrar um novo tipo de notação. Se você comparar a forma do comando IF acima com o código assembly que deve ser produzido você vai notar que há certas ações associada com cada palavra chave no comando:

~~~
    IF: primeiro, pegar a condição e emitir o código para ela.
        depois, criar um rótulo único e
        emitir um desvio-se-falso para ele.

    ENDIF: emitir o rótulo criado.
~~~

Estas ações podem ser mostradas de forma concisa, se escrevermos a sintaxe assim:

~~~
    IF
    <condition>    { <condition>
                     L = NewLabel()
                     EmitLn(desvio se falso para L) }
    <block>
    ENDIF          { PostLabel(L) }
~~~

Isto é um exemplo de tradução dirigida pela sintaxe. Nós fizemos isso o tempo todo... apenas nunca a escrevemos desta forma antes. O que está dentro das chaves representa as AÇÕES que devem ser executadas. A parte interessante desta representação é que ela não só mostra o que deve ser reconhecido, mas também que ações temos que tomar, e em que ordem. Uma vez que temos esta sintaxe, o código praticamente está pronto.

A única coisa que falta fazer é ser mais específico sobre o que é um "desvio se falso".

Estou assumindo que haverá código executado por `Condition()` que vai tratar de álgebra booleana e computar algum resultado. Ele deve também alterar os flags de condição correspondentes ao resultado. Agora, a convenção usual para uma variável booleana é que 0000 represente "falso" e qualquer outra coisa (como FFFF, ou 0001) representa "verdadeiro".

No 80x86 os flags condicionais são alterados sempre que qualquer dado é movido ou calculado. Se o dado for 0000 (correspondente a um falso, lembra?) o flag correspondendo a "zero" (ZF - Zero Flag) será alterado para 1. O código para "desvie se zero" é JZ. Então, para os nossos propósito aqui:

~~~asm
    JZ      ; desvie se falso
    JNZ     ; desvie se verdadeiro
~~~

É de certa forma natural que a maioria dos desvios que vamos encontrar seja da forma JZ... nós vamos "contornar" o código que deveria ser executado se a condição fosse verdadeira.

O comando IF
------------

Com esta pequena explicação, finalmente estamos prontos para começar a codificar o comando IF no nosso analisador. Na verdade, nós quase já o fizemos! Como de costume, vamos usar nossa abordagem de um caracter só, "i" para IF, e "e" para ENDIF (como também para END... mas esta duplicidade não vai causar confusão). Eu também vou, por enquanto, pular completamente o caracter para a condição, que ainda temos que definir.

O código para `DoIf()` (repare que "if" é uma palavra reservada, logo, precisamos usar um identificador diferente) é:

~~~c
/* analisa e traduz um comando IF */
void DoIf()
{
    int l;

    Match('i');
    l = NewLabel();
    Condition();
    EmitLn("JZ L%d", l);
    Block();
    Match('e');
    PostLabel(l);
}
~~~

Adicione esta rotina ao programa, altere `Block()` para se referir a `DoIf()` desta forma:

~~~c
/* analisa e traduz um bloco de comandos */
void Block()
{
    while (Look != 'e') {
        switch (Look) {
            case 'i':
                DoIf();
                break;
            default:
                Other();
                break;
        }
    }
}
~~~

Note a referência à rotina `Condition()`. Eventualmente, vamos escrever uma rotina que possa analisar e traduzir expressões condicionais booleanas. Mas isto é assunto pra um capítulo inteiro ([o próximo](06_expressoes_booleanas.md), na verdade). Por enquanto, vamos apenas fazer uma rotina que só emite algum texto. Escreva a seguinte rotina:

~~~c
/* analisa e traduz uma condição */
void Condition()
{
    EmitLn("; condition");
}
~~~

Insira esta rotina e execute o programa. Teste algo assim:

~~~
    AiBeCe
~~~

>**Nota de tradução:** Recomendo usar letras maiúsculas para os identificadores, para não confundir com as diversas "palavras-chaves" de uma letra usadas a seguir. Se preferir, altere a função `GetName()` para indicar um erro se for usado um identificador em minúscula, conforme abaixo:
>
> ~~~c
> char GetName()
> {
>     char name;
> 
>     if (!isupper(Look))
>         Expected("Name");
>     name = Look;
>     NextChar();
> 
>     return name;
> }
> ~~~

Como você pode ver, o analisador reconhece a construção corretamente e insere o código nos lugares corretos. Agora tente alguns IFs aninhados, como:

~~~
    AiBiCeDeFe
~~~

Esta começando a parecer real, não?

Agora que já temos uma idéia geral (e as ferramentas de notação, e também as rotinas `NewLabel()` e `PostLabel()`), é uma moleza estender o analisador para incluir outras construções. A primeira (e também uma das mais complicadas) é adicionar a cláusula ELSE ao IF. A BNF é:

~~~
    IF <condition> <block> [ ELSE <block> ] ENDIF
~~~

É um pouco complicado por causa da parte opcional, que não ocorre em outras construções.

A saída correspondente deve ser:

~~~
        <condition>
        JZ L1
        <block>
        JMP L2
    L1:
        <block>
    L2:
        ...
~~~

O que nos leva à seguinte tradução dirigida pela sintaxe:

~~~
    IF
    <condition>     { L1 = newLabel
                      L2 = newLabel
                      EmitLn(JZ L1) }

    <block>
    ELSE            { EmitLn(JMP L2)
                      PostLabel(L1) }

    <block>
    ENDIF           { PostLabel(L2) }
~~~

Comparando isso com o caso de um IF sem ELSE nos dá uma dica de como tratar de ambas situações. O código abaixo faz isto. (Note que eu uso um "l" para ELSE, já que "e" está sendo usado pra outra coisa.)

~~~c
/* analisa e traduz um comando IF */
void DoIf()
{
    int l1, l2;

    Match('i');
    Condition();
    l1 = NewLabel();
    l2 = l1;
    EmitLn("JZ L%d", l1);
    Block();
    if (Look == 'l') {
        Match('l');
        l2 = NewLabel();
        EmitLn("JMP L%d", l2);
        PostLabel(l1);
        Block();
    }
    Match('e');
    PostLabel(l2);
}
~~~

Aí está. Um analisador/tradutor completo de um IF em 20 linhas de código. Altere também a função `Block()`. Troque o teste em `while (Look != 'e')` por `Look != 'e' && Look != 'l'`, ou então o L será tratado por `Other()` e nosso IF não vai funcionar.

Faça o teste agora, com alguma coisa assim:

~~~
    AiBlCeDe
~~~

Funcionou? Agora, pra ter certeza de que está tudo correto com o caso sem ELSE, teste:

~~~
    AiBeCe
~~~

Agora tente alguns IFs aninhados. Teste tudo o que quiser, incluindo alguns comandos mal formados. Apenas lembre-se que "e" não é um comando válido e sim um terminador.

O comando WHILE
---------------

O próximo tipo de comando que deve ser fácil, já que temos o processo assimilado. A sintaxe que eu escolhi para o WHILE é:

~~~
    WHILE <condition> <block> ENDWHILE
~~~

Eu sei, eu sei, nós não precisamos REALMENTE de tipos diferentes de terminadores para cada construção... você pode perceber isto pelo fato de que em nossa versão de um caracter, "e" é usado para todos eles. Mas eu também lembro de muitas horas de depuração em Pascal, tentando identificar um END que o compilador imaginou que eu deveria ter colocado em outro lugar. É uma questão de experiência que me leva a crer que palavras-chave únicas e específicas, apesar de aumentar o vocabulário da linguagem, adicionam um pouco de checagem de erro mas é um trabalho que vale a pena para o programador do compilador.

Agora, considere que o WHILE deve ser traduzido como:

~~~
    L1:
        <condition>
        JZ L2
        <block>
        JMP L1
    L2:
~~~

Como antes, comparar as duas representações nos dá uma idéia de que ações são necessárias em cada ponto.

~~~
    WHILE         { L1 = newLabel
                    L2 = newLabel
                    PostLabel(L1) }
    <condition>   { EmitLn(JZ L2) }
    <block>
    ENDWHILE      { EmitLn(JMP L1)
                    PostLabel(L2) }
~~~

O código segue diretamente a sintaxe:

~~~c
/* analisa e traduz um comando WHILE */
void DoWhile()
{
    int l1, l2;

    Match('w');
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    Condition();
    EmitLn("JZ L%d", l2);
    Block();
    Match('e');
    EmitLn("JMP L%d", l1);
    PostLabel(l2);
}
~~~

>**Nota de tradução:** Por favor, não confundam `DoWhile()` com o comando `do ... while (<cond>);` da linguagem C. O "do" foi acrescentado ao nome da rotina pra não confundí-lo com a palavra chave `while` de C.

Como temos um comando novo, temos que adicionar a chamada à rotina `Block()`:

~~~c
/* analisa e traduz um bloco de comandos */
void Block()
{
    while (Look != 'e' && Look != 'l') {
        switch (Look) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            default:
                Other();
                break;
        }
    }
}
~~~

Nenhuma outra mudança é necessária por enquanto.

Agora tente usar a nova construção. Note que desta vez, a condição está "dentro" do rótulo de cima, que é justamente onde queríamos que estivesse. Teste alguns laços aninhados. Teste alguns laços com IFs, e alguns IFs dentro de laços. Se você ficar meio confuso com o que deve ser feito, não fique desencorajado, você escreve bugs em outras linguagens também, não escreve? Não se preocupe, vai fazer mais sentido quando usarmos palavras chaves inteiras.

Eu espero que agora você esteja começando a captar a idéia de que isto é realmente fácil. Tudo o que temos a fazer pra acomodar a nova construção é trabalhar na tradução dirigida pela sintaxe dela. O código praticamente sai diretamente de lá, e quase não afeta outras rotinas. Uma vez que você tenha pego o jeito da coisa, logo vai perceber que é possível adicionar novas construções tão rápido quanto você consegue sonhar com elas.

O comando LOOP
--------------

Poderíamos parar por aqui e ter uma linguagem que funciona. Já foi provado que uma linguagem de alto nível com duas construções, o IF e o WHILE, são suficientes para escrever código estruturado. Mas já que estamos no meio do caminho, vamos enriquecer um pouco o repertório.

Esta estrutura é mais fácil ainda, já que ela não tem uma condição... é um laço de repetição infinito. Qual o objetivo de um laço destes? Nenhum, por si só, porém mais tarde, vamos adicionar o comando BREAK, que vai servir para sairmos do laço. Isto deixa a linguagem mais rica e evita coisas estranhas como `while (1)` ou `WHILE TRUE DO` de C e Pascal, respectivamente.

A sintaxe é simples:

~~~
    LOOP <block> ENDLOOP
~~~

e a tradução dirigida pela sintaxe:

~~~
    LOOP       { L = newLabel
                 PostLabel(L) }
    <block>
    ENDLOOP    { EmitLn(JMP L) }
~~~

O código correspondente é mostrado abaixo. Como eu já usei "l" pra ELSE, vou usar a última letra "p" como a "palavra-chave" no momento.

~~~c
/* analisa e traduz um comando LOOP */
void DoLoop()
{
    int l;

    Match('p');
    l = NewLabel();
    PostLabel(l);
    Block();
    Match('e');
    EmitLn("JMP L%d", l);
}
~~~

Quando você inserir esta rotina, não esqueça de adicionar uma linha em `Block()` para chamá-la.

Repeat-Until
------------

Aí está uma construção extraída diretamente do Pascal. A sintaxe é:

~~~
    REPEAT <block> UNTIL <condition>
~~~

A tradução dirigida fica assim:

~~~
    REPEAT         { L = newLabel
                     PostLabel(L) }
    <block>
    UNTIL
    <condition>    { EmitLn(JZ L) }
~~~

Como de costume o código é bem fácil:

~~~c
/* analisa e traduz um REPEAT-UNTIL*/
void DoRepeat()
{
    int l;

    Match('r');
    l = NewLabel();
    PostLabel(l);
    Block();
    Match('u');
    Condition();
    EmitLn("JZ L%d", l);
}
~~~

Como sempre, tivemos que adicionar a chamada de `DoRepeat()` a `Block()`. Desta vez há uma diferença. Eu decidi usar "r" para REPEAT (naturalmente), mas também decidi usar "u" para UNTIL. Isto significa que o "u" deve ser adicionado ao conjunto de caracteres no teste do `while` em `Block()`. Estes são os caracteres que indicam o fim do bloco atual... os caracteres "seguidores" (follow), em jargão de compiladores. Eu alterei a rotina pra deixar o teste dentro do `switch`. Assim fica mais simples:

~~~c
/* analisa e traduz um bloco de comandos */
void Block()
{
    int follow;

    follow = 0;

    while (!follow) {
        switch (Look) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'p':
                DoLoop();
                break;
            case 'r':
                DoRepeat();
                break;
            case 'e':
            case 'l':
            case 'u':
                follow = 1;
                break;
            default:
                Other();
                break;
        }
    }
}
~~~

O laço FOR
----------

O laço FOR é bastante útil pra se ter por perto, mas é complicado pra traduzir. Não por que a construção em si é difícil... é só um laço afinal de contas... mas simplesmente por que é difícil de implementar em linguagem assembly. Uma vez que o código é determinado, a tradução até que é simples.

Adoradores de linguagem C amam a construção FOR da linguagem (e na verdade, é até mais fácil de codificar para o compilador), mas eu escolhi uma sintaxe que é mais parecida com a do bom (?) e velho BASIC:

~~~
    FOR <ident> = <expr1> TO <expr2> <block> ENDFOR
~~~

A tradução de um laço FOR pode ser tão difícil quanto você quiser que ela seja, depende de como você quer que as regras sejam e como tratar dos limites. Por exemplo, a expressão "expr2" deve ser reavaliada toda vez que o laço repete ou deve ser tratada como um limite constante? Você sempre executa o laço ao menos uma vez, como em FORTRAN, ou não? Fica mais simples, se você adota o ponto de vista que a construção é equivalente a:

~~~
    <ident> = <expr1>
    temp = <expr2>
    WHILE <ident> <= temp
        <block>
    ENDWHILE
~~~

Note que com esta definição de um laço FOR, <block> não será executado nenhuma vez se <expr1> for inicialmente maior que <expr2>.

O código 80x86 pra fazer isto é mais complicado do que tudo o que já fizemos até agora. Eu fiz uma série de tentativas, colocando o contador e o limite superior na pilha, ambos como registradores, etc. Finalmente eu cheguei a um arranjo híbrido, em que o contador do loop está na memória (de forma que ele possa ser acessado pelo laço), e o limite superior está na pilha. O código traduzido fica assim:


~~~
        <ident>           ; pega o nome do contador do laço
        <expr1>           ; pega o valor inicial
        DEC AX            ; pré-decrementa
        MOV [<ident>], AX ; salva o valor do contador
        <expr2>           ; pega o limite superior
        PUSH AX           ; salva na pilha
    L1:
        MOV AX, [<ident>] ; coloca o contador em AX
        INC AX            ; incrementa o contador
        MOV [<ident>], AX ; salva o novo valor
        POP BX            ; pega limite superior...
        PUSH BX           ; ...mas devolve na pilha
        CMP AX, BX        ; compara contador com limite superior
        JG L2             ; termina se contador > limite superior
        <block>
        JMP L1            ; próximo passo
    L2:
        POP AX            ; retira limite superior da pilha
~~~

Uau! É um monte de código... a linha contendo <block> parece simplesmente se perder. Mas é o melhor que consegui fazer. Eu acho que ajuda manter em mente que são apenas alguns bytes da memória. Se alguém souber de uma maneira para otimizar isto, por favor me avisem.

Apesar disso, a rotina do analisador é bem simples agora que temos o código:

~~~c
/* analisa e traduz um comando FOR*/
void DoFor()
{
    int l1, l2;
    char name;

    Match('f');
    l1 = NewLabel();
    l2 = NewLabel();
    name = GetName();
    Match('=');
    Expression();
    EmitLn("DEC AX");
    EmitLn("MOV [%c], AX", name);
    Expression();
    EmitLn("PUSH AX");
    PostLabel(l1);
    EmitLn("MOV AX, [%c]", name);
    EmitLn("INC AX");
    EmitLn("MOV [%c], AX", name);
    EmitLn("POP BX");
    EmitLn("PUSH BX");
    EmitLn("CMP AX, BX");
    EmitLn("JG L%d", l2);
    Block();
    Match('e');
    EmitLn("JMP L%d", l1);
    PostLabel(l2);
    EmitLn("POP AX");
}
~~~

Como não temos expressões para este analisador, eu usei o mesmo truque de `Condition()`, e escrevi a rotina:

~~~c
void Expression()
{
    EmitLn("; EXPR");
}
~~~

Faça um teste. Mais uma vez, não esqueça de adicionar a chamada em `Block()`. Como não temos nenhuma entrada para esta versão de `Expression()`, uma entrada típica deveria parecer-se com:

~~~
    AfI=BeCe
~~~

Bem, isto gera um monte de código, não gera? Mas pelo menos é o código correto.

O Comando DO
------------

Tudo isso me fez desejar uma versão mais simples do laço FOR. A razão para todo este código acima é a necessidade de ter um contador de laço acessível dentro do mesmo. Se tudo o que nós desejamos é um laço que deve ser executado um certo número de vezes, mas que não requer acesso ao contador em si, há uma solução muito mais fácil. O 80x86 tem até uma instrução para facilitar a nossa vida, que decrementa o contador e desvia se não for zero. Vamos adicionar esta construção também. Esta vai ser a nossa última estrutura de laço.

A sintaxe e a tradução:

~~~
    DO
    <expr>    { expression
                EmitLn(MOV CX, AX)
                L = newLabel
                PostLabel(L)
                EmitLn(PUSH CX)
    <block>
    ENDDO     { EmitLn(POP CX)
                EmitLn(LOOP L) }
~~~

Isto é bem mais simples! O laço vai executar <expr> vezes. Este é o código:

~~~c
/* analisa e traduz um comando DO */
void DoDo()
{
    int l;

    Match('d');
    l = NewLabel();
    Expression();
    EmitLn("MOV CX, AX");
    PostLabel(l);
    EmitLn("PUSH CX");
    Block();
    Match('e');
    EmitLn("POP CX");
    EmitLn("LOOP L%d", l);
}
~~~

Eu acho que você tem que concordar, que isso é muito mais simples que o FOR clássico. Cada construção tem seu lugar.

O comando BREAK
---------------

Antes eu prometi a você um comando BREAK para acompanhar o comando LOOP. Isto é uma das coisas das quais eu me orgulho. Um comando BREAK parece algo realmente difícil. Minha primeira abordagem foi usá-lo como um terminador extra para `Block()`, e separar todos os laços em duas partes, exatamente como eu fiz com a metade ELSE do IF. Isto realmente não funcionou, pois o comando BREAK nem sempre vai aparecer no mesmo nível que o laço em si. O lugar mais comum pra um BREAK aparecer é logo após um IF, o que faria com que ele saísse da construção IF, e não do laço que o envolve. ERRADO! O BREAK tem que sair do laço mais interno, mesmo que esteja aninhado dentro de vários níveis de IF.

Minha segunda tentativa era armazenar em alguma variável global o rótulo final do laço mais interno. Isto também não funciona, pois pode haver um BREAK de um laço interno seguido de um BREAK de um laço mais externo. Armazenar o rótulo para o laço interno iria apagar o do mais externo. Então esta variável se transformou numa pilha. As coisas estavam começando a ficar bagunçadas.

Então eu decidi usar meu próprio conselho. Lembra-se da última parte quando eu disse como a pilha implícita de um analisador descendente recursivo estava nos ajudando? Eu disse que se você começar a ver uma necessidade para uma pilha externa você provavelmente estava fazendo alguma coisa errado. Bem, eu estava. É possível, de fato, fazer com que a recursão do analisador cuide de tudo, e a solução é tão simples que chega a ser surpreendente.

O segredo é notar que todo comando BREAK deve ocorrer dentro de um bloco... não há outro lugar para ele estar. Então tudo o que temos que fazer é passar para a rotina `Block()` o endereço de saída para o laço mais interno. Então ele pode passar o endereço para a rotina que traduz a instrução BREAK. Uma vez que o comando IF não faz nada para alterar o nível de aninhamento dos laços, a rotina `DoIf()` só tem que passar o rótulo para seus blocos (ambos se houver um ELSE). Como os laços ALTERAM os níveis, cada construção de laço simplesmente ignora o rótulo acima dele e passa o seu próprio rótulo de saída.

Tudo isto é mais simples de mostrar do que de descrever. Eu vou mostrar primeiro com o laço mais fácil, que é o LOOP:

~~~c
/* analisa e traduz um comando LOOP*/
void DoLoop()
{
    int l1, l2;

    Match('p');
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    Block(l2);
    Match('e');
    EmitLn("JMP L%d", l1);
    PostLabel(l2);
}
~~~

Note que agora `DoLoop()` tem 2 rótulos, não apenas um. O segundo é para dar à instrução BREAK um lugar para onde desviar. Se não houver nenhum BREAK dentro do laço, nós gastamos um rótulo à toa, mas isto não vai machucar ninguém.

Note também que agora `Block()` tem um parâmetro, que será sempre o endereço de saída do laço. A nova versão de `Block()` é:

~~~c
/* analisa e traduz um bloco de comandos */
void Block(int exitLabel)
{
    int follow;

    follow = 0;

    while (!follow) {
        switch (Look) {
            case 'i':
                DoIf(exitLabel);
                break;
            case 'w':
                DoWhile();
                break;
            case 'p':
                DoLoop();
                break;
            case 'r':
                DoRepeat();
                break;
            case 'f':
                DoFor();
                break;
            case 'd':
                DoDo();
                break;
            case 'b':
                DoBreak(exitLabel);
                break;
            case 'e':
            case 'l':
            case 'u':
                follow = 1;
                break;
            default:
                Other();
                break;
        }
    }
}
~~~

Novamente, repare que tudo o que `Block()` faz é passar o rótulo para `DoIf()` e `DoBreak()`. As construções de laço não precisam dele, pois elas vão passar o próprio rótulo de qualquer maneira.

A nova versão de `DoIf()` é:

~~~c
/* analisa e traduz um comando IF */
void DoIf(int exitLabel)
{
    int l1, l2;

    Match('i');
    Condition();
    l1 = NewLabel();
    l2 = l1;
    EmitLn("JZ L%d", l1);
    Block(exitLabel);
    if (Look == 'l') {
        Match('l');
        l2 = NewLabel();
        EmitLn("JMP L%d", l2);
        PostLabel(l1);
        Block(exitLabel);
    }
    Match('e');
    PostLabel(l2);
}
~~~

Agora, a única coisa que muda é a adição do parâmetro à rotina `Block()`. Um comando IF não muda o nível de aninhamento de laços, então `DoIf()` apenas passa o rótulo adiante. Não importa quantos níveis de aninhamento existam, o mesmo rótulo será usado.

Agora, lembre-se que `Program()` também chama `Block()`, agora ele também tem que passar um rótulo. A tentativa de sair do bloco mais externo é um erro, então `Program()` passa um rótulo inválido que é detectado por `DoBreak()`:

~~~c
/* analisa e traduz um comando BREAK */
void DoBreak(int exitLabel)
{
    Match('b');
    if (exitLabel == -1)
        Abort("No loop to break from.");
    EmitLn("JMP L%d", exitLabel);
}
~~~

E `Program()` fica assim:

~~~c
/* analisa e traduz um programa completo */
void Program()
{
    Block(-1);
    if (Look != 'e')
        Expected("End");
    EmitLn("; END");
}
~~~

Isto cuida de quase tudo. Teste e veja se é possível fazer um BREAK de qualquer laço. Com cuidado, é claro. Até aqui usamos tantas letras que é até difícil diferenciar o que representa palavras reservadas e o que não representa. Lembre-se: **antes de testar o programa você deve alterar cada ocorrência de `Block()` nas outras construções de laço para incluir o novo parâmetro.** Da mesma forma que eu fiz em `DoLoop()`.

Eu disse "quase" acima. Há um pequeno problema: se você der uma olhada no código gerado para DO, vai ver que se você sair deste laço, o valor do contador do laço vai ser deixado na pilha. Temos que arrumar isto! Uma vergonha... era uma das menores rotinas, mas não tem jeito. Aqui uma versão que não tem problemas:

~~~c
/* analisa e traduz um comando DO*/
void DoDo()
{
    int l1, l2;

    Match('d');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("MOV CX, AX");
    PostLabel(l1);
    EmitLn("PUSH CX");
    Block(l2);
    Match('e');
    EmitLn("POP CX");
    EmitLn("LOOP L%d", l1);
    EmitLn("PUSH CX");
    PostLabel(l2);
    EmitLn("POP CX");
}
~~~

As duas instruções extras, `PUSH CX` e `POP CX`, dão conta de deixar a pilha no estado correto.

Conclusão
---------

Neste ponto criamos uma série de construções de controle... um conjunto muito mais rico, realmente, que o proporcionado por muitas outras linguagens de programação. E, com a exceção do laço FOR, foi algo bem fácil de fazer. Mesmo este foi complicado só por causa da parte em assembly.

Vou concluir esta seção por aqui. Pra melhorar as coisas de vez, nós realmente deveríamos ter palavras-chave reais ao invés desta coisa de caracteres únicos. Você já viu que a extensão para palavras de mais de um caracter não é difícil, mas neste caso vai fazer uma grande diferença na aparência do código. Vou deixar isto para o próximo capítulo. Nele também vamos tratar de expressões booleanas, para nos livrarmos da versão vazia de `Condition()` que usamos agora. Até lá.

Para referência, aqui está o analisador completo para esta parte do tutorial:

~~~c
{% include_relative src/cap05-control.c %}
~~~

> Download do código-fonte [cap05-control.c](src/cap05-control.c)

{% include footer.md %}