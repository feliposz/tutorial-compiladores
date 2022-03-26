Parte 14: Tipos
===============

> **Autor:** Jack W. Crenshaw, Ph.D. (26/05/1990)<br>
> **Tradução e adaptação:** Felipo Soranz (31/05/2002)

No último capítulo ([Parte 13: Procedimentos](13_procedimentos.md)) eu mencionei que naquela parte e nesta também, iríamos cobrir as duas características que tendem a separar as linguagens de brinquedo das linguagens reais e usáveis. Cobrimos chamadas de procedimentos no último capítulo e desta vez vamos tratar da segunda parte.

Neste capítulo, vamos falar sobre como lidar com tipos de dados diferentes. Como eu fiz no último segmento, eu NÃO vou incorporar estas característica diretamente no compilador TINY por enquanto. Ao invés disto, vou usar a mesma abordagem que funcionou tão bem para nós no passado: usar apenas fragmentos do analisador e tokens de um só caracter. Como de costume, isto permite que tratemos diretamente do núcleo do problema, sem ter que nos preocuparmos em arrumar código desnecessário. Já que a maioria dos problemas em tratar de tipos múltiplos ocorrem em operações aritméticas, é aí que iremos concentrar nosso foco.

Algumas palavras de aviso: 

1. Há alguns tipos que eu NÃO vou cobrir neste capítulo. Aqui, nós vamos APENAS falar dos tipos simples e pré-definidos. Não vamos tratar nem sequer de vetores, ponteiros ou sequências; mas talvez tratemos deles mais adiante.

2. Também não vamos discutir tipos definidos pelo usuário. Isto não virá a acontecer até muito mais tarde, pela simples razão de eu não estar convencido de que tipos definidos pelo usuário pertecem a uma linguagem que denominamos KISS. Em capítulos futuros, eu pretendo cobrir pelo menos os conceitos gerais de tipos definidos pelo usuário, registros (*record/struct*), etc., apenas para que a série esteja completa. Mas se eles vão entrar ou não na linguagem KISS ainda é uma questão em aberto.

3. Eu devo avisá-lo: o que estamos a ponto de fazer PODE adicionar complicação extra tanto ao analisador quanto ao gerador de código. Tratar de variáveis de tipos diferentes não é tão complicado assim, a complexidade aparece quando você adiciona as regras para a conversão entre tipos. Em geral, você pode tornar o compilador tão simples ou tão complexo quanto quiser, dependendo da maneira como você define as regras de conversão de tipo. Mesmo que você decida não permitir qualquer tipo de conversão (como em Ada, por exemplo) o problema ainda permanece, e o problema está na matemática. Quando você multiplica dois números curtos, por exemplo, você pode obter resultados longos.

Eu tentei abordar este problema com muito cuidados, em uma tentativa de mantê-lo simples. Mas não é possível evitar a complexidade por completo. Como tem acontecido com frequência, acabamos tendo que escolher entre manter a qualidade de código e evitar a complexidade, e como de costume eu procuro optar pela abordagem mais simples.

O Que Vem Em Seguida?
---------------------

Antes de nos aprofundarmos no tutorial, eu acho que você gostaria de saber para onde estamos indo... especialmente depois de tanto tempo após o [último capítulo](13_procedimentos.md).

Eu não estive ocioso neste meio-tempo. O que eu tenho feito é reorganizar o compilador em si em módulos separados. Um dos problemas que eu encontrei é que conforme cobrimos novas áreas e adicionamos novas características a TINY, ele se torna cada vez maior. Eu percebi alguns capítulos atrás que isto estava causando problemas, e é por isso que eu voltei a usar apenas fragmentos do compilador no capítulo anterior e neste também. O problema é que simplesmente parece inútil ter que reproduzir o código para, digamos, processamento booleano de Ou-exclusivo, quando nosso assunto é passagem de parâmetros.

A maneira óbvia de tratar do problema é separar o compilador em módulos compiláveis separadamente. Desta forma podemos esconder código realmente complexo (como a analise aritmética e booleana completa) em uma única unidade, e simplesmente anexá-lo sempre que necessário. Desta forma, o único código que terei que reproduzir nestes capítulos será o código diretamente relacionado à discussão.

Em todo caso, a idéia nos próximos capítulos da série, é apresentar um conjunto completo de módulos, e compiladores funcionais completos também. O plano, de fato, é ter TRÊS compiladores: Um para a versão de um caracter de TINY (para usar nas experiências), um para TINY completo, e outro para KISS. Neste ponto eu isolei as diferenças entre TINY e KISS, que são as seguintes:

    - TINY terá suporte a apenas dois tipos de dados: o caracter e o inteiro de 16-bits. Eu posso tentar fazer algo com strings também, já que sem elas um compilador não seria de grande utilidade. KISS vai suportar todos os tipos simples usuais, incluindo vetores e até mesmo números de ponto flutuante.

    - TINY só terá duas construções de controle, o IF e o WHILE. KISS vai suportar um conjunto bem mais rico de construções, incluindo uma que não discutimos ainda... o comando CASE.

    - KISS terá suporte a módulos compilados separadamente.

Um aviso: os módulos vão continuar sendo escritos para suportar Assembly para processadores 80x86, usando modelamento real de memória e 16-bits. Porém, em todos os programas a geração do código será devidamente encapsulada em uma única unidade, para que qualquer estudante dedicado possa facilmente portar a saída para o processador ou sintaxe que preferir. Esta tarefa é deixada como um "exercício para o estudante".

Mas chega de falação. Vamos começar com o estudo de tipos. Como eu disse anteriormente, vamos tratar disto da mesma forma que no capítulo anterior: fazendo pequenos experimentos usando tokens de um só caracter.

A Tabela de Símbolos
--------------------

Deve parecer evidente que, se vamos tratar de variáveis de tipos diferentes, vamos ter que armazenar que tipos são estes. O lugar ideal para isto obviamente é a tabela de símbolos, e nós já a usamos para fazer a distinção, por exemplo, entre variáveis locais e globais, e entre variáveis e procedimentos.

A estrutura para a tabela de símbolos para tokens de um só caracter é particularmente simples, e nós já fizemos uso dela diversas vezes antes. Para tratar dela, vamos emprestar algumas rotinas que já usamos antes.

Primeiro, temos que declarar a tabela de símbolos em si:

~~~c
#define SYMBOLTABLE_SIZE 26
char SymbolTable[SYMBOLTABLE_SIZE]; /* Tabela de símbolos */
~~~

Em seguida, temos que nos certificar de que ela foi inicializada na rotina "init":

~~~c
/* Inicialização do compilador */
void Init()
{
        int i;

        for (i = 0; i < SYMBOLTABLE_SIZE; i++)
                SymbolTable[i] = '?';

        NextChar();
}
~~~

Nós realmente não precisamos da próxima rotina, mas ela será útil para depuração. Tudo o que ela faz é exibir todo o conteúdo da tabela de símbolos:

~~~c
/* Exibe a tabela de símbolos */
void DumpTable()
{
    int i;

    printf("Symbol table dump:\n");

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
            printf("%c = %c\n", i + 'A', SymbolTable[i]);
}
~~~

Realmente não importa muito onde você coloca esta rotina... eu pretendo colocar todas as rotinas da tabela de símbolo juntas.

Se você é o tipo cuidadoso (como eu sou), vai preferir começar com um programa de teste que não faz nada a não ser inicializar, e em seguida exibir a tabela. Apenas para ter certeza de que estamos sintonizados, vou reproduzir o programa completo abaixo, com as novas rotinas. Repare que esta versão tem suporte a espaços em branco:

~~~c
{% include_relative src/cap14-base.c %}
~~~

> Download do código-fonte [cap14-base.c](src/cap14-base.c)

OK, execute este programa. Você deve obter como saída todas as letras do alfabeto (potenciais identificadores), cada um seguido por um sinal de interrogação. Nada muito excitante, mas é um começo.

É claro, em geral nós só queremos ver os tipos das variáveis que já foram definidas. Podemos eliminar as outras inserindo uma condição em "dumpTable". Altere o laço FOR para que fique:

~~~c
    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        if (SymbolTable[i] != '?')
            printf("%c = %c\n", i + 'A', SymbolTable[i]);
~~~

Agora, execute o programa novamente. O que aconteceu?

Bem, isto é mais chato que antes até! Não houve qualquer saída, já que neste ponto nenhum dos nomes foi declarado. Podemos melhorar as coisas inserindo alguns comandos para declarar algumas entradas no programa principal. Tente algo assim:

~~~c
    SymbolTable[0] = 'a';
    SymbolTable[5] = 'p';
    SymbolTable[10] = 'x';
~~~

Desta vez, quando você executar o programa, você deve obter uma saída mostrando a tabela de símbolos funcionando corretamente.

Adicionando Entradas
--------------------

É claro que escrever diretamente na tabela é uma prática muito pobre, e não é algo que vai nos ajudar muito depois. Ao mesmo tempo, sabemos que vamos precisar testar a tabela, para ter certeza que não estamos redeclarando uma variável que já está em uso (fácil de fazer com apenas 26 opções!). Para tratar disto tudo, entre com as seguintes novas rotinas:

~~~c
/* Retorna o tipo de um identificador */
char SymbolType(char name)
{
        return SymbolTable[name - 'A'];
}

/* Verifica se "name" consta na tabela de símbolos */
char InTable(char name)
{
        return (SymbolTable[name - 'A'] != '?');
}

/* Avisa a respeito de um identificador desconhecido */
void Duplicate(char name)
{
    fprintf(stderr, "Error: Duplicated identifier %c\n", name);
    exit(1);
}

/* Verifica se um identificador já foi declarado */
void CheckDuplicate(char name)
{
        if (InTable(name))
                Duplicate(name);
}

/* Adiciona nova entrada à tabela de símbolos */
void AddEntry(char name, char type)
{
    CheckDuplicate(name);
        SymbolTable[name - 'A'] = type;
}
~~~

Agora altere as três linhas no programa principal para:

~~~c
    AddEntry('A', 'a');
    AddEntry('F', 'p');
    AddEntry('K', 'x');
~~~

e execute o programa novamente. Funcionou? Então temos as rotinas da tabela de símbolos necessárias para suportar nosso trabalho com tipos. Na próxima seção, vamos começar a usá-las.

Alocando Espaço de Armazenamento
--------------------------------

Em outros programas como este, incluindo o compilador TINY em si, já tratamos do problema de declarar variáveis globais, e o código gerado para elas. Vamos construir uma versão simplificada de um "compilador" aqui, cuja única função é permitir que declaremos variáveis. Lembre-se, a sintaxe para uma declaração é:

    <data decl> ::= VAR <identifier>

Novamente, podemos emprestar a maior parte do código dos programas anteriores. O que segue é uma versão simplificada das versões anteriores destas rotinas. Elas estão muito simplificadas já que eu eliminei coisas como listas de variáveis e inicializadores. Na rotina "allocvar", repare que a chamada a "addSymbol" também vai cuidar de verificar declarações duplicadas:

/* Avisa a respeito de uma palavra-chave desconhecida */
void Unrecognized(char name)
{
        fprintf(stderr, "Error: Unrecognized keyword %c\n", name);
        exit(1);
}

/* Aloca espaço de armazenamento para variável */
void AsmAllocVar(char name)
{
        AddEntry(name, 'v');
        printf("%c: dw 0\n", name);
}

/* Analiza e traduz uma declaração de variável */
void Declaration()
{
        Match('v');
        AsmAllocVar(GetName());
}

/* Analiza e traduz as declarações globais */
void TopDeclarations()
{
        while (Look != '.') {
                switch (Look) {
                  case 'v':
                        Declaration();
                        break;
                  default:
                        Unrecognized(Look);
                        break;
                }
                NewLine();
        }
}

Agora, no programa principal, adicione uma chamada a "topDeclarations" e execute o programa. Tente alocar algumas variáveis, e repare o código sendo gerado. Isto é algo velho e conhecido pra você, portanto o resultado deve ser familiar. Repare no código de "topDeclarations". O programa é terminado por um ponto (".").

Enquanto estiver testanto, tente declarar duas variáveis com o mesmo nome, e verifique que o analisador avisa a respeito do erro.

Declarando Tipos
----------------

Alocar armazenamento de tamanhos diferentes é simples como modificar o procedimento "topDeclarations" para reconhecer mais de uma palavra-chave. Há um certo número de decisões a serem tomadas aqui, em termos de como a sintaxe deve ser, etc., mas por enquanto eu vou deixar de lado todos os problemas e simplesmente declarar que a nossa sintaxe será:

~~~ebnf
    <data-decl> ::= <typename> <identifier>
~~~

onde:

~~~ebnf
    <typename> ::= BYTE | WORD | LONG
~~~

Podemos criar o código para tratar destas modificações com alterações simples. Nas rotinas abaixo, repare que eu separei a parte de geração de código de `asmAllocVar` e `AllocVar()`. Isto vai manter nosso desejo de encapsular o código dependente de máquina do compilador.

~~~c
/* Gera código para armazenamento de variável */
void AsmAllocVar(char name, char type)
{
    int btype; /* Tamanho em bytes */

    switch (type) {
        case 'b':
        case 'w':
            btype = type;
            break;
        case 'l':
            btype = 'd';
            break;
    }
    printf("%c d%c 0\n", name, btype);
}

/* Aloca espaço de armazenamento para variável */
void AllocVar(char name, char type)
{
    AddEntry(name, type);
    AsmAllocVar(name, type);
}

/* Analiza e traduz uma declaração de variável */
void Declaration()
{
    char type = Look;
    NextChar();
    AllocVar(GetName(), type);
}

/* Analiza e traduz as declarações globais */
void TopDeclarations()
{
    while (Look != '.') {
        switch (Look) {
            case 'b':
            case 'w':
            case 'l':
                Declaration();
                break;
            default:
                Unrecognized(Look);
                break;
        }
        NewLine();
    }
}
~~~

Faça as mudanças nestas rotinas, altere o programa principal para chamar `TopDeclarations()` antes de `DumpTable()` e faça um teste. Use os caracteres "b", "w" e "l" no lugar das palavras-chave (eles devem estar em letras minúsculas por enquanto). Você verá que em cada caso, estamos armazenando o tamanho de armazenamento apropriado. Repare através da tabela de símbolos exibida que os tamanhos (tipos) estão armazenados para uso posterior. Que uso posterior? Bem, este é o assunto do resto deste capítulo.

Atribuições
-----------

Agora que podemos declarar variáveis de tipos e tamanhos diferentes, chegamos ao momento em que deveríamos fazer algo de útil com elas. Pra começar, vamos apenas tentar carregá-las no registrador primário (AX no nosso caso). Faz sentido usar a mesma idéia de `AllocVar()`, isto é, fazer uma rotina de carregamento capaz de ler tamanhos diferentes. Também gostaríamos de manter o código dependente de máquina isolado. A rotina de carregamento é algo assim:

~~~c
/* Gera código para carregar variável de acordo com o tipo */
void AsmLoadVar(char name, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV AL, [%c]", name);
            break;
        case 'w':
            EmitLn("MOV AX, [%c]", name);
            break;
        case 'l':
            EmitLn("MOV DX, [%c+2]", name);
            EmitLn("MOV AX, [%c]", name);
    }
}
~~~

(Repare que como estamos usando apenas registradores de 16-bits, precisamos usar um par de registradores para representar números de 32-bits, como os números do tipo "long". No caso, estou usando o par DX:AX, pois é o padrão, no caso de assembly 80x86 16-bits. Se usássemos os registradores de 32-bits, seria possível usar um só registrador, no caso EAX, para representar os números. Isto é relativamente fácil de modificar e é deixado como um "exercício para o estudante".)

Antes de mais nada, precisamos ter certeza de que o tipo com o qual estamos lidando é um tipo carregável. Isto parece ser o trabalho de mais um reconhecedor:

~~~c
/* Reconhece um tipo de variável válido */
int IsVarType(char c)
{
    return (c == 'b' || c == 'w' || c == 'l');
}
~~~

Em seguida, seria bom ter uma rotina que captura o tipo de uma variável da tabela ao mesmo tempo que verifica se o tipo é válido:

~~~c
/* Avisa a respeito de um identificador que não é uma variável */
void NotVar(char name)
{
    Abort("'%c' is not a variable", name);
}

/* Pega o tipo da variável da tabela de símbolos */
char VarType(char name)
{
    char type;

    type = SymbolType(name);
    if (!IsVarType(type))
        NotVar(name);

    return type;
}
~~~

Armado com estas ferramentas, o procedimento para efetuar o carregamento destas variáveis é trivial:

~~~c
/* Carrega variável */
void LoadVar(char name)
{
    AsmLoadVar(name, VarType(name));
}
~~~

(Nota: Eu sei que isto tudo é um tanto ineficiente. Em um programa de produção, provavelmente faríamos algo para evitar ter de aninhar tantas chamadas de procedimento assim. Não se preocupe com isto. Isto é um EXERCÍCIO, lembra-se? É mais importante fazer a coisa certa e entendê-la, ao invés de obter a resposta errada, rapidamente. Se o compilador já estiver completo e você estiver infeliz com seu desempenho, sinta-se livre para voltar e otimizá-lo!)

Seria uma boa idéia testar o programa a esta altura. Como ainda não temos um procedimento para tratar de atribuições ainda, simplesmente adicionamos as linhas:

~~~c
    LoadVar('A');
    LoadVar('B');
    LoadVar('C');
    LoadVar('X');
~~~

ao programa principal. Portanto, depois da seção de declarações terminar, será gerado código para os carregamentos. Você pode fazer diversos testes com isto, e tentar diferentes combinações de declarações para A, B, C e X e ver como erros são tratados.

Tenho certeza que você não ficaria surpreso em saber que o código para armazenar variáveis é muito parecido com aquele para carregá-las. Os procedimentos necessários são mostrados a seguir:

~~~c
/* Gera código para armazenar variável de acordo com o tipo */
void AsmStoreVar(char name, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV [%c], AL", name);
            break;
        case 'w':
            EmitLn("MOV [%c], AX", name);
            break;
        case 'l':
            EmitLn("MOV [%c+2], DX", name);
            EmitLn("MOV [%c], AX", name);
    }
}

/* Armazena variável */
void StoreVar(char name)
{
    AsmStoreVar(name, VarType(name));
}
~~~

Você pode testar isto da mesma forma que os carregamentos.

Agora, é claro, que é um passo relativamente pequeno usar isto para tratar comandos de atribuição. O que vamos fazer é criar uma versão especial do procedimento "block" para suportar apenas comandos de atribuição, e também uma versão simplificada de `Expression()` que suporta apenas variáveis simples como expressões válidas. Aqui estão:

~~~c
/* Analisa e traduz uma expressão */
void Expression()
{
    LoadVar(GetName());
}

/* Analisa e traduz uma atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    Expression();
    StoreVar(name);
}

/* Analisa e traduz um bloco de comandos */
void Block()
{
    while (Look != '.') {
        Assignment();
        NewLine();
    }
}
~~~

(Vale a pena notar que as novas rotinas que nos permitem manipular tipos são mais simples até do que já fizemos antes. Isto acontesse graças à encapsulação da geração de código dos procedimentos.)

Há um pequeno problema. Antes, usávamos o ponto final para sair da rotina `TopDeclarations()`. Agora este é o caracter errado... ele é usado para terminar `Block()`. Em programas anteriores, usamos o símbolo BEGIN (abreviado como "b") para sair. Mas agora ele está sendo usado como um símbolo para tipo.

Há uma solução, que não é das melhores, mas que é simples de fazer. Vamos usar um "B" MAIÚSCULO para representar BEGIN. Então, troque o caracter no laço de `TopDeclarations()`, de "." para "B" e tudo estará bem. Agora, podemos alterar o programa principal para que fique:

~~~c
/* Programa principal */
int main()
{
    Init();
    TopDeclarations();
    Match('B');
    NewLine();
    Block();
    DumpTable();

    return 0;
}
~~~

(Agora repare que eu precisei espalhar algumas chamadas a "newLine" para tratar da questão da quebra de linha.)

Certo, execute o programa. Tente esta entrada:

~~~
    ba        { byte a }   *** NÃO DIGITE OS COMENTÁRIOS!!! ***
    wb        { word b }
    lc        { long c }
    B         { begin  }
    a=a
    a=b
    a=c
    b=a
    b=b
    b=c
    c=a
    c=b
    c=c
    .
~~~

Para cada declaração, você deve obter o código gerado para o espaço alocado de armazenamento. Para cada atribuição, você deve obter o código para carregar as variáveis com o tamanho correto, e armazená-las, também com o tamanho correto.

Só há um pequeno problea: O código gerado está ERRADO!

Obserce o código para "a=c" acima. O código é:

~~~asm
    MOV DX, [C+2]
    MOV AX, [C]
    MOV [A], AL
~~~

Este código está correto. Ele vai fazer com que os 8 bits menos significativos de C sejam armazenados em A, o que é um comportamento razoável. É basicamente o que se espera que aconteça.

Mas agora, observe o caso oposto. Para "c=a", o código gerado é:

~~~asm
    MOV AL, [A]
    MOV [C+2], DX
    MOV [C], AX
~~~

Isto NÃO está correto. Ele vai fazer com que o byte armazenado em A, seja armazenado no registrador AL, que corresponde aos 8-bits inferiores de AX. De acordo com as regras para o processador 80x86, os 8-bits superiores (registrador AH) não são modificados, e muito menos o valor de DX, que seriam os 16-bits mais significativos do nosso valor de 32-bits. Isto significa que quando armazenamos o valor inteiro de 32-bits em "C", qualquer valor que tenha ficado anteriormente em AH e DX será armazenado também. Nada bom.

Esta questão com o qual nos defrontamos anteriormente é o problema de CONVERSÃO DE TIPO, ou COERÇÃO.

Antes de fazer qualquer outra coisa com variáveis de tipos diferentes, mesmo que seja só copiá-los, temos que tratar deste problema. Não é a parte mais simples do compilador. A maior parte dos bugs que eu já vi em compiladores de produção tem a ver com erros em conversão de tipos para certas combinações obscuras de argumentos. Como de costume, há uma troca entre complexidade do compilador e a qualidade potencial do código gerado. Vamos continuar tomando o caminho que leva à simplicidade do compilador.

A Saída dos Covardes
--------------------

Antes de nos afundarmos em detalhes (e complexidade em potencial) de conversão de tipos, eu gostaria de lhe mostrar que há uma maneira extremamente simples de resolver o problema: simplesmente converta toda variável para um inteiro longo quando carregá-la!

Isto requer a inserção de poucas linhas a `AsmLoadVar()`, cujo trabalho é zerar o(s) registrador(es) envolvido(s) para que o valor resultante seja válido em qualquer tipo. Aqui está a versão modificada:

~~~c
/* Gera código para carregar variável de acordo com o tipo */
void AsmLoadVar(char name, char type)
{
    switch (type) {
        case 'b':
            EmitLn("XOR DX, DX");
            EmitLn("XOR AX, AX");
            EmitLn("MOV AL, [%c]", name);
            break;
        case 'w':
            EmitLn("XOR DX, DX");
            EmitLn("MOV AX, [%c]", name);
            break;
        case 'l':
            EmitLn("MOV DX, [%c+2]", name);
            EmitLn("MOV AX, [%c]", name);
    }
}
~~~

(Repare que `AsmStoreVar()` não precisa de mudança similar.)

Se você fizer alguns testes com a nova versão, vai descobrir que tudo funciona corretamente, apesar de um pouco ineficiente às vezes. Por exemplo, considere o caso "a=b" (para as mesmas declarações acima). Agora o código gerado é:

~~~asm
    XOR DX, DX
    MOV AX, [B]
    MOV [A], AL
~~~

Neste caso, ocorre que é desnecessário limpar o valor dos registradores, já que o resultado está sendo armazenado numa variável do tipo byte. Com um pouco de trabalho é possível fazer melhor. No entanto, isto não é tão ruim, e é o tipo de ineficiência típica que já vimos em compiladores simplificados.

Eu devo avisar que, alterando os bits superiores para zero, estamos de fato tratando os números como inteiros SEM SINAL. Se queremos tratar deles com sinal (o caso mais comum) devemos fazer uma extensão de sinal após o carregamento, ao invés de zerar o valor antes. Isto é feito através de CBW (Convert Byte to Word) e CWD (Convert Word to Dword). Para encerrar esta parte da discussão, vamos alterar `AsmLoadVar()`:

~~~c
/* Gera código para carregar variável de acordo com o tipo */
void AsmLoadVar(char name, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV AL, [%c]", name);
            EmitLn("CBW");
            EmitLn("CWD");
            break;
        case 'w':
            EmitLn("MOV AX, [%c]", name);
            EmitLn("CWD");
            break;
        case 'l':
            EmitLn("MOV DX, [%c+2]", name);
            EmitLn("MOV AX, [%c]", name);
    }
}
~~~

(Use CWDE para converter AX em EAX se estiver usando registradores de 32 bits.)

Por último, para tratar de um byte como sendo sem sinal (como em Pascal e C) e de um word com sinal, basta remover a conversão de byte para word (CBW) após o carregamento e zerar AX antes:

~~~c
        case 'b':
            EmitLn("XOR AX, AX");
            EmitLn("MOV AL, [%c]", name);
            EmitLn("CWD");
            break;
~~~

Uma Solução Mais Razoável
-------------------------

Como já vimos, promover toda variável a um número longo resolve o problema, mas dificilmente pode ser considerado eficiente, e provavelmente não seria aceitável mesmo para aqueles de nós que julgam não se importarem com a eficiência. Significa que todas as operações aritméticas serão feitas com precisão de 32-bits, o que vai DUPLICAR o tempo de execução da maioria das operações, e fazer algo até pior na multiplicação e divisão. Para estas operações, precisaríamos chamar sub-rotinas para fazer o cálculo, mesmo se os dados fossem do tipo byte ou word. A coisa toda não parece muito boa, pois ignora os problemas reais.

Certo, portanto esta solução não é muito boa. Haveria uma forma relativamente fácil de obter conversão de dados? Podemos continuar Mantendo a Coisa Simples? (*KISS... Keep It Simple?*)

Sim, de fato. Tudo o que devemos fazer é colocar a conversão na outra ponta... isto é, convertemos os valores quando eles serão armazenados, ao invés de quando são carregados.

Mas lembre-se, a parte do armazenamento de uma atribuição é de certa forma independente do carregamento dos dados, que é tratado pelo procedimento `Expression()`. Em geral, a expressão pode ser arbitrariamente complexa. Como a rotina `Assignment()` pode saber que tipo de dado foi deixado nos registradores?

Novamente, a resposta é simples: Nós simplesmente PERGUNTAMOS à rotina `Expression()`! A resposta é retornada pela função.

Tudo isto requer que diversas rotinas sejam modificadas, mas as modificações, como o próprio método, são simples. Primeiramente, já que não vamos pedir para que `AsmLoadVar()` faça a conversão, retornemos à versão mais simples:

~~~c
/* Gera código para carregar variável de acordo com o tipo */
void AsmLoadVar(char name, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV AL, [%c]", name);
            break;
        case 'w':
            EmitLn("MOV AX, [%c]", name);
            break;
        case 'l':
            EmitLn("MOV DX, [%c+2]", name);
            EmitLn("MOV AX, [%c]", name);
    }
}
~~~

Em seguida, vamos adicionar uma nova rotina que converte de tipo para outro:

~~~c
/* Converte tipo origem para destino */
void AsmConvert(char src, char dst)
{
    if (src == dst)
        return;
    if (src == 'b')
        EmitLn("CBW");
    if (dst == 'l')
        EmitLn("CWD");
}
~~~

Esta versão da conversão considera todos os tipos como sendo com sinal. Se fossemos considerar "byte" sem sinal, basta trocar "CBW", que faz a extensão de sinal de "byte" para "word", por "XOR AH, AH", que zera os bits mais significativos de AX.

Em seguida, temos que fazer a lógica necessária para carregar e armazenar uma variável de qualquer tipo. Aqui estão as rotinas:

~~~c
/* Carrega variável */
char LoadVar(char name)
{
    char type = VarType(name);
    AsmLoadVar(name, type);
    return type;
}

/* Armazena variável */
void StoreVar(char name, char srcType)
{
    char dstType = VarType(name);
    AsmConvert(srcType, dstType);
    AsmStoreVar(name, dstType);
}
~~~

Note que `LoadVar()` é uma função, que não só emite código para o carregamento, mas também retorna o tipo da variável. Desta forma, sempre sabemos com que tipo de dados estamos tratando. Quando executarmos `StoreVar()`, passamos o tipo do valor em AX (ou DX:AX). Já que `StoreVar()` sabe o tipo da variável de destino, ele pode converter conforme necessário.

Armado com estas novas rotinas, a implementação de nosso comando de atribuição rudimentar é essencialmente trivial. O procedimento `Expression()` agora se torna uma função, que retorna seu tipo para o procedimento `Assignment()`:

~~~c
/* Analisa e traduz uma expressão */
char Expression()
{
    return LoadVar(GetName());
}

/* Analisa e traduz uma atribuição */
void Assignment()
{
    char name, type;

    name = GetName();
    Match('=');
    type = Expression();
    StoreVar(name, type);
}
~~~

Novamente, repare como são incrívelmente simples estas duas rotinas. Encapsulamos toda a lógica de tipos em `LoadVar()` e `StoreVar()`, e o truque de passar o tipo torna o resto do trabalho extremamente fácil. É claro, tudo isto é para o nosso caso especial e trivial de `Expression()`. Naturalmente, para o caso geral isto será mais complexo. Mas você está olhando agora para a versão FINAL de `Assignment()`!

Tudo isto parece ser uma solução muito simples e limpa, e realmente é. Compile esta versão do programa e e faça os mesmos testes de antes. Você verá que todos os tipos de dados serão convertidos apropriadamente, e há poucas (ou nenhuma) instrução desperdiçada.

Apesar de não termos considerado variáveis sem sinal neste caso, eu acho que seria fácil arrumar o procedimento `AsmConvert()` para tratar destes casos também. Isto fica como um "exercício para o estudante".

Argumentos Literais
-------------------

Leitores atentos devem ter reparado, que nós sequer temos uma forma apropriada de um fator simples ainda, pois não permitimos o carregamento de constantes literais, apenas variáveis. Vamos arrumar isto agora.

Para começar, vamos precisar de uma função `GetNum()`. Até agora vimos diversas versões desta versão, algumas retornando apenas um caracter, outras uma string, e outras um valor inteiro. A que vamos usar aqui vai retornar um inteiro longo, para que ela possa tratar qualquer coisa que usarmos. Repare que nenhuma informação de tipo é retornada aqui: `GetNum()` não se importa em como o número será usado:

~~~c
/* Analisa e traduz um número inteiro longo */
long GetNum()
{
    long num;

    if (!isdigit(Look))
        Expected("Integer");
    num = 0;
    while (isdigit(Look)) {
        num *= 10;
        num += Look - '0';
        NextChar();
    }
    SkipWhite();
    return num;
}
~~~

Agora, quando tratarmos de dados literais, temos um pequeno problema. Com variáveis, sabemos que tipo as coisas devem ter, pois elas foram declaradas para ser daquele tipo. Não possuímos esta informação para literais. Quando o programador diz "-1" isto significa um valor "byte", "word" ou "long"? Não temos nenhuma pista. A coisa mais óbvia a fazer seria usar a maior versão possível, isto é, "long". Mas esta é uma má idéia, pois quando estivermos tratando de expressões mais complexas, isto vai fazer com que toda expressão envolvendo literais seja promovida para "long" também.

Uma abordagem melhor é selecionar o tipo baseado no valor do literal, conforme o seguinte:

~~~c
/* Carrega uma constante no registrador primário */
char LoadNum(long val)
{
    char type;

    if (val >= -128 && val <= 127)
        type = 'b';
    else if (val >= -32768 && val <= 32767)
        type = 'w';
    else
        type = 'l';
    AsmLoadConst(val, type);

    return type;
}
~~~

Repare que `LoadNum()` chama a nova da rotina geradora de código `AsmLoadConst()`, que tem um argumento adicional para definir o tipo:

~~~c
/* Gera código para carregar uma constante de acordo com o tipo */
void AsmLoadConst(long val, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV AL, %d", (int) val);
            break;
        case 'w':
            EmitLn("MOV AX, %d", (int) val);
            break;
        case 'l':
            EmitLn("MOV DX, %u", val >> 16);
            EmitLn("MOV AX, %u", val & 0xFFFF);
            break;
    }
}
~~~

(Repare que para o caso de uma constante "long", não há como carregá-la diretamente em DX e AX. Por isso foi preciso carregar os registradores separadamente, colocando em DX os 16 bits mais significativos e em AX os 16 menos significativos. Isto não é necessário se você estiver usando o registrador EAX de 32-bits ao invés do par DX:AX de registradores de 16-bits)

Agora podemos modificar `Expression()` para acomodar os dois tipos de fatores possíveis:

~~~c
/* Analisa e traduz uma expressão */
char Expression()
{
    char type;

    if (isalpha(Look))
        type = LoadVar(GetName());
    else
        type = LoadNum(GetNum());

    return type;
}
~~~

(Certamente isto não machucou muito, apenas algumas linhas extra pra fazer o trabalho.)

Certo, compile este código no seu programa e faça um teste. Você vai constatar que ele funciona para qualquer expressão com variáveis ou constantes válidas.

Expressões com Soma
-------------------

Se você tem acompanhado esta série deste o início, eu tenho certeza que você sabe o que vem em seguida: vamos expandir a expressão para tratar primeiro de expressões com soma e subtração, então multiplicação e em seguida expressões gerais com parênteses.

A parte boa é que já temos um padrão para tratar destas expressões mais complexas. Tudo o que temos que fazer é ter certeza que todos os procedimentos chamados por `Expression()` (`Term()`, `Factor()`, etc.) sempre retornem um identificador de tipo. Se fizermos desta forma, dificilmente será preciso alterar a estrutura do programa.

A primeira parte é fácil: podemos **renomear** nossa função existente para `Term()`, como já fizemos tantes vezes antes, e criar uma nova versão para `Expression()`:

~~~c
/* Analisa e traduz uma expressão */
char Expression()
{
    char type;

    if (IsAddOp(Look))
        type = UnaryOp();
    else
        type = Term();
    while (IsAddOp(Look)) {
        AsmPush(type);
        switch (Look) {
            case '+':
                type = Add(type);
                break;
            case '-':
                type = Subtract(type);
                break;
        }
    }

    return type;
}
~~~

Note nesta rotina como cada chamada a procedimento se transformou numa chamada de função, e como a variável local "type" é atualizada a cada passagem.

Repare também na nova chamada à função `UnaryOp()`, que trata dos operadores unários de sinal. Esta mudança não é necessária... poderíamos manter uma forma como a usada anteriormente. Eu decidi introduzir `UnaryOp()` como uma rotina separada pois torna as coisas mais fáceis, mais tarde, ao produzir um código melhor do que o que estamos produzindo. Em outras palavras, estou prestando atenção nos detalhes de otimização.

Para esta versão, porém, vou manter o mesmo código bobo que faz com que a rotina seja trivial:

~~~c
/* Tratamento de operador unário */
char UnaryOp()
{
    AsmClear();
    return 'w';
}
~~~

O procedimento `AsmPush()` é uma rotina de geração de código, e agora possui um argumento de tipo:

~~~c
/* Coloca valor na pilha */
void AsmPush(char type)
{
    if (type == 'b')
        EmitLn("CBW"); /* Só é possível empilhar "word"s */
    if (type == 'l')
        EmitLn("PUSH DX");
    EmitLn("PUSH AX");
}
~~~

Agora vamos dar uma olhada nas funções `Add()` e `Subtract()`. Nas versões anteriores destas rotinas, fizemos com que elas chamassem as rotinas de geração de código `AsmPopAdd()` e `AsmPopSub()`. Vamos continuar fazendo assim, o que torna as funções em si extremamente simples:

~~~c
/* Reconhece e traduz uma soma */
char Add(char type)
{
    Match('+');
    return AsmPopAdd(type, Term());
}

/* Reconhece e traduz uma subtração */
char Subtract(char type)
{
    Match('-');
    return AsmPopSub(type, Term());
}
~~~

A simplicidade chega a ser decepcionante, pois o que fizemos foi delegar a lógica para `AsmPopAdd()` e `AsmPopSub()`, que não são somente rotinas de geração de código. Elas devem também tomar conta da conversão de tipos necessária.

E que conversão é esta? Simples: ambos argumentos devem ter o mesmo tamanho e o resultado também possui este tamanho. O menor dos argumentos deve ser "promovido" para o tamanho do maior.

Mas isto apresenta um certo problema. Se o argumento a ser promovido é o segundo (isto é, o que está no registrador primário), está tudo bem. Caso contrário, temos um problema: não podemos mudar o tamanho da informação que já está na pilha.

A alternativa é introduzir uma rotina `AsmPop()` análoga a `AsmPush()`. Mas esta rotina deve armazenar o valor no registrador secundário (BX ou CX:BX para "long").

~~~c
/* Coloca em registrador(es) secundário(s) valor da pilha */
void AsmPop(char type)
{
    EmitLn("POP BX");
    if (type == 'l')
        EmitLn("POP CX");
}
~~~

A idéia geral é que todas as rotinas `AsmPopXXX()` usem esta rotina para obter o primeiro operando. Quando isto é feito, teremos os dois operandos em registradores, para que possamos promover o operando cujo tipo é menor.

Agora, porém, temos um problema referente à plataforma utilizada. No processador 80x86 só podemos usar as instruções "CBW" e "CWD" no registrador primário, ou seja AL e AX respectivamente. Isto requer uma solução um pouco complicada.

Você pode não gostar muito da minha solução, mas ela consiste no seguinte:

- Assuma que o registrador AL, AX ou DX:AX possui o SEGUNDO operando
- Assuma que o registrador BL, BX ou CX:BX possui o PRIMEIRO operando
- Se for necessário promover o PRIMEIRO, trocamos os valores entre os registradores (de acordo com o tipo), para deixar em AL ou AX o operando a ser promovido.
- Se for necessário promover algum operando, então convertemos AL ou AX (que já está com o operando correto).
- Se não ocorreu a troca antes, troque agora, para manter os operandos em uma ordem consistente.

O processo é difícil de explicar mas fácil de implementar, está longe de ser o mais eficiente possível mas serve para os nossos propósitos. Além disso, após o processo, os registradores SEMPRE vão ter esta configuração:

- O registrador AL, AX ou DX:AX terá o valor do PRIMEIRO operando
- O registrador BL, BX ou CX:BX terá o valor do SEGUNDO operando

Desta forma teremos sempre os registradores na ordem esperada para a subtração e a divisão. Uma deficiência do código é que faremos uma troca desnecessária no caso da adição e multiplicação, mas isto é fácil de corrigir; fica como exercício. Vamos começar com a rotina que faz a troca:

~~~c
/* Gera código para trocar registradores primário e secundário */
void AsmSwap(char type)
{
    switch (type) {
      case 'b':
          EmitLn("XCHG AL, BL");
          break;
      case 'w':
          EmitLn("XCHG AX, BX");
          break;
      case 'l':
          EmitLn("XCHG AX, BX");
          EmitLn("XCHG DX, CX");
          break;
    }
}
~~~

Em seguida temos a rotina que faz a promoção em si, forçando os dois tipos a serem iguais:

~~~c
/* Faz a promoção dos tipos dos operandos e inverte a ordem dos mesmos */
char AsmSameType(char t1, char t2)
{
    int swaped = 0;
    int type = t1;

    if (t1 != t2) {
        if ((t1 == 'b') || (t1 == 'w' && t2 == 'l')) {
            type = t2;
            AsmSwap(type);
            AsmConvert(t1, t2);
            swaped = 1;
        } else {
            type = t1;
            AsmConvert(t2, t1);
        }
    }
    if (!swaped)
        AsmSwap(type);
        
    return type;
}
~~~

(Esta rotina não gera código diretamente, mas está intimamente ligada com a geração do código já que ela é dependente da arquitetura.)

"t1" deve ser o tipo do primeiro operando e "t2" o do segundo. Esta rotina verifica se os dois tipos são diferentes (um deles deve ser promovido). Se sim, verificamos se é o primeiro operando que deve ser promovido (caso em que fazemos a troca antes) ou o segundo (caso em que a troca deve ser feita depois).

Com estas rotinas em mãos, temos toda a munição necessária para incluir `AsmPopAdd()` e `AsmPopSub()`:

~~~c
/* Soma valor na pilha com valor no registrador primário */
char AsmPopAdd(char t1, char t2)
{
    char type;

    AsmPop(t1);
    type = AsmSameType(t1, t2);
    switch (type) {
        case 'b':
            EmitLn("ADD AL, BL");
            break;
        case 'w':
            EmitLn("ADD AX, BX");
            break;
        case 'l':
            EmitLn("ADD AX, BX");
            EmitLn("ADC DX, CX");
            break;
    }

    return type;
}

/* Subtrai do valor da pilha o valor no registrador primário */
char AsmPopSub(char t1, char t2)
{
    char type;

    AsmPop(t1);
    type = AsmSameType(t1, t2);
    switch (type) {
        case 'b':
            EmitLn("SUB AL, BL");
            break;
        case 'w':
            EmitLn("SUB AX, BX");
            break;
        case 'l':
            EmitLn("SUB AX, BX");
            EmitLn("SBB DX, CX");
            break;
    }

    return type;
}
~~~

Depois de tudo pronto, os resultados finais são quase anti-climáticos. A lógica destas rotinas é bem simples. Tudo o que elas fazem é remover o resultado da pilha no registrador secundário, forçar os dois operadores para o mesmo tipo, e então gerar o código para o cálculo.

Repare que o código que faz a soma e a subtração depende dos tipos dos operandos. A nova versão da subtração não requer que invertamos o sinal em seguida. Isto porque os operandos já estão na ordem correta (o que é um efeito colateral de `AsmSameType()`). Esta inversão de ordem é boa para a subtração e divisão, mas não é necessária na soma e multiplicação, pois a ordem não faz diferença nestes casos e há uma instrução de troca desnecessária. Podemos passar um parâmetro a mais para `AsmSameType()` dizendo se a ordem dos operandos importa ou não:

~~~c
/* Faz a promoção dos tipos dos operandos e inverte a ordem dos mesmos se necessário */
char AsmSameType(char t1, char t2, int ordMatters)
{
    int swaped = 0;
    int type = t1;

    if (t1 != t2) {
        if ((t1 == 'b') || (t1 == 'w' && t2 == 'l')) {
            type = t2;
            AsmSwap(type);
            AsmConvert(t1, t2);
            swaped = 1;
        } else {
            type = t1;
            AsmConvert(t2, t1);
        }
    }
    if (!swaped && ordMatters)
        AsmSwap(type);
        
    return type;
}
~~~

Certo, eu concordo que eu lhes mostrei muitas rotinas desde a última vez que testamos o código. Mas você deve admitir que cada nova rotina é simples e transparente, mesmo com as limitações da extensão de sinal e tudo mais. Se você (como eu) não gosta de testar tantas rotinas de uma só vez, tudo bem. É possível criar versões vazias de `AsmConvert()`, e `AsmSameType()`, já que elas não processam a entrada. Você não vai obter o código correto, é claro, mas as coisas devem funcionar. Então, adicione as rotinas uma de cada vez.

Quando estiver testando o programa, não esqueça de declarar algumas variáveis antes, e então começar o "corpo" do programa com um "B" maiúsculo (para BEGIN). Você deve reparar que o analisador vai tratar de qualquer expressão aditiva. Uma vez que as rotinas de conversão estejam no lugar, você vai ver que código correto está sendo gerado, com conversões de tipo inseridas quando necessárias. Tente misturar variáveis de tipos diferentes, e também literais. Certifique-se de que tudo está funcionando apropriadamente. Como de costume, é uma boa idéia verificar se expressões inválidas são tratadas apropriadamente pelo compilador.

Por Que Tantas Rotinas?
-----------------------

Neste ponto, você deve achar que eu já atingi o ponto mais profundo em termos de chamadas de rotina aninhadas. Eu admito que há muito overhead aqui. Mas há um método na minha loucura. Como no caso de `UnaryOp()`, estou prestando atenção ao momento em que vamos querer uma melhor geração de código. Da forma como o código está organizado, podemos fazer isto sem modificações grandes ao programa.

Expressões com Multiplicação
----------------------------

Os procedimento para operações de multiplicação e divisão são quase os mesmos. De fato, no primeiro nível, eles são quase idênticos, então eu vou simplesmente mostrá-los aqui sem muito demora. O primeiro é nossa forma geral para `Factor()`, que inclui expressões entre parênteses:

~~~c
/* Analisa e traduz um fator matemático */
char Factor()
{
    char type;

    if (Look == '(') {
        Match('(');
        type = Expression();
        Match(')');
    } else if (isalpha(Look))
        type = LoadVar(GetName());
    else
        type = LoadNum(GetNum());

    return type;
}

/* Reconhece e traduz uma multiplicação */
char Multiply(char type)
{
    Match('*');
    return AsmPopMul(type, Factor());
}

/* Reconhece e traduz uma multiplicação */
char Divide(char type)
{
    Match('/');
    return AsmPopDiv(type, Factor());
}

/* Analisa e traduz um termo matemático */
char Term()
{
    char type;

    type = Factor();
    while (IsMulOp(Look)) {
        AsmPush(type);
        switch (Look) {
            case '*':
                type = Multiply(type);
                break;
            case '/':
                type = Divide(type);
                break;
        }
    }

    return type;
}
~~~

Estas rotinas combinam quase que exatamente com as aditivas. Como antes, a complexidade é encapsulada dentro de "asmPopMul" e "asmPopDiv". Se você quiser testar o programa antes de chegarmos lá, você pode criar versões "fantasma" para elas, similar ao que foi feito com `AsmPopAdd()` e `AsmPopSub()`. Novamente, o código não vai estar correto neste ponto, mas o analisador deve ser capaz de tratar de expressões de complexidade arbitrária.

Multiplicação
-------------

Uma vez que você tenha se convencido de que o analisador em si está funcionando apropriadamente, precisamos descobrir o que devemos fazer para gerar o código correto. É aí que as coisas começam a ficar difíceis, pois as regras são mais complexas.

Vamos tratar do caso da multiplicação primeiro. Esta operação é parecida com as de soma já que os operandos devem ser do mesmo tamanho. Ela é diferente em dois aspectos importantes:

- O tipo do produto não é o mesmo dos operandos. Para o produto de dois "words", temos um resultado "long".

- O 8086 não suporta multiplicação de dois operandos de 32-bits (não para o nosso caso, em que estamos tratando de registradores de 16-bits), portanto uma chamada a uma rotina externa é necessária. Esta rotina se tornará parte de nossa biblioteca.

> NOTA: Em processadores de 32 ou 64-bits, é possível multiplicar números de 32-bits usando os registradores estendidos e as próprias instruções da CPU. Para o tutorial, vamos nos limitar às instruções de 16-bits, pois o importante é aprender a técnica, que pode ser aplicada em outras situações.

As ações a serem tomadas podem ser melhor traduzidas na tabela:

T1 →<br>T2 ↓  | B                                            | W                                            | L                                            |
--------------|----------------------------------------------|----------------------------------------------|----------------------------------------------|
B             | <br>IMUL BL<br>Resultado = W                 | Promover T2→w<br>IMUL BX<br>Resultado = L    | Promover T2→L<br>CALL MUL32<br>Resultado = L |
W             | Promover T1→W<br>IMUL BX<br>Resultado = L    | <br>IMUL BX<br>Resultado = L                 | Promover T2→L<br>CALL MUL32<br>Resultado = L |
L             | Promover T1→L<br>CALL MUL32<br>Resultado = L | Promover T1→L<br>CALL MUL32<br>Resultado = L | <br>CALL MUL32<br>Resultado = L              |


Esta tabela mostra as ações a serem tomadas para cada combinação de tipos de operandos. Há três coisas a notar: Primeiro, assumimos que temos uma rotina de biblioteca MUL32 que faz multiplicações de 32 x 32 bits, com um produto de 32-bits (não 64-bits)! Se houve qualquer "overflow" no processo, nós ignoramos e retornamos apenas os 32 bits inferiores.

Segundo, repare que a tabela é simétrica... os dois operandos entram da mesma forma. Finalmente, repare que o produto é SEMPRE um "long", exceto quando ambos operandos são "byte"s. (Vale a pena notar, que muitas expressões vão acabar sendo do tipo "long", queiramos ou não. Talvez nossa idéia de simplesmente promover todos os tipos para "long" não fosse tão ruim assim, no fim das contas!)

Agora, claramente, vamos gerar código diferente para as multiplicações de 8, 16 e 32 bits.

~~~c
/* Multiplica valor na pilha com valor no registrador primário */
char AsmPopMul(char t1, char t2)
{
    char type, mulType;

    AsmPop(t1);
    type = AsmSameType(t1, t2, 0);
    mulType = 'l';
    switch (type) {
        case 'b':
            EmitLn("IMUL BL");
            mulType = 'w';
            break;
        case 'w':
            EmitLn("IMUL BX");
            break;
        case 'l':
            EmitLn("CALL MUL32");
            break;
    }

    return mulType;
}
~~~

Como você pode ver a rotina começa da mesma forma que `AsmPopAdd()`. Os dois argumentos são forçados para o mesmo tipo. Finalmente o código é gerado para efetuar a multiplicação. Repare que o tipo de retorno é diferente dos operandos. Não é muito complicado, realmente.

Neste ponto, eu sugiro que você vá em frente e teste o programa. Tente todas as combinações de tamanho que você conseguir imaginar.

Divisão
-------

O caso da divisão não é tão simétrico. Eu também tenho algumas más notícias pra você.

Toda CPU moderna de 16-bits suporta divisão de inteiros. A especificação do fabricante descreve esta operação como sendo uma divisão de 32 por 16-bits, ou seja, você pode ter um dividendo de 32-bits dividido por um divisor de 16-bits. Aqui vai a má notícia:

> ELES ESTÃO MENTINDO PRA VOCÊ!!!

Se você não acredita, tente dividir qualquer número de 32-bits de tamanho (ou seja, que tenha bits diferentes de zero nos 16-bits mais significativos) pelo valor inteiro 1. Você vai se deparar com um erro de estouro de divisão.

O problema é que a instrução realmente requer que o quociente resultante caiba num resultado de 16-bits. Isto não vai acontecer A MENOS que o divisor seja grande o suficiente. Quando um número qualquer é dividido por uma unidade, o quociente é claro será o mesmo que o dividendo, que DEVE caber nos 16-bits do resultado.

Desde o princípio dos tempos (bem, dos computadores pelo menos), os criadores de CPUs tem provido este pequeno detalhe nos circuitos de divisão. Ele provê uma certa simetria nas coisas, já que é o modo inverso de como a multiplicação funciona. Mas já que "1" é um divisor perfeitamente válido (e comum), a divisão, conforme implementada em hardware, requer alguma ajuda dos programadores.

As implicações são:

- O tipo do quociente deve ser sempre o mesmo do dividendo. É independente do divisor.

- Apesar da CPU suportar um dividendo do tipo "long", as instruções providas pelo hardware só são confiáveis para divisão de dividentos tipo "byte" e "word". Para dividentos do tipo "long", precisamos de outra rotina de biblioteca capaz de retornar um resultado "long".

Parece que vamos precisar de outra tabela, para resumir as ações necessárias:

T1 →<br>T2 ↓  | B                                             | W                                             | L                                             |
--------------|-----------------------------------------------|-----------------------------------------------|-----------------------------------------------|
B             | Converter T1→W<br>IDIV BL<br>Resultado = B    | <br>IDIV BL<br>Resultado = W                  | Converter T2→L<br>CALL DIV32<br>Resultado = L |
W             | Converter T1→L<br>IDIV BX<br>Resultado = B    | Converter T1→L<br>IDIV BX<br>Resultado = W    | Converter T2→L<br>CALL DIV32<br>Resultado = L |
L             | Converter T1→L<br>CALL DIV32<br>Resultado = B | Converter T1→L<br>CALL DIV32<br>Resultado = W | <br>CALL DIV32<br>Resultado = L               |


(Você pode se perguntar porque é necessário fazer uma divisão de 32-bits, quando o dividendo é, digamos um simples valor "byte". Já que o número de bits no resultado só pode no máximo o número de bits do dividendo, por que se importar? A razão é que, se o divisor for um "long", e houver algum bit diferente de zero na parte mais significativa, o resultado da divisão deve ser zero. Podemos não conseguir isto se apenas a parte mais baixa do divisor for usada.)

O código a seguir provê o funcionamento correto para "asmPopDiv":

~~~c++
/* Divide valor na pilha por valor do registrador primário */
char AsmPopDiv(char t1, char t2)
{
    AsmPop(t1);

    /* Se dividendo for 32-bits divisor deve ser também */
    if (t1 == 'l')
        AsmConvert(t2, 'l');

    /* Coloca operandos na ordem certa conforme o tipo */
    if (t1 == 'l' || t2 == 'l')
        AsmSwap('l');
    else if (t1 == 'w' || t2 == 'w')
        AsmSwap('w');
    else
        AsmSwap('b');

    /* Dividendo _REAL_ sempre será LONG...
        mas WORD se divisor for BYTE */
    if (t2 == 'b')
        AsmConvert(t1, 'w');
    else
        AsmConvert(t1, 'l');

    /* Se um dos operandos for LONG, divisão de 32-bits */
    if (t1 == 'l' || t2 == 'l')
        EmitLn("CALL DIV32");
    else if (t2 == 'w') /* 32 / 16 */
        EmitLn("IDIV BX");
    else if (t2 == 'b') /* 16 / 8 */
        EmitLn("IDIV BL");

    /* Tipo do quociente é sempre igual ao do dividendo */
    return t1;
}
~~~

Note que assumimos que DIV32 deixa o resultado ("long") em DX:AX.

Instale este novo procedimento para divisão. Neste ponto você deve estar apto a gerar código para qualquer tipo de expressão aritmética. Teste o programa à vontade!

Segue o código completo da nossa implementação de tipos inteiros:

~~~c
{% include_relative src/cap14-types.c %}
~~~

> Download do código-fonte [cap14-types.c](src/cap14-types.c).


Começando a se aprofundar
-------------------------

Finalmente, neste capítulo, aprendemos como tratar de variáveis (e literais) de tipos diferentes. Como você pode ver, não foi tão difícil. De certa forma, boa parte do código parece até mais simples do que em programas anteriores. Apenas os operadores de multiplicação e divisão nos exigiram um pouco mais de planejamento.

O conceito que tornou as coisas simples foi o de conversão de procedimentos como `Expression()` em funções que retornam o tipo do resultado. Uma vez estando isto feito, fomos capazes de reter a mesma estrutura geral do compilador.

Não vou fingir que nós cobrimos todos os aspectos do problema. Eu ignorei por completo aritmética sem sinal. Do que já foi visto, eu acho que você consegue ver que incluir isto não adiciona desafios novos, apenas novas possibilidades a serem testadas.

Eu também ignorei operadores lógicos como AND, OR, etc. Acontece que estes são muito fáceis de tratar. Todos os operadores lógicos são operações bit-a-bit, então eles são simétricos e portanto funcionam da mesma forma que `AsmPopAdd()`. Há uma diferença, porém: se for necessário estender o tamanho de uma variável lógica, a extensão deve ser feita como um número SEM SINAL. Números de ponto flutuante, também não são muito difíceis de lidar... apenas algumas novas funções na biblioteca, ou talvez instruções para o co-processador matemático.

Talvez mais importante que isso, eu também ignorei o problema de CHECAGEM de tipos, ao invés de conversão. Em outras palavras, nós permitimos operações entre variáveis de todas as combinações de tipos. Em geral isto não será sempre válido... certamente você não desejaria adicionar um inteiro a uma string por exemplo. Muitas linguagens também não permitem misturar variáveis tipo caracter e variáveis numéricas.

Novamente, não há nenhum grande problema a ser tratado neste caso. Já estamos verificando os tipos dos dois operandos... a maior parte da verificação é feita em procedimentos como `AsmSameType()`. É muito fácil incluir uma chamada a um tratador de erros, se os tipos dos operandos forem incompatíveis.

No caso geral, podemos pensar em cada operador sendo tratado por um procedimento diferente, dependendo do tipo dos dois operandos. Isto é simples de implementar, embora tedioso, criando uma simples tabela de desvio com os tipos de operando como índices. Em Pascal, a operação equivalente pode ser conseguida com comandos "case" aninhados. Algumas das rotinas chamadas poderiam ser simplesmente rotinas de erro, enquanto outras podem fazer qualquer tipo de conversão necessária. Conforme novos tipos são adicionados, o número de procedimentos cresce numa escala de N ao quadrado, mas talvez não chegue a ser um número extraordinário de procedimentos.

O que fizemos aqui foi compactar tal tabela de desvios em poucas rotinas, simplesmente fazendo uso de simetria e outras regras de simplificação.

Coagir ou não coagir?
---------------------

Caso você não tenha entendido a mensagem ainda, certamente aparenta que TINY e KISS provavelmente NÃO serão linguagens fortemente tipadas, já que eu permiti mistura e conversão automática de todos os tipos. O que nos leva à próxima questão:

> É realmente isto que queremos fazer?

A resposta vai depender do tipo de linguagem desejada, e da forma como você espera que ela se comporte. O que nós não tratamos é a questão de permitir ou negar o uso de operações envolvendo tipos diferentes. Em outras palavras, qual deve ser a SEMÂNTICA do nosso compilador? Nós desejamos conversão de tipos automática para todos os casos, para alguns casos, ou simplesmente não queremos qualquer tipo de conversão automática?

Vamos parar e pensar a respeito mais um pouco. Para isto, vamos rever um pouco da história.

FORTRAN II suportava apenas dois tipos de dados simples: números inteiros e reais. Era feita conversão implícita entre os tipos real e inteiro durante atribuições, mas não no meio de uma expressão. Todos os itens de dados (incluindo constantes literais) do lado direito de uma atribuição deveriam ser do mesmo tipo. Isto tornou as coisas bem fáceis... bem mais simples do que o que fizemos aqui.

Isto foi mudado em FORTRAN IV para suportar aritmética "de tipos misturados". Se uma expressão possuísse algum item real, todos os ítens seriam convertidos para reais e a expressão em si seria real. Para completar a figura, funções foram providas para converter explícitamente um tipo em outro, para forçar uma expressão a ter um tipo ou outro.

Isto levou a duas coisas: código que era mais fácil de escrever, e código que era menos eficiente. Isto porque programadores descuidados escreveriam expressões com constantes simples como 0 e 1, as quais seriam compiladas para que fosse feita desnecessariamente a conversão em tempo de execução. Ainda assim, o sistema funcionava bem, o que tende a indicar que conversão implícita de tipos é algo positivo.

C também é uma linguagem com tipagem fraca, embora suporte um número maior de tipos. Um compilador C não reclamaria se você adicionasse um caracter a um inteiro, por exemplo. Parcialmente, isto ocorre graças à convenção de C de promover todo caracter a um inteiro quando carregado, ou passado para uma lista de parâmetros. Isto simplifica bastante a conversão. De fato, em compiladores de sub-conjuntos de C que não suportam tipos "long" ou "float", acabamos no mesmo estágio em que nos encontrávamos anteriormente, em nossa primeira abordagem simplificada: toda variável possuía a mesma representação, quando carregada em um registrador. Isto torna a vida muito fácil!

O máximo da conversão automática de tipos em uma linguagem está em PL/I. Esta linguagem suporta um grande número de tipos de dados, e você pode misturá-los livremente. Se a conversão implícita de FORTRAN parecia boa, então as de PL/I deveriam ser como o Paraíso, mas elas acabaram parecendo mais com o Inferno! O problema é que com tantos tipos de dados, deveria haver um volume grande de conversões de tipos, E um volume correspondente de regras a respeito de como os operandos misturados deveriam ser convertidos. Estas regras tornaram-se tão complexas que ninguém mais conseguia se lembrar delas! Muitos erros em programas PL/I tinham a ver com conversões inesperadas ou indesejadas. Às vezes uma coisa Positiva em excesso pode ser prejudicial!

Pascal, em oposição, é uma linguagem "fortemente tipada", o que significa que em geral você não pode misturar tipos, mesmo que eles sejam diferentes apenas no NOME, e tenham a mesma representação básica! Niklaus Wirth fez de Pascal uma linguagem com tipos bem definidos para ajudar os programadores a se manterem longe de problemas, e estas restrições de fato salvaram muitos programadores deles mesmos, pois o compilador evitava que fizessem algo incorreto. É melhor encontrar o erro durante a compilação do que na fase de depuração. As mesmas restrições podem também causar frustração quando você REALMENTE quer misturar tipos, e isto tente a jogar ex-programadores C contra a parede.

Mesmo assim, Pascal permite algumas conversões implícitas. É possível atribuir um inteiro a um valor real. É possível também misturar os tipos inteiro e real em expressões do tipo Real. Os inteiros seriam automáticamente forçados para o tipo real, da mesma forma que em FORTRAN (e com o mesmo custo em tempo de execução escondido).

Você não pode, no entanto, converter da outra forma, de real para inteiro, sem aplicar a função de conversão explícita, "Trunc". A teoria aqui é, já que o valor numérico de um número real necessariamente pode ser alterado pela conversão (a parte fracionária será perdida), você realmente não deve fazê-lo em "segredo".

No espírito de tipagem forte, Pascal não permite que sejam misturadas variáveis do tipo caracter e inteira, sem aplicar as funções explícitas de conversão "Chr" e "Ord".

Turbo Pascal também inclui os tipos Byte, Word, e LongInt. Os dois primeiros são basicamente a mesma coisa que inteiros sem sinal. Em Turbo Pascal, eles podem ser livremente misturados com variáveis do tipo inteiro, e o compilador trata da conversão automaticamente. Porém, há checagens em tempo de execução, para evitar estouro dos valores ou respostas erradas. Repare que ainda assim não é possível misturar os tipos "Byte" e "Char", mesmo sendo eles armazenadas internamente na mesma representação.

O extremo em termos de linguagem fortemente tipada é Ada, que não permite NENHUMA conversão implícita, e também não permite aritmética misturada. A posição de Jean Ichbiah é que as conversões custam tempo de execução, e você não deveria poder adicionar este custo sem estar consciente disto. Forçar o programador a solicitar a conversão de tipos explícitamente, faz com que ele perceba que há um custo envolvido.

Eu fiz uso de outra linguagem fortemente tipada, uma pequena linguagem chamada Whimsical, criada por John Spray. Apesar de Whimsical ser uma linguagem voltada para programação de sistemas, ela requer conversão explícita SEMPRE. Nunca é feita uma conversão automática, mesmo aquelas suportadas por Pascal.

Esta abordagem possui certas vantagens: o compilador nunca precisa adivinhar o que fazer: o programador sempre deve dizer precisamente o que ele quer. Como resultado, a tendência é que exista uma correspondência de um-para-um entre o código-fonte e o código-objeto, e o compilador de John produzia código muito robusto.

Por outro lado, eu acho que as conversões explícitas podem ser dolorosas. Se eu quero, por exemplo, adicioner 1 a um caracter, ou aplicar uma operação "AND" com uma máscara, há uma série de conversões a serem feitas. Se eu fizer algo de errado, a única mensagem de erro é "Tipos não são compatíveis". Acontece que, a implementação de John da linguagem em seu compilador não diz exatamente quais tipos não são compatíveis... apenas diz em que LINHA o erro ocorreu.

Eu devo admitir que a maioria dos meus erros com este compilador são erros deste tipo, e eu perdi muito tempo com o compilador de Whimsical, tentando descobrir ONDE eu estava errando. A única maneira de arrumar o erro é continuar tentando algo até que alguma coisa funcione.

Então o que devemos fazer em TINY e KISS? Para o primeiro, eu tenho a resposta: TINY vai suportar apenas os tipos caracter e inteiro, e nós vamos usar o truque de C para promover caracteres a inteiros internamente. Isto significa que o compilador TINY será MUITO mais simples do que o que já fizemos. A conversão de tipos em expressões é muito simples, já que nenhuma conversão será necessária! Já que o tipo longo não será suportado, também não vamos precisar das rotinas MUL32 e DIV32, nem a lógica para descobrir quando chamá-las. Eu GOSTO disto!

KISS, por outro lado, vai suportar o tipo longo.

Deve suportar aritmética com e sem sinal? Por razões de simplicidade eu preferiria que não. Isto adiciona uma certa complexidade à conversão de tipos. Mesmo Niklaus Wirth eliminou os tipos sem sinal (Cardinais) de sua nova linguagem Oberon, com o argumento de que inteiros de 32-bits devem ser grandes o suficiente para todos.

Mas KISS supostamente é uma linguagem de programação de sistemas, o que significa que deveríamos poder fazer qualquer operação que pode ser feita em assembly. Já que o processador 80x86 suporta os dois tipos de operações com inteiro, eu acho que KISS também deve. Já vimos que operações lógicas devem ser estendidas como inteiros sem sinal, portanto as conversões sem sinal são necessárias de qualquer forma.

Conclusão
---------

Isto encerra nossa seção sobre conversão de tipos. Sinto muito por ter feito você esperar tanto por ela, mas eu acho que você sente que valeu a pena.

Nos próximos capítulos, pretendo estender os tipos simples e incluir matrizes e ponteiros, e dar uma olhada sobre o que fazer a respeito de strings de caracteres. Isto deve acrescentar muito ao núcleo desta série. Depois disto, vou lhes apresentar as novas versões dos compiladores de TINY e KISS, e então vamos tratar de questões de otimização.

Até lá!

{% include footer.md %}