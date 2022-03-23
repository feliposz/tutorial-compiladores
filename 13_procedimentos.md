Parte 13: Procedimentos
=======================

> **Autor:** Jack W. Crenshaw, Ph.D. (07/08/1989)<br>
> **Tradução e adaptação:** Felipo Soranz (27/05/2002)

Finalmente chegamos na parte boa!

Neste ponto estudamos quase todas as características básicas de compiladores, análise léxica e sintática. Aprendemos como traduzir expressões aritméticas, booleanas, estruturas de controle, declaração de dados, e comandos de E/S. Definimos uma linguagem (TINY 1.3) que engloba todas estas características, e escrevemos um compilador rudimentar capaz de traduzí-la. Adicionando alguma E/S em arquivos podemos ter de fato um compilador funcional que pode produzir arquivos objeto executáveis a partir de programas escritos em TINY. Com um compilador destes, podemos escrever programas simples que podem ler da entrada valores inteiros, executar cálculos e processamentos com certa complexidade, e produzir resultados de sáida.

Muito bem, mas o que temos ainda é apenas uma linguagem "de brinquedo". Não podemos ler nem mesmo um único caracter de texto, e ainda não temos as sub-rotinas.

São as características que serão discutidas nos próximos capítulos que separam os homens dos meninos, por assim dizer. Linguagens "reais" tem mais de um tipo de dados, e suportam chamadas a rotinas. Mais que quaisquer outras, são estas duas características que dão a uma linguagem muito do seu caráter e personalidade. Uma vez que tenhos tratado disto, nossa linguagems, TINY e suas sucessoras, vão deixar de ser linguagens de brinquedo e vão ter um caráter de linguagem mais real, convenientes para trabalhos sérios de programação.

Por várias vezes agora, estive prometendo seções tratando destes importantes assuntos. Em cada vez, assuntos novos surgiram que nos fizeram desviar do principal e tratar deles. Finalmente, fomos capazes de resolver todos estes problemas e podemos voltar ao núcleo da coisa. Neste capítulo, vou tratar de procedimentos. No próximo, vamos falar dos tipos de dados básicos.

Um Último Desvio
----------------

Este capítulo foi de uma difuldade extraordinária para eu escrever. A razão não tem nada a ver com o assunto em si... eu já sei o que eu quero dizer há algum tempo, e na verdade eu apresentei a maior parte na conferência Software Development '89, em Fevereiro. Tem mais a ver com a abordagem. Deixe-me explicar.

Quando eu comecei esta série, eu disse que usaríamos diversos "truques" para manter as coisas simples, e permitir que os conceitos sejam apresentados sem ficarmos nos prendendo em detalhes. Um destes truques era tratar das peças individuais de um compilador de cada vez, isto é, fazer experiências baseadas no "berço". Quando estudamos expressões, por exemplo, tratamos apenas daquela parte da teoria dos compiladores. Quando estudamos estruturas de controle, fizemos um programa diferente para tratar desta parte, ainda baseado no "berço". Só fomos incorporar estes conceitos em uma linguagem completa recentemente. Mas estas técnicas nos ajudaram muito realmente, e nos levaram ao desenvolvimento de um compilador para TINY versão 1.3.

Inicialmente, quando eu comecei esta seção, eu tentei construir em cima do que já havíamos feito, e tentei simplesmente adicionar novas características ao compilador existente. Isto acabou se tornando um pouco esquisito e complicado... demais para mim.

Mas eu finalmente descobri porque. Nesta série de experiências, eu abandonei a técnica que nos foi tão útil e que permitiu que chegássemos até aqui, e sem compreender que eu havia mudado para um novo método de trabalho, que envolvia mudanças incrementais ao compilador TINY completo.

Você deve compreender que o que estamos fazendo aqui é algo único. Já existiram diversos artigos, como o artigo sobre Small C por Cain e Hendrix, que apresentaram compiladores completos para uma linguagem ou outra. Isto é diferente. Nesta série de tutoriais, **você está acompanhando o projeto e a implementação de uma linguagem e um compilador em tempo real.**

Nos experimentos que estive fazendo em preparação para este artigo, eu tentei colocar as alterações no compilador TINY de forma que, em cada passo, ainda teríamos um compilador real e funcional. Em outras palavras, eu estava tentando um avanço incremental da linguagem e seu compilador, enquanto explicava a você ao mesmo tempo o que estava fazendo.

Isto é algo difícil de fazer! Eu finalmente percebi que era tolice tentar. Tendo alcançado até aqui usando a idéia de experimentos simples baseados em tokens de um caracter e programas simples de propósito especial, eu abandonei tudo em troca de trabalhar com o compilador completo. Não estava funcionando.

Portanto, vamos voltar às nossas raízes, por assim dizer. Neste capítulo e no próximo, vou usar tokens de um só caracter novamente conforme aprendemos o conceito de procedimentos, sem ser afetados pela bagagem que acumulamos das seções anteriores. Na verdade, nem sequer vou tentar, no final desta seção, juntar as construções ao compilador TINY. Vamos deixar isto para depois.

Depois de todo este tempo, você não precisa de mais do que isso, portanto vamos parar de perder tempo e começar logo com isso.

O Básico
--------

Todas as CPUs modernas dão suporte a chamadas de procedimentos, e o 80x86 não é exceção. Para o 80x86 a chamada é uma instrução CALL, e o retorno é RET. Tudo o que temos que fazer é ajustar o compilador para emitir estes comandos no lugar certo.

Realmente há três coisas que temos que discutir. Uma delas é o mecanismo de chamada/retorno. A segunda é o mecanismo de DEFINIR o procedimento primeiro. E, finalmente, há a questão de passagem de parâmetros para o procedimento chamado. Nenhuma destas coisas é realmente muito difícil, e é claro que podemos "emprestar" o que as outras pessoas fizeram nas outras linguagens... não há porque reinventar a roda aqui. Dos três problemas, aquele da passagem de parâmetros vai ocupar a maior parte da nossa atenção, simplesmente porque existem muitas opções disponíveis.

Uma Base para as Experiências
-----------------------------

Como sempre, vamos precisar de algum software para servir de base para o que estamos fazendo. Não precisamos do compilador TINY completo, mas precisamos de um programa suficiente para algumas das outras construções apresentadas. Especificamente, temos que pelo menos estar aptos a tratar de comandos de algum tipo, e declarações de dados.

O programa mostrado abaixo é esta base. É uma forma resultante de TINY, com tokens de um caracter. Ele possui declarações de dados, mas somente a forma mais simples... sem listas de inicializadores. Possui comandos de atribuição, mas apenas de um tipo:

~~~ebnf
    <ident> = <ident>
~~~

Em outras palavras, a única expressão válida é um único nome de variável. Não há construções de controle... o único comando válido é a atribuição.

A maior parte do programa são as rotinas padrão do "berço". Estou mostrando o código inteiro abaixo, apenas para ter certeza de que estamos começando do mesmo ponto:

~~~c
{% include_relative src/cap13-base.c %}
~~~

> Download da [base](src/cap13-base.c).

Repare que temos uma tabela de símbolo, e há uma lógica para verificar se o nome da variável é válido. Vale a pena notar que eu inclui o código que já foi visto antes para prover espaço em branco e quebras de linha. Finalmente, repare que o programa principal está delimitado, como de custome, por BEGIN-END.

Uma vez que o programa esteja pronto, o primeiro passo é compilá-lo e rodá-lo, para ter certeza que ele funciona. Dê-lhe algumas declarações, e um bloco BEGIN. Tente algo como:

    va    (para VAR A)
    vb    (para VAR B)
    vc    (para VAR C)
    b     (para BEGIN)
    a=b
    b=c
    e.    (para END.)

Como sempre, você deve cometer alguns erros de propósito, e verificar se o programa os aponta corretamente.

Declarando um Procedimento
--------------------------

Se você está satisfeito que o nosso pequeno programa funciona, então é hora de tratar das rotinas. Já que ainda não falamos sobre parâmetros ainda, vamos começar considerando apenas procedimentos que não possuem lista de parâmetros.

Para começar, vamos considerar um program simples com um procedimento, e pensar no código que queremos gerado para ele:

~~~
    PROGRAM FOO;                      
    .                                 
    .                                 
    PROCEDURE BAR;                    BAR:
    BEGIN                                     .
    .                                         .
    .                                         .
    END;                                      RET

    BEGIN { programa principal }      MAIN:
    .                                         .
    .                                         .
    BAR;                                      CALL BAR
    .                                         .
    .                                         .
    END.                                      END MAIN
~~~

Aqui eu mostro a construção em linguagem de alto nível à esquerda, e o código assembly desejado na direita. A primeira coisa a notar é que não há muito código a ser gerado aqui! Para a maior parte tanto do código do procedimento como do programa principal, nossa construção existente toma conta do código gerado.

A chave para lidar com o corpo do procedimento é reconhecer que apesar de um procedimento poder ser bem longo, declará-lo não é muito diferente de declarar uma variável. É só mais um tipo de declaração. Podemos escrever a BNF:

~~~ebnf
    <declaration> ::= <data decl> | <procedure>
~~~

Isto significa que deve ser fácil modificar `TopDeclarations()` para tratar de procedimentos. E quanto à sintaxe do procedimento? Bem, aqui vai uma sintaxe sugerida, que é essencialmente a mesma de Pascal:

~~~ebnf
    <procedure> ::= PROCEDURE <ident> <begin-block>
~~~

Praticamente não é necessária geração de código, além daquele gerado no bloco BEGIN. Só temos que emitir um rótulo no início do procedimento, e um RET no final.

Aqui está o código necessário:

~~~c
/* analisa e traduz uma declaração de procedimento */
void DoProcedure()
{
    char name;

    Match('p');
    name = GetName();
    NewLine();
    if (InTable(name))
        Duplicate(name);
    AddEntry(name, 'p');
    printf("%c:\n", name);
    BeginBlock();
    AsmReturn();
}
~~~

Repare que eu adicionei uma nova rotina de geração de código, `AsmReturn()`, que simplesmente emite uma instrução RET.

Para terminar esta versão, adicione o seguinte dentro do comando switch em `TopDeclarations()`:

~~~c
            case 'p':
                DoProcedure();
                break;
~~~

Eu devo mencionar que esta estrutura para declarações, e a BNF que a define, é diferente do padrão Pascal. Na definição de Pascal de Jensen & Wirth, declarações de variáveis, e na verdade TODO tipo de declaração, devem aparecer em uma sequência específica, isto é, rótulos, constantes, tipos, variáveis, procedimentos e programa principal. Para seguir este esquema, deveríamos separar as duas declarações, e ter um código como este no programa principal:

~~~c
    doVars();
    doProcedurees();
    DoMain();
~~~

No entanto, a maioria das implementações de Pascal, incluindo Turbo Pascal, não requerem esta ordem e permitem que você misture as diversas declarações livremente, desde que você não pretenda referenciar alguma coisa antes de declará-la. Apesar de ser esteticamente correto declarar as variáveis globais no início do programa, certamente não causa nenhum mal permitir que elas fiquem espalhadas. De fato, pode até fazer algum bem, no sentido que lhe dá a oportunidade de fazer um "encapsulamento" rudimentar de informaçÕes. Variáveis que só devem ser acessadas somente pelo programa principal, por exemplo, podem ser declaradas diretamente antes e portanto serão inacessíveis por outros procedimentos.

Certo, teste esta nova versão. Repare que podemos declarar quantos procedimentos quisermos (desde que não fiquemos sem nenhum identificador de uma letra livre!), e os rótulos e RETs ficam nos locais corretos.

Vale a pena ressaltar aqui que eu NÃO permito procedimentos aninhados. Em TINY, todos os procedimentos devem ser declarados no nível global, da mesma forma que em C. Houve uma boa discussão sobre isto no Fórum de Linguagem de Programação da [CompuServe](https://pt.wikipedia.org/wiki/CompuServe). Acontece que há uma penalidade significativa no aumento da complexidade em troca do luxo dos procedimentos aninhados. E mais, esta penalidade ocorre em TEMPO DE EXECUÇÃO, pois código extra deve ser adicionado e executado em cada vez que um procedimento é chamado. Eu também acredito que aninhamento de procedimentos não é uma idéia muito boa, simplesmente pelo fato de eu ter visto muitos "abusos" desta característica. Antes de continuarmos com o próximo passo, vale a pena notar que o "programa principal" como está agora está incompleto, já que ele não possui o rótulo e o comando END do assembler. Vamos arrumar isto:

~~~c
/* analiza e traduz o bloco principal do programa */
void DoMain()
{
    Match('b');
    NewLine();
    AsmProlog();
    DoBlock();
    AsmEpilog();
}

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    AsmHeader();
    TopDeclarations();
    DoMain();

    return 0;
}
~~~

(Nota: Você pode reutilizar os códigos de `AsmHeader()`, `AsmProlog()` e `AsmEpilog()` usados anteriormente.)

Repare que `DoProcedure()` e `MainBlock()` não são muito simétricos. `DoProcedure()` faz uma chamada a `BeginBlock()`, mas `MainBlock()` não pode fazer isto. Isto porque um procedimento é identificado pela palavra-chave PROCEDURE (abreviada por "p" aqui), enquanto o programa principal não tem nenhuma palavra-chave além do próprio BEGIN.

E ISTO nos leva a uma questão interessante: POR QUÊ?

Se observarmos a estrutura dos programas C, descobrimos que todas as funções são tratadas da mesma forma, exceto o programa principal, que é identificado pelo seu nome "main". Como as funções C podem aparecer em qualquer ordem, o programa principal também pode estar em qualquer lugar na unidade de compilação.

Em Pascal, por outro lado, todas as variáveis e procedimentos devem ser declaradas antes de serem usadas, o que significa que não faz sentido colocar nada depois do programa principal... pois não poderia ser acessado. O programa principal não é nem mesmo identificado, a não ser por ser a única parte do código que vem depois do BEGIN global. Em outras palavras, se não é outra coisa, deve ser o programa principal.

Isto causa não poucos problemas para os programadores iniciantes e para os mais experientes às vezes é difícil achar o início do programa principal em si. Isto leva a convençÕes, como identificação com comentários:

    BEGIN { of MAIN }

Isto sempre pareceu ser um pouco bizarro. A questão surge: Por que o programa principal deve ser tratado de forma tão diferente de um procedimento? Na verdade, agora que reconhecemos que as declaraçÕes de procedimentos são apenas isto... parte das declarações globais... não seria o programa principal apenas uma outra declaração também?

A resposta é sim, e tratando-o desta forma, podemos simplificar o código e fazê-lo consideravelmente mais "ortogonal". Minha proposta é que podemos usar uma palavra-chave explícita, PROGRAM, para identificar o programa principal (Repare que isto significa que não podemos começar o programa com ela, como em Pascal). Neste caso, nossa BNF se torna:

~~~ebnf
    <declaration> ::= <data decl> | <procedure> | <main-program>
    <procedure> ::= PROCEDURE <ident> <begin-block>
    <main-program> ::= PROGRAM <ident> <begin-block>
~~~

O código também parece muito melhor, ao menos no sentido que `DoMain()` e `DoProcedure()` se parecem agora:

~~~c
/* analiza e traduz o bloco principal do programa */
void DoMain()
{
    char name;

    Match('P');
    name = GetName();
    NewLine();
    if (InTable(name))
        Duplicate(name);
    AsmProlog();
    BeginBlock();
    AsmEpilog();
}

/* analiza e traduz as declarações globais */
void TopDeclarations()
{
    while (Look != '.') {
        switch (Look) {
            case 'v':
                Declaration();
                break;
            case 'p':
                DoProcedure();
                break;
            case 'P':
                DoMain();
                break;
            default:
                Unrecognized(Look);
                break;
        }
        NewLine();
    }
}

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    AsmHeader();
    TopDeclarations();
    AsmEpilog();

    return 0;
}
~~~

Já que a declaração do programa principal agora está dentro do laço em `TopDeclarations()`, isto apresenta algumas dificuldades. Como podemos ter certeza que é a última coisa no arquivo? E como saímos do laço? Minha resposta para a segunda pergunta foi, como você pode ver, trazer de volta nosso velho amigo ponto ("."). Uma vez que o analisador o veja, está terminado.

Para responder a primeira pergunta: depende de quanto você quer proteger o programador de cometer erros bobos. No código que eu mostrei, não há nada para evitar que o programador adicione código depois do programa principal... mesmo outro programa principal (?). O código simplesmente não será acessível. Porém, PODERÍAMOS acessá-lo através de um comando FORWARD em Pascal (ou protótipos, no caso de C), o qual vamos prover mais tarde. Na verdade, muitos programadores assembly gostam de usar a área logo após o programa para declarar blocos grandes de dados não inicializados, portanto, pode haver algum valor em não exigir que o programa principal venha por último. Vamos deixá-lo como está.

Se decidirmos que devemos dar ao programador um pouco mais de ajuda que isto, é relativamente fácil adicionar alguma lógica para sair do laço assim que o programa principal for processado. Ou poderíamos pelo menos indicar um erro se alguém tentar incluir dois programas principais.

Chamando o Procedimento
-----------------------

Se você está satisfeito que as coisas estejam funcionando, vamos tratar da segunda parte da equação... a chamada.

Considere a BNF para a chamada de procedimento:

~~~ebnf
    <proc-call> ::= <identifier>
~~~

para um comando de atribuição, por outro lado, a BNF é:

~~~ebnf
    <assignment> ::= <identifier> '=' <expression>
~~~

Neste ponto parece que temos um problema. As duas sintaxes BNF começam com o mesmo token <identifier> do lado direito. Como podemos saber, quando vemos um identificador, se ele é uma chamada de procedimento ou um comando de atribuição? Este parece ser um caso em que nosso analisador deixa de ser preditivo, e de fato é exatamente o caso. Porém, este é um problema fácil de resolver, já que tudo o que temos que fazer é verificar o tipo do identificador, conforme armazenado na tabela de símbolos. Como já foi descoberto antes, uma violação pequena e local da regra de análise preditiva pode ser facilmente tratada como um caso especial.

Aqui está como fazê-lo:

~~~c
/* analisa e traduz um comando de atribuição */
void Assignment(char name)
{
    Match('=');
    Expression();
    AsmStoreVar(name);
}

/* analisa e traduz um comando de atribuição ou chamada de procedimento */
void AssignOrProc()
{
    char name;

    name = GetName();
    switch (SymbolType(name)) {
        case ' ':
            Undefined(name);
            break;
        case 'v':
            Assignment(name);
            break;
        case 'p':
            AsmCall(name);
            break;
        default:
            Abort("Identifier %c cannot be used here!", name);
    }
}

/* analiza e traduz um bloco de comandos */
void DoBlock()
{
    while (Look != 'e') {
        AssignOrProc();
        NewLine();
    }
}
~~~

Como você pode ver, a rotina `DoBlock()` agora chama `AssignOrProc()` ao invés de `Assignment()`. A função desta nova rotina é simplificar a leitura do identificador, determinar seu tipo, e então chamar a rotina apropriada para este tipo. Como o nome já foi lido, precisamos passá-lo para as duas rotinas, e modificar `Assignment()` para tratar disto. A rotina `AsmCall()` é simplesmente uma rotina de geração de código:

~~~c
/* gera uma chamada de procedimento */
void AsmCall(char name)
{
    EmitLn("CALL %c", name);
}
~~~

Bem, neste ponto temos um compilador que é capaz de tratar de procedimentos. Vale a pena notar que os procedimentos podem chamar outros procedimentos em qualquer nível. Portanto, mesmo não permitindo DECLARAÇÕES aninhadas, certamente não há nada que nos impeça de fazer CHAMADAS aninhadas, da mesma forma que esperamos fazer em qualquer linguagem. Chegamos até lá, e não foi muito difícil, foi?

É claro que até agora só tratamos de procedimento que não tem parâmetros. Os parâmetros podem operar apenas nas variáveis globais através dos nomes globais. Então temos algo equivalente à construção GOSUB da linguagem BASIC. Nada mal... afinal de contas, muitos programas sérios foram escritos usando GOSUBs, mas podemos fazer melhor, e nós vamos. É o próximo passo.

Passagem de Parâmetros
----------------------

Novamente, todos nós conhecemos a idéia básica da passagem de parâmetros, mas vamos revisá-la só pra ter certeza.

Em geral um procedimento tem uma lista de parâmetros, por exemplo:

    PROCEDURE FOO(X, Y, Z)

Na declaração do procedimento, os parâmetros são chamados de parâmetros formais, e podem ser referenciados no corpo do procedimento por estes nomes. Os nomes usados pelos parâmetros formais são realmente arbitrários. Apenas a posição conta de verdade. No exemplo acima, o nome "X" simplesmente significa "o primeiro parâmetro" sempre que é usado.

Quando um procedimento é chamado, os "parâmetros verdadeiros" passados para ele são associados com os parâmetros formais, em uma base de um-para-um.

A sintaxe BNF para isto é algo assim:

~~~ebnf
    <procedure> ::= PROCEDURE <ident> '(' <param-list> ')' <begin-block>
    <param-list> ::= <parameter> ( ',' <parameter> )* | null
~~~

De forma similar, a chamada do procedimento é:

~~~ebnf
    <proc-call> ::= <ident> '(' <param-list> ')'
~~~

Repare que já existe uma decisão implícita nesta sintaxe. Algumas linguagens, como Pascal e Ada, permitem que a passagem de listas de parâmetros seja opcional. Se não houver parâmetros, você simplesmente não coloca os parênteses. Outras linguagens, como C e Modula-2, exigem os parênteses mesmo que a lista esteja vazia. Claramente, o exemplo que acabamos de completar corresponde ao primeiro ponto de vista. Mas para dizer a verdade eu prefiro o segundo. Para os procedimentos em si, a decisão parece favorecer a abordagem "sem lista". O comando:

    Initialize;

sozinho, só pode significar uma chamada de procedimento. Nos analisadores que escrevemos até agora, fizemos um grande uso de procedimentos sem parâmetros, mas por estarmos usando linguagem C, tivemos que colocar um par de parênteses após as chamadas.

Porém mais tarde, vamos fazer uso de funções também. E como funções podem aparecer no mesmo lugar que um identificador, não há como dizer a diferença entre os dois. Você precisa voltar às declarações para descobrir. Algumas pessoas consideram isto uma vantagem. Seu argumento é que um identificador é trocado por um valor, e qual a importância de saber se isto é feito por substituíção ou por uma função? Mas algumas vezes nós nos importamos, pois a função pode consumir algum tempo considerável. Se podemos incorrer em uma penalidade de tempo de execução alta, escrevendo um mero identificador em uma expressão, parece importante estar ciente disto.

Afinal, Niklaus Wirth projetou Pascal e Modula-2. Eu vou dar a ele o benefício da dúvida e assumir que ele tinha uma boa razão para alterar a regra da segunda vez!

É desnecessário dizer, que é fácil acomodar qualquer um dos pontos de vista conforme projetamos a linguagem, portanto é estritamente um problema de preferência pessoal. Faça da forma que você achar melhor.

Antes de continuar mais, vamos alterar o tradutor para tratar de uma lista de parâmetros (possivelmente vazia). Por enquanto não vamos gerar qualquer código extra... apenas analisar a sintaxe. O código para processar a declaração tem uma forma muito semelhante ao tratamento que fazíamos com as listas de variáveis:

~~~c
/* processa a lista de parâmetros formais de um procedimento */
void FormalList()
{
    Match('(');
    if (Look != ')') {
        FormalParam();
        while (Look == ',') {
            Match(',');
            FormalParam();
        }
    }
    Match(')');
}
~~~

O procedimento `DoProcedure()` precisa de uma linha adicional para chamar `FormalList()`:

~~~c
/* analisa e traduz uma declaração de procedimento */
void DoProcedure()
{
    char name;

    Match('p');
    name = GetName();
    FormalList();
    NewLine();
    if (InTable(name))
        Duplicate(name);
    AddEntry(name, 'p');
    printf("%c:\n", name);
    BeginBlock();
    AsmReturn();
}
~~~

Por enquanto, o código para `FormalParam()` é apenas uma rotina vazia que simplesmente pula o nome do parâmetro:

~~~c
/* processa um parâmetro formal */
void FormalParam()
{
    char name;

    name = GetName();
}
~~~

Para a chamada do procedimento, deve haver um código similar para processar a lista de parâmetros:

~~~c
/* processa um parâmetro de chamada */
void Param()
{
    Expression();
}

/* processa a lista de parâmetros para uma chamada de procedimento */
void ParamList()
{
    Match('(');
    if (Look != ')') {
        Param();
        while (Look == ',') {
            Match(',');
            Param();
        }
    }
    Match(')');
}

/* processa uma chamada de procedimento */
void CallProc(char name)
{
    ParamList();
    AsmCall(name);
}
~~~

Altere a chamada de `AsmCall()` por uma chamada a `CallProc()`, na rotina `AssignOrProc()`.

Certo, se você adicionou tudo isto ao seu código e fez alguns testes, vai descobrir que a sintaxe está sendo tratada corretamente. Devo avisar que NÃO HÁ checagem para ter certeza que o número (e, posteriormente, os tipos) de parâmetros formais e os parâmetros da chamada combinam. Em um compilador de produção, é claro que precisamos fazer isto. Vamos ignorar a questão no momento apenas pelo fato da nossa tabela de símbolos atual não nos dar um lugar para armazenar a informação necessária. Posteriormente, teremos um espaço para estes dados e então poderemos tratar do problema.

A Semântica dos Parâmetros
--------------------------

Até aqui estivemos lidando com a SINTAXE da passagem de parâmetros, e criamos os mecanismos de análise para tratar dela. Na sequência, temos que dar uma olhada na SEMÂNTICA, isto é, as ações que devem ser tomadas quando encontramos parâmetros. Isto nos leva diretamente ao problema das diferentes maneiras de se passar parâmetros.

Há mais de uma forma de passar parâmetros, e a maneira de fazê-lo pode ter um efeito profundo no caráter da linguagem. Então esta é outra das áreas em que eu não posso simplesmente lhe dar minha solução. É importante que apliquemos algum tempo analizando as alternativas para que você siga outra rota se preferir.

Há duas formas principais para se passar parâmetros:

- Por valor
- Por referência (endereço)

As diferenças podem ser melhor esclarecidas com um pouco de história.

Os antigos compiladores FORTRAN passavam parâmetros por referência. Em outras palavras, o que era passado na verdade era o endereço dos parâmetros. Isto significa que a sub-rotina chamada se encontrava livre para ler ou alterar os parâmetros, desde que decida fazer isto, como se ele fosse uma variável global. Isto na verdade era uma forma muito eficiente de fazer as coisas, e era bem simples já que o mesmo mecanismo era usado em todos os casos, com uma exceção da qual vou tratar em breve.

Havia problemas, porém. Muitas pessoas acharam que este método criava muita dependência entre a sub-rotina chamada e a que chamou. Na realidade, dava à sub-rotina acesso completo a todas as variáveis que apareciam na lista de parâmetros.

Muitas vezes, não queremos realmente alterar um parâmetro, mas apenas usá-lo como entrada. Por exemplo, podemos passar uma contagem de elementos para uma sub-rotina, e então desejamos usar aquele contagem dentro de um laço DO. Para evitar a alteração na variável da rotina que chamou, temos que fazer uma cópia local do parâmetro de entrada, e operar somente na cópia. Alguns programadores FORTRAN, de fato, criaram uma prática de copiar TODOS os parâmetros, exceto aqueles que deveriam ser usados como valores de retorno. É desnecessário dizer que toda esta cópia acabava com a melhora de eficiência que estava associada a esta abordagem.

Havia, porém, um problema ainda mais clandestino, que não era realmente uma falha da convenção de "passagem por referência", mas uma convergência de diversas decisões de implementação ruins.

Suponha que temos uma sub-rotina:

    SUBROUTINE FOO(X, Y, N)

onde N é uma espécie de contador de entrada, ou flag. Muitas vezes, gostaríamos de passar um valor literal ou mesmo uma expressão no lugar de uma variável, como:

    CALL FOO(A, B, J + 1)

Aqui o terceiro parâmetro não é uma variável, e portanto não tem endereço. Os primeiros compiladores FORTRAN não permitiam coisas deste tipo, portanto tínhamos que apelar para construçÕes como:

    K = J + 1
    CALL FOO(A, B, K)

Novamente, foi necessária uma cópia, e foi o programador que teve de fazer isto. Nada bom.

Implementações posteriores de FORTRAN livraram-se disto permitindo expressões como parâmetros. O que eles fizeram foi gerar uma variável criada pelo compilador, armazenar o valor da expressão na variável, e então passar o endereço da expressão.

Até então sem problemas. Mesmo que a sub-rotina alterasse por engano a variável anônima, quem iria saber ou se importar? Na próxima chamada, ela seria recalculada de qualquer forma.

O problema ocorreu quando alguém decidiu tornar as coisas mais eficientes. Eles logo perceberam que o tipo mais comum de "expressão" era um simples valor inteiro, como:

    CALL FOO(A, B, 4)

Parece ineficiente tratar do problema de "computar" um inteiro e armazená-lo em uma variável temporária, apenas para passá-la na lista de parâmetros de chamada. Já que precisávamos passar o endereço de qualquer forma, parecia fazer mais sentido passar o endereço do literal inteiro, 4 no exemplo acima.

Para tornar os problemas mais interessantes, muitos compiladores (antigos e atuais) identificam todas as literais e as armazenam em um "poço de literais" (literal pool), de forma que só temos que armazenar uma literal para cada literal única. Esta combinação de decisões de projeto: passar expressões, otimização de literais como um caso especial, e uso do "poço de literais", é o que leva ao desastre.

Para entender como funciona, imagine que é feita uma chamada a FOO como no exemplo acima, passando um 4 literal. Realmente, o que é passado é o endereço do literal (4), que está armazenado no poço de literais. Este endereço corresponde ao parâmetro formal K, na sub-rotina em si.

Agora suponha que, sem o conhecimento do programador, a sub-rotina FOO modifica K para, digamos, -7. Subitamente, a literal 4 no poço de literais é ALTERADA, para -7. Daí em diante, toda expressão que usava 4 e cada sub-rotina que passava 4 como parâmetro vai usar o valor -7 no lugar!!! É desnecessário dizer que isto gerou um compartamento bizarro e muito difícil de achar. A coisa toda deixou com uma reputação muito ruim a passagem por referência, no entanto, como já vimos, na verdade foi uma certa combinação de decisões ruins de projeto que criou o problema.

Apesar do problema, a abordagem de FORTRAN tem seus pontos positivos. A maior delas é o fato de que não é necessário suportar mecanismos múltiplos. O mesmo esquema, passando o endereço do argumento, funciona para TODOS os casos, incluindo matrizes. Portanto o tamanho do compilador pode ser reduzido.

Em parte por conta do problema com FORTRAN, e também por causa da redução da dependência das rotinas chamadora e chamada, linguagens modernas como C, Pascal, Ada, e Modula-2 geralmente passam os parâmetros escalares por valor.

Isto significa que o valor é COPIADO como um valor separado usando apenas na chamada. Já que o valor passado é uma cópia, o procedimento chamado pode usá-lo como uma variável local e modificá-lo da forma que quiser. O valor na rotina que fez a chamada não é alterado.

A princípo pode parecer que isto é um pouco ineficiente, por causa da necessidade de copiar o parâmetro. Mas lembre-se que vamos precisar passar um valor de qualquer forma, seja ele o parâmetro em si ou um endereço para ele. Dentro da sub-rotina usar passagem por valor é definitivamente mais eficiente, já que eliminamos um nível de indireção. Finalmente, vimos antes que em FORTRAN, era necessário fazer cópias dentro da sub-rotina de qualquer jeito, portando passagem por valor pode reduzir o número de variáveis locais. No fim das contas, passagem por valor é melhor.

Excetor por um pequeno detalhe: se todos os parâmetros são passados por valor, não há como o procedimento chamado retornar um resultado para o que fez a chamada! O parâmetro passado NÃO é alterado na rotina que chamou, apenas dentro da rotina chamada. Está claro, que isto não faz todo o trabalho.

Houve duas respostas para este problema, que são equivalentes. Em Pascal, Wirth criou parâmetros VAR, que são passados por referência. O que um parâmetro VAR é, de fato, não é nada além do nosso amigo, o parâmetro FORTRAN, com um novo nome disfarçado. Wirth de uma forma engenhosa evita o problema de "mudança de literais" e também o problema do "endereço de uma expressão", permitindo que apenas variáveis sejam usadas como parâmetros de chamada no caso de passagem por referência. Em outras palvras, é a mesma restrição imposta pelos primeiros compiladores FORTRAN.

C faz a mesma coisa, mas explicitamente. Em C, TODOS os parâmetros são passados por valor. No entanto, um dos tipos de variáveis suportados por C, é o ponteiro. Portanto, passar um ponteiro por valor, é o mesmo que passar uma referência. Em alguns casos isto funciona melhor ainda, pois mesmo que você possa alterar a variável apontada da forma que você quiser, ainda assim você NÃO pode alterar o ponteiro em si. Em uma função como `strcpy()`, por exemplo, onde os ponteiros são incrementados conforme a string é copiada, estamos na verdade incrementando apenas cópias dos ponteiros, portanto os valores destes ponteiros no procedimento que fez a chamado permanecem os mesmos. Para modificar um ponteiro, é preciso passar um ponteiro para o ponteiro.

Como estamos fazendo experimentos aqui, vamos olhar para as duas abordagens: passagem por valor e por referência. Desta forma, poderemos usar qualquer um dos dois quando precisarmos (ou ambos). Devo mencionar que seria difícil usar a abordagem de ponteiros em C aqui, já que isto envolveria um tipo diferente e não estudamos tipos ainda!

Passagem por Valor
------------------

Vamos apenas tentar algumas coisas simples e ver até onde elas nos levam. Começamos com o caso de passagem por valor. COnsiderer a chamada de procedimento:

    FOO(X, Y)

Quase sempre a única forma razoável de se passar os dados é através da pilha da CPU. Então, o código que gostaríamos de ver gerado é algo assim:

~~~asm
PUSH WORD PTR X
PUSH WORD PTR Y
CALL FOO
~~~

Isto certamente não parece muito complexo!

Quando a chamada é executada, a CPU coloca o endereço de retorno na pilha e desvia para FOO. Neste ponto a pilha vai parecer com o seguinte:

~~~
        .
        .
        .
        Valor de X (2 bytes)
        Valor de Y (2 bytes)
  SP -> Endereço de retorno (2 bytes *)
~~~

Portanto, os valores dos parâmetros tem endereços que são deslocamentos fixos em relação ao ponteiro da pilha (SP). Neste exemplo, os endereços seriam:

    X: [bp+4]
    Y: [bp+2]

Agora considere uma forma possível para o procedimento chamado:

    PROCEDURE FOO(A, B)
    BEGIN
        A = B
    END

(Lembre-se, os nomes dos parâmetros formais são arbitrários... apenas a posição conta.)

O código de saída desejado deve ser algo como:

~~~asm
    FOO:
        MOV AX, WORD PTR [SP+2]
        MOV WORD PTR [SP+4], AX
        RET
~~~

Repare que, para endereçar os parâmetros formais, temos que saber qual sua posição na lista de parâmetros. Isto significa algumas mudanças na tabela de símbolos. De fato, para o nosso caso de caracteres únicos, é melhor simplesmente criar uma nova tabela de símbolos de parâmetros formais.

Vamos começar declarando uma nova tabela:

~~~c
#define PARAMTABLE_SIZE 26
int ParamTable[PARAMTABLE_SIZE]; /* lista de parâmetros formais para os procedimentos */
~~~

Vamos precisar saber também, quantos parâmetros um procedimento tem:

~~~c
int ParamCount; /* número de parâmetros formais */
~~~

E precisamos inicializar a nova tabela. Agora, lembre-se que a lista de parâmetros formais será diferente para cada procedimento que processarmos, então precisaremos reinicializar esta tabela várias vezes para cada procedimento. Aqui está o inicializador:

~~~c
/* limpa a tabela de parâmetros formais */
void ClearParams()
{
    int i;
    for (i = 0; i < PARAMTABLE_SIZE; i++)
        ParamTable[i] = 0;
    ParamCount = 0;
}
~~~

Vamos adicionar uma chamada a esta rotina em `Init()`, e também em `DoProcedure()`:

~~~c
/* inicialização do compilador */
void Init()
{
    int i;

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        SymbolTable[i] = ' ';

    ClearParams();

    NextChar();
    SkipWhite();
}

/* analisa e traduz uma declaração de procedimento */
void DoProcedure()
{
    char name;

    Match('p');
    name = GetName();
    FormalList();
    NewLine();
    if (InTable(name))
        Duplicate(name);
    AddEntry(name, 'p');
    printf("%c:\n", name);
    BeginBlock();
    AsmReturn();
    ClearParams();
}
~~~

Repare que a chamada a `DoProcedure()` assegura que a tabela estará livre assim que começarmos com o programa principal.

Agora precisamos de algumas rotinas para trabalhar com a tabela. As próximas funções são essencialmente cópias de `InTable()`, `SymbolType`, etc.:

~~~c
/* retorna número indicando a posição do parâmetro */
int ParamNumber(char name)
{
    return ParamTable[name - 'A'];
}

/* verifica se nome é parâmetro */
int IsParam(char name)
{
    return (ParamTable[name - 'A'] != 0);
}

/* adiciona parâmetro à lista */
void AddParam(char name)
{
    if (IsParam(name))
        Duplicate(name);
    ParamTable[name - 'A'] = ++ParamCount;
}
~~~

Finalmente, precisamos de algumas rotinas de geração de código:

~~~c
/* calcula deslocamento do parâmetro na pilha */
int AsmOffsetParam(int pos)
{
    int offset;

    /* offset = endereço de retorno + tamanho do parâmetro * posição relativa */
    offset = 2 + 2 * (ParamCount - pos); 

    return offset;
}

/* carrega parâmetro em registrador primário */
void AsmLoadParam(int pos)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV AX, WORD PTR [SP+%d]", offset);
}

/* armazena conteúdo do registrador primário em parâmetro */
void AsmStoreParam(int pos)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV WORD PTR [SP+%d], AX", offset);
}

/* coloca registrador primário na pilha */
void AsmPush()
{
    EmitLn("PUSH AX");
}
~~~

Observações:

1. A última rotina nós já conhecemos, mas ela não estava nesta versão do nosso programa.
2. Já que o cálculo do offset aparece em dois lugares, coloquei-o numa rotina separada.

Com estas preliminares prontas, estamos prontos para lidar com a semântica dos procedimentos com lista de parâmetros (lembre-se, o código para tratar da sintaxe já está pronto também).

Vamos começar com o processamento dos parâmetros formais. Tudo o que temos que fazer é adicionar cada parâmetro à tabela de símbolos dos parâmetros:

~~~c
/* processa um parâmetro formal */
void FormalParam()
{
    char name;

    name = GetName();
    AddParam(name);
}
~~~

Agora, o que fazer quando um parâmetro é encontrado quando ele aparece no corpo do procedimento? Isto dá um pouco mais de trabalho. Precisamos primeiro determinar se ele É um parâmetro formal. Para fazer isto, eu escrevi a seguinte modificação de `SymbolType()`:

~~~c
/* retorna o tipo de um identificador */
char SymbolType(char name)
{
    if (IsParam(name))
        return 'f';
    return SymbolTable[name - 'A'];
}
~~~

Também devemos modificar `AssignOrProc()` para tratar deste novo tipo:

~~~c
/* analisa e traduz um comando de atribuição ou chamada de procedimento */
void AssignOrProc()
{
    char name;

    name = GetName();
    switch (SymbolType(name)) {
        case ' ':
            Undefined(name);
            break;
        case 'v':
        case 'f':
            Assignment(name);
            break;
        case 'p':
            CallProc(name);
            break;
        default:
            Abort("Identifier %c cannot be used here!", name);
    }
}
~~~

Finalmente, o código para processar uma atribuição e uma expressão devem ser estendidos:

~~~c
/* analisa e traduz uma expressão */
void Expression()
{
    char name = GetName();
    if (IsParam(name))
        AsmLoadParam(ParamNumber(name));
    else
        AsmLoadVar(name);
}

/* analisa e traduz um comando de atribuição */
void Assignment(char name)
{
    Match('=');
    Expression();
    if (IsParam(name))
        AsmStoreParam(ParamNumber(name));
    else
        AsmStoreVar(name);
}
~~~

Como você pode ver, estes procedimentos vão tratar de cada nome de variável encontrado como um parâmetro formal ou como uma variável global, dependendo do fato de ele constar ou não na tabela de símbolos de parâmetros. Lembre-se que estamos usando apenas uma forma reduzida de `Expression()`. No programa final, a alteração mostrada aqui deve ser adicionada em `Factor()`, não em `Expression()`.

O resto é fácil. Só temos que adicionar a semântica para a chamade de procedimento, o que podemos fazer apenas com uma nova linha de código:

~~~c
/* processa um parâmetro de chamada */
void Param()
{
    Expression();
    AsmPush();
}
~~~

É só isto. Adicione estas mudanças ao seu programa e faça um teste. Experimente declarar um ou dois procedimentos, cada um com uma lista de parâmetros formais. Então faça algumas atribuições, usando combinações de variáveis globais e parâmetros formais. Você pode chamar um procedimento dentro do outro, mas você não pode DECLARAR procedimentos aninhados. Você pode até mesmo passar parâmetros formais de um procedimento para outro. Se tivessemos a sintaxe completa da linguagem agora, seria possível fazer coisas como ler e alterar parâmetros formais ou usá-los em expressões complexas.

O Que Está Errado?
------------------

Neste ponto, talvez você deva estar pensando: certamente deve haver algo mais além destes PUSHs. Deve haver mais sobre passagem de parâmetros do que isto.

Você está certo. Na realidade, o código que fizemos deixa muito a desejar em uma série de aspectos.

O que mais chama atenção é que ele está ERRADO! Se você olhar novamente o código para a chamada de procedimentos, vai notar que a chamada do procedimento coloca cada parâmetro na pilha antes de fazer a chamada em si. O procedimento usa a informação, mas ele não muda o ponteiro de pilha. Isto significa que a coisa continua lá quando retornarmos. ALGUÉM deve limpar a pilha, ou em breve vamos entrar numa fria!

Felizmente, é algo fácil de arrumar. Tudo o que devemos fazer é incrementar o ponteiro de pilha quando terminarmos.

Devemos fazê-lo dentro do procedimento que fez a chamada, ou dentro da próprio procedimento executado? Algumas pessoas fazem com que o procedimento faça a limpeza da pilha, já que isto requer menos código a ser gerado por chamada, e já que o procedimento, afinal de contas, sabe quantos parâmetros recebeu. Mas isto significa também que algo deve ser feito com o endereço de retorno, para que ele não se perca. Afinal ele está no topo da pilha, e queremos limpar os parâmetros que estão abaixo dele.

Eu prefiro permitir que a rotina que fez a chamada faça a limpeza, para que a rotina executada só precise retornar. Além disso, parece ser algo mais balanceado, já que aquele que fez a chamada é que "fez uma bagunça" na pilha. Mas isto significa que a rotina solicitante deve lembrar quantos itens foram colocados na pilha. Para tornar as coisas mais simples, eu alterei o procedimento `ParamList()` para que ele seja uma função retornando o número de bytes que foram colocados na pilha:

~~~c
/* processa a lista de parâmetros para uma chamada de procedimento */
int ParamList()
{
    int count = 0;;

    Match('(');
    if (Look != ')') {
        for (;;) {
            Param();
            count++;
            if (Look != ',')
                break;
            Match(',');
        }
    }
    Match(')');

    return count * 2; /* número de parâmetros * bytes por parâmetro */
}
~~~

O procedimento `CallProc()` usa este resultado para limpar a pilha:

~~~c
/* processa uma chamada de procedimento */
void CallProc(char name)
{
    int bytes = ParamList();
    AsmCall(name);
    AsmCleanStack(bytes);
}
~~~

Aqui eu criei outra rotina de geração de código:

~~~c
/* ajusta o ponteiro da pilha acima */
void AsmCleanStack(int bytes)
{
    if (bytes > 0)
        EmitLn("ADD SP, %d", bytes);
}
~~~

Certo, se você adicionar este código ao compilador, já vai ter a pilha sob controle.

O próximo problema tem a ver com a maneira de endereçamento relativa ao ponteiro de pilha. Isto funciona bem em nossos exemplos simples, já que estamos usando formas rudimentares de expressões e ninguém mais está mexendo com a pilha. Mas considere um exemplo diferente como:

    PROCEDURE FOO(A, B)
    BEGIN
        A = A + B
    END

O código gerado por um analisador simplístico poderia ser:

~~~asm
    FOO:
        MOV AX, WORD PTR [SP+4] ;acessa A
        PUSH AX                 ;coloca-o na pilha
        MOV AX, WORD PTR [SP+2] ;acessa B
        POP BX                  ;recupera valor de A
        ADD AX, BX              ;adiciona B + A
        MOV WORD PTR [SP+4], AX ;armazena resultado em A
        RET
~~~

Isto seria errado. Quando colocamos o primeito argumento da expressão na pilha, o deslocamento dos dois parâmetros formais não é mais 2 e 4, mas 4 e 6. Então o segundo acesso `MOV AX, WORD PTR [SP+2]` iria acessar A novamente, e não B como desejaríamos.

Porém, isto não é o fim do mundo. Eu acho que você percebe que tudo o que devemos fazer é alterar o deslocamento cada vez que colocamos algo na pilha, e de fato é o que é feito quando a CPU não suporta outros métodos.

Felizmente, o 80x86 tem um suporte direto a este tipo de coisa.

O problema, como você pode ver, é que quando o procedimento é executado, o ponteiro da pilha (SP) é movido acima e abaixo, e portanto seria algo estranho usá-lo como referência para acessar os parâmetros formais. A solução é definir algum OUTRO registrador, e usá-lo em seu lugar. Este registrador é tipicamente alterado para o valor do ponteiro da pilha. Para este propósito, podemos usar o registrador de base de pilha (BP) já que ele não tem outros usos para os nossos propósitos.

Armazenamos o valor de BP na pilha (para poder restaurá-lo depois) e movemos o valor de SP para BP. A partir de então usaremos BP ao invés de SP como a base para o acesso aos parâmetros formais. Antes de retornarmos do procedimento, restauramos o valor de BP, que estava armazenado na pilha. Como o valor de BP é adicionado à pilha antes de atribuirmos a ele o valor de SP, precisamos adicionar mais um deslocamento de 2 bytes para cada parâmetro formal, além do deslocamento do endereço de retorno (repare que ao invés de usarmos 2 e 4 para o deslocamento, usamos 4 e 6).

Usando esta técnica, o código para o procedimento anterior torna-se:

~~~asm
    FOO:
        PUSH BP                 ;armazena BP na pilha
        MOV BP, SP              ;move SP para BP
        MOV AX, WORD PTR [BP+6] ;acessa A
        PUSH AX                 ;coloca-o na pilha
        MOV AX, WORD PTR [BP+4] ;acessa B
        POP BX                  ;recupera valor de A
        ADD AX, BX              ;adiciona B + A
        MOV WORD PTR [SP+6], AX ;armazena resultado em A
        POP BP
        RET
~~~

Arrumar o compilador para gerar este código é muito mais fácil do que explicá-lo. Tudo o que temos que fazer é alterar a geração do código criado por `DoProcedure()`. Eu criei novas rotinas para tratar disto, paralelas a `AsmProlog()` e `AsmEpilog()` chamadas em `DoMain()`:

~~~c
/* escreve o prólogo para um procedimento */
void AsmProcProlog(char name)
{
    printf("%c:\n", name);
    EmitLn("PUSH BP");
    EmitLn("MOV BP, SP");
}

/* escreve o epílogo para um procedimento */
void AsmProcEpilog()
{
    EmitLn("POP BP");
    EmitLn("RET");
}
~~~

A rotina `DoProcedure()` fica:

~~~c
/* analisa e traduz uma declaração de procedimento */
void DoProcedure()
{
    char name;

    Match('p');
    name = GetName();
    FormalList();
    NewLine();
    AddEntry(name, 'p');
    AsmProcProlog(name);
    BeginBlock();
    AsmProcEpilog();
    ClearParams();
}
~~~

Finalmente, precisamos alterar as referências a SP e alterar o cálculo do deslocamento para considerar o registrador BP que foi colocado na pilha também:

~~~c
int AsmOffsetParam(int pos)
{
    int offset;

    /* offset = (ret_address + BP) + param_size * param_pos */
    offset = 4 + 2 * (ParamCount - pos); 

    return offset;
}

/* carrega parâmetro em registrador primário */
void AsmLoadParam(int pos)
{
    EmitLn("MOV AX, WORD PTR [BP+%d]", AsmOffsetParam(pos));
}

/* armazena conteúdo do registrador primário em parâmetro */
void AsmStoreParam(int pos)
{
    EmitLn("MOV WORD PTR [BP+%d], AX", AsmOffsetParam(pos));
}
~~~

É o que basta. Tente isto e veja se você gosta.

Neste ponto estamos gerando um código relativamente bom para procedimentos e chamadas. Apesar da limitação de não permitimos variáveis locais (ainda) e não ser permitido aninhamento de procedimentos, este código é tudo o que precisamos.

Ainda existe um pequeno problema:

> NÃO HÁ COMO RETORNAR OS RESULTADOS À ROTINA SOLICITANTE!

Mas isto é claro, não é uma limitação do código que estamos gerando, mas algo inerente à chamada por valor. Repare que PODEMOS usar parâmetros formais de qualquer forma dentro do procedimento. Podemos calcular novos valores para eles, usá-los como contadores de laços (isto é, se tivéssemos laços nesta versão!), etc. Portanto o código está fazendo o que deveria. Para tratarmos deste último problema, temos que observar o tipo alternativo de chamada.

Segue o código completo da nossa implementação de passagem de parâmetros por valor:

~~~c
{% include_relative src/cap13-byval.c %}
~~~

> Download da implementação ["por valor"](src/cap13-byval.c).

Passagem por Referência
-----------------------

Isto é simples, agora que temos os mecanismos necessários já construídos. Só precisamos fazer algumas alterações na geração do código. Ao invés de colocar um valor na pilha, devemos colocar um endereço. Na verdade, existe uma maneira própria para se fazer isto no 80x86.

Nós vamos fazer uma nova versão do programa de teste para isto. Antes de mais nada,

[!!!! FAÇA UMA CÓPIA !!!!](src/cap13-byval.c)

do programa como ele está agora, pois vamos precisar dele depois.

Vamos começar observando o código que gostaríamos que fosse gerado para o novo caso. Usando o mesmo exemplo de anteriormente, queremos que a seguinte chamada:

    FOO(X, Y)

seja traduzida para:

~~~asm
    MOV AX, OFFSET X      ;obtém endereço de X
    PUSH AX               ;coloca na pilha
    MOV AX, OFFSET Y      ;obtém endereço de Y
    PUSH AX               ;coloca na pilha
    CALL FOO              ;chama FOO
~~~

É só uma questão de alterar "param":

~~~c
/* processa um parâmetro de chamada */
void Param()
{
    char name = GetName();
    AsmPushParam(name);
}
~~~

(Note que na passagem por referência, não podemos ter expressões na lista de parâmetros, então "param" lê um identificador diretamente.)

O código em `AsmPushParam()` se encarrega de fazer a passagem do endereço do parâmetro. Repare que se o parâmetro sendo passado for uma variável podemos passar o endereço dela na memória (como vimos no exemplo). Mas e se a chamada estiver sendo feita de outro procedimento e um dos parâmetros de CHAMADA for um dos parâmetros FORMAIS deste procedimento? Simplesmente devemos passar o valor do parâmetro, pois ele em si já é um endereço.

~~~c++
/* coloca parâmetros na pilha */
void AsmPushParam(char name)
{
    switch (SymbolType(name)) {
        case 'v':
            EmitLn("MOV AX, OFFSET %c\n", name);
            AsmPush();
            break;
        case 'f':
            EmitLn("MOV AX, WORD PTR [BP+%d]", AsmOffsetParam(ParamNumber(name)));
            AsmPush();
            break;
        default:
            Abort("Identifier %c cannot be used here!", name);
    }
}
~~~

Na outra extremidade, as referências aos parâmetros formais devem ter um nível de indireção:

~~~c
FOO:
    PUSH BP
    MOV BP, SP
    MOV BX, WORD PTR [BP+6]   ;obtém endereço de A
    MOV AX, WORD PTR [BX]     ;obtém A
    PUSH AX                   ;coloca-o na pilha
    MOV BX, WORD PTR [BP+4]   ;obtém endereço de B
    MOV AX, WORD PTR [BX]     ;obtém B
    POP BX                    ;retira A da pilha
    ADD AX, BX                ;e soma com B
    MOV BX, WORD PTR [BP+6]   ;obtém endereço de A
    MOV WORD PTR [BX], AX     ;armazena resultado em A
    POP BP
    RET
~~~

Tudo isto pode ser tratado por mudanças a `AsmLoadParam()` e `AsmStoreParam()`:

~~~c
/* carrega parâmetro em registrador primário */
void AsmLoadParam(int pos)
{
    int offset = AsmOffsetParam(pos);
    EmitLn("MOV BX, WORD PTR [BP+%d]", offset);
    EmitLn("MOV AX, WORD PTR [BX]");
}

/* armazena conteúdo do registrador primário em parâmetro */
void AsmStoreParam(int pos)
{
    int offset = AsmOffsetParam(pos);
    EmitLn("MOV BX, WORD PTR [BP+%d]", offset);
    EmitLn("MOV WORD PTR [BX], AX");
}
~~~

Isto deve ser o suficiente. Faça alguns testes e veja que estamos gerando código razoávelmente bom. Como você pode ver, o código é difícilmente ótimo, já que precisamos recarregar o endereço toda vez que um parâmetro é necessário. Mas pelo menos isto é consistente com nossa abordagem KISS, de gerar código que funciona. Vamos fazer apenas uma anotação aqui, que este é outro candidato a otimização, e continuar.

Agora aprendemos a processar parâmetros usando passagem por valor e por referência. No mundo real, é claro, gostaríamos de tratar dos dois métodos. Não podemos fazer isto ainda, porém, pois ainda não tratamos de tipos, e isto deve vir antes.

Se só poderemos ter UM método, então é claro que deve ser o nosso velho método FORTRAN de passagem por referência, já que é a única maneira dos procedimentos retornarem algum valor para as rotinas que fizeram a chamada.

Esta, de fato, será uma das diferenças entre TINY e KISS. Na próxima versão de TINY, vamos usar passagem por referência para todos os parâmetros. KISS vai suportar os dois métodos.

> Download da implementação ["por referência"](src/cap13-byref.c).

Variáveis Locais
----------------

Até agora, não falamos nada sobre variáveis locais, e nossa definição de procedimentos atual não permite o uso delas. Nem é necessário falar que isto é uma grande falha da nossa linguagem, a qual deve ser corrigida.

Novamente nos deparamos com uma escolha: alocamento estático ou dinâmico?

Nos antigos programas FORTRAN, as variáveis locais possuiam alocamento estático da mesma forma que as globais. Isto é, cada variável local possuia um nome e um endereço alocado, como qualquer outra variável, e era referenciada por este nome.

Para nós, isto é algo fácil de fazer, usando os mecanismos de alocação que já possuímos. Lembre-se, porém, que variáveis locais podem ter o mesmo nome das globais. Portanto precisamos de alguma técnica para designar nomes únicos para estas variáveis.

A característica do alocamento estático, é claro, é que os dados sobrevivem entre uma chamada de procedimento e o retorno. Quando o procedimento é chamado novamente, os dados ainda estão lá. Isto pode ser uma vantagem em algumas aplicações. Nos dias de FORTRAN costumávamos usar alguns truques como inicializar um flag, para saber se o procedimento estava sendo chamado pela primeira vez e fazer as inicializações da primeira vez quando necessário.

É claro, essa mesma "característica" é também o que torna a "recursão" impossível com armazenamento estático. Qualquer nova chamada ao procedimento iria sobrescrever os dados que já estavam nas variáveis locais.

A alternativa é o armazenamento dinâmico, no qual o armazenamento é alocado na pilha da mesma forma que os parâmetros passados. Nós já possuímos até o mecanismo para fazer isto. Na verdade, as mesmas rotinas que tratam dos parâmetros passados (por valor) na pilha podem ser adaptadas para lidar com variáveis locais também... o código a ser gerado é praticamente o mesmo. Tudo o que temos que fazer é ajustar o ponteiro da pilha (SP) para alocar espaço para as variáveis locais. A alocação dinâmica suporta a recursão inerentemente.

Quando eu comecei a planejar TINY, eu devo admitir que eu tive um preconceito em favor da alocação estática. Isto porque os velhos programas FORTRAN eram muito eficientes... os antigos compiladores FORTRAN produziam código com qualidade que é difícilmente equiparável aos compiladores modernos. Mesmo nos dias de hoje, um dado programa escrito em FORTRAN pode ter um desempenho melhor que o mesmo programa escrito em C ou Pascal, às vezes com uma vantagem grande. (UAU! Eu vou ser xingado por esta declaração!)

Eu sempre acreditei que a razão tinha a ver com as duas diferenças principais entre as implementações de FORTRAN e dos outros: armazenamenteo estático e passagem por referência. Eu sei que armazenamento dinâmico suporta recursão, mas eu sempre achei um pouco peculiar aceitar código mais lento nos 95% dos casos em que não é necessária a recursão, apenas para ter esta característica quando for necessária. A idéia é que, com armazenamento estático, é possível usar endereçamento absoluto ao invés de endereçamento indireto, o que deveria resultar em código mais rápido.

Mais recentemente porém, algumas pessoas me disseram que realmente não há uma grande penalidade na performance associada à alocação dinâmica. Uma instrução

~~~asm
MOV AX, WORD PTR [BP+8]
~~~

tem pouquíssima diferença de tempo de execução em relação a

~~~asm
MOV AX, WORD PTR [X]
~~~

Portanto, estou convencido de que não há uma boa razão para NÃO usar alocação dinâmica.

Já que este uso de variáveis locais se encaixa tão bem no esquema de parâmetros passados por valor, vamos usar aquela versão do tradutor para ilustrá-la. (Eu realmente espero que você tenha mantido [uma cópia](src/cap13-byval.c)!)

A idéia geral é saber quantas variáveis locais existem. Então alteramos o valor do ponteiro da pilha para baixo para abrir espaço para elas. Parâmetros formais são endereçados como deslocamentos positivos em relação ao ponteiro de base (BP) da pilha, e as variáveis locais como deslocamentos negativos. Com muito pouco  trabalho, as rotinas que já criamos podem tomar conta da coisa toda.

Vamos começar criando uma nova variável, `BaseParam`:

~~~c
int BaseParam; /* base para cálculo no deslocamento na pilha */
~~~

Vamos usar esta variável, ao invés de `ParamCount`, para calcular os deslocamentos na pilha. Isto significa alterar a referência a `ParamCount` em `AsmOffsetParam()`:

~~~c
/* calcula deslocamento do parâmetro na pilha */
int AsmOffsetParam(int par)
{
    int offset;

    /* offset = (endereço de retorno + BP) + tamanho do parâmetro * posição relativa */
    offset = 4 + 2 * (BaseParam - par); 

    return offset;
}
~~~

A idéia é que o valor de `BaseParam` será congelado depois que processarmos os parâmetros formais, e não irá mais aumentar quando as variáveis locais forem inseridas na tabela de símbolos. Isto é tratado no final de `FormalList()`:

~~~c
/* processa a lista de parâmetros formais de um procedimento */
void FormalList()
{
    Match('(');
    if (Look != ')') {
        FormalParam();
        while (Look == ',') {
            Match(',');
            FormalParam();
        }
    }
    Match(')');
    NewLine();
    BaseParam = ParamCount;
    ParamCount += 2;
}
~~~

(Adicionamos dois parâmetros fictícios para considerar o endereço de retorno e o antigo valor de BP, que ficam entre os parâmetros formais e as variáveis locais.)

`AsmLoadParam()` e `AsmStoreParam()` também devem ser sofrer uma alteração mínima, quase imperceptível:

~~~c
/* carrega parâmetro em registrador primário */
void AsmLoadParam(int par)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV AX, WORD PTR [BP%+d]", offset);
}

/* armazena conteúdo do registrador primário em parâmetro */
void AsmStoreParam(int par)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV WORD PTR [BP%+d], AX", offset);
}
~~~

Repare na sutil diferença na string: "+%d" fica "%+d". Não podemos manter o sinal de adição, já que podemos ter deslocamentos negativos. Porém, usando-se "%+d" forçamos que o sinal seja mostrado para os valores negativos e para os POSITIVOS também. Agora as rotinas estão certas para os dois casos.

Praticamente o que deve ser feito em seguida é instalar a semântica para a declaração das variáveis ao analisador. As rotinas são muito similares a `Declaration()` e `TopDeclarations()`:

~~~c
/* analisa e traduz uma declaração local de dados */
void LocalDeclaration()
{
    Match('v');
    AddParam(GetName());
    NewLine();
}

/* analisa e traduz declarações locais */
int LocalDeclarations()
{
    int count;

    for (count = 0; Look == 'v'; count++) {
        LocalDeclaration();
    }

    return count;
}
~~~

Repare que `LocalDeclarations()` é uma FUNÇÃO, retornando o número de variáveis locais a `DoProcedure()`.

Em seguida, modificamos `DoProcedure()` para usar esta informação:

~~~c
/* analisa e traduz uma declaração de procedimento */
void DoProcedure()
{
    int countLocals;
    char name;

    Match('p');
    name = GetName();
    AddEntry(name, 'p');
    FormalList();
    countLocals = LocalDeclarations();
    AsmProcProlog(name, countLocals);
    BeginBlock();
    AsmProcEpilog();
    ClearParams();
}
~~~

Repare na diferença da chamada a `AsmProcProlog()`. No lugar de passar o número de bytes, o novo argumento é o número de PALAVRAS (sendo 2 bytes por palavra) a alocar. Aqui estão as novas versões de `AsmProcProlog()` e `AsmProcEpilog()`:

~~~c
/* escreve o prólogo para um procedimento */
void AsmProcProlog(char name, int countLocals)
{
    printf("%c:\n", name);
    EmitLn("PUSH BP");
    EmitLn("MOV BP, SP");
    EmitLn("SUB SP, %d", countLocals * 2);
}

/* escreve o epílogo para um procedimento */
void AsmProcEpilog()
{
    EmitLn("MOV SP, BP");
    EmitLn("POP BP");
    EmitLn("RET");
}
~~~

Isto deve bastar. Adicione estas mudanças e veja como elas funcionam.

Segue o código completo da nossa implementação com variáveis locais (e passagem de parâmetro por valor):

~~~c
{% include_relative src/cap13-locals.c %}
~~~

> Download da implementação com [variáveis locais](src/cap13-locals.c).

Conclusão
---------

Neste ponto, você sabe como compilar declarações e chamadas de procedimentos, com parâmetros passados por referência e por valor. Você já sabe também como é possível tratar de variáveis locais alocadas estática e dinâmicamente. Como é possível observar, a parte difícil não é prover os mecanismos, mas sim decidir quais mecanismos usar. Uma vez tomadas as decisões, o código para traduzir as construções não é realmente tão difícil. Eu não mostrei como tratar da combinação de variáveis locais e parâmetros passados por referência, mas esta é uma extensão simples em relação ao que você já viu. As coisas só ficam um pouco mais bagunçadas, já que precisamos suportar os dois mecanismos ao mesmo tempo. Eu prefiro deixar isto para depois, quando tivermos uma forma de tratar de tipos de variáveis diferentes.

Este será o [próximo capítulo](14_tipos.md), até lá!

{% include footer.md %}