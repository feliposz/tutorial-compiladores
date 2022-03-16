# 2 - Análise de Expressões

Se você já leu a introdução desta série, você já estará por dentro do que estamos fazendo. Você provavelmente deve ter compilado o "berço" em seu compilador C favorito e executado ele com sucesso. Portanto, você deve estar pronto para começar.

O objetivo deste artigo é aprender como analisar e traduzir expressões matemáticas. O que nós gostaríamos de ver como saída é uma série de comandos em linguagem assembly que execute as ações desejadas. Para propósitos de definição, uma expressão é o lado direito de uma equação, como em:

    x = 2*y + 3/(4*z)

Eu vou começar com passos BEM pequenos, de forma que os mais inexperientes não fiquem totalmente perdidos. Há também algumas lições muito boas pra se aprender desde cedo que vão servir pra mais tarde. Para os mais experientes: confiem em mim. Nós vamos começar com a parte mais séria logo, logo.

## Dígitos simples

Mantendo o mesmo lema desta série (KISS, lembra?), vamos começar com o caso mais simples que podemos pensar. Pra mim, é uma expressão que consiste de um único dígito.

Antes de começar a programar, certifique-se de ter uma cópia do "berço" que você viu na lição passada. Nós vamos usá-la novamente para outros experimentos. Adicione este código:

~~~c
/* analisa e traduz uma expressão */
void expression()
{
    emit("MOV AX, %c", getNum());
}
~~~

Não esqueça de adicionar o protótipo desta função ao programa. Depois adicione a chamada a `expression` no programa principal, de forma que fique assim:

~~~c
/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    expression();

    return 0;
}
~~~

Agora execute o programa. Tente um dígito único qualquer como entrada. Você deve obter uma linha única de saída em linguagem assembly. Agora tente qualquer outro caracter como entrada. Você vai notar que o analisador indica um erro.

**PARABÉNS! Você acaba de criar um tradutor que funciona!**

OK, eu garanto que ele é bastante limitado. Mas não o tome por pouca coisa. Este pequeno "compilador" faz, em uma escala bastante limitada, o que todo compilador de porte faz: ele corretamente reconhece instruções válidas na "linguagem" de entrada definida, e produz código executável assembly correto e apropriado para montagem (assembling) em um formato objeto. Tão importante quanto isso, ele reconhece corretamente instruções INVÁLIDAS e dá uma mensagem de erro compreensível. Quem esperava mais que isso? Conforme expandimos nosso analisador, é melhor ter certeza que estas duas características permaneçam verdadeiras.

Há outras características deste pequeno programa que valem a pena mencionar. Primeiro, você pode notar que nós não separamos a geração de código da análise... uma vez que o analisador sabe o que nós queremos que seja feito, ele gera o código objeto diretamente. Em um compilador real, as leituras na função `nextChar` vão ser de um arquivo em disco, e a saída vai ser para outro arquivo, mas deste jeito é melhor enquanto estamos apenas experimentando.

Observe também que a expressão (MOV) tem que deixar o resultado em algum lugar. Eu escolhi o registrador AX. Eu poderia ter escolhido outro, mas este faz sentido.

## Expressões binárias

Agora que já contamos com isso, vamos continuar. Assumidamente, uma "expressão" consistindo de apenas um único caracter não vai nos ajudar a alcançar nossas necessidades por muito tempo, portanto, vejamos o que podemos fazer para estender nosso compilador. Supondo que queremos lidar com expressões da seguinte forma:

                    1  +  2
    ou              4  -  3
    ou então, <termo> +/- <termo>
    (Isto é um "pouco" de Backus-Naur Form, ou BNF)

Para fazer isso, precisamos de uma rotina que reconhece um termo e deixa seu resultado em algum lugar, e outra que reconhece e sabe diferenciar um "+" e um "-" e gera o código apropriado. Mas se `expression` vai deixar seu resultado em AX, onde `term` (a função que analisa os termos) deve deixar seu resultado? Resposta: no mesmo lugar. Nós precisamos salvar o primeiro resultado de `term` em algum outro lugar antes de pegar o segundo.

OK, basicamente o que nós queremos é que a rotina `term` faça o que a rotina `expression` estava fazendo anteriormente, portanto, nós vamos apenas RENOMEAR a rotina `expression` para `term`, e entrar com a seguinte nova versão de `expression`:

~~~c
/* reconhece e traduz uma expressão */
void expression()
{
    term();
    emit("MOV BX, AX");
    switch (look) {
        case '+':
            add();
            break;
        case '-':
            subtract();
            break;
        default:
            expected("AddOp");
            break;
    }
}
~~~

Adicione também as seguintes rotinas:

~~~c
/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    emit("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void subtract()
{
    match('-');
    term();
    emit("SUB AX, BX");
}
~~~

Você deverá ter então as seguintes rotinas novas:

- `term` (antiga `expression`)
- `add`
- `subtraction`
- `expression`

Obs: A ordem delas não importa, desde que os protótipos esteja propriamente declarados.

Agora execute o programa. Tente todas as combinações que você conseguir pensar de dois dígitos, separados por um "+" ou um "-". Você deve obter uma série de quatro instruções assembly de cada vez que executar. Depois experimente expressões com erros de propósito. O analisador pegou os erros?

Dê uma olhada no código objeto gerado. Há duas observações que podemos fazer. Primeiro, o código gerado NÃO é o que nós mesmos escreveríamos se estivéssemos fazendo o programa diretamente em assembly. A sequência:

~~~asm
    MOV AX, <valor>
    MOV BX, AX
~~~

é ineficiente. Se estivéssemos escrevendo este código manualmente, iríamos provavelmente carregar os dados diretamente no registrador BX.

Há uma lição a se tirar disso: o código gerado pelo nosso compilador é menos eficiente que o código que escreveríamos manualmente. Acostume-se com isso. Isto será válido no decorrer da série. É válido para a maioria dos compiladores de certa forma. Cientistas da computação tem devotado vidas inteiras ao problema de otimização de código, e há de fato algo que pode ser feito para melhorar a qualidade do código gerado. Alguns compiladores fazem isto muito bem, mas há um preço muito alto a ser pago em complexidade, e é uma batalha perdida de qualquer forma... provavelmente nunca vai haver um tempo em que um bom programador assembly não consiga produzir código melhor que um compilador. Antes de terminar esta seção eu vou mostrar brevemente algumas maneiras de fazer uma pequena otimização, apenas pra mostrar que nós podemos otimizar as coisas sem muitos problemas. Mas lembre-se, estamos aqui pra aprender, não pra ver quão otimizado o código objeto pode ficar. Por enquanto, e na verdade por toda a série de artigos, vamos ignorar a otimização e concentrar-nos em gerar código que funciona.

Falando nisso: o nosso NÃO FUNCIONA! O código está ERRADO! Conforme as coisas estão funcionando agora, o processo de subtração está subtraindo BX (que é o PRIMEIRO argumento na verdade) de AX (que é o segundo). Este é o jeito errado! Portanto, nós acabamos com o sinal errado para o resultado. Vamos arrumar o procedimento `subtract` com uma mudança de sinal, de forma que fique:

~~~c
/* reconhece e traduz uma subtração */
void subtract()
{
    match('-');
    term();
    emit("SUB AX, BX");
    emit("NEG AX");
}
~~~

Agora o código está menos eficiente ainda, mas pelo menos ele dá a resposta certa! Infelizmente, as regras que dão o sentido de expressões matemáticas requerem que os termos de uma expressão se apresentem em uma forma inconveniente para nós. Novamente, isto é apenas um dos fatos da vida com os quais temos que aprender a conviver. Isto vai voltar a nos assombrar quando chegarmos na divisão.

OK, neste momento temos um analisador que reconhece a soma e a diferença entre dois dígitos. Antes, nós só conseguíamos reconhecer um único dígito. Mas expressões de verdade podem ter ambas as formas (ou uma infinidade de outras). Só pra ter certeza, volte e rode o programa com um único dígito: "1".

Não deu certo, deu? E por que deveria? Nós acabamos de dizer ao nosso analisador que o único tipo de expressão que ele deve aceitar são aquelas com dois termos. Nós temos que reescrever a rotina `expression` de forma que ela seja mais abrangente, e é aqui que as coisas começam a tomar a forma de um analisador de verdade.

## Expressões gerais

No mundo real, uma expressão pode consistir de um ou mais termos, separados por operadores de soma ("+" ou "-"). Em BNF, isto é escrito:

    <expression> ::= <term> [<addop> <term>]*

Nós podemos acomodar esta definição de uma expressão com a adição de um simples laço de repetição na rotina `expression`:

~~~c
/* reconhece e traduz uma expressão */
void expression()
{
    term();
    while (look == '+' || look == '-') {
        emit("MOV BX, AX");
        switch(look) {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
            default:
                expected("AddOp");
                break;
        }
    }
}
~~~

AGORA nós estamos chegando a algum lugar! Esta versão consegue lidar com qualquer número de termos, e só nos custou algumas linhas extras de código. Conforme nós continuamos, você vai perceber que isto é característico de analisadores top-down... você só precisa de algumas linhas de código para acomodar extensões à linguagem. É isto que torna nossa abordagem incremental possível. Note, também, como o código da rotina `expression` combina com a definição em BNF. Isto, também, é característico do método. Conforme você se torna proficiente na abordagem, vai notar que é possível transformar BNF em código do analisador tão rápido quando você consegue digitar!

OK, então compile a nova versão do nosso analisador, e faça alguns testes. Como de costume, verifique que o "compilador" consegue lidar com qualquer expressão válida e que ele vai dar uma mensagem de erro para qualquer expressão inválida. Legal, não é? Você vai notar em sua versão de teste que toda mensagem de erro acaba embutida no meio do código já gerado. Mas lembre-se, isto é apenas por estarmos usando o vídeo como "arquivo de saída" para estas experiências. Em uma versão final, as duas saídas devem ser separadas: uma para um arquivo de saída e outra pra tela.

## Usando a pilha

Neste momento, eu devo violar a minha própria regra que diz que não devemos adicionar complexidade até que seja absolutamente necessário, apenas para apontar um problema com o código que estamos gerando. Como as coisas estão agora, o compilador usa AX como o registrador "primário", e BX como um lugar para armazenar a soma parcial. Isto funciona bem por enquanto, pois enquanto estamos lidando apenas com "+" e "-", todo termo novo pode ser adicionado assim que é encontrado. Mas no geral, isto não é verdade. Considere, por exemplo, a expressão:

    1+(2-(3+(4-5)))

Se colocarmos o "1" em BX, onde vamos colocar o "2"? Como uma expressão geral pode ter qualquer grau de complexidade, nós vamos em breve ficar sem registradores!

Felizmente, há uma solução simples. Como todo processador moderno, o 80x86 possui uma pilha, onde é o lugar perfeito para salvar um número variável de itens. Portanto, ao invés de mover o termo em AX para BX, vamos simplesmente colocá-lo (push) na pilha. Para aqueles que não estão acostumados com assembly para 80x86 para colocar e tirar algo da pilha use:

~~~asm
    PUSH registrador
    POP registrador
~~~

Portando, vamos alterar as funções `expression`, `add` e `subtract`, de forma que fiquem assim:

~~~c
/* reconhece e traduz uma expressão */
void expression()
{
    term();
    while (look == '+' || look == '-') {
        emit("PUSH AX");
        switch(look) {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
            default:
                expected("AddOp");
                break;
        }
    }
}

/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    emit("POP BX");
    emit("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void subtract()
{
    match('-');
    term();
    emit("POP BX");
    emit("SUB AX, BX");
    emit("NEG AX");
}
~~~

Agora tente o analisador novamente para ter certeza de que não estragamos nada.

Mais uma vez, o código gerado é menos eficiente do que antes, mas é um passo necessário, como você vai ver.

## Multiplicação e Divisão

Agora vamos começar com algo REALMENTE sério. Como vocês todos sabem, há outros operadores matemáticos diferentes de "+" e "-"... expressões podem ter operações de multiplicação e divisão. Você também sabe que há um "operador" implícito chamado PRECEDÊNCIA, ou hierarquia, associado com as expressões, de forma que em uma expressão como

    2 + 3 * 4

sabemos que é correto multiplicar PRIMEIRO, e então somar. (Viu por que é necessária a pilha?)

Nos primeiros dias da tecnologia de compiladores, as pessoas usaram algumas técnicas bem complexas pra ter certeza que as regras de precedência de operadores fossem obedecidas. No entanto, nada disso é necessário... as regras podem ser acomodadas facilmente pela nossa técnica de analise top-down. Até agora, a única forma que consideramos para um termo é a de um único dígito decimal.

Mais geralmente, podemos redefinir um termo como um PRODUTO de FATORES, isto é:

    <term> ::= <factor> [ <mulop> <factor> ]*

O que é um fator? Por enquanto, é o que um termo costumava ser... um dígito simples.

Note a simetria: um termo tem a mesma forma de uma expressão. Nós podemos adicionar isso ao nosso compilador cuidadosamente apenas copiando e renomeando as coisas. Mas para evitar confusão, abaixo está a listagem completa do conjunto de rotinas de análise sintática. (Repare no modo como é tratada a inversão de operandos na divisão. Além disso, é preciso usar a instrução CWD antes de dividir, pois a multiplicação e a divisão usam o par de registradores DX:AX para armazenar os valores. CWD expande o valor de AX para DX:AX. Para quem não conhece bem estas particularidades do processador 80x86, o assunto é tratado mais pra frente, portanto não se preocupe.)

~~~c
/* analisa e traduz um fator matemático */
void factor()
{
    emit("MOV AX, %c", getNum());
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    emit("POP BX");
    emit("IMUL BX");
}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}

/* analisa e traduz um termo */
void term()
{
    factor();
    while (look == '*' || look == '/') {
        emit("PUSH AX");
        switch(look) {
            case '*':
                multiply();
                break;
            case '/':
                divide();
                break;
            default:
                expected("MulOp");
                break;
        }
    }
}

/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    emit("POP BX");
    emit("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void subtract()
{
    match('-');
    term();
    emit("POP BX");
    emit("SUB AX, BX");
    emit("NEG AX");
}

/* reconhece e traduz uma expressão */
void expression()
{
    term();
    while (look == '+' || look == '-') {
        emit("PUSH AX");
        switch(look) {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
            default:
                expected("AddOp");
                break;
        }
    }
}
~~~

Caramba! Um analisador/tradutor QUASE funcional, em cerca de 80 linhas de linguagem C! A saída está começando a parecer realmente útil, se você continuar a ignorar a ineficiência, e eu espero que você continue. Lembre-se, não estamos tentando produzir código perfeito aqui.

## Parênteses

Nós podemos melhorar esta parte do analisador com a adição de parênteses com expressões matemáticas. Como você sabe, parênteses são mecanismos que forçam uma determinada ordem de precedência dos operadores. Por exemplo, na expressão:

    2 * (3+4)

Os parênteses forçam a adição antes da multiplicação. Mais importante que isso, os parênteses nos dão um mecanismo para definir expressões com qualquer grau de complexidade, como:

    (1+2) / ((3+4) + (5-6))

A chave para incorporar parênteses em nosso analisador é perceber que não importa quão complicada uma expressão entre parênteses possa ser, para o resto do mundo ela não passa de um simples fator! Ou seja, uma das formas de um fator é:

    <factor> ::= ( <expression> )

É aqui que a recursão entra em ação. Uma expressão pode conter um fator, que pode conter outra expressão, que pode conter um fator, e assim por diante, infinitamente.

Complicado ou não, podemos cuidar disso ajustando apenas umas poucas linhas do programa na rotina factor():

~~~c
/* analisa e traduz um fator */
void factor()
{
    if (look == '(') {
        match('(');
        expression();
        match(')');
    } else
        emit("MOV AX, %c", getNum());
}
~~~

Note novamente como foi simples estender o analisador, e como o código se assemelha à sintaxe BNF.

Como de costume, teste a nova versão para ter certeza que ela compila corretamente sentenças válidas e avisa quando encontra uma expressão ilegal com mensagens de erro.

## Menos Unário

Neste ponto, temos um analisador que consegue lidar com qualquer expressão, certo? OK, tente esta entrada:

    -1

OPA! Não funcionou, funcionou? A rotina `expression` espera que tudo comece com um inteiro, então ela indica um erro quando acha o sinal de menos inicial. Você vai perceber que "+3" também não vai funcionar, nem algo como:

    -(3-2)

Há várias formas de resolver o problema. A mais fácil (embora não a melhor) é usar um zero imaginário na frente da expressão: então -3, se torna 0-3. Podemos colocar isto facilmente em nossa versão de `expression`:

~~~c
/* analisa e traduz uma expressão */
void expression()
{
    if (isAddOp(look))
        emit("XOR AX, AX");
    else
        term();
    while (isAddOp(look)) {
        emit("PUSH AX");
        switch(look) {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
            default:
                expected("AddOp");
                break;
        }
    }
}
~~~

(O XOR AX, AX vai fazer o trabalho de colocar o nosso zero imaginário em AX. Eu poderia usar MOV AX, 0 mas XOR AX, AX é um pouco mais econômico.)

Eu DISSE que fazer mudanças era fácil! Desta vez foram mudadas apenas 3 linhas de código. Note a nova referência à função isAddOp(). Como o teste para saber se é um operador de soma aparece duas vezes, eu decidi colocá-lo em uma função separada. A forma de `isAddOp` deve ser assim:

~~~c
/* reconhece operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}
~~~

OK, faça estas mudanças no programa e recompile-o. Você deve incluir `isAddOp` também à sua copia reserva do "berço". Nós vamos usá-la novamente mais tarde. Agora tente a entrada -1 novamente. UAU! A eficiência do código está bem pobre... 6 linhas de código pra ler uma simples constante... mas pelo menos está certo. Lembre-se, não estamos tentando derrubar a Borland, nem a Microsoft aqui.

Neste ponto estamos quase no fim da estrutura do nosso analisador de expressões. Esta versão do programa deve analisar e compilar quase toda expressão que você tentar. Ela ainda está um pouco limitada, pois ainda só podemos tratar de fatores envolvendo dígitos decimais simples. Mas espero que você esteja começando a entender que podemos acomodar mais extensões com apenas mudanças simples no analisador. Você provavelmente não ficaria surpreso de saber que uma variável ou mesmo uma chamada de função é apenas outra forma de fator.

Na próxima parte, eu vou lhe mostrar como é fácil estender o analisador para que ele cuide destas coisas também, e como é simples usar números de mais de um dígito e nomes de variáveis. Então você vai ver que não estamos assim tão distantes de um analisador sintático verdadeiro e útil.

## Um Pouco Sobre Otimização

No começo desta parte do tutorial, eu prometi que daria a vocês algumas dicas sobre como podemos melhorar a qualidade do código gerado. Como foi dito, a produção de código otimizado não é o objetivo inicial desta série de artigos. Mas você precisa pelo menos estar ciente de que não estamos apenas perdendo tempo... que podemos modificar o analisador ainda mais pra fazer com que ele produza um código melhor, sem jogar fora tudo o que fizemos até agora. Como de costume, saiba que ALGUNS tipos de otimização não são tão difíceis de se fazer... apenas requer um pouco mais de código no compilador.

Há duas abordagens básicas que podemos tomar:

### 1) Tentar arrumar o código depois de ele ter sido gerado:

Este é o conceito da otimização "peephole" (olho mágico). A idéia geral é que nós sabemos as combinações de instruções que o compilador vai gerar, e também sabemos quais delas são ruins (como a do -1, acima). Portanto, tudo o que fazemos é vasculhar o código produzido, procurando por estas combinações e as trocamos por versões melhores. É uma espécie de "expansão de macro", só que ao contrário, e é um excelente exercício de procura de padrões (pattern-matching). A única complicação, realmente, é que pode haver UM MONTE destes tipos de combinações que podem ser procuradas. É chamada de otimização "peephole" porque ela procura por apenas pequenos grupos de instruções de cada vez. Ela pode ter um efeito dramático na qualidade do código, com pequenas mudanças na estrutura do compilador em si. Há um preço a pagar, porém, em termos de tamanho, velocidade e complexidade do compilador. Procurar por todas estas combinações requer montes de testes condicionais, onde cada um deles pode ser uma fonte de erros, e é claro, leva tempo.

Na implementação clássica de um otimizador "peephole", é feita uma segunda passagem pelo compilador. O código de saída é escrito em disco, e o otimizador lê e processa o arquivo em disco novamente. Como você pode observar, o otimizador pode até mesmo ser um PROGRAMA separado do compilador propriamente dito. Uma vez que o otimizador "olha" para o código através de uma "pequena janela" de instruções (portanto o nome), uma implementação melhor seria criar um "buffer" onde se pode guardar apenas algumas linhas de código e então vasculhar este buffer a cada vez que for gerada a saída de código.

### 2) Tentar gerar código melhor desde o início:

Esta abordagem nos leva a procurar casos especiais ANTES de emití-los. Como um exemplo trivial, nós deveríamos estar preparados para identificar uma constante de valor zero e emitir um XOR AX, AX ao invés de um MOV AX, 0, ou mesmo não fazer nada, no caso de uma soma com zero, por exemplo. Se nós tivéssemos escolhido reconhecer o menos unário em `factor` ao invés de em `expression`, poderíamos tratar constantes como -1 como constantes comuns, ao invés de gerar o valor do 1 positivo e depois inverter o sinal. Nenhuma destas coisas é tão difícil de fazer... elas apenas adicionam testes adicionais no código, e é por isso que eu não as inclui em nosso programa. Da forma como eu vejo, uma vez que cheguemos ao ponto em que temos um compilador funcional, gerando código executável útil, nós podemos sempre voltar e melhorá-lo para produzir código mais eficiente. É por isso que existem "Versões 2.0" no mundo.

Há outro técnica de otimização que vale a pena mencionar, que parece produzir código eficiente sem muito trabalho. É minha "invenção" no sentido de que eu nunca a vi sendo sugerida em qualquer lugar, apesar de eu não ter a ilusão de que é originalmente minha.

A técnica consiste em evitar um uso exagerado da pilha, fazendo um uso melhor dos registradores da CPU. Lembre-se quando estávamos fazendo apenas adição e subtração, que usávamos os registradores AX e BX, além da pilha? Funcionava, porque com aquelas operações apenas, a pilha nunca precisava de mais de duas entradas.

>**NOTA DE TRADUÇÃO:** A idéia original do autor era aplicar esta técnica num processador Motorola 68000, que possui um esquema de registradores e operações diferentes da usada em computadores 80x86. Se você deseja usar esta técnica, use-a cuidadosamente, pois instruções como DIV e LOOP, entre outras, podem fazer uso de outros registradores, comprometendo os valores armazenados previamente. De qualquer forma, uma descrição traduzida e adaptada da técnica segue abaixo, mas sem prestar atenção a estes cuidados.

Bem, o 8086 e 8088 possuem 4 registradores gerais (AX, BX, CX, DX) e mais alguns outros registradores que podem ser usados com cuidado (SI, DI, BP). CPUs mais avançadas como o 80386 possuem mais registradores ainda e algumas máquinas diferentes do padrão 80x86 possuem dezenas de registradores adicionais. Por que não usá-los como uma espécie de pilha particular? A chave é reconhecer que, a qualquer ponto no processamento, o analisador SABE quantos itens estão na pilha, de forma que ele é capaz de organizá-la de forma apropriada. Nós podemos definir um "ponteiro de pilha" privado que mantém o controle sobre o nível da pilha em que estamos, e associar o registrador correspondente. A rotina `factor`, por exemplo, não iria fazer com que os dados fossem lidos no registrador AX, mas sim, no registrador que é no momento o "topo da pilha".

O que estamos fazendo é trocar o uso da memória RAM da pilha por uma pilha local controlada e feita de registradores. Para a maioria das expressões, o nível de pilha não vai exceder mais que 8, então conseguiríamos código de boa qualidade. No entanto, devemos continuar tratando os casos em que o nível da pilha excede o número de registradores disponíveis. Mas isto também não é problema. Nós simplesmente deixamos a pilha crescer na direção da pilha da CPU. Para níveis acima disto, o código não vai ser pior do que o que estávamos gerando, e para níveis menores, será consideravelmente melhor.

Apenas para constar, eu implementei este conceito, apenas para ter certeza que ia funcionar antes de mencioná-lo. Funciona! Na prática você não vai poder usar todos os níveis, você vai precisar de 1 ou mais registradores livres para algumas operações (veja Nota de Tradução). Para expressões que envolvem chamadas de função, nós precisamos de um registrador para ela também. Ainda assim, há uma redução de tamanho de código para muitas expressões.

Então, você deve notar que gerar código melhor não é tão difícil, mas adiciona complexidade ao nosso tradutor... complexidade que nós não queremos neste ponto. Por esta razão, EU RECOMENDO que continuemos ignorando problemas de eficiência pelo resto desta série, com a certeza que podemos melhorar de fato a qualidade do código sem jogar fora o que fizemos.

O código completo até aqui:

~~~c
{% include_relative src/cap02-expr.c %}
~~~

Na [próxima lição](03_mais_expressoes), vou mostrar como lidar com variáveis e chamadas de função nas expressões. Também vou mostrar como é fácil lidar com tokens de vários caracteres e espaços separadores.

Faça download do código-fonte: [cap02-expr.c](src/cap02-expr.c)