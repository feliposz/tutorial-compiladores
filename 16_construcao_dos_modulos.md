Parte 16: Construção dos Módulos
================================

> **Autor:** Jack W. Crenshaw, Ph.D. (29/05/1995)<br>
> **Tradução e adaptação:** Felipo Soranz (07/06/2002)

Esta série de tutoriais promete ser talvez uma das mais extensas mini-séries da história, rivalizada apenas pelo atraso do Volume IV d'[A Arte da Programação de Computadores](https://pt.wikipedia.org/wiki/The_Art_of_Computer_Programming) de [Donald Knuth](https://pt.wikipedia.org/wiki/Donald_Knuth). Começando em 1988, a série teve uma pausa de 4 anos em 1990 quando as "preocupações deste mundo", me fizeram mudar as prioridades e interesses, e a necessidade de sobreviver fez com que parássemos no [capítulo 14](14_tipos.md). Aqueles de vocês com muita paciência foram finalmente recompensados, no começo do ano passado, com o muito esperado [capítulo 15](15_de_volta_para_o_futuro.md). Nele eu comecei a colocar a série novamente no caminho, para que fosse mais fácil continuar até o objetivo final, que não só é prover-lhes entendimento suficiente das dificuldades no ramo da teoria dos compiladores, mas também as ferramentes, na forma de sub-rotinas e conceitos para que você fosse capaz de continuar por conta própria e se tornar proficientes o suficiente para construir seus próprios analisadores sintáticos e tradutores. Por causa desta longa pausa, eu decidi que era apropriado voltar e rever os conceitos que cobrimos até agora, e refazer parte do software, também. No passado, nós nunca nos preocupamos muito com o desenvolvimento de ferramentas com qualidade de produção... afinal de contas, eu estava tentando lhes ensinar (e aprender) conceitos, e não prática de produção. Para fazer isto, eu tentei lhes dar, não compiladores ou analisadores completos, mas apenas os fragmentos de código que ilustravam o ponto particular que estava sendo considerado no momento.

Eu ainda acho que esta é uma boa forma de aprender qualquer assunto; ninguém gosta de ter que fazer mudanças em programas de 100.000 linhas apenas para tentar uma ideia nova. Mas a ideia de tratar apenas de fragmentos de código, ao invés de programas completos, também tem seus pontos negativos pois parece que estamos reescrevendo o mesmo código toda vez. Apesar da repetição ser comprovadamente uma boa forma de aprender novas ideias, também é provado que o excesso de uma coisa boa é ruim. No momento em que eu completei o capítulo 14 eu achei que havia atingido o limite das minhas habilidades para tratar de múltiplos arquivos e múltiplas versões das mesmas funções do software. Quem sabe foi esta uma das razões de eu ter perdido o fôlego neste ponto.

Felizmente, podemos manter módulos separados para manter nosso programa principal pequeno e simples enquanto testamos coisas novas. Mas, uma vez que o código tenha sido escrito ele sempre estará lá, e adicioná-los aos nossos programas é indolor e transparente.

>*NOTA DE TRADUÇÃO:* Novamente aqui, o autor faz alguns comentários comparando Pascal e linguagem C, mas decide por manter usando Pascal em sua implementação, já que foi a linguagem usada desde o começo da série. Como esta versão em português já foi escrita utilizando C, achei desnecessário incluir este trecho. Para quem tiver interesse, o texto original está disponível em: <https://compilers.iecc.com/crenshaw/tutor16.txt>

A questão, realmente, é que: usando o mecanismo de módulos separados, podemos ter a vantagem e a conveniência de escrever programas pequenos para teste, aparentemente independentes, sem ter que constantemente reescrever as funções de suporte necessárias.

Usando este princípio, eu comecei no capítulo 15 a minimizar nossa tendência a reinventar a roda organizando o código em unidades separadas, cada uma contendo partes diferentes do compilador. Acabamos com as seguintes unidades:

Módulo  | Descrição
--------|-------------------------------
input   | Tratamento da entrada
output  | Geração da saída
errors  | Tratamento de erros
scanner | Rotinas do analisador léxico
parser  | Implementação da sintaxe da linguagem propriamente dita
codegen | Rotinas de geração de código assembly

Cada uma destas unidades possui uma função diferente, e mantém encapsuladas áreas específicas de funcionalidade. Por exemplo, as unidades de entrada e saída, como seus nomes já dizem, provêem o tratamento de E/S e o importante caractere "lookahead" sobre o qual se baseia nosso analisador preditivo.

As duas unidades com que mais vamos trabalhar, e as que representam a maior parte da personalidade do compilador, são a do analisador sintático e a de geração de código. Teoricamente, o analisador sintático deveria encapsular todos os aspectos do compilador que dependem da sintaxe da linguagem compilada (contudo, como vimos da ultima vez, parte desta sintaxe acaba indo no próprio analisador léxico). De forma similar, a unidade de geração de código contém todo o código dependente da máquina-alvo. Neste capítulo, vamos continuar com o desenvolvimento das funções nestas duas unidades tão importantes.

De volta ao clássico?
---------------------

Contudo, antes de continuarmos, eu acho que eu deveria esclarecer a relação entre as unidades e a funcionalidade delas. É claro que, aqueles de vocês que estão familiarizados com a teoria de compiladores ensinada nas universidades, reconhecem os nomes analisador léxico (*scanner*), analisador sintático (*parser*), e gerador de código (*code generator*), os quais são componentes da implementação clássica dos compiladores. Vocês devem estar pensando que eu abandonei o meu compromisso de manter a filosofia KISS, e preferi usar uma abordagem mais convencional. Porém, uma observação mais atenta, deve convencê-los de que, embora os nomes sejam similares, as funcionalidades são bem diferentes.

Juntas, as implementações clássicas dos analisadores léxico e sintático caracterizam o *front-end* ou "vanguarda" e o gerador de código, o *back-end* ou "retaguarda" de um compilador. As rotinas de vanguarda processam a parte dependente da linguagem, relacionadas à sintaxe, enquanto o gerador de código, ou retaguarda, trata da parte dependente da máquina-alvo do problema. Em compiladores clássicos, as duas partes se comunicam através de um arquivo de instruções escritas em uma linguagem intermediária (IL).

Tipicamente, um analisador léxico clássico é um procedimento único, operando como um co-procedimento com o analisador sintático. Ele "tokeniza" o arquivo fonte, lendo-o caractere por caractere, reconhecendo os elementos da linguagem, e traduzindo-os em tokens, e depois passando-os para o analisador sintático. Você pode pensar no analisador sintático como uma máquina abstrata, executando os códigos de operação, que são os tokens. De forma similar, o analisador gera códigos de operação de uma segunda máquina abstrata, que é alimentada pela linguagem intermediária. Tipicamente, o arquivo intermediário é gerado pelo analisador sintático, e lido pelo gerador de código.

Nossa organização é bem diferente. Nós não temos analisador léxico, no sentido clássico; nossa unidade "scanner", apesar de ter um nome similar, não é um procedimento único ou co-procedimento, mas um conjunto de sub-rotinas separadas que são chamadas pelo analisador sintático conforme necessárias.

De forma similar, o gerador de código clássico, a retaguarda, é um tradutor por si só, lendo o arquivo "fonte" na linguagem intermediária, e emitindo um arquivo-objeto. Nosso gerador de código não funciona desta forma. Em nosso compilador, não há linguagem intermediária; cada construção na sintaxe da linguagem fonte é convertida em linguagem assembly conforme é reconhecida pelo analisador sintático. Assim como o módulo "scanner", o módulo "codegen" consiste de procedimentos individuais cada um chamado pelo analisador sintático conforme necessário.

Esta filosofia "codifique conforme necessário" pode não produzir o código mais eficiente do mundo -- por exemplo, ainda não construíamos (ainda!) um lugar conveniente para que o otimizador faça sua mágica -- mas certamente simplifica o compilador, não simplifica?

E a observação me faz refletir, mais uma vez, em como fomos capazes de reduzir as funções do compilador em termos tão simples. Eu já fui bastante eloquente quanto a este assunto em capítulos anteriores, portanto não vou perder muito tempo com este ponto agora. No entanto, como um bom tempo passou desde as últimas seções, eu espero que você me garanta ao menos uma breve reflexão de como chegamos até aqui. Nós conseguimos isto aplicando diversos princípios que diversos criadores de compiladores comerciais raramente tiveram o prazer de usar. Estes são:

- A filosofia KISS: nunca fazer as coisas da forma mais difícil sem uma razão
- Codificação preguiçosa: "Nunca deixar para amanhã o que pode ser adiado para sempre" (com os créditos para P.J.Plauger)
- Ceticismo: Recusar teimosamente fazer algo apenas por ser a forma como sempre foi feito.
- Aceitação de código ineficiente
- Rejeição de limitações arbitrárias

Quando eu fiz uma revisão da história da construção de compiladores, eu aprendi que virtualmente todo compilador de produção na história sofreu de condições pré-impostas que influenciaram muito no projeto. O compilador original FORTRAN de John Backus e outros, precisavam competir com a linguagem assembly, e portanto deveriam produzir um código extremamente eficiente. Os compiladores IBM para os minicomputadores da década de 70 precisavam rodar com quantidades muito reduzidas de memória RAM -- reduzidas como 4k. O primeiro compilador Ada deveria compilar a si mesmo. Brinch Hansen declarou que seu compilador Pascal desenvolvido para o IBM PC deveria executar em uma máquina de 64k. Compiladores desenvolvidos em cursos de Ciência da Computação deveriam compilar a maior variedade possível de linguagens, e portanto precisavam de analisadores LALR.

Em cada um destes casos, os requisitos pré-concebidos literalmente dominaram o projeto destes compiladores.

Um bom exemplo é o compilador de Brinch Hansen, descrito em seu excelente livro, "Brinch Hansen on Pascal Compilers". Apesar de seu compilador ser uma das implementações mais limpas e não-obscuras que eu já vi, aquela decisão, de compilar arquivos grandes em uma RAM limitada, dirigiu totalmente o projeto, e ele terminou com não somente um, mas vários arquivos intermediários, junto com as interfaces para ler e escrevê-los.

Com o tempo, as estruturas resultantes destas decisões encontraram seus caminhos na ciência da computação como artigos de fé. Na opinião deste homem, já é hora de re-examinar o processo de um ponto de vista crítico. As condições, ambientes, e requerimentos que levaram às arquiteturas clássicas não são os mesmos de hoje em dia. Não há razão para crer que a solução deveria ser a mesma.

Neste tutorial, nós seguimos a liderança dos pioneiros no mundo de compiladores pequenos para computadores pessoais, como [Leor Zolman](https://en.wikipedia.org/wiki/BDS_C), [Ron Cain e James Hendrix](https://en.wikipedia.org/wiki/Small-C), que não sabiam teoria de compiladores o suficiente para saber que eles "não conseguiriam fazer daquela forma". Nós temos decididamente evitado aceitar limitações arbitrárias, e feito o que quer que fosse mais fácil. Como resultado, acabamos construindo uma arquitetura que, embora bem diferente da clássica, faz bem seu trabalho de uma forma muito simples e descomplicada.

Eu vou terminar esta nota filosófica com uma observação da noção de uma linguagem intermediária. Embora eu tenha dito anteriormente que não temos uma em nosso compilador, isto não é exatamente verdadeiro; nós TEMOS uma, ou pelo menos estamos evoluindo uma, no sentido de que estamos definindo funções de geração de código para que o analisador sintático as use. Em essência, cada chamada a uma rotina de geração de código pode ser encarada como uma instrução em uma linguagem intermediária. Se em algum momento nós acharmos necessário formalizar uma linguagem intermediária, esta é a forma de fazê-lo: emitir códigos no analisador sintático, cada um representando uma chamada a um dos procedimentos do gerador de código, e então processar cada código chamando estes procedimentos em um passo separado, implementado na retaguarda. Francamente, eu não acho que nós vamos encontrar uma necessidade real para esta abordagem, mas aí está a conexão, caso você queira seguí-la, entre as abordagens clássica e a que estamos usando.

Expandindo o Analisador Sintático
---------------------------------

Apesar de ter prometido que, em algum lugar do capítulo 14, que nós nunca mais iríamos ter que refazer cada função novamente, eu acabei começando a fazer isto outra vez no capítulo 15. A razão: a longa pausa entre os dois capítulos fez com que a revisão fosse justificada... para você e para mim. Mais importante do que isso, a decisão de coletar os procedimento em módulos (unidades), nos forçou a revisar tudo novamente, querendo ou não. E, finalmente e francamente, eu tive algumas ideias novas nos últimos quatro anos que garantiram uma nova olhada em alguns dos velhos amigos. Quando eu comecei esta série, eu francamente fiquei espantado, e ao mesmo tempo satisfeito, em saber quão simples as rotinas de análise podem ser. Mas desta vez, eu me surpreendi novamente, sendo capaz de fazê-las ainda mais simples.

Apesar de toda esta reescrita do módulo de análise, eu só consegui incluir uma parte no último capítulo. Por causa disto, nosso herói, o analisador sintático, quando visto pela última vez, era apenas uma sombra de sua versão anterior, consistindo apenas de código suficiente para analisar e processar fatores consistindo de uma variável ou constante. O principal objetivo deste capítulo será ajudar o analisador a alcançar sua glória inicial. No processo, eu espero que você me acompanhe enquanto cobrimos um território que já foi tratado há muito tempo.

Em primeiro lugar, vamos tratar de um problema que já vimos anteriormente: nossa versão atual do procedimento "factor", conforme a deixamos no capítulo 15, não pode tratar de argumentos negativos. Para arrumar isto, vamos introduzir o procedimento `SignedFactor()`:

~~~c
/* Analisa e traduz um fator com um sinal opcional */
void SignedFactor()
{
    char sign = Look;
    if (IsAddOp(Look))
        NextChar();
    Factor();
    if (sign == '-')
        AsmNegate();
}
~~~

Repare que este procedimento chama uma nova rotina de geração de código, `AsmNegate()`:

~~~c
/* Inverte sinal de registrador primário */
void AsmNegate()
{
    EmitLn("NEG AX");
}
~~~

(Aqui, e em todo lugar nesta série, eu só vou lhe mostrar as novas rotinas. Eu espero que você as coloque no módulo apropriado, e eu tenho certeza de que você normalmente não terá problema em identificar. Não esqueça de adicionar o protótipo da função ao cabeçalho de cada módulo.)

No programa principal, simplesmente altere o procedimento chamado para `SignedFactor()` e faça um teste.

Sim, eu sei que o código não é muito eficiente. Se colocarmos um número, como -3, o código gerado será:

~~~asm
    MOV AX, 3
    NEG AX
~~~

o que é realmente estúpido. Podemos fazer melhor, é claro, simplesmente adicionando um sinal de menos à string passada para `AsmLoadConst()`, mas isto adiciona algumas linhas de código a `SignedFactor()`, e eu estou aplicando a filosofia KISS muito agressivamente aqui. Além disso, pra falar a verdade, eu acho que eu estou de forma subconsciente gostando de gerar código "realmente estúpido", para que eu tenha o prazer de vê-lo ficar dramaticamente melhor quando chegarmos aos métodos de otimização.

Creio que a maioria de vocês nunca ouviu falar de John Spray, então permitam-me apresentá-lo. John é da Nova Zelândia, e costumava ensinar ciência da computação em uma das universidades de lá. John escreveu um compilador para o processador Motorola 6809, baseado em uma fantástica linguagem que ele mesmo criou, parecida com Pascal, chamada "Whimsical". Mais tarde ele portou o compilador para o 68000, e por um tempo este foi o único compilador que eu possuía em meu sistema doméstico.

Por curiosidade, um dos meus testes padrão para qualquer novo compilador é ver como o programa trata de um programa vazio como:

~~~pascal
    program main;
    begin
    end.
~~~

Meu teste consiste em medir o tempo necessário para compilar e "linkar", e o tamanho do arquivo-objeto gerado. O **PERDEDOR** indisputável no meu teste foi o compilador DEC C para o VAX, que levou 60 segundos para compilar, em um VAX 11/780, e gerou um código-objeto de 50K. O compilador de John é e para sempre será o único e indisputável rei no departamento de tamanho de código. Dado um programa vazio, Whimsical gerou precisamente dois bytes de código (é um processador 68000, lembre-se), implementando a única instrução,

    RET

Fazendo o compilador gerar código para um arquivo de inclusão (uma espécie de módulo ou biblioteca) ao invés de um programa executável independente, John conseguiu cortar o tamanho, de dois bytes para zero! É meio difícil ganhar de um arquivo-objeto vazio, você não acha?

Não é necessário dizer que, eu considero John uma espécie de especialista em otimização de código, e gosto do que ele tem para dizer: "A melhor forma de otimizar é não ter que otimizar nada. Ao invés disso produzir código de boa qualidade já da primeira vez." Palavras com as quais devemos conviver. Quando começarmos com otimização, vamos seguir o conselho de John, e nosso primeiro passo não será adicionar um otimizador "peephole" ou outro mecanismo *a posteriori*, mas sim melhorar a qualidade do código emitido antes da otimização. Portanto, tome nota a respeito de `SignedFactor()` como sendo um bom candidato para nossa primeira tentativa, e por enquanto vamos deixá-lo como está.

Termos e Expressões
-------------------

Eu estou certo de que você sabe o que vem em seguida: nós temos que, outra vez, criar o resto dos procedimentos que implementam a análise descendente-recursiva de uma expressão. Nós todos já conhecemos a hierarquia de procedimentos para expressões aritméticas:

~~~
    expression
        |
        +---> term
               |
               +---> factor
~~~

Mas no momento vamos continuar fazendo as coisas uma de cada vez, e considerar expressões que só tem termos aditivas. O código para implementar expressões, incluindo um possível termo sinalizado, é mostrado a seguir:

~~~c
/* Analisa e traduz uma expressão */
void Expression()
{
    SignedFactor();
    while (IsAddOp(Look)) {
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

Este procedimento chama outros procedimento para processar as operações:

~~~c
/* Analisa e traduz uma operação de soma */
void Add()
{
    Match('+');
    AsmPush();
    Factor();
    AsmPopAdd();
}

/* Analisa e traduz uma operação de subtração */
void Subtract()
{
    Match('-');
    AsmPush();
    Factor();
    AsmPopSub();
}
~~~

Os três procedimentos `AsmPush()`, `AsmPopAdd()` e `AsmPopSub()` são novas rotinas de geração de código. Como o próprio nome implica, o procedimento `AsmPush()` gera código para colocar o registrador primário (AX, em nossa implementação) na pilha. `AsmPopAdd()` e `AsmPopSub()` removem o valor do topo da pilha, e adicionam, ou subtraem dele o registrador primário. O código é mostrado a seguir:

~~~c
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

/* Subtrai do topo da pilha o registrador primário */
void AsmPopSub()
{
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    AsmNegate();
}
~~~

Adicione estas rotinas aos módulos "parser" e "codegen", e mude o programa principal para chamar `Expression()`. Voilà!

O próximo passo, é claro, é adicionar capacidade para tratar de termos multiplicativos. Para isto, vamos adicionar um procedimento `Term()`, e os procedimentos de geração de código `AsmPopMul()` e `AsmPopDiv()`.

~~~c
/* Multiplica topo da pilha e registrador primário */
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

Eu admito que a rotina de divisão está um pouco "cheia", mas não há muita saída. Infelizmente os registradores acabam com os valores na ordem inversa, por isso é preciso invertê-los. Além disso precisamos fazer a extensão de sinal de AX (pois se o divisor é de 16-bits, BX, o dividendo deve ter 32-bits, DX:AX). Repare que estou usando a multiplicação e divisão com sinal. Estamos assumindo que nossas variáveis serão inteiros de 16-bits. Esta decisão vai voltar a nos assombrar mais tarde, quando começarmos a tratar de múltiplos tipos de dados, conversão de tipos, etc.

Nosso procedimentos `Term()` é virtualmente um clone de `Expression()` e se parece com isto:

~~~c
/* Analisa e traduz um termo */
void Term()
{
    Factor();
    while (IsMulOp(Look)) {
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
~~~

Nosso próximo passo é alterar alguns nomes. `SignedFactor()` agora torna-se `SignedTerm()`, e as atuais chamadas a `Factor()`, serão alteradas para `Term()`:

~~~c
/* Analisa e traduz um termo com um sinal opcional */
void SignedTerm()
{
    char sign = Look;
    if (IsAddOp(Look))
        NextChar();
    Term();
    if (sign == '-')
        AsmNegate();
}

/* Analisa e traduz uma operação de soma */
void Add()
{
    Match('+');
    AsmPush();
    Term();
    AsmPopAdd();
}

/* Analisa e traduz uma operação de subtração */
void Subtract()
{
    Match('-');
    AsmPush();
    Term();
    AsmPopSub();
}

/* Analisa e traduz uma expressão */
void Expression()
{
    SignedTerm();
    while (IsAddOp(Look)) {
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

Se não me falha a memória, antes nós tínhamos tanto `SignedFactor()` quanto `SignedTerm()`. Eu tinha razões para manter as duas daquela vez... tinha a ver com o tratamento da álgebra booleana e, em particular, a função booleana "NOT". Mas certamente, para operações aritméticas, a duplicação não é necessária. Em uma expressão como:

    -x*y

é aparente que o sinal se aplica ao termo inteiro, x*y, e não apenas ao fator "x", e é desta forma que a expressão é codificada.

Teste este novo código. Agora você deve ser capaz de tratar das quatro operações aritméticas.

Nossa última tarefa, com relação às expressões, é modificar o procedimento `Factor()` para permitir expressões entre parênteses. Usando uma chamada recursiva a `Expression()`, é possível reduzir o código necessário a quase nada. Algumas linhas de código adicionadas a `Factor()` resolvem isso:

~~~c
/* Analisa e traduz um fator matemático */
void Factor()
{
    char name[MAXNAME+1], num[MAXNUM+1];

    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (isdigit(Look)) {
        GetNum(num);
        AsmLoadConst(num);
    } else if (isalpha(Look)) {
        GetName(name);
        AsmLoadVar(name);
    } else
        Error("Unrecognized character: '%c'", Look);
}
~~~

Neste ponto, seu "compilador" deve estar apto a produzir qualquer expressão válida que você tentar. Melhor ainda, ele deve rejeitar as inválidas!

Atribuição
----------

Já que chegamos até aqui, podemos criar também o código para comandos de atribuição. Este código só precisa lembrar do nome da variável onde deve ser armazenado o resultado de uma expressão, chamar "expression", e então armazenar o valor. O procedimento é o seguinte:

~~~c
/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name[MAXNAME+1];

    GetName(name);
    Match('=');
    Expression();
    AsmStoreVar(name);
}
~~~

A atribuição precisa de mais uma rotina de geração de código:

~~~c
/* Armazena registrador primário em variável */
void AsmStoreVar(char *name)
{
    EmitLn("MOV [%s], AX", name);
}
~~~

Agora, altere a chamada no programa principal para `Assignment()`, e você deverá ver um comando completo de atribuição sendo processado corretamente. Legal, não? E indolor, também.

No passado, eu tentei mostrar as relações BNF para definir a sintaxe que estávamos desenvolvendo. Eu ainda não fiz isto, e já está na hora de fazê-lo. Aí está:

~~~ebnf
<factor>      ::= <variable> | <constant> | '(' <expression> ')'
<signed-term> ::= [<addop>] <term>
<term>        ::= <factor> (<mulop> <factor>)*
<expression>  ::= <signed-term> (<addop> <term>)*
<assignment>  ::= <variable> '=' <expression>
~~~

Expressões Booleanas
--------------------

O próximo passo, como já vimos diversas vezes antes, é adicionar álgebra booleana. No passado, este passo praticamente dobrou a quantidade de código que tivemos que escrever. Conforme eu repassava isto várias vezes mentalmente, eu me encontrei divergindo mais e mais do que fizemos nos capítulos anteriores. Para refrescar a memória, eu notei que Pascal trata os operadores booleanos de forma quase idêntica ao tratamento dos aritméticos. Um "AND" booleano tem o mesmo nível de precedência da multiplicação e o "OR" o mesmo da adição. C, por outro lado, os coloca em níveis diferentes de precedência, e como já foi dito, são 17 níveis! Em nossos trabalhos anteriores, eu preferi algo intermediário, com sete níveis. Como resultado, acabamos com as chamadas expressões booleanas, paralelas às expressões aritméticas na maioria dos detalhes, mas em um nível de precedência diferente. Tudo isto, veio à tona, por que eu não gosto de colocar parênteses em expressões booleanas em comandos como:

~~~pascal
    IF (c >= 'A') AND (c <= 'Z') THEN ...
~~~

Em retrospecto, isto não parece uma razão muito boa para adicionar diversas camadas de complexidade ao compilador. Além disso, eu não estava sequer certo se poderia ou não evitar os parênteses.

Vamos começar tudo outra vez, usando uma abordagem mais próxima do Pascal, tratando os operadores booleanos com o mesmo nível de precedência dos aritméticos. Vamos ver até onde chegamos. Se parecer algo muito estranho podemos voltar à abordagem anterior.

Vamos começar modificando `IsAddOp()` para incluir os dois operadores extra: `|` para OU e `~` para OU-exclusivo:

~~~c
/* Reconhece um operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-' || c == '|' || c == '~');
}
~~~

Em seguida, temos que incluir a análise destas operações em "expression":

~~~c
/* Analisa e traduz uma expressão */
void Expression()
{
    SignedTerm();
    while (IsAddOp(Look)) {
        switch (Look) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
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

Em seguida, as rotinas `BoolOr()` e `BoolXor()`:

~~~c
/* Analisa e traduz uma operação OU booleana */
void BoolOr()
{
    Match('|');
    AsmPush();
    Term();
    AsmPopOr();
}

/* Analisa e traduz uma operação OU-exclusivo booleana */
void BoolXor()
{
    Match('~');
    AsmPush();
    Term();
    AsmPopXor();
}
~~~

E, finalmente, as novas rotinas de geração de código:

~~~c
/* Aplica OU com topo da pilha a registrador primário */
void AsmPopOr()
{
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* Aplica OU-exclusivo com topo da pilha a registrador primário */
void AsmPopXor()
{
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}
~~~

Agora, vamos testar o tradutor (talvez você queira trocar a chamada no programa principal para `Expression()`, apenas para evitar ter que digitar "x=" para uma atribuição o tempo todo).

Até aqui tudo bem. O analisador trata corretamente de expressões da forma:

    x|y~z

Infelizmente, ele também não faz nada para nos proteger de misturar algebra aritmética e booleana. Ele simplesmente gera o código para:

    (a+b)*(c~d)

Nós falamos um pouco sobre isto, no passado. Em geral, as regras para operações que são válidas ou não não podem ser reforçadas pelo analisador sintático em si, por que isto não faz parte da sintaxe da linguagem, mas da semântica. Um compilador que não permite expressões misturadas deste tipo deve reconhecer que "c" e "d" são variáveis booleanas, ao invés de numéricas, e deve avisar sobre a multiplicação delas no próximo passo. Mas este "policiamento" não pode ser feito pelo analisador; deve ser tratado em algum outro lugar entre o analisador e o gerador de código. Nós não estamos em condição de forçar tais regras ainda, pois ainda nem sequer temos uma forma de declarar tipos, ou uma tabela de símbolos para armazenar os tipos declarados. Então, pelo que temos feito até o momento, o analisador está fazendo precisamente o que deveria.

De qualquer forma, estamos certos de que NÃO queremos operações entre tipos misturados? Tomamos a decisão algum tempo atrás (ou, pelo menos, eu tomei), de adotar o valor 0000 para o "falso" booleano, e -1, ou FFFFh, como "verdadeiro". O bom desta decisão é que as operações bit-a-bit funcionam exatamente da mesma forma que as lógicas. Em outras palavras, quando fazemos uma operação em uma variável lógica, fazemos nela inteira. Isto significa que não é preciso diferenciar operações lógicas e bit-a-bit, como é feito em C com os operadores `&` e `&&`, e `|` e `||`. Reduzir o número de operadores pela metade certamente não é tão ruim assim.

Do ponto de vista do armazenamento de dados, é claro que o computador e o compilador não dão a menor importância se o número FFFFh representa VERDADEIRO, ou o valor numérico -1. Deveríamos nós? Eu acho que não. Eu posso pensar em muitos exemplos (embora alguns não sejam considerados bons exemplos de código claro) onde a habilidade de misturar tipos se torna útil. Por exemplo, a função delta de Dirac, pode ser codificada em uma única linha:

    -(x=0)

Ou a função de valor absoluto (DEFINITAMENTE estranha!):

    x*(1+2*(x<0))

Por favor, entenda. Eu não estou tentando dizer que este tipo de programação é um estilo de vida. Eu certamente escreveria estas funções em formas mais legíveis, usando IFs, apenas para evitar confundir programadores que venham a manter os programas. Porém, uma questão moral surge: nós temos o direito de IMPOR nossas ideias de boa prática de programação ao programador, escrevendo a linguagem de forma que ele não tenha saída? Foi isto que Niklaus Wirth fez, em muitos partes de Pascal, e Pascal foi muito criticado por isto -- por não ser tão "permissível" quanto C.

Um paralelo interessante no exemplo do projeto do processador Motorola 68000. Por exemplo, é possível ler uma variável a partir de seu endereço:

    MOVE X, D0 (onde X é o nome da variável e D0 é o registrador primário)

(repare que a ordem dos operandos é Origem -> Destino e não Destino <- Origem)

Porém, não é possível fazer o inverso, isto é, é preciso carregar em um registrador o endereço de X.

    LEA X(PC), A0
    MOVE D0, (A0)

O mesmo vale para endereçamento relativo ao contador de programa:

    MOVE X(PC), D0 (válido)
    MOVE D0, X(PC) (inválido)

Quando você começa a se perguntar como um comportamento não-ortogonal surgiu, você descobre que alguém na Motorola tinha algumas teorias a respeito de como o software deveria ser escrito. Especificamente, neste caso, eles decidiram que código auto-modificável, que pode ser implementado usando escritas relativas ao contador de programa (PC) é uma *Coisa Má*. Portanto, eles projetaram o processador para proibir isto. Infelizmente, no processo eles também proibiram TODAS as instruções de escrita no formato mostrado acima, não importa quão benignas. Repare que isto não foi algo feito por padrão. Trabalho extra foi adicionado ao projeto, e portas extras foram adicionadas, para destruir a ortogonalidade natural do conjunto de instruções.

Uma das lições que eu aprendi com a vida: se você tem duas escolhas, e não consegue decidir qual tomar, algumas vezes a melhor coisa a fazer é nada. Por que adicionar portas lógicas extras a um processador para forçar algumas ideias estranhas a respeito de boa prática de programação? Deixe as instruções lá, e deixe que os programadores discutam o que é boa prática de programação. De forma similar, por que deveríamos adicionar código extra ao nosso compilador, para testar e prevenir condições que o usuário preferiria fazer de qualquer jeito? Eu prefiro manter o compilador simples, e deixar que os especialistas do software discutam se a prática deve ser usada ou não.

Tudo isto serve como uma explicação para a minha decisão a respeito de prevenir ou não aritmética de tipos misturados: eu não vou! Para uma linguagem cujo objetivo é programação de sistemas, quanto menos regras, melhor. Se você não concordar, e preferir testar tais condições, podemos fazê-lo uma vez que tenhamos uma tabela de símbolos.

"AND" lógico
--------------

Com esta discussão filosófica fora do nosso caminho, podemos continuar com o operador AND, que entra no procedimento `Term()`. A esta altura, você provavelmente já consegue fazer isto sem mim, mas aqui está o código de qualquer forma:

No analisador léxico:

~~~c
/* Reconhece um operador multiplicativo */
int IsMulOp(char c)
{
    return (c == '*' || c == '/' || c == '&');
}
~~~

No analisador sintático:

~~~c
/* Analisa e traduz um termo */
void Term()
{
    Factor();
    while (IsMulOp(Look)) {
        switch (Look) {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
            case '&':
                BoolAnd();
                break;
        }
    }
}

/* Analisa e traduz uma operação AND */
void BoolAnd()
{
    Match('&');
    AsmPush();
    Factor();
    AsmPopAnd();
}
~~~

E no gerador de código:

~~~c
/* Aplica AND com topo da pilha e registrador primário */
void AsmPopAnd()
{
    EmitLn("POP BX");
    EmitLn("AND AX, BX");
}
~~~

Seu analisador deve a esta altura estar apto a processar quase todo tipo de expressões lógicas, e expressões misturadas também.

Por que não "todo tipo de expressões lógicas"? Por que, até aqui, não tratamos do operador lógico NOT, e é aí que as coisas começam a fica complicadas. O operador lógico NOT parece, numa primeira olhada, ser idêntico em comportamento ao menos unário. Portanto, minha primeira ideia foi permitir que o operador OU-exclusivo `~`, fizesse o papel do NOT, quando operador unário. Isto não funcionou. Na minha primeira tentativa, o procedimento "signedTerm" simplesmente ignorou o `~`, pois o caractere passou o teste de "isAddOp", mas "signedTerm" ignora qualquer coisa exceto "-". Seria fácil adicionar outro teste a "signedTerm", mas isto ainda não resolveria o problema, pois, note que "expression" só aceita um termo com sinal para o PRIMEIRO argumento.

Matematicamente, uma expressão como:

    -a * -b

não faz sentido, e o analisador deve avisar sobre o erro. Mas a mesma expressão, usando um NOT faz total sentido:

    NOT a AND NOT b

No caso destes operadores unários, escolher fazê-los agir da mesma forma, parece ser uma medida artificial, sacrificando o comportamento razoável no altar da facilidade de implementação. Embora eu seja devoto de manter a implementação tão simples quanto possível, eu não acho que devemos fazer isto ao custo da perda do senso racional. Remendos como este nos deslocam do foco principal: o operador lógico NOT não é do mesmo tipo do menos unário. Considere o OU-exclusivo, que é escrito mais naturalmente como:

    a~b  que equivale a  (a AND NOT b) OR (NOT a AND b)

Se permitirmos que NOT se aplique ao termo inteiro, o último termo entre parênteses seria interpretado como:

    NOT(a AND b)

que não é a mesma coisa. Portanto está claro que o NOT lógico deve ser considerado conectado ao FATOR, não ao termo.

A ideia de sobrecarregar (overload) o operador "~" também não faz sentido do ponto de vista matemático. A implicação do menos unário é equivalente a uma subtração de zero:

    -x  equivale a  0-x

De fato, em uma das minhas versões mais simplistas de "expression", eu reagi a um operador aditivo simplesmente pré-carregando um zero, e então processado o operador como se ele fosse um operador binário. Mas um NOT não é equivalente a um OU-exclusivo com zero... isto só daria como resultado o próprio número. Ao invés disto, seria um OU-exclusivo com FFFFh, ou -1.

Em resumo, o paralelo entre o NOT e o menos unário é falso. NOT modifica o fator, não o termo, e não está relacionando ao sinal de menos e nem ao OU-exclusivo. Portanto, ele merece um símbolo próprio. Que símbolo seria melhor que o mais óbvio de todos: o caractere "!" da linguagem C? Usando as regras da forma como achamos que o NOT deveria ser, poderíamos codificar o OU-exclusivo (se é que um dia vamos precisar fazer isto), na forma natural:

    a & !b | !a & b

Repare que não há necessidade de parênteses - os níveis de precedência que escolhemos automaticamente tomam conta das coisas.

Se você tem acompanhado os níveis de precedência, esta definição coloca o "!" no topo da pilha. Os níveis se tornam:

1. `!`
2. `-` (unário)
3. `*` `/` `&`
4. `+` `-` `|` `~`

Olhando para esta lista, certamente não é difícil ver porque teríamos problemas usando "~" como o símbolo para NOT!

Então, como implementar as regras? Da mesma forma que fizemos com `SignedTerm()`, mas no nível do fator. Vamos definir um procedimento `NotFactor()`:

~~~c
/* Analisa e traduz um fator com NOT opcional */
void NotFactor()
{
    if (Look == '!') {
        Match('!');
        Factor();
        AsmNot();
    } else
        Factor();
}
~~~

Coloque uma chamada a esta rotina em todos os lugares onde `Factor()` era chamada. Note a nova rotina de geração de código:

~~~c
/* Aplica NOT a registrador primário */
void AsmNot()
{
    EmitLn("NOT AX");
}
~~~

Teste isto agora, com alguns casos simples. Teste também o nosso exemplo do OU-exclusivo:

    a&!b|!a&b

Você deve obter o código (sem os comentários, é claro):

~~~asm
    MOV AX, [A] ; carrega A
    PUSH AX     ; coloca na pilha
    MOV AX, [B] ; carrega B
    NOT AX      ; !b
    POP BX      ; pega A da pilha
    AND AX, BX  ; !b & a
    PUSH AX     ; coloca resultado na pilha
    MOV AX, [A] ; carrega A
    NOT AX      ; !a
    PUSH AX     ; coloca na pilha
    MOV AX, [B] ; carrega B
    POP BX      ; pega !a da pilha
    AND AX, BX  ; b & !a
    POP BX      ; pega resultado anterior (!b & a)
    OR AX, BX   ; resultado OR resultado anterior
~~~

Isto é precisamente o que gostaríamos de obter. Portanto, pelo menos para operadores lógicos e aritméticos, nossa nova sintaxe e níveis de precedência estão bons. Mesmo uma expressão peculiar, mas válida, como:

    ~x

faz sentido. "signedTerm" ignora o "~" inicial, como deveria, já que a expressão é equivalente a:

    0~x

que é a mesma coisa que "x".

Se olharmos na BNF que criamos, vamos descobrir que nossa algebra booleana adiciona apenas uma linha extra:

~~~ebnf
<not-factor>      ::= [!] <factor>
<factor>          ::= <variable> | <constant> | '(' <expression> ')'
<signed-term>     ::= [<addop>] <term>
<term>            ::= <not-factor> (<mulop> <not-factor>)*
<expression>      ::= <signed-term> (<addop> <term>)*
<assignment>      ::= <variable> '=' <expression>
~~~

Esta é uma grande melhoria em relação às tentativas passadas. Nossa sorte vai continuar quando tratarmos de operadores relacionais? Nós vamos descobrir em breve, mas isto terá que esperar pelo próximo capítulo. Chegamos num bom ponto de parada por enquanto, e estou ansioso para que este capítulo chegue às suas mãos logo. Já se passou um ano desde o [capítulo 15](15_de_volta_para_o_futuro.md). Eu devo admitir que parte destes capítulos atuais estiveram prontos por muito tempo, com a exceção dos operadores relacionais. Mas a informação não tem valor algum, enquanto permanecer no meu disco rígido, e segurá-la até que os operadores relacionais estivessem prontos iria atrasar demais as coisas. Está na hora de liberá-la para que você possa tirar algum valor dela. Além disso, há uma série de questões filosóficas associadas aos operadores relacionais, também, e eu gostaria de deixá-las para um capítulo posterior, onde eu possa fazer justiça a elas.

Divirta-se com o novo analisador lógico e aritmético, e quem sabe em breve estaremos tratando dos operadores relacionais.

## Conclusão

Segue abaixo a listagem dos módulos modificados neste capítulo (somente a [versão MULTI](https://github.com/feliposz/tutorial-compiladores/tree/master/src/cap16/MULTI), [clique aqui para a versão SINGLE](https://github.com/feliposz/tutorial-compiladores/tree/master/src/cap16/SINGLE)).

Arquivo [main.c](src/cap16/MULTI/main.c):

~~~c
{% include_relative src/cap16/MULTI/main.c %}
~~~

Arquivo [scanner.c](src/cap16/MULTI/scanner.c):

~~~c
{% include_relative src/cap16/MULTI/scanner.c %}
~~~

Arquivo [parser.h](src/cap16/MULTI/parser.h):

~~~c
{% include_relative src/cap16/MULTI/parser.h %}
~~~

Arquivo [parser.c](src/cap16/MULTI/parser.c):

~~~c
{% include_relative src/cap16/MULTI/parser.c %}
~~~

Arquivo [codegen.h](src/cap16/MULTI/codegen.h):

~~~c
{% include_relative src/cap16/MULTI/codegen.h %}
~~~

Arquivo [codegen.c](src/cap16/MULTI/codegen.c):

~~~c
{% include_relative src/cap16/MULTI/codegen.c %}
~~~

{% include footer.md %}
