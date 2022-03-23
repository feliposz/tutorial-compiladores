Parte 15: De volta para o futuro
================================

> **Autor:** Jack W. Crenshaw, Ph.D. (05/03/1994)<br>
> **Tradução e adaptação:** Felipo Soranz (06/06/2002)

Será que realmente se passaram quatro anos desde que eu escrevi o [décimo quarto capítulo](14_tipos.md) desta série? Será possível que seis anos se passaram desde que eu [comecei](01_introducao.md)? Engraçado como o tempo voa quando você está se divertindo, não?

Não vou perder muito tempo com desculpas; apenas lembrar que coisas acontecem, e prioridades mudam. Nos quatro anos desde o capítulo quatorze, eu consegui ser demitido, divorciar-me, ter um colapso nervoso, começar uma nova carreira como escritor, começar outra como consultor, mudar de casa, trabalhar em dois sistemas de tempo real, criar quatorze passarinhos, três pombos, seis [possums](https://pt.wikipedia.org/wiki/Phalangeriformes), e um pato. Por um tempo então, a análise e processamento de código fonte não esteve muito alta na minha lista de prioridades. Nem mesmo escrever coisas de graça, ao invés de ser pago para isto. Mas eu tento ser fiel, e reconhecer a responsabilidade que eu sinto com relação a você, o leitor, de terminar o que comecei. Como a tartaruga disse em uma das velhas histórias para o meu filho: eu posso ser lento, mas seguro. Eu tenho certeza de que há pessoas ansiosas para ver o último rolo deste filme, e eu pretendo lhes mostrar isto. Então, se você é um destes que esteve aguardando, mais ou menos pacientemente, para ver a coisa chegar a um fim, obrigado pela paciência. Eu peço desculpas pela demora. Vamos continuar.

Novo começo, Mesma direção
--------------------------

Como muitas outras coisas, linguagens e estilos de programação mudam com o tempo. Em 1994, parece um pouco deslocado estar programando em Turbo Pascal (linguagem em que se baseava o tutorial original), quando o resto do mundo parece estar voltado à programação em C++. Parece estranho também programar no estilo clássico enquanto todo mundo está usando métodos orientados a objeto. Mesmo assim, apesar do atraso de quatro anos, seria muito complicado mudar neste ponto para C++ com orientação a objeto.

>*NOTA DE TRADUÇÃO:* O autor faz menção às vantagens de usar Turbo Pascal ao invés de linguagem C na implementação de um compilador "educacional". Como temos visto até aqui, não tivemos muita dificuldade em trabalhar com C. Eu pensei se deveria me manter fiel ao original, mas acabei decidindo não incluir a discussão completa, por não acrescentar muita coisa ao objetivo do tutorial. Para quem tiver interesse, o texto original está disponível em: <https://compilers.iecc.com/crenshaw/tutor15.txt>

Eu disse desde o princípio que o propósito do tutorial não era gerar o compilador mais rápido, mas ensinar os fundamentos da tecnologia dos compiladores, sem perder muito tempo com a sintaxe da linguagem ou outros aspectos da implementação de software.

Eu pensei muito a respeito de introduzir métodos orientados a objeto em nossa discussão. Eu sou um grande defensor da orientação a objeto para todos os usos, e tais métodos definitivamente tem seu lugar na tecnologia de compiladores. Na verdade, eu escrevi artigos justamente sobre estes assuntos (consulte as referências no final). Mas a arquitetura de um compilador baseado em abordagens orientadas a objeto é bem diferente do compilador mais clássico que estivemos construindo. Novamente, parece que seria demais trocar a base de tudo no meio do caminho. Como eu já disse, os estilos de programação mudam. Quem sabe, pode levar mais uns seis anos antes de terminarmos esta série, e se continuarmos mudando o código toda vez que o estilo de programação mudar, pode ser que NUNCA terminemos.

Ao menos por enquanto, eu decidi continuar com o estilo clássico, embora possamos discutir objetos e orientação a objetos enquanto avançamos.

Desta vez vou fazer uso extensivo de compilação separada em módulos. No passado, procuramos manter o tamanho e a complexidade do código baixos. Muito trabalho foi feito na forma de experiências diretas no computador, observando apenas um aspecto da tecnologia de compiladores de cada vez. Fizemos isto para evitar ter que carregar programas grandes para discutir apenas conceitos simples. No processo, nós reinventamos a roda e reprogramamos as mesmas funções mais vezes do que eu gostaria de contar. Compilando os módulos separadamente teríamos a funcionalidade e simplicidade que desejamos ao mesmo tempo: você escreve código reutilizável, e faz uso dele numa única linha. Seu programa de testes continua pequeno, mas ele pode fazer coisas mais poderosas.

Agora porém, cada módulo separado que precisa de uma inicialização terá sua própria versão de `Init()`. As diversas rotinas que possuíamos antes no "berço" serão agora distribuídas em diversos módulos.

A interface dos módulos em C é feita via pré-processador e comandos para incluir os arquivos de cabeçalho que contém as declarações dos protótipos e estruturas necessárias. O processo para compilar e juntar os módulos é muito dependente do ambiente de desenvolvimento que está sendo usado, portanto não vou tratar das questões específicas de cada compilador aqui. Se você estiver usando um compilador GCC ou equivalente, é possível fazer o seguinte.

~~~sh
gcc -c modulo1.c
gcc -c modulo2.c
gcc -o executavel modulo1.o modulo2.o
~~~

É claro que é muito mais simples criar um "Makefile" para fazer o trabalho para você, mas isto seria um assunto extenso demais para discutirmos aqui. Deixo a seu cargo escolher o melhor processo para o seu ambiente de desenvolvimento.

Começando outra vez?
--------------------

Quatro anos atrás, no [capítulo 14](14_tipos.md), eu prometi que nossos dias de reinventar a roda, e recodificar as mesmas rotinas para cada lição haviam acabado, e que daí em diante iríamos permanecer com programas mais completos aos quais iríamos apenas adicionar características. Eu ainda pretendo manter esta promessa; e este é um dos principais motivos de usarmos módulos separados. Porém, graças ao longo tempo desde o capítulo 14, é natural querer ao menos alguma revisão, e de qualquer forma teríamos que fazer mudanças no código para fazer a transição para unidades. Além disso, francamente, após todo este tempo eu não consigo me lembrar das grandes idéias que eu tinha há quatro anos. A melhor forma de me lembrar delas é refazer alguns dos passos que demos até o capítulo 14. Então eu espero que você compreenda e continue comigo conforme voltamos às raízes, de certa forma, e reconstruímos o núcleo do software, distribuindo as rotinas nas várias unidades, e retornando ao ponto em que estivemos quando paramos da última vez. Da mesma forma de sempre, você vai acompanhar todos os enganos e mudanças de direção em "tempo real". Por favor, continue comigo... vamos começar a ver as novidades antes que você perceba.

Já que vamos usar múltiplos módulos em nossa nova abordagem, temos que tratar da questão de organização dos arquivos. Se você acompanhou todas as outras seções deste tutorial, você vai saber que, conforme nossos programas evoluem, vamos ter que trocar nossas unidades antigas e simplísticas por outras mais capacitadas. Isto nos leva a um problema de controle de versão. Certamente haverá tempos em que vamos trocar as unidades mais simples, porém mais tarde vamos desejar tê-las de volta outra vez. Um caso a ser notado é a nossa predileção por usar nomes de variáveis e palavras-chave de um só caracter, para testar conceitos sem nos enroscarmos com os detalhes do analisador léxico. Graças ao uso de módulos, vamos ter que fazer isto cada vez menos no futuro. Eu não só suspeito, mas estou certo de que vamos precisar guardar versões antigas dos arquivos, para ocasiões especiais, mesmo que eles tenham sido trocados por versões mais poderosas.

Para tratar deste problema, eu sugiro que você crie diretórios diferentes, com versões diferentes das unidades conforme necessário. Se fizermos isto corretamente, o código em cada diretório vai continuar consistente. Provisoriamente, eu criei quatro diretórios: SINGLE (para experimentos com um só caracter por token), MULTI (obviamente, para as versões multi-caracter), TINY e KISS.

Já tendo falado bastante sobre a filosofia e os detalhes. Vamos continuar reavivando o software.

O Módulo de Entrada
-------------------

Um conceito chave que temos usado desde o primeiro dia é a idéia de entrada com um caracter "lookahead". Todas as rotinas examinam este caracter, sem alterá-lo, para decidir o que devem fazer em seguida. (Compare esta abordagem com a abordagem padrão em C/Unix que seria usar simplesmente `getchar` e `ungetc`, e eu acho que você vai concordar que esta abordagem é mais simples). Nós vamos começar nossa viagem traduzindo este conceito em nossa nova organização por módulos. A primeira unidade, apropriadamente chamada `input`, é mostrada abaixo. Repare que ela está dividida entre o cabeçalho (que é a interface) e o arquivo de implementação que define as funções. Usarei esta abordagem para todos os outros módulos.

Crie o arquivo [input.h](src/cap15/SINGLE/input.h):

~~~c
{% include_relative src/cap15/SINGLE/input.h %}
~~~

Crie o arquivo [input.c](src/cap15/SINGLE/input.c):

~~~c
{% include_relative src/cap15/SINGLE/input.c %}
~~~

Como você pode ver, não há nada muito profundo nem complicado neste módulo, já que ele possui apenas dois simples procedimentos. A única coisa a ser feita antes de usarmos este módulo é chamar o procedimento `InitInput()`.

Crie esta unidade no seu ambiente de desenvolvimento. Para testar o software, precisamos do programa principal, é claro. Eu usei o seguinte programa, que em breve vai evoluir para o programa principal do nosso compilador.

Crie o arquivo `main.c`:

~~~c
#include <stdio.h>
#include "input.h"

/* PROGRAMA PRINCIPAL */
int main()
{
    InitInput();
    printf("%c\n", look);

    return 0;
}
~~~

Note a inclusão do cabeçalho `input.h` na nossa rotina. Se você não fizer isto terá um erro de compilação. Não esqueça de juntar os módulos depois da compilação separada, para obter o programa executável.

Repare que podemos acessar a variável `look` mesmo não a tendo declarado em nosso programa. Se estivéssemos escrevendo o programa numa forma orientada a objeto, não deveríamos permitir que módulos externos tivessem acesso a variáveis internas, mas para o nosso caso a abordagem é apropriada.

Eu devo lembrar, como eu já fiz antes, que a função do módulo de entrada é, e sempre foi, considerada como uma simplificação de um módulo real. Em um compilador de produção, a entrada será proveniente de um arquivo ao invés do teclado. E certamente isto vai incluir um buffer para melhorar a eficiência de entradas do disco. A melhor parte dos módulos separados, como na própria orientação a objetos em si, é que podemos modificar o código em uma unidade para que ele seja tão simples ou tão sofisticado quanto necessário. Desde que a interface não mude, o resto do programa permanece inalterado. E já que os módulos são compilados, ao invés de meramente incluídos, o tempo necessário para juntá-los com o ligador (*linker*) é basicamente zero. O resultado é que podemos ter todos os benefícios de implementações sofisticadas, sem ter que carregar o código todo como bagagem.

Futuramente poderíamos prover uma IDE completa para o compilador KISS. Por enquanto, porém, vamos obedecer a regra número 1: Mantenha Simples! (*Keep It Simple!*)

O Módulo de Saída
-----------------

É claro, que todo programa decente deve ter saída, e o nosso não é exceção. Nossa rotinas de saída consistem na função `EmitLn()`. O código para o módulo de saída correspondente é:

Crie o arquivo [output.h](src/cap15/SINGLE/output.h):

~~~c
{% include_relative src/cap15/SINGLE/output.h %}
~~~

Crie o arquivo [output.c](src/cap15/SINGLE/output.c):

~~~c
{% include_relative src/cap15/SINGLE/output.c %}
~~~

(Repare que esta rotina não requer nenhum código de inicialização.)

Teste a nova unidade com o seguinte programa (altere o arquivo `main.c`):

~~~c
#include <stdio.h>
#include "input.h"
#include "output.h"

/* PROGRAMA PRINCIPAL */
int main()
{
    InitInput();
    printf("%c\n", look);
    EmitLn("Hello, World!");

    return 0;
}
~~~

Você viu alguma coisa surpreendente? Talvez você tenha ficado surpreso pelo fato de ter que digitar alguma coisa, mesmo que o programa principal não esteja requisitanto nenhuma entrada. É por causa da inicialização do módulo de entrada. Bem, este é um problema que só vai ocorrer em casos simples de teste como este, portanto não precisamos fazer nada com relação a isto.

Repare também, que a instrução foi deslocada corretamente pela nossa tabulação.

O Módulo de Erros
-----------------

Nosso próximo conjunto de rotinas está relacionado ao tratamento de erros. Para refrescar sua memória, nós usamos a abordagem de parar no primeiro erro encontrado. Não só isto simplifica nosso código, evitando completamente o problema de recuperação de erros, mas também faz mais sentido, na minha opinião, em um ambiente interativo. Eu sei que esta pode ser uma posição extremista, mas eu considero que a prática de avisar sobre todos os erros em um programa é apenas um reflexo dos dias do processamento em lote. Nesta série vamos ignorar esta prática.

Em nosso "berço" original, tínhamos dois procedimentos para tratar erros: um que apenas mostrava o erro e outro que mostrava e saía do programa. Eu nunca achei um bom uso para o procedimento que simplesmente mostrava o erro, ficaremos com a que aborta o programa.

Crie o arquivo [errors.h](src/cap15/SINGLE/errors.h):

~~~c
{% include_relative src/cap15/SINGLE/errors.h %}
~~~

Crie o arquivo [errors.c](src/cap15/SINGLE/errors.c):

~~~c
{% include_relative src/cap15/SINGLE/errors.c %}
~~~

Como de costume, aí vai um programa de teste (altere o arquivo `main.c`):

~~~c
#include <stdio.h>
#include "input.h"
#include "output.h"
#include "errors.h"

/* PROGRAMA PRINCIPAL */
int main()
{
    char name = 'A';
    Error("'%c' is not a variable", name);

    return 0;
}
~~~

Você reparou que a lista de "includes" no nosso programa principal só vem crescendo? Tudo bem. Na versão final, o programa principal só vai fazer chamadas ao analisador sintático, portanto só teremos algumas entradas "include". Mas por enquanto é melhor incluirmos todas para podermos testar as rotinas.

Análise Léxica e Sintática
--------------------------

A arquitetura clássica dos compiladores consiste em módulos separados para o analisador léxico, que provê os tokens na linguagem, e o analisador sintático, que tenta fazer dos tokens elementos da sintaxe. Se você ainda se lembrar do que fizemos em capítulos anteriores, você vai perceber que não fizemos nada desta forma. Por estarmos usando um analisador preditivo, podemos quase sempre dizer qual elemento da linguagem vem em seguida, simplesmente analisado o caracter "lookahead". Portanto, não encontramos nenhuma necessidade de pré-carregar tokens, como faria um *scanner* (ou *lexer*, ambos termo em inglês para "analisador léxico").

Porém, mesmo não havendo um procedimento chamado "scanner", faz sentido separar as funções do analisador léxico das funções do analisador sintático. Portanto eu criei mais duas unidades chamadas, sem nenhuma novidade, "scanner" e "parser". A unidade "scanner" contém todas as rotinas conhecidas como reconhecedores. Algumas delas como `IsAddOp()` são simples rotinas booleanas que operam no caracter "lookahead". As outras rotinas são aquelas que coletam tokens, como identificadores e constantes numéricas. A unidade "parser" deve conter todas as rotinas que fazem parte do analisador descendente-recursivo. A regra geral deveria ser que a unidade "parser" contenha toda a informação que é específica para a linguagem; em outras palavras, toda a sintaxe da linguagem deve estar contida em "parser". Em um mundo ideal, esta regra deveria ser verdadeira a ponto de podermos alterar o compilador para compilar uma linguagem diferente, simplesmente trocando o módulo de análise sintática.

Na prática, as coisas quase nunca são tão puras assim. Sempre há uma pequena parcela de regras sintáticas no analisador léxico também. Por exemplo, as regras que definem um identificador ou uma constante válida podem mudar de linguagem para linguagem. Em algumas linguagens, as regras a respeito de comentários permitem que eles sejam filtrados pelo analisador léxico, enquanto em outras não. Portanto, na prática, ambas unidades acabam tento componentes específicos da linguagem, mas as alterações necessárias ao analisador léxico devem ser relativamente triviais.

Lembre-se que fizemos uso de duas versões de rotinas de análise léxica: uma que tratava de tokens de um só caracter, que usamos para diversos testes, e outra que possuía suporte a tokens multi-caracter. Agora que temos nosso software separado em unidades, eu acho que não custa muito prover ambos. Eu criei duas versões da unidade de análise léxica. A primeira contém a versão de um só caracter:

Crie o arquivo [scanner.h](src/cap15/SINGLE/scanner.h):

~~~c
{% include_relative src/cap15/SINGLE/scanner.h %}
~~~

Crie o arquivo [scanner.c](src/cap15/SINGLE/scanner.c):

~~~c
{% include_relative src/cap15/SINGLE/scanner.c %}
~~~

O seguinte fragmento de código provê um bom teste para o analisador léxico (altere o arquivo `main.c`):

~~~c
#include <stdio.h>
#include "input.h"
#include "scanner.h"

/* PROGRAMA PRINCIPAL */
int main()
{
    InitInput();
    printf("Name: %c\n", GetName());
    Match('=');
    printf("Num: %c\n", GetNum());
    Match('+');
    printf("Name: %c\n", GetName());

    return 0;
}
~~~
    
Este código vai reconhecer todas as senteças da forma:

~~~
x=0+y
~~~

onde x e y podem ser quaisquer nomes de variáveis, e 0 qualquer dígito. O código deve rejeitar qualquer outra sentença, e dar uma mensagem de erro compreensível. Se foi o que aconteceu, então está tudo bem e podemos continuar.

O Módulo de Análise Léxica
--------------------------

A próxima, é de longe, a mais importante versão do nosso analisador léxico: é a que trata dos tokens multi-caracter, os quais toda linguagem real deve ter. Apenas duas funções, `GetName()` e `GetNum()`, foram alteradas entre as duas unidades.

Já que esta já faz parte da nossa versão multi-caracter, convém copiar todos os arquivos para um diretório diferente (eu escolhi os nomes [SINGLE](https://github.com/feliposz/tutorial-compiladores/tree/master/src/cap15/SINGLE) e [MULTI](https://github.com/feliposz/tutorial-compiladores/tree/master/src/cap15/MULTI).

Copie o código da unidade anterior OBSERVANDO a mudança dos nomes das unidade e adicione as rotinas abaixo:

~~~c
/* retorna um identificador */
void GetName(char *name)
{
    int i;
    
    if (!isalpha(look))
        Expected("Name");
    for (i = 0; isalnum(look); i++) {
        if (i >= MAXNAME)
            Error("Identifier too long.");
        name[i] = toupper(look);
        NextChar();
    }
    name[i] = '\0';
}

/* retorna um número */
void GetNum(char *num)
{
    int i;
    
    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look); i++) {
        if (i >= MAXNUM)
            Error("Integer too large.");
        num[i] = look;
        NextChar();
    }
    num[i] = '\0';
}
~~~

> Não esqueça de alterar a definição no arquivo de cabeçalho `scanner.h` e de adicionar um valor para as constante MAXNAME e MAXNUM.

> Versões multi-caracter de [`scanner.h`](src/cap15/MULTI/scanner.h) e [`scanner.c`](src/cap15/MULTI/scanner.c)

O mesmo programa de teste, com pequenas modificações pode testar esta versão também. Substitua a inclusão do cabeçalho de `scanner1.h` por `scanner.h`. 

Altere o arquivo `main.c`:

~~~c
#include <stdio.h>
#include "input.h"
#include "scanner.h"

/* PROGRAMA PRINCIPAL */
int main()
{
    char name[MAXNAME+1], num[MAXNUM+1];

    InitInput();

    GetName(name);
    printf("Name: %s\n", name);

    Match('=');

    GetNum(num);
    printf("Num: %s\n", num);

    Match('+');

    GetName(name);
    printf("Num: %s\n", name);

    return 0;
}
~~~

Agora você poder usar nomes e números multi-caracter:

~~~
alfa=123+beta
~~~

Decisões, decisões
------------------

Apesar da relativa simplicidade dos dois analisadores, foi preciso pensar muito para chegar neles, e muitas decisões foram tomadas. Eu gostaria de compartilhar algumas destas idéias com você para que você possa tomar suas próprias decisões, apropriadas para a sua aplicação. Primeiro, note que as duas versões de `GetName()` convertem os caracteres de entrada para letras maiúsculas. Obviamente, houve uma decisão feita aqui, e é um dos casos em que a sintaxe da linguagem acaba envolvendo o analisador léxico também. A linguagem C é sensível a diferenças de letras maísculas e minúsculas. Para uma linguagem como esta, obviamente não podemos converter todos os caracteres para maiúscula. O projeto que estou usando assume uma linguagem como Pascal, onde maísculas e minúsculas não tem diferença. Para tais linguagens, é mais fácil fazer esta conversão no analisador léxico, para que não tenhamos que nos preocupar mais tarde com comparação de strings.

Poderíamos ter dado um passo além, e mapear os caracteres para maíscula no momento em que são lidos, em `NextChar()`. Esta abordagem funciona também, e eu já a usei uma vez, mas ela é muito restritiva. Especificamente, ela acaba convertendo também caracteres que podem fazer parte de uma string entre aspas, o que não é uma boa idéia. Portanto, se você pretende converter para maiúsculas mesmo, `GetName()` é o lugar apropriado para fazer isto.

Repare que a função `GetNum()` neste analisador retorna o resultado em uma string, da mesma forma que `GetName()`. Esta é outra das coisas em que tivemos bastante oscilação. A alternativa seria, como já fizemos em vários outras capítulos", retornar o resultado como um valor inteiro.

As duas abordagens tem pontos positivos. Já que estamos tratando de um número, a abordagem que vem à mente imediatamente é retornar o número como um inteiro. Mas lembre-se que o eventual uso do número será num comando de escrita que será usado no mundo exterior. Alguém - nós mesmo ou o código dentro do comando de escrita - vai ter que converter o número de volta a uma string novamente. Há rotinas para fazer tais conversões é claro, mas por que usá-las se não é realmente necessário? Por que converter um número de uma string para um inteiro para convertê-lo novamente, apenas alguns comandos mais tarde, na sua forma string?

Além disso, como você logo vai ver, vamos precisar de um local temporário para armazenar o valor do token que acabamos de ler. Se armazenarmos o número na forma de string, podemos armazenar o nome de uma variável ou um número na mesma string. De outra forma, teríamos que criar uma segunda variável de tipo inteiro.

Por outro lado, vamos verificar que carregar o número como uma mera string virtualmente elimina qualquer chance de otimização mais tarde. Quando chegarmos ao ponto em que começamos a nos preocupar com geração de código, vamos encontrar casos em que estaremos fazendo aritmética com constantes. Para estes casos, é realmente um desperdício gerar código para efetuar a aritmética de constantes em tempo de execução. É muito melhor permitir que o analisador faça a aritmética em tempo de compilação, e então simplesmente codificar o resultado. Para fazer isto, iríamos desejar ter as constantes armazenadas como inteiros ao invés de strings.

O que finalmente me levou de volta à abordagem com strings foi a aplicação agressiva do teste KISS, além de lembrar que estamos cuidadosamente evitando problemas de eficiência de código. Uma das coisas que faz com que nossa análise simplificada funcione, sem as complexidades de um compilador "real", é não nos preocuparmos com a eficiência do código. Isto nos dá muita liberdade para fazer as coisas do jeito mais fácil ao invés de fazê-las da forma mais eficiente, e esta é uma liberdade que devemos manter cuidadosamente sem abandoná-la voluntariamente, apesar do desejo por eficiência gritando em nossos ouvidos. Além de ser um grande seguidor da filosofia KISS, eu também defendo o princípio da "programação preguiçosa", que neste contexto significa não programar qualquer coisa até que seja necessário. Como dizia P.J.Plauger, "Nunca deixe para amanhã o que você pode adiar indefinidamente!". Através dos tempos, muito código foi escrito para lidar com eventualidades que nunca aconteceram. Eu aprendi esta lição, através de uma experiência amarga. Para resumir: nós não vamos converter para um inteiro aqui pois não temos que fazer isto. É simples assim.

>**Nota de tradução:** Atualmente, isto é conhecido como princípio [YAGNI](https://pt.wikipedia.org/wiki/YAGNI) (*You Ain't Gonna Need It* - Você não vai precisar disto)

Para aqueles de vocês que acham que vamos precisar da versão inteira (e de fato talvez precisemos), aqui está ela:

~~~c
/* retorna um número */
long GetNum()
{
    long num = 0;
    
    if (!isdigit(look))
        Expected("Integer");
    do {
        num *= 10;
        num += look - '0';
        NextChar();
    } while (isdigit(look));
    
    return num;
}
~~~

Você deve mantê-la separada, para um dia chuvoso. :)

Análise Sintática
-----------------

Neste ponto, distribuímos todas as rotinas que fizeram parte de nosso "berço" em unidades das quais podemos fazer uso conforme necessárias. Obviamente, elas vão evoluir conforme continuamos com o processo, mas para a maior parte seu conteúdo e arquitetura estão definidos. O que falta agora é incorporar a sintaxe da linguagem na unidade de análise sintática. Não vamos fazer a maior parte disto neste capítulo, mas eu pretendo fazer um pouco, apenas para continuar com o sentimento de que sabemos o que estamos fazendo. Então, antes de continuar, vamos gerar o suficiente de um analisador para tratar de fatores simples em uma expressão. No processo, por uma questão de necessidade, vamos criar uma unidade de geração de código também.

Você se lembra do primeiro capítulo desta série? Nós lemos um valor inteiro, digamos 1, e geramos o código para carregá-lo no registrador AX por uma atribuição imediata:

~~~asm
    MOV AX, 1
~~~

Logo em seguida, repetimos o processo para uma variável,

~~~asm
    MOV AX, [VAR]
~~~

e então para um fator que poderia ser uma constante ou uma variável. Vamos rever o processo. Defina o seguinte módulo:

Crie o arquivo [parser.h](src/cap15/MULTI/parser.h):

~~~c
{% include_relative src/cap15/MULTI/parser.h %}
~~~

Crie o arquivo [parser.c](src/cap15/MULTI/parser.c):

~~~c
#include "scanner.h"
#include "codegen.h"
#include "parser.h"

/* analisa e traduz um fator matemático */
void Factor()
{
    char num[MAXNUM+1];

    GetNum(num);
    AsmLoadConst(num);
}
~~~

Como você pode ver, esta unidade chama um procedimento, `AsmLoadConst()`, que efetivamente emite o código assembly. Este módulo também faz uso de uma nova unidade, "codegen". Este passo representa a última "grande" mudança em nossa arquitetura, dos capítulos anteriores: a remoção do código dependente de máquina para um módulo separado.

Para aqueles de vocês que desejam usar uma arquitetura diferente da que estou usando (80x86, 16-bits, DOS), aqui vai a resposta: simplesmente troque o código de "codegen" por outro que sirva à sua CPU favorita.

Até aqui, nosso gerador de código só tem um procedimento. Aqui está o módulo:

Crie o arquivo [codegen.h](src/cap15/MULTI/codegen.h):

~~~c
#ifndef _CODEGEN_H
#define _CODEGEN_H

void AsmLoadConst(char *s);

#endif
~~~

Crie o arquivo [codegen.c](src/cap15/MULTI/codegen.c):

~~~c
#include "output.h"
#include "codegen.h"

/* carrega uma constante no registrador primário */
void AsmLoadConst(char *s)
{
    EmitLn("MOV AX, %s", s);
}
~~~

Compile e teste esta unidade com o seguinte programa principal:

~~~c
#include "input.h"
#include "parser.h"

/* PROGRAMA PRINCIPAL */
int main()
{
    InitInput();
    Factor();

    return 0;
}
~~~

Ai está, o código está sendo gerado conforme esperado.

Agora, eu espero que você possa ver a vantagem do nosso novo projeto baseado em unidades separadas. Agora temos um programa principal que possui cinco linhas de tamanho. É tudo o que precisamos ver no programa, a menos que queiramos ver mais. E mesmo assim, todas as outras unidades estão lá, esperando para nos ajudar. Agora temos um código simples e curto, mas aliados poderosos. O que precisa ser feito agora é adicionar códigos às unidades para combinar com as capacidades desenvolvidas nos capítulos anteriores. Nós vamos fazer isto no próximo capítulo, mas antes de terminar, vamos terminar a análise de um fator, apenas para ficarmos satisfeitos de que ainda sabemos como fazê-lo. A versão final de "codegen" inclui o novo procedimento `AsmLoadVar()`. Inclua este novo procedimento na implementação do módulo (e não esqueça de acrescentá-lo no cabeçalho também):

~~~c
/* carrega uma variável no registrador primário */
void AsmLoadVar(char *s)
{
    EmitLn("MOV AX, [%s]", s);
}
~~~

O módulo de análise em si não muda, mas temos uma versão mais complexa de `Factor()`:

~~~c
#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* analisa e traduz um fator matemático */
void Factor()
{
    char name[MAXNAME+1], num[MAXNUM+1];

    if (isdigit(look)) {
        GetNum(num);
        AsmLoadConst(num);
    } else if (isalpha(look)) {
        GetName(name);
        AsmLoadVar(name);
    } else
        Error("Unrecognized character: '%c'", look);
}
~~~

Agora, sem alterar o programa principal, você vai verificar que nosso programa processa tanto variáveis como fatores constantes. Neste ponto, nossa estrutura está quase completa; já temos módulos para fazer todo o trabalho sujo, e código suficiente no analisador sintático e no gerador de código para demonstrar que tudo funciona. O que falta é adicionar funcionalidade às unidades que definimos, particularmente o analisador sintático e o gerador de código, para suportar os elementos mais complexos da sintaxe que fazem uma linguagem real. Já que tratamos disto muitas vezes antes em capítulos anteriores, não deve levar muito tempo para que alcancemos o ponto em que estávamos antes desta longa pausa. Vamos continuar o processo no [capítulo 16](16_construcao_dos_modulos.md). Até lá.

{% include footer.md %}