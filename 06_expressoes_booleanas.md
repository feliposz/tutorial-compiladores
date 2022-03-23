Parte 6: Expressões booleanas
=============================

> **Autor:** Jack W. Crenshaw, Ph.D. (31/08/1988)<br>
> **Tradução e adaptação:** Felipo Soranz (18/05/2002)

Na [quinta parte](05_estruturas_controle.md) desta série, demos uma olhada nas estruturas de controle, e desenvolvemos rotinas para analisar e traduzir estas rotinas em código objeto. Terminamos com um conjunto bom e rico de construções.

Da maneira como deixamos o analisador, porém, há uma grande brecha em nossas capacidade: não tratamos do problema das condições. Para preencher o vazio, eu introduzi uma rotina "fantasma" chamada `Condition()`, que só servia como um marcador para alguma coisa real.

Uma das coisas que faremos nesta seção é preencher este vazio expandindo `Condition()` em um verdadeiro analisador/tradutor.

O Plano
-------

Nós vamos usar uma abordagem, neste capítulo, um pouco diferente da usada em todos os outros. Nos outros capítulos começamos imediatamente com experimentos diretos usando nosso compilador C, construindo o analisador desde princípios rudimentares até as formas finais, sem desperdiçar muito tempo planejando antecipadamente. Isto é chamado programar sem especificação e normalmente não é algo bem visto. Nós conseguimos nos dar bem com isso antes pois as regras aritméticas estão muito bem estabelecidas. Sabemos o que o sinal "+" significa sem precisar de uma longa discussão. O mesmo serve para condições e laços. Mas a maneira como as linguagens de programação implementam lógica pode variar muito de linguagem pra linguagem. Então, antes de começarmos a codificar seriamente, é melhor decidirmos melhor o que queremos. E a maneira de fazê-lo é no nível das regras de sintaxe BNF (a gramática).

A Gramática
-----------

Até agora, estivemos implementando as equações da sintaxe BNF para expressões aritméticas, sem nunca realmente tê-las escrito em lugar algum. É hora de fazer isto então. Elas são:

~~~ebnf
    <expression> ::= <unary op> <term> [<addop> <term>]*
    <term>       ::= <factor> [<mulop> <factor>]*
    <factor>     ::= <integer> | <variable> | "(" <expression> ")"
~~~

(Lembre-se, o bom da gramática é que ela obriga uma certa hierarquia na precedência de operadores que nós esperamos da álgebra.)

Na verdade, já que estamos falando disso, eu gostaria de dar uma pequena melhorada nesta gramática. A maneira como tratamos do menos unário (-1, -(3/2)) é um pouco esquisito. Eu acho que é melhor escrever a gramática desta forma:

~~~ebnf
    <expression>    ::= <term> [<addop> <term>]*
    <term>          ::= <signed-factor> [<mulop> <factor>]*
    <signed-factor> ::= [<addop>] <factor>
    <factor>        ::= <integer> | <variable> | "(" <expression> ")"
~~~

Isto faz com que o trabalho de lidar com o menos unário fique em `Factor()`, que é na verdade onde ele pertence.

Isto não quer dizer que você deve voltar e recodificar os programas que já escrevemos, embora você esteja livre pra fazer isto se quiser. Mas vou usar a sintaxe nova por enquanto.

Agora, provavelmente não vai ser um choque pra você aprender que podemos definir uma gramática análoga para a álgebra booleana. Um conjunto típico de regras seria:

~~~ebnf
    <b-expression> ::= <b-term> [<orop> <b-term>]*
    <b-term>       ::= <not-factor> [AND <not-factor>]*
    <not-factor>   ::= [NOT] <b-factor>
    <b-factor>     ::= <b-literal> | <b-variable> | "(" <b-expression> ")"
~~~

Note que nesta gramática, o operador AND é análogo ao "*", e OR (e OU-exclusivo, XOR) a "+". O operador NOT é como o menos unário. Esta hierarquia não é um padrão absoluto... algumas linguagens, como Ada, tratam todos os operadores como tendo o mesmo nível de precedência... mas isto parece mais natural.

Repare também a sutil diferença com que o operador NOT e o menos unário são tratados. Em álgebra, o menos unário é considerado um termo inteiro e nunca aparece a não ser em um dado termo. Então uma expressão como:

~~~
    a * -b
~~~

ou pior ainda,

~~~
    a - -b
~~~


não é permitida. Em álgebra booleana porém, a expressão

~~~
    a AND NOT b
~~~

faz sentido, e a sintaxe mostrada permite isto.

Operadores relacionais
----------------------

Certo, assumindo que você vai aceitar a gramática que eu mostrei acima, temos agora regras sintáticas para álgebra aritmética e booleana. A parte complicada é quando temos que combinar ambas. Por que temos que fazer isto? Bem, o assunto todo apareceu por precisarmos processar as condições associadas com as estruturas de controle como o IF. A condição precisa ter um valor booleano, ou seja, deve ter os valores VERDADEIRO ou FALSO. O desvio condicional então é feito ou não, dependendo do valor. O que esperamos ver então em `Condition()` é a avaliação de uma expressão "booleana".

Mas há mais do que isso. Uma expressão booleana pura pode ser em si a condição para uma estrutura de controle... como:

~~~
    IF a AND NOT b THEN ...
~~~

Só que vemos com mais frequência coisas como:

~~~
    IF (x >= 0) AND (x <= 100) THEN ...
~~~

Aqui, os dois termos em parênteses são expressões booleanas, mas os termos individuais sendo comparados: x, 0, e 100, são de natureza numérica. Os operadores relacionais >= e <= são os catalisadores que fazem com que os ingredientes aritméticos e booleanos se misturem.

Agora, no exemplo acima, os termos sendo comparados são apenas isto: termos. De qualquer forma, cada lado pode ser em geral qualquer expressão matemática. Então podemos definir uma relação como:

~~~ebnf
    <relation> ::= <expression> <relop> <expression>
~~~

onde as expressões das quais estamos falando são do tipo numérico, e os operadores relacionais são estes símbolos individuais:

~~~
    =, <> (ou !=), <, >, <= e >=
~~~

Se você pensar nisso um pouco, vai concordar que, uma vez que estes predicados tem um único valor, VERDADEIRO ou FALSO, como resultado, eles são apenas outro tipo de fator. Então podemos redefinir um fator booleano como:

~~~ebnf
    <b-factor> ::= <b-literal> | <b-variable> |
                   "(" <b-expression> ")" | <relation>
~~~

Aí está a conexão! Os operadores relacionais e a relação servem para conectar os dois tipos de álgebra. Vale a pena notar que isto implica numa hierarquia de precedência onde expressões aritméticas tem precedência maior que fatores booleanos, e portanto maior que qualquer outro operador booleano. Se você escrever os níveis de precedência para todos os operadores, vai acabar com a seguinte lista:

Nível|Elemento da sintaxe|Operador
:---:|-------------------|---------------------
0    |factor             |literal, variável
1    |signed factor      |menos unário
2    |term               |*, /
3    |expression         |+, -
4    |b-factor           |literal, variável, operador relacional
5    |not-factor         |NOT
6    |b-term             |AND
7    |b-expression       |OR, XOR

Se aceitarmos todos estes níveis de precedência, esta gramática vai parecer aceitável. Infelizmente, não vai funcionar! A gramática pode ser ótima na teoria, mas não é muito boa na prática para um analisador top-down. Para entender o problema, considere o seguinte fragmento:

~~~
    IF ((((((a + b + c) < 0) AND ...
~~~

Quando o analisador estiver analisando este código, ele sabe que após o IF deve haver uma expressão booleana que vem em seguida. Então ele se prepara pra analisar tal expressão. Mas a primeira expressão do exemplo é ARITMÉTICA, "a + b + c". O que é pior, no momento em que o analisador leu tudo isto da linha de entrada:

~~~
    IF ((((((a
~~~

ele ainda não tem como saber com que tipo de expressão está lidando. Isto não vai dar certo, pois temos que ter reconhecedores diferentes para os dois casos. A situação pode ser tratada sem alterar nossas definições, mas apenas se quisermos aceitar um número arbitrário de retornos para poder nos livrar de adivinhações erradas. Nenhum escritor de compilador estaria em seu perfeito estado mental concordando com isso.

O que acontece aqui é que a beleza e a elegância da gramática BNF encontrou-se face a face com as realidades da tecnologia de compiladores.

Para tratar desta situação, os projetistas de compiladores tiverem que fazer alguns acordos para que um analisador simples pudesse tratar da gramática sem prejuízos com retornos, etc.

Arrumando a gramática
---------------------

O problema que encontramos ocorre pois nossa definição de fatores aritméticos e booleanos permitem o uso de expressões com parênteses. Como as definições são recursivas, acabamos com qualquer número de níveis de parênteses, e o analisador não consegue saber com que tipo de expressão está lidando.

A solução é simples, embora acabe causando mudanças profundas na gramática. Só podemos aceitar parênteses em um tipo de fator. A maneira de fazer isto varia consideravelmente de linguagem pra linguagem. Este é um ponto onde não há acordo ou convenção para nos ajudar.

Quando Niklaus Wirth projetou a linguagem Pascal, ele desejou limitar o número de níveis de precedência (o que representa menos rotinas de análise, afinal de contas). Então OR e XOR (ou-exclusivo) são tratados como operadores de adição e processados no nível de expressão matemática. De maneira similar, o AND é tratado como um operador de multiplicação e é processado como um termo. Os níveis de precedência são:

Nível|Elemento da sintaxe|Operador
:---:|-------------------|-----------------
0    |factor             |literal, variável
1    |signed factor      |menos unário, NOT
2    |term               |*, /, AND
3    |expression         |+, -, OR

Note que há apenas um conjunto de regras sintáticas, aplicando-se a ambos tipos de operadores. De acordo com esta gramática, expressões como:

~~~
    x + (y AND NOT z) / 3
~~~

são perfeitamente legais. E na verdade são mesmo... desde que o analisador concorde. Pascal não permite a mistura de variáveis aritméticas e booleanas, e coisas como estas são tratadas no nível semântico, quando é hora de gerar código para elas, ao invés de no nível sintático.

Os autores da linguagem C tomaram uma abordagem diametricalmente oposta: eles tratam os operadores com diferença, e tem algo um pouco além dos nossos 7 níveis de precedência. Na verdade, em C há, nada menos que, 17 níveis! Isto ocorre pois C tem operadores como "=", "+=" e também, "<<", "++", etc. Ironicamente, apesar dos operadores C aritméticos e booleanos serem tratados separadamente, as variáveis NÃO são... não há variáveis booleanas ou lógicas em C, então um teste booleano pode ser feito em qualquer valor inteiro.

Nós vamos fazer uma coisa que está entre as duas abordagens. Eu me sinto tentado a ficar mais próximo da abordagem Pascal, pois parece mais simples do ponto de vista da implementação, mas que resulta em coisas estranhas que eu nunca gostei muito, como o fato da expressão

~~~
    IF (c >= 'A') AND (c <= 'Z') THEN ...
~~~

Os parênteses acima são OBRIGATÓRIOS! Eu nunca entendi isto antes, e nem meu compilador e nenhum humano explicou isto satisfatoriamente. Mas agora, pelo fato do operador AND ter a mesma precedência de uma multiplicação, ele tem também precedência maior que a dos operadores relacionais. Portanto, sem os parênteses, a expressão equivale a:

~~~
    IF c >= ('A' AND c) <= 'Z' THEN ...
~~~

O que não faz muito sentido.

Em todo caso, eu decidi separar os operadores em níveis diferentes, porém não em tantos quanto em C.

~~~ebnf
    <b-expression>  ::= <b-term> [<orop> <b-term>]*
    <b-term>        ::= <not-factor> [AND <not-factor>]*
    <not-factor>    ::= [NOT] <b-factor>
    <b-factor>      ::= <b-literal> | <b-variable> | <relation>
    <relation>      ::= <expression> [<relop> <expression]
    <expression>    ::= <term> [<addop> <term>]*
    <term>          ::= <signed-factor> [<mulop> factor]*
    <signed-factor> ::= [<addop>] <factor>
    <factor>        ::= <integer> | <variable> | ( <b-expression> )
~~~

Esta gramática resulta no mesmo conjunto de 7 níveis anterior. Realmente, é quase a mesma gramática... eu apenas removi a opção de expressões booleanas (b-expressions) com parênteses como um possível fator booleano (b-factor), e adicionei a relação (relation) como uma forma válida de b-factor.

Há uma diferença pequena, mas crucial, que faz com que tudo funcione. Note os colchetes na definição de `<relation>`. Isto faz com que o operador relacional (`<relop>`) e a segunda expressão sejam OPCIONAIS.

Uma consequência estranha desta gramática (que também é compartilhada por C) é que TODA expressão possível é potencialmente uma expressão booleana. O analisador vai sempre procurar uma expressão booleana, mas vai "aceitar" uma aritmética. Pra ser honesto, isto vai atrasar o analisador, pois ele vai ter que passar por mais camadas de chamadas de rotinas.

O Analisador
------------

Agora que já passamos pelo processo de tomada de decisão, podemos começar o desenvolvimento do analisador. Você já fez isto comigo várias vezes agora, então já deve conhecer o processo: começamos com uma cópia do "berço" e adicionamos as rotinas uma a uma. Então vamos lá.

Começamos, como fizemos antes no caso aritmético: tratando apenas de literais booleanos ao invés de variáveis. Isto nos dá um novo tipo de token de entrada. Vamos precisar portanto de um novo reconhecedor, e uma nova rotina para ler as instâncias deste tipo de token. Vamos começar definindo as duas novas rotinas:

~~~c
/* reconhece uma literal Booleana */
int IsBoolean(char c)
{
    return (c == 'T' || c == 'F');
}

/* recebe uma literal Booleana */
int GetBoolean()
{
    int boolean;

    if (!IsBoolean(Look))
        Expected("Boolean Literal");
    boolean = (Look == 'T');
    NextChar();

    return boolean;
}
~~~

Adicione estás duas rotinas ao programa. Você pode testá-las adicionando o seguinte ao programa principal:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    printf("%d", GetBoolean());
    return 0;
}    
~~~

Certo, agora compile e teste. Como de costume, não é muito impressionante até aqui, mas logo vai ser.

Quando estávamos tratando dos dados numéricos, tivemos que criar código para carregar os valores em AX. Temos que fazer o mesmo para dados booleanos. A maneira usual de codificar variáveis booleana é permitir que 0 signifique FALSO, e qualquer outra coisa signifique VERDADEIRO. Muitas outras linguagens, como C, usam o valor inteiro 1 para representá-lo, mas eu prefiro FFFF hexadecimal (ou -1), porque um NOT binário se torna também um NOT booleano. Então agora temos que emitir o código assembly correto para ler estes valores. O primeiro passo na direção do analisador sintático de expressões booleanas (`BoolExpression()`, claro) é:

~~~c
/* analisa e traduz uma expressão Booleana */
void BoolExpression()
{
    if (!IsBoolean(Look))
        Expected("Boolean Literal");
    if (GetBoolean())
        EmitLn("MOV AX, -1");
    else
        EmitLn("MOV AX, 0");
}
~~~

Adicione esta rotina ao analisador sintático, e chame-a do programa principal (substituindo o `printf()` que você colocou lá para teste). Como você pode ver, não temos muita coisa no analisador ainda, mas o código de saída está começando a parecer mais realista.

Depois, é claro, temos que expandir a definição de uma expressão booleana. Nós já temos a regra BNF:

~~~ebnf
    <b-expression> ::= <b-term> [<orop> <b-term>]*
~~~

Eu prefiro a versão Pascal para os operadores OR e XOR. Mas como estamos mantendo a abordagem de tokens de um único caracter, vou usar `|` e `~` respectivamente. A próxima versão de `BoolExpression()` é quase uma cópia da rotina aritmética de `Expression()`:

~~~c
/* analisa e traduz um termo Booleano */
void BoolTerm()
{
    if (!IsBoolean(Look))
        Expected("Boolean Literal");
    if (GetBoolean())
        EmitLn("MOV AX, -1");
    else
        EmitLn("MOV AX, 0");
}

/* reconhece e traduz um operador OR */
void BoolOr()
{
    Match('|');
    BoolTerm();
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* reconhece e traduz um operador XOR */
void BoolXor()
{
    Match('~');
    BoolTerm();
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}

/* analisa e traduz uma expressão booleana */
void BoolExpression()
{
    BoolTerm();
    while (IsOrOp(Look)) {
        EmitLn("PUSH AX");
        switch (Look) {
          case '|':
              BoolOr();
              break;
          case '~' :
              BoolXor();
              break;
        }
    }
}
~~~


Note o novo reconhecedor `IsOrOp()`, que é também uma cópia, desta vez da rotina `IsAddOp()`:

~~~c
/* reconhece um operador OU */
int IsOrOp(char c)
{
    return (c == '|' || c == '~');
}
~~~

A versão antiga de `BoolExpression()` virou `BoolTerm()`, no código acima. Compile e teste esta versão. Neste ponto, o código de saída está começando a parecer bom. É claro que não faz muito sentido fazer um monte de álgebra em valores constantes, mas logo vamos expandir os tipos de expressões com que lidamos.

Provavelmente você já adivinhou qual é o próximo passo: a versão booleana de `Term()`.

Renomeie a rotina `BoolTerm()` para `NotFactor()`, e entre com a nova versão de `BoolTerm()` abaixo. Repare que ela é bem mais simples que a versão numérica, logo que não há equivalente à divisão.

~~~c
/* analisa e traduz um termo booleano*/
void BoolTerm()
{
    NotFactor();
    while (Look == '&') {
        EmitLn("PUSH AX");
        Match('&');
        NotFactor();
        EmitLn("POP BX");
        EmitLn("AND AX, BX");
    }
}
~~~

Estamos quase chegando lá. Estamos traduzindo expressões booleanas complexas, já, embora apenas para valores constantes. O próximo passo é permitir o operador NOT. Escreva esta rotina agora:

~~~c
/* analisa e traduz um fator booleno com NOT opcional */
void NotFactor()
{
    if (Look == '!') {
        Match('!');
        BoolFactor();
        EmitLn("NOT AX");
    } else
        BoolFactor();
}
~~~

Mas não esqueça de renomear a anterior para `BoolFactor()`. Agora faça um teste. Neste ponto o analisador é capaz de tratar de qualquer expressão booleana que você testar, não é mesmo? Ele é capaz de detectar expressões mal formadas?

Se você tem acompanhado o que fizemos no analisador para expressões matemáticas, você sabe que o próximo passo foi expandir a definição de fator para acomodar variáveis e parênteses. Nós não precisamos fazer isto para o fator booleano, pois estes itens são tratados no próximo passo. Só precisamos alterar algumas linhas de código para fazer `BoolFactor()` tratar de relações:

~~~c
/* analisa e traduz um fator booleano */
void BoolFactor()
{
    if (IsBoolean(Look)) {
        if (GetBoolean())
            EmitLn("MOV AX, -1");
        else
            EmitLn("MOV AX, 0");
    } else
        Relation();
}
~~~

Talvez você esteja imaginando quando eu vou adicionar variáveis booleanas e expressões booleanas com parênteses. A resposta é, eu NÃO vou! Lembre-se, nós tiramos isto da gramática antes. Agora tudo o que estamos fazendo é codificar a gramática com a qual já concordamos. O compilador em si não sabe dizer a diferença de uma variável ou expressão booleana e seus equivalentes matemáticos... tudo isto vai ser tratado por `Relation()`, em ambos os casos.

É claro que vai ajudar termos algum código para `Relation()`. Eu não me sinto confortável no entanto, em adicionar mais código sem antes checar o que já temos. Então vamos apenas adicionar uma versão "fantasma" de `Relation()` que não faz nada a não ser aceitar um único caracter e imprimir uma pequena mensagem:

~~~c
/* analisa e traduz uma relação */
void Relation()
{
    EmitLn("; relation");
    NextChar();
}
~~~

Certo, copie este novo código e teste-o. Todas as outras coisas devem continuar funcionando, você deve conseguir gerar código para ANDs, ORs e NOTs. Além disso, se você entrar com algum outro caracter você deve ter um marcador "; relation", onde deveria estar um fator booleano. Entendeu? Muito bem, vamos passar agora para a versão completa de `Relation()`.

Para chegarmos lá porém, precisamos de mais algum preparo primeiro. Lembre-se que a forma de uma relação é:

~~~ebnf
    <relation> ::= <expression> [<relop> <expression>]
~~~

Como temos um novo tipo de operador, também precisamos de uma nova função para reconhecê-lo. Esta função é mostrada abaixo. Por termos uma limitação de um único caracter, eu vou ficar só com os 4 operadores que podem ser codificados assim (o "diferente" vai ser codificado como "#", meio estranho mas vai servir).

~~~c
/* reconhece operadores relacionais */
int IsRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}
~~~

Agora lembre-se que estamos usando um zero e um -1 no registrador AX para representar um valor booleano, e também que as construções de laço esperam os flags correspondentes. Implementando tudo isto em um 80x86, as coisas podem ficar um pouco complicadas.

Como as construções de laço operam apenas baseadas em flags, seria útil (e talvez mais eficiente) apenas alterar estes flags, e simplesmente não carregar nada em AX. Isto seria aceitável para as condições e os laços, mas lembre-se que a relação pode ser usada EM QUALQUER LUGAR em que um fator booleano pode ser usado também. Nós podemos armazenar seu resultado em uma variável booleana. Como não há como saber até aqui como o resultado vai ser usado, é melhor permitir ambos os casos.

Comparar dados numéricos é bem fácil... o 80x86 tem operadores que fazem isto... mas eles alteram os flags, não os valores. Além disso, eles alteram vários flags de acordo com o resultado da comparação. Nós precisamos saber apenas se a relação é verdadeira ou falsa.

Uma solução (talvez não a melhor) é usar o desvio condicional J??, onde ?? corresponde à condição a ser testada. No caso, JE, JNE, JG, JL, que significam: desvia se igual, desvia se não igual, desvia se maior, desvia se menor. Então nós mesmos atribuímos o valor para AX dependendo do resultado do desvio. Quando fazemos esta atribuição o flag de zero será alterado corretamente... zero (Z) para falso e não-zero (NZ) para verdadeiro.

Por exemplo, para fazer a comparação "1>2", usamos algo assim:

~~~
        MOV AX, 1    ; Pega o valor 1
        PUSH AX      ; Salva-o na pilha
        MOV AX, 2    ; Pega o valor 2
        POP BX       ; Recupera o valor da pilha (o primeiro)
        CMP BX, AX   ; Compara valores (BX é o 1o. e AX o 2o.)
        JG L1        ; Se maior, vai pra L1, senão
        XOR AX, AX   ; AX = 0 (FALSO)
        JMP L2       ; Vai pra L2
    L1:
        MOV AX, -1   ; AX = -1 (VERDADEIRO)
    L2:
~~~

>**Nota de tradução:** Existe uma instrução no 80x86 SET**XX**, que tem um funcionamento parecido com os desvios, JNE, JG, etc. Ela coloca 1 no registrador passado como parâmetro ou zero, dependendo do estado dos flags. Poderia se usar SETG AX no trecho acima e evitar os desvios. Como estamos usando -1 (FFFF) para valores verdaderos, bastaria usar um NEG AX para acertar o valor depois. Eu preferi manter a forma com os desvios pois ela vai funcionar na maioria dos montadores Assembly de 16-bits, enquanto a instruçãoSET**XX** só vai dar certo em montadores de 32-bits, mas sinta-se livre para alterar da forma que você preferir.

Eu devo mencionar agora que esta área, em minha opinião, é a que representa a maior diferença entre a eficiência do código programado à mão e o gerado pelo compilador. Nós já vimos que perdemos eficiência em operações aritméticas, embora eu tenha planejado mostrar mais tarde como melhorar isto um pouco. Nós já vimos também que as estruturas de controle em si podem ser feitas de forma bem eficiente... em geral é bem difícil melhorar o código gerado para um IF ou um WHILE. Porém, virtualmente todo compilador que eu já vi gera código terrível, comparado à programação manual, para computações envolvendo funções booleanas, e principalmente para relações. A razão é justamente o que eu falei acima. Quando escrevo código assembly, eu faço o teste da maneira mais conveniente possível, e faço a construção da maneira que deveria. Na verdade eu faço uma "lapidação" do código para cada situação. O compilador não tem como fazer isto (na prática), e também não tem como saber que nós não queremos armazenar o resultado em uma variável. Então ele precisa gerar o código em uma ordem muito particular, e no fim acaba carregando o resultado em uma variável booleana que pode acabar nunca sendo usada pra nada.

Em todo caso, nós estamos prontos agora para ver o código para `Relation()`. Ele é mostrado abaixo com suas rotinas acompanhantes:

>**Nota de tradução:** Repare que eu precisei acrescentar o código para `NewLabel()` e `PostLabel()` em nosso programa por causa dos desvios. Não se esqueça de acrescentá-lo também, junto com a declaração da variável de controle dos rótulos.

~~~c
/* reconhece e traduz um operador de igualdade */
void Equals()
{
    int l1, l2;

    Match('=');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JE L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* reconhece e traduz um operador de não-igualdade */
void NotEquals()
{
    int l1, l2;

    Match('#');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JNE L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* reconhece e traduz um operador de maior que */
void Greater()
{
    int l1, l2;

    Match('>');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JG L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* reconhece e traduz um operador de menor que */
void Less()
{
    int l1, l2;

    Match('<');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JL L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* analisa e traduz uma relação */
void Relation()
{
    Expression();
    if (IsRelOp(Look)) {
        EmitLn("PUSH AX");
        switch (Look) {
            case '=':
                Equals();
                break;
            case '#':
                NotEquals();
                break;
            case '>':
                Greater();
                break;
            case '<':
                Less();
                break;
        }
    }
}
~~~

Agora a chamada a `Expression()` parece familiar! É nesse momento que o editor do nosso sistema se torna útil. Nós já geramos o código para `Expression()` e seus amigos em sessões anteriores. Você poderia copiá-los em seu arquivo agora se quisesse. Lembre-se de usar as versões de um caracter apenas. Mas, apenas para ter certeza, eu dupliquei as rotinas aritméticas abaixo. Se você for observador, vai ver que fiz algumas pequenas mudanças para fazê-las corresponder à última versão da sintaxe. Estas mudanças NÃO são necessárias, então se você preferir pode manter a versão anterior até ter certeza de que tudo está funcionando. Repare que `Factor()` agora deve chamar `BoolExpression()` e não `Expression()` (consulte a gramática que definimos anteriormente).

~~~c
/* analisa e traduz um identificador */
void Ident()
{
    char name;

    name = GetName();
    if (Look == '(') {
        Match('(');
        Match(')');
        EmitLn("CALL %c", name);
    } else
        EmitLn("MOV AX, [%c]", name);
}

/* analisa e traduz um fator matemático */
void Factor()
{
    if (Look == '(') {
        Match('(');
        BoolExpression();
        Match(')');
    } else if(isalpha(Look))
        Ident();
    else
        EmitLn("MOV AX, %c", GetNum());
}

/* analisa e traduz um fator com sinal opcional */
void SignedFactor()
{
    int minusSign = (Look == '-');
    if (IsAddOp(Look))
    {
        NextChar();
        SkipWhite();
    }
    Factor();
    if (minusSign)
        EmitLn("NEG AX");
}

/* reconhece e traduz uma multiplicação */
void Multiply()
{
    Match('*');
    Factor();
    EmitLn("POP BX");
    EmitLn("IMUL BX");
}

/* reconhece e traduz uma divisão */
void Divide()
{
    Match('/');
    Factor();
    EmitLn("POP BX");
    EmitLn("XCHG AX, BX");
    EmitLn("CWD");
    EmitLn("IDIV BX");
}

/* analisa e traduz um termo matemático */
void Term()
{
    SignedFactor();
    while (IsMulOp(Look)) {
        EmitLn("PUSH AX");
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

/* reconhece e traduz uma soma */
void Add()
{
    Match('+');
    Term();
    EmitLn("POP BX");
    EmitLn("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void Subtract()
{
    Match('-');
    Term();
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    EmitLn("NEG AX");
}

/* analisa e traduz uma expressão matemática */
void Expression()
{
    Term();
    while (IsAddOp(Look)) {
        EmitLn("PUSH AX");
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
~~~

Aí está... um analisador que é capaz de tratar de álgebra aritmética e booleana, e meios capazes de combiná-las através de operadores relacionais. Eu sugiro que você faça uma [cópia](src/cap06-bool1.c) deste analisador para referência futura, pois logo vamos mexer um pouco com isto.

Adicionando estruturas de controle
----------------------------------

Neste ponto, podemos voltar ao arquivo que construímos antes para analisar [estruturas de controle](src/cap05-control.c). Lembra daquelas rotinas fantasmas chamadas `Condition()` e `Expression()`? Agora você sabe o que vai no lugar delas!

Eu aviso que você vai precisar fazer algumas edições bem criativas aqui, então não tenha pressa e faça o melhor possível. O que você deve fazer é:

- Copie todas as rotinas do analisador lógico, que não fazem parte do "berço" no analisador de estruturas de controle. 
- Apague a rotina `Condition()` e também a versão fantasma de `Expression()`. 
- Depois, altere toda chamada a `Condition()` para `BoolExpression()`. 
- Copie `IsOrOp()`, `IsRelOp()`, `IsBoolean()`, `GetBoolean()` em seu devido lugar. 
- No programa principal, altere `BoolExpression()` para `Program()`.
- Isto deve funcionar. Não esqueça de copiar os protótipos também.

Compile o programa resultante e faça um teste. Como não usamos este programa por um temo, não se esqueça que nós estamos usando os tokens de um só caracter para IF, WHILE, etc. Então não esqueça que toda letra que não é uma palavra-chave é apenas exibida como um bloco.

Tente

~~~
    iA=BXlYee
~~~

que significa `IF a=b x ELSE y ENDIF END`. (O END final indica o fim de programa, lembra?)

Teste mais este aqui `x FOR a = 1 TO 9 y ENDFOR z END`:

~~~
    XfA=1t9YeZe
~~~

Não deu muito certo, não é? Parece que ele está tendo problemas com o "t". Ele pensa que o "t" é uma variável, mas deveria ser a palavra-chave TO. Claro, a versão atual de for não reconhece o "t". Apenas acrescente uma chamada a `Match('t')` após o primeiro `Expression()` de `DoFor()`.

O que você acha? Funcionou? Teste outros exemplos.

Adicionando Comandos de Atribuição
----------------------------------

Como chegamos até aqui, e já temos as rotinas de expressões em seu devido lugar, podemos já trocar os "blocos" imaginários por comandos de atribuição reais. Nós já fizemos isto antes, então não vai ser tão difícil. Antes de fazer isto, porém, precisamos arrumar outra coisa.

Logo vamos perceber que nossos "programas" de uma só linha que escrevemos até agora vão logo destruir nosso estilo. Até o momento tivemos que conviver com isto, pois nosso analisador não reconhece o fim de uma linha. Então, antes de continuar vamos arrumar isto.

Há varias formas de tratar do fim da linha. Uma (a abordagem C/Unix) é tratá-lo como um simples caracter de espaço e ignorá-lo. Não é na verdade uma abordagem ruim, mas ela produz alguns resultados estranhos para o nosso analisador como ele está agora. Se ele estivesse lendo sua entrada de um arquivo real como qualquer verdadeiro compilador de respeito faria, não haveria problema. Mas estamos lendo a entrada do teclado, e estamos condicionados a esperar alguma coisa acontecer quando pressionamos a tecla ENTER. Não vai acontecer nada se simplesmente pularmos o retorno de linha. Então eu vou usar uma abordagem diferente aqui, que não é necessariamente a melhor abordagem no geral. Considere apenas como algo temporário até que resolvamos seguir em frente.

Ao invés de pular o retorno de linha, vamos permitir que o analisador trate dele, então vamos introduzir uma rotina especial, análoga a `SkipWhite()`, que pula o retorno de linha apenas em pontos específicos "válidos".

Eis a rotina:

~~~c
/* reconhece uma linha em branco */
void NewLine()
{
    if (Look == '\n')
        NextChar();
}
~~~

Agora, acrescente duas chamadas a `NewLine()` na rotina `Block()`, assim:

~~~c
/* analisa e traduz um bloco de comandos */
void Block(int exitLabel)
{
    int follow;

    follow = 0;

    while (!follow) {
        NewLine();
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
        NewLine();
    }
}
~~~

Agora, você vai descobrir que é possível usar programas de múltiplas linhas. A única restrição é que não é possível separar os tokens IF ou WHILE, por exemplo, de seus predicados.

Agora estamos prontos para incluir o comando de atribuição. Apenas altere aquela chamada a `Other()` na rotina `Block()` para uma chamada a `Assignment()`, e adicione a seguinte rotina, copiada de um de nossos programas anteriores. Note agora que `Assignment()` chama `BoolExpression()`, para podermos atribuir valores booleanos.

~~~c
/* analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    BoolExpression();
    EmitLn("MOV [%c], AX", name);
}
~~~

Com esta mudança, é possível agora escrever programas razoavelmente realísticos, sujeitos apenas à nossa limitação de tokens de um só caracter. Minha intenção original era livrar-me desta limitação pra você. Porém, isto iria precisar de uma mudança ainda maior do que já fizemos. Nós precisamos de um analisador léxico (lexical scanner), e isto requer algumas mudanças estruturais. NÃO são mudanças GRANDES que nos obrigam a jogar fora tudo o que fizemos até agora... com cuidado, pode ser feito com mudanças mínimas, na verdade. Mas isto requer cuidado de qualquer forma.

Este capítulo acabou sendo longo demais, e contém algumas coisas bem "pesadas", então eu decidi deixar este próximo passo para depois, quando você vai ter mais tempo para entender o que fizemos e esteja pronto para começar outra vez. 

Eis o [código completo](src/cap06-bool2.c) do nosso compilador até aqui:

~~~c
{% include_relative src/cap06-bool2.c %}
~~~

> Download do código fonte: [cap06-bool1.c](src/cap06-bool1.c) / [cap06-bool2.c](src/cap06-bool2.c)

No [próximo capítulo](07_analise_lexica.md), vamos construir um analisador léxico e eliminar a barreira de um caracter de uma vez por todas. Nós vamos também escrever pela primeira vez nosso primeiro compilador completo, baseado no que fizemos nesta seção. Até lá!

{% include footer.md %}