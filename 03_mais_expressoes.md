Parte 3 - Mais expressões
=========================

> **Autor:** Jack W. Crenshaw, Ph.D. (04/08/1988)<br>
> **Tradução e adaptação:** Felipo Soranz (15/05/2002)

No [último capítulo](02_analise_expressoes.md), examinamos as técnicas usadas para analisar e traduzir uma expressão matemática genérica. Terminamos com um analisador simples que pode tratar de uma expressão arbitrariamente complexa, com duas restrições:

- Nenhuma variável era permitida, apenas fatores numéricos
- Os fatores numéricos estavam limitados a dígitos simples

Nesta parte do tutorial, nós vamos nos livrar destas restrições. Nós vamos também estender o que já fizemos para incluir comandos de atribuição e chamadas de função. Lembre-se, porém, que a segunda restrição foi algo que nós mesmos determinamos... uma escolha conveniente de nossa parte, para tornar a vida mais fácil e permitir que nos concentremos nos conceitos fundamentais. Como você verá logo mais, é uma restrição muito fácil de se livrar, portanto, não se importe muito com ela. Nós vamos usar o truque quando ele for apropriado, tendo em mente que podemos descartá-lo assim que estivermos prontos pra isso.

## Variáveis

A maioria das expressões que vamos ver na prática envolvem variáveis, como:

    b * b - 4 * a * c

Nenhum compilador pode ser considerado aceitável sem estar pronto para tratar delas. Felizmente, também é fácil fazer isto.

Lembre-se que em nosso analisador, como ele está agora, há dois tipos de fatores permitidos: constantes inteiras e expressões entre parênteses. Em notação BNF fica assim:

~~~ebnf
    <factor> ::= <number> | '(' <expression> ')'
~~~

O sinal `|` quer dizer "ou", o que claramente significa que ambas as formas são formas válidas para um fator. Lembre-se também que não tivemos problemas em diferenciar qual é qual... o caractere "lookahead" é um "(" em um caso, e um dígito no outro.

Provavelmente não vai ser uma surpresa tão grande saber que uma variável é apenas outro tipo de fator. Então, nós vamos estender a regra BNF acima para que fique assim:

~~~ebnf
    <factor> ::= <number> | '(' <expression> ')' | <variable>
~~~

Novamente, não há ambiguidades: se o caractere lookahead é uma letra, temos uma variável; se é um dígito, temos um número. Antes, quando traduzíamos o número, nós apenas fizemos o código ler o valor, como um dado imediato, em AX. Agora faremos o mesmo, mas desta fez vamos carregar uma variável.

No assembly 80x86 podemos fazer o seguinte para ler uma variável:

~~~asm
    MOV AX, [VAR]
~~~

>**Nota de tradução:** Estou considerando aqui uma abordagem simplística. Em assembly para 80x86 sempre acabamos nos deparando com segmentos de memória e outras questões de endereçamento. No próprio 68000 que era a plataforma do autor original havia estes tipos de complicações.

Sabendo como fazer agora, vamos alterar o código de Factor():

~~~c
/* Analisa e traduz um fator */
void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (isalpha(Look))
        EmitLn("MOV AX, [%c]", GetName());
    else
        EmitLn("MOV AX, %c", GetNum());
}
~~~

Eu já disse anteriormente quão fácil é adicionar extensões ao compilador, por causa da forma como ele está estruturado. Você pode notar que isto ainda é verdadeiro. Desta vez nos custou apenas 2 linhas de código. Note, também, como a estrutura "if-else-else" é condizente com a equação em sintaxe BNF acima.

OK, teste esta nova versão do compilador. Não machucou muito, machucou?

## Funções

Há apenas mais um tipo comum de fator suportado pela maioria das linguagens: a chamada de função. É ainda muito cedo pra tratarmos de funções por enquanto, por que ainda não tratamos o problema de passagem de parâmetros. Além disso, uma linguagem "real" incluiria um mecanismos para suportar mais de um tipo, um dos quais deveria ser um tipo função. Nós ainda não chegamos lá de qualquer forma. Mas mesmo assim, eu gostaria de lidar com funções por algumas razões. Primeiro, isto permite que nós finalmente melhoremos o analisador para que ele tome uma forma muito próxima de sua versão final. Em segundo, isto nos mostra um novo problema que vale a pena ser comentado.

Até agora, nós fomos capazes de escrever o que é chamado de "analisador preditivo" (predictive parser). O que significa que em qualquer ponto, nós podemos saber exatamente, apenas olhando no caractere "lookahead" (avançado) corrente, o que fazer em seguida. Não é o caso quando adicionamos funções. Toda linguagem tem algumas regras de nomenclatura para o que constitui um identificador válido. No momento, a nossa regra é que ele deve ser uma única letra de A a Z. O problema é que um nome de variável e um nome de função obedecem a mesma regra. Então como podemos dizer qual é qual? Uma forma é obrigar que ambos sejam declarados antes de serem usados. A linguagem Pascal usa esta abordagem. A outra é que podemos obrigar que uma função seja sempre seguida por uma lista de parâmetros (possivelmente vazia). Esta é a regra usada em C.

Como nós não temos ainda um mecanismo para declarar tipos, vamos usar a regra da linguagem C por enquanto. Como nós ainda não temos um mecanismo para lidar com parâmetros, nós só podemos lidar com listas vazia, de forma que nossas chamadas vão ter a forma:

    x()

Como nós não estamos tratando de listas de parâmetro ainda, não há nada a ser feito a não ser chamar a função, então nós só temos que emitir uma chamada de função (CALL) ao invés de um MOV.

Agora que há duas possibilidades no `if (isalpha(Look))` do teste em `Factor()`, vamos tratá-las em uma rotina separada. Modifique `Factor()` assim:

~~~c
/* Analisa e traduz um fator */
void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if(isalpha(Look))
        Ident();
    else
        EmitLn("MOV AX, %c", GetNum());
}
~~~

E adicione a seguinte função:

~~~c
/* Analisa e traduz um identificador */
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
~~~

OK, compile e teste esta versão. Ela analisa todas as expressões válidas? Ela avisa sobre as expressões mal-formadas?

O importante a notar é que mesmo não tento mais um analisador preditivo, praticamente não há qualquer complicação adicionada à abordagem descendente recursiva que estamos usando. No momento em que `Factor()` encontra um identificador (uma letra), ele não sabe se é um nome de variável ou de uma função, e na verdade ele nem se importa com isso. Ele simplesmente passa a escolha para `Ident()` e deixa que a rotina descubra por si só. `Ident()` por sua vez, simplesmente pega o identificador e então lê mais um caractere para decidir com que tipo de identificador está lidando.

Mantenha esta abordagem em mente. É um conceito muito poderoso, e deve ser usado sempre que você encontrar uma situação ambígua que requer uma "espiada" mais adiante. Mesmo que você deva olhar vários tokens adiante, o princípio ainda vai funcionar.

## Mais sobre tratamento de erros

Como estamos tratando de filosofia agora, há outro problema importante a ser tratado: tratamento de erros. Note que apesar do analisador rejeitar corretamente (quase) toda expressão mal-formada que nós experimentarmos, com uma mensagem de erro compreensível, nós não tivemos que trabalhar muito para que isso acontecesse. Na verdade, no analisador inteiro há apenas 2 chamadas de erro à rotina expect. Mesmos estas não são necessárias... se você olhar cuidadosamente `Term()` e `Expression()`, vai notar que não é possível que o programa alcance aquelas instruções. Eu as coloquei ali antes, apenas como precaução, mas elas não são mais necessárias. Por que você não as apaga agora?

Então, como conseguimos este ótimo tratamento de erro virtualmente de graça? Nós cuidadosamente evitamos ler caracteres diretamente usando `NextChar()`. Ao invés disso, nós confiamos no tratamento de erros de `GetName()`, `GetNum()` e `Match()` para que façam todo o tratamento por nós. Leitores espertos vão notar que algumas chamadas a `Match()` (por exemplo, as que estão em `Add()` e `Subtract()`) são desnecessárias... nós já sabemos qual o caractere quando chegamos lá... mas estas chamadas mantém uma certa simetria quando as mantemos lá, e a regra geral de se usar `Match()` ao invés de `NextChar()` é uma boa regra.

Eu mencionei um "quase" mais acima. Há um caso em que nosso tratamento de erro deixa um pouco a desejar. Até aqui não dissemos ao nosso analisador como é um "fim-de-linha", ou o que fazer com espaços separadores. Então, um caractere de espaço (ou qualquer outro caractere não reconhecido pelo analisador) simplesmente faz com que o analisador pare, ignorando caracteres não reconhecidos.

Pode-se dizer que isto é um comportamento razoável neste ponto. Em um compilador "de verdade", há normalmente outros comandos em sequência. Caracteres que não foram tratados como parte da expressão são usados ou rejeitados como parte da próxima expressão.

Mas isto é algo fácil de arrumar, mesmo sendo apenas temporário. Tudo o que temos que fazer é assegurar que uma expressão deve terminar com um caractere de "fim-de-linha".

Para entender do que estou falando, tente a seguinte linha:

        1+2 <ESPAÇO> 3+4

Viu como o espaço foi tratado como um terminador? Para fazer com que o compilador trate isto corretamente, adicione a linha:

~~~c
    if (Look != '\n')
        Expected("NewLine");
~~~

no programa principal, logo após a chamada a `Expression()`. Isto vai pegar tudo o que foi deixado de lado na entrada.

Como de costume, recompile o programa e verifique se ele faz o que deveria.

## Comandos de atribuição

Neste ponto nós temos um analisador que funciona muito bem. Eu gostaria de ressaltar que o analisador não tem mais do que 100 linhas de código em C, sem contar o código referente ao "berço". No meu caso, o código executável ficou em torno de 20 Kbytes compilando com `gcc -s`. Nada mal, considerando que nós praticamente não estamos fazendo esforço algum pra economizar no tamanho do código fonte nem no código objeto. Nós apenas nos mantivemos dentro do princípio KISS.

É claro que compilar uma expressão não é muito útil sem ter o que fazer com ela depois. Expressões NORMALMENTE (mas nem sempre) aparecem em comandos de atribuição, na forma:

    <ident> = <expression>

Estamos a apenas um passo de analisar um comando de atribuição, então vamos dar este último passo. Adicione esta nova rotina:

~~~c
/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;
    name = GetName();
    Match('=');
    Expression();
    EmitLn("MOV [%c], AX", name);
}
~~~

Note novamente que o código combina com a BNF, e também que a checagem de erros foi transparente, tratada por `GetName()` e `Match()`.

Note a simetria entre a atribuição do valor de AX a uma variável e o seu respectivo carregamento.

Agora, troque a chamada a `Expression()`, no programa principal, por uma chamada a `Assignment()`. É tudo que precisa ser feito.

Caramba! Nós estamos realmente compilando comandos de atribuição. Se este fosse o único tipo de comando em uma linguagem, tudo o que teríamos que fazer é colocar isto em um laço de repetição e nós teríamos um compilador praticamente completo!

Bom, é claro que não é o único tipo. Há também outros itens como comandos de controle (IFs e estruturas de repetição), funções e procedimentos, declarações, etc. Mas alegre-se. As expressões aritméticas com as quais estamos lidando constituem uma das partes mais difíceis em uma linguagem. Comparado com o que já fizemos, estruturas de controle vão ser mais fáceis. Eu vou cuidar delas na quinta parte. Todos os outros comandos vão seguir a mesma lógica, enquanto nós nos lembrarmos do princípio KISS.

## Tokens de mais de um caractere

Nesta série toda, eu procuro restringir tudo o que fazemos para usar tokens de um único caractere, durante todo o tempo assegurando-lhe que não seria difícil estendê-los para tokens de múltiplos caracteres. Eu não sei se você acreditou em mim ou não... eu na verdade não culparia você se você tivesse sido um pouco cético. Eu vou continuar usando esta abordagem nas próximas partes, por que isso ajuda a manter a complexidade de lado, mas no momento, eu gostaria de melhorar esta parte do analisador, apenas pra lhe provar quão fácil realmente é. No processo, nós também vamos permitir espaços em branco como separadores. Antes de você fazer as próximas mudanças, **grave a versão atual do analisador com [outro nome](src/cap03-single.c)**. Eu tenho outros usos pra ele no próximo capítulo e nós vamos continuar trabalhando com a versão de um único caractere.

A maioria dos compiladores separa o tratamento da entrada em um módulo separado chamado analisador léxico (lexical scanner). A ideia é que o analisador léxico trate de toda a entrada, caractere por caractere, e retorne apenas as unidades separadas (tokens). Talvez futuramente nós queiramos fazer algo assim também, mas no momento não é necessário. Nós podemos lidar com tokens multi-caractere com apenas algumas modificações locais em `GetName()` e `GetNum()` e pequenas alterações em outras partes.

A definição usual de um identificador é que o primeiro caractere deve ser uma letra, mas o resto pode ser alfanumérico (letras ou números). Para tratar disso vamos usar a função `isalnum()`.

Altere a função `GetName()` da seguinte forma:

~~~c
/* Recebe o nome de um identificador */
void GetName(char *name)
{
    int i;
    if (!isalpha(Look))
        Expected("Name");
    for (i = 0; isalnum(Look); i++) {
        if (i >= MAXNAME)
            Abort("Identifier too long!");
        name[i] = toupper(Look);
        NextChar();
    }
    name[i] = '\0';
}
~~~

Da mesma forma, altere GetNum():

~~~c
/* Recebe um número inteiro */
void GetNum(char *num)
{
    int i;
    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look); i++) {
        if (i >= MAXNUM)
            Abort("Integer too long!");
        num[i] = Look;
        NextChar();
    }
    num[i] = '\0';
}
~~~

Declare as definições para os valores de MAXNAME e MAXNUM. Eu coloquei estes valores:

~~~c
#define MAXNAME 30
#define MAXNUM 5
~~~

Além destas funções você deve alterar também estas outras que usam GetName() e GetNum():

~~~c
/* Analisa e traduz um identificador */
void Ident()
{
    char name[MAXNAME+1];
    GetName(name);
    if (Look == '(') {
        Match('(');
        Match(')');
        EmitLn("CALL %s", name);
    } else
        EmitLn("MOV AX, [%s]", name);
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name[MAXNAME+1];
    GetName(name);
    Match('=');
    Expression();
    EmitLn("MOV [%s], AX", name);
}

/* Analisa e traduz um fator */
void Factor()
{
    char num[MAXNUM+1];
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if(isalpha(Look)) {
        Ident();
    } else {
        GetNum(num);
        EmitLn("MOV AX, %s", num);
    }
}
~~~

>**Nota de tradução:** Em Pascal há pouca distinção entre um caractere e uma string e as operações com ambos são muito simples e semelhantes. Mas em linguagem C as strings recebem um "tratamento especial" e por isso a alteração teve uma repercussão "um pouco maior" no código em C, do que na versão original em Pascal. No entanto, eu espero que você tenha se convencido de que as alterações não foram tão grandes assim.

De forma surpreendente, isto é virtualmente tudo o que deve ser feito no compilador! As declarações de name em `Ident()` e `Assignment()` foram trocadas de `char name` para `char name[MAXNAME+1]` pois precisamos de uma string e não apenas um caractere. O "+1" da declaração é para acomodar o terminador da string '\0'. (Eu coloquei as coisas de uma forma que o tamanho dos nomes deve ser limitado, apenas para não complicar mais o código com alocação dinâmica de memória, etc. No entanto, muitos montadores, ou "assemblers", limitam o tamanho de qualquer forma.) Note que em todo chamada a printf, o "%c" foi trocado por "%s", pois estamos tratando de strings e não caracteres. Faça estas mudanças, recompile e teste. Experimente isto e veja o resultado:

    nota=(prova1+prova2*2)/3

AGORA você acredita que as mudanças foram simples?

## Espaços em branco

Antes de deixarmos este analisador por enquanto, vamos lidar com o problema de um espaço em branco. Como está agora, o analisador vai reclamar (ou simplesmente terminar) se um espaço em branco for colocado em algum lugar da entrada. Não é um comportamento muito amigável. Então vamos melhorar o analisador para eliminar esta última restrição.

A chave para tratar facilmente de espaços em branco é criar uma regra simples de como o analisador deve tratar a entrada, e usar esta regra em todo lugar. Até agora, pelo fato do espaço em branco não ser permitido, fomos capazes de assumir que depois de cada ação do analisador, o caractere de lookahead continha o próximo caractere útil, de forma que pudéssemos testá-lo imediatamente. Nosso projeto foi baseado neste princípio.

Parece ser uma regra boa, portanto é a que vamos usar. Isto significa que toda rotina que avança na entrada deve pular os espaços em branco e deixar o próximo caractere não-branco em Look. Felizmente, pelo fato de termos cuidadosamente usado GetName(), GetNum() e Match() para a maioria do processo de entrada, apenas estas 3 rotinas (além de Init()) precisam ser mudadas.

Precisamos de uma rotina para "engolir" caracteres de espaço em branco, até que encontre um que não seja.

~~~c
/* Pula caracteres de espaço */
void SkipWhite()
{
    while (Look == ' ' || Look == '\t')
        NextChar();
}
~~~

Agora adicione chamadas a SkipWhite() a Match(), GetName() e GetNum() conforme abaixo:

~~~c
/* Verifica se entrada combina com o esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
    SkipWhite();
}

/* Recebe o nome de um identificador */
void GetName(char *name)
{
    int i;
    if (!isalpha(Look))
        Expected("Name");
    for (i = 0; isalnum(Look); i++) {
        if (i >= MAXNAME)
            Abort("Identifier too long!");
        name[i] = toupper(Look);
        NextChar();
    }
    name[i] = '\0';
    SkipWhite();
}

/* Recebe um número inteiro */
void GetNum(char *num)
{
    int i;
    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look); i++) {
        if (i >= MAXNUM)
            Abort("Integer too long!");
        num[i] = Look;
        NextChar();
    }
    num[i] = '\0';
    SkipWhite();
}
~~~

Finalmente, vamos pular também os espaços iniciais em Init().

~~~c
/* Inicialização do compilador */
void Init()
{
    NextChar();
    SkipWhite();
}
~~~

Faça estas mudanças e recompile o programa. Teste o programa como sempre para ver se ele funciona apropriadamente. Experimente:

    nota = (prova1 + prova2*2) / 3

desta vez com espaços!

Como nós fizemos uma série de mudanças desta vez, vou reproduzir o código inteiro do nosso pequeno compilador:

~~~c
{% include_relative src/cap03-multi.c %}
~~~

> Download do código-fonte: [cap03-single.c](src/cap03-single.c) / [cap03-multi.c](src/cap03-multi.c)

Agora o analisador está completo. Ele tem todas as características que se pode colocar em um "compilador de uma só linha". Salve-o num lugar seguro. Da próxima vez vamos passar para um novo assunto, mas ainda vamos falar sobre expressões por um bom tempo. No [próximo capítulo](04_interpretadores.md) eu pretendo falar um pouco sobre interpretadores ao invés de compiladores, e mostrar como a estrutura de um analisador muda um pouco conforme mudamos o tipo de ação a ser tomada. O conhecimento que adquirirmos ali nos será muito útil futuramente, mesmo que você não tenha interesse em interpretadores. Até a próxima!

{% include footer.md %}