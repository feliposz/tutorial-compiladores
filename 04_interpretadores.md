4 - Interpretadores
===================

> **Autor:** Jack W. Crenshaw, Ph.D. (24/07/1988)<br>
> **Tradução e adaptação:** Felipo Soranz (15/05/2002)

Nos primeiros 3 capítulos desta série, tratamos de analisar e compilar expressões matemáticas. Traçamos nosso caminho de forma gradual e metódica, lidando apenas com caracteres simples, de expressões de um único termo até outras mais gerais. Finalmente completamos o analisador de forma que ele pudesse analisar sintaticamente e traduzir comandos de atribuição completos, com tokens de vários caracteres, separados por espaços, e chamadas de função. Desta vez, vamos passar pelo processo novamente, mas agora com o objetivo de interpretar, ao invés de compilar o código.

Como esta é uma série sobre compiladores, por que deveríamos nos importar com interpretadores? Apenas pra que você entenda como a natureza do analisador sintático muda conforme mudamos os objetivos. Eu também quero unificar os conceitos de dois tipos de tradutores, de forma que você possa não só ver as diferenças, mas também as similaridades.

Considere o seguinte código:

    x = 2 * y + 3

Em um compilador, queremos que a CPU alvo execute esta atribuição em tempo de EXECUÇÃO. O tradutor em si não faz a aritmética... ele apenas cria o código objeto que vai fazer com que a CPU faça isto quando o código for executado. Para o exemplo acima, o compilador vai gerar código que calcule a expressão e guarde o resultado na variável X.

Para um interpretador, por outro lado, nenhum código objeto é gerado. Ao contrário, a aritmética é computada imediatamente, conforme a análise ocorre. No momento em que a análise do comando estiver completa, X vai ter um novo valor.

A abordagem que temos tomado durante toda a série é chamada de "tradução dirigida pela sintaxe". Como você já deve saber, a estrutura do analisador é muito parecida com a sintaxe das instruções que analisamos. Nós criamos rotinas em C que reconhecem construções da linguagem. Associadas a cada uma destas construções (e rotinas) existem "ações" correspondentes, que fazem o que está de acordo com a construção que foi reconhecida. Em nosso compilador, cada ação envolve emitir código objeto, para ser executado mais tarde, durante o tempo de execução. Em um interpretador, cada ação envolve algo a ser feito imediatamente.

O que eu gostaria de tentar fazer é não mudar muito a estrutura do analisador. Apenas as ações mudam. Então, se você consegue escrever um interpretador para uma dada linguagem, você também pode escrever um compilador, e vice-versa. Porém, como você vai ver, EXISTEM diferenças, e significantes. Como as ações são diferentes, as rotinas que fazem o reconhecimento acabam sendo escritas de forma diferente. Especialmente no interpretador, as rotinas de reconhecimento serão codificadas mais como funções que retornam valores numéricos do que como procedimentos que não retornam nada. Praticamente nenhuma de nossas funções do analisador fazia isso.

Nosso compilador, de fato, é o que podemos chamar de compilador "puro". Cada vez que uma construção é reconhecida, o código objeto é gerado IMEDIATAMENTE. (Esta é uma das razões do código não ser muito eficiente.) O interpretador que vamos construir aqui é um interpretador puro, no sentido de que não há "compilação" alguma sendo feita no código fonte. Isto representa 2 extremos na tradução. No mundo real, os tradutores raramente são tão "puros", mas tendem a ter um pouco de cada técnica.

Eu não consigo pensar em muitos exemplos. Vou mencionar um: a maioria dos interpretadores, como o Microsoft BASIC, por exemplo, traduz o código fonte em uma forma intermediária pra que seja mais fácil de analisar e executar em tempo real.

Outro exemplo é um montador (assembler). O objetivo de um assembler, naturalmente é produzir código objeto de máquina, e normalmente faz isso em uma base de 1 pra 1: uma instrução de máquina por linha de código fonte. Mas a maioria dos montadores também permite expressões como argumentos. Neste caso, as expressões são sempre expressões constantes, e o montador não deveria gerar código para elas. Ao invés disso, ele "interpreta" a expressão e calcula o valor constante correspondente, que é o valor emitido como código objeto.

Bem, nós poderíamos fazer isto nós mesmos. O tradutor que nós construímos no capítulo anterior vai emitir código objeto de forma "impecável" para todo tipo de expressão complicada, mesmo que todos os termos da expressão sejam constantes. Neste caso seria muito melhor se o tradutor agisse mais como um interpretador e simplesmente calculasse a constante equivalente resultante.

Há um conceito em teoria de compiladores chamado "tradução preguiçosa" (lazy translation). A ideia é que você não emite código em cada ação. Em casos extremos você nem emite nada, até que você tenha certeza que precisa. Para fazer isso, as ações associadas com as rotinas de análise tipicamente não emitem código apenas. Algumas vezes elas o fazem, mas frequentemente elas apenas retornam informação para a rotina que fez a chamada. Armada com esta informação, a rotina pode então fazer uma escolha melhor sobre o que deve ser feito.

Por exemplo, dado o comando:

    x = x + 3 - 2 - (5 - 4)

Nosso compilador vai gerar uma saída de 20 ou mais instruções para carregar cada termo em registradores, fazer a aritmética e guardar o resultado. Uma avaliação mais preguiçosa iria reconhecer que a aritmética envolvendo constantes pode ser avaliada em tempo de compilação, e poderia ser reduzida para a expressão:

    x = x + 0

Uma avaliação mais preguiçosa ainda seria esperta o suficiente para descobrir que isto é equivalente a

    x = x

Que significa que não é necessária qualquer ação. Podemos reduzir 22 instruções a simplesmente nenhuma!

Note que não há chance de otimização com este método no nosso tradutor da forma como ele está, por que toda ação é executada imediatamente.

Avaliação "preguiçosa" de expressões pode produzir código com uma melhora significativa em relação ao que temos feito até agora. Eu devo avisar que: isto pode complicar o código do analisador consideravelmente, pois cada rotina deverá tomar decisões se deve emitir o código objeto ou não. A avaliação preguiçosa certamente não tem esse nome por ser mais fácil para o programador do compilador!

Uma vez que estamos operando principalmente com o princípio KISS aqui, eu não vou ir mais fundo neste assunto. Só quero que você fique ciente que é possível obter otimização de código combinando as técnicas de compilação e interpretação. Em particular, você deve saber que as rotinas do analisador em um tradutor esperto vão geralmente retornar valores e em outros devem esperar algo também. Esta é a razão principal para tratarmos de interpretação neste capítulo.

O Interpretador
---------------

Agora que você sabe PORQUE iremos ver tudo isso, vamos lá. Apenas para praticar vamos começar com um novo "berço" e criar o tradutor desde o começo. Desta vez podemos ir mais rápido, claro.

Como desta vez vamos fazer aritmética, a primeira coisa a fazer é alterar a função `GetNum()`, pois até agora ela estava retornando um caractere (ou uma string) e agora é melhor que ela retorne um inteiro. FAÇA UMA CÓPIA do [berço](src/cap01-craddle.c) (não altere o berço em si!) e modifique `GetNum()` como segue:

~~~c
/* Recebe um número inteiro */
int GetNum()
{
    char num;

    if (!isdigit(Look))
        Expected("Integer");
    num = Look;
    NextChar();

    return num - '0';
}
~~~

Agora escreva a seguinte versão de `Expression()`:

~~~c
/* Avalia o resultado de uma expressão */
int Expression()
{
    return GetNum();
}
~~~

Finalmente, insira o seguinte comando no programa principal:

~~~c
    printf("%d\n", Expression());
~~~

Então compile e teste.

Tudo o que este programa faz é "analisar" e traduzir uma simples "expressão" inteira. Como sempre, certifique-se de que ele faz isso com os dígitos de 0 a 9 e dá mensagens de erro com qualquer outra coisa. Não vai lhe custar muito tempo!

Vamos estender isso para incluir operadores de adição. Mude `Expression()` pra ficar assim:

~~~c
/* Reconhece operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* Avalia o resultado de uma expressão */
int Expression()
{
    int val;

    if (IsAddOp(Look))
        val = 0;
    else
        val = GetNum();

    while (IsAddOp(Look)) {
        switch (Look) {
            case '+':
                Match('+');
                val += GetNum();
                break;
            case '-':
                Match('-');
                val -= GetNum();
                break;
        }
    }

    return val;
}
~~~

A estrutura de `Expression()` combina com o que fizemos antes, então não vamos ter tanto problema depurando. Porém, houve um desenvolvimento SIGNIFICATIVO, não é mesmo? As rotinas `Add()` e `Subtract()` ficaram de fora. A razão é que a ação a ser tomada requer os dois argumentos da operação. Eu poderia ter escolhido manter as rotinas e passar o valor da expressão já calculado (que é o `val`) mas ficou mais claro pra mim que é melhor manter este valor como uma variável local, o que significa que o código de `Add()` e `Subtract()` deveriam ser colocados dentro da função. Isto significa que, apesar da estrutura que desenvolvemos ser boa e limpa para nosso esquema de tradução simplista, provavelmente não vai ser tão bom para a avaliação "preguiçosa" vista anteriormente. Isto é algo que é bom manter em mente pra mais tarde.

E então? O tradutor funcionou? Vamos para o próximo passo. Não deve ser difícil descobrir como a rotina `Term()` deveria ser agora. Troque as chamadas a `GetNum()` em `Expression()` por uma chamada a `Term()` e entre com o seguinte código:

~~~c
/* Avalia um termo */
int Term()
{
    int val;

    val = GetNum();
    while (IsMulOp(Look)) {
        switch (Look) {
            case '*':
                Match('*');
                val *= GetNum();
                break;
            case '/':
                Match('/');
                val /= GetNum();
                break;
        }
    }

    return val;
}
~~~

Agora faça o teste. Mas não se esqueça de duas coisas. Em primeiro lugar, estamos lidando com divisão de inteiros, portanto, 1/3 deve ser zero. Segundo, mesmo podendo emitir resultados de mais de um dígito nossa entrada ainda é limitada a apenas um único dígito.

Isto parece ser uma restrição boba neste ponto, logo que nós já sabemos como é simples estender a função `GetNum()`. Então vamos arrumá-la agora mesmo:

~~~c
/* Recebe um número inteiro */
int GetNum()
{
    int i;

    i = 0;

    if (!isdigit(Look))
        Expected("Integer");

    while (isdigit(Look)) {
        i *= 10;
        i += Look - '0';
        NextChar();
    }

    return i;
}
~~~

Se você compilou e testou esta versão do interpretador, o próximo passo é acrescentar a função `Factor()`, completa com as expressões entre parênteses. Por enquanto vamos ficar um pouco mais sem os nomes de variáveis. Primeiro, altere as referências a `GetNum()`, na função `Term()`, de forma que elas passem a ser referências a `Factor()`. Então crie a seguinte versão de `Factor()`:

~~~c
/* Avalia um fator */
int Factor()
{
    int val;

    if (Look == '(') {
        Match('(');
        val = Expression();
        Match(')');
    } else
        val = GetNum();

    return val;
}
~~~

Foi bem fácil, não foi? Estamos rapidamente chegando perto de um interpretador útil.

Um pouco de filosofia
---------------------

Antes de continuar, há algo que eu gostaria de ressaltar. É um conceito do qual temos feito uso todo esse tempo, mas eu não o mencionei explicitamente até agora. Eu acho que já é hora, pois é um conceito tão útil e tão poderoso, que faz toda a diferença entre um analisador que é trivialmente simples, e outro que é complicado demais.

No princípio da tecnologia de compiladores, as pessoas tinham problemas terríveis tentando descobrir como lidar com coisas como precedência de operadores... a maneira como operadores de multiplicação e divisão tem precedência sobre adição e subtração, etc. Eu lembro de um colega há uns 30 anos, quando ele estava contente por ter encontrado como fazer isto. A técnica envolvia criar 2 pilhas, nas quais você colocava cada operador e operando, associados com cada nível de precedência, e as regras ditavam que você só faria uma operação ("reduzindo" a pilha) se a ordem de precedência no topo da pilha estivesse correta. Para ficar mais interessante, um operador como ")" tinha níveis de precedência diferentes, dependendo do fato de ele já estar ou não na pilha. Você deve dar-lhe um valor antes de colocá-lo na pilha, e outro para decidir se deve tirá-lo. Só por curiosidade, eu decidi fazer isto por mim mesmo alguns anos atrás, e eu posso lhe afirmar que é bem complicado.

Nós não tivemos que fazer deste jeito. Na verdade até agora, analisar comandos de expressões matemáticas parece até brincadeira de criança. Como tivemos tanta sorte? E onde está a pilha de precedência?

Uma coisa parecida está acontecendo com nosso interpretador acima. Você simplesmente SABE que para que ele faça os comandos de cálculos aritméticos (ao contrário da análise acima), deve haver números colocados numa pilha em algum lugar. Mas onde está a pilha?

Finalmente, em livros-texto sobre compiladores, há uma diversidade de lugares onde pilhas e outras estruturas são discutidas. Em outros métodos de análise preditiva (LR), uma pilha explícita é usada. De fato, a técnica é muito parecida com a velha maneira como fazemos cálculos de expressões aritméticas. Outro conceito importante, é de uma árvore de análise sintática. Os autores gostam de desenhar diagramas de tokens de um comando, conectados em uma árvore de operadores nos nós internos. Mais uma vez, onde estão as árvores e pilhas na nossa técnica? Nós não vimos nenhuma. A resposta em todos os casos é que as estruturas estão implícitas, não explícitas. Em qualquer linguagem de computadores, há uma pilha envolvida toda vez que você faz uma chamada a uma sub-rotina. Sempre que uma sub-rotina é chamada, o endereço de retorno é colocado na pilha da CPU. No fim da sub-rotina, o endereço é tirado da pilha e o controle é devolvido no lugar onde parou. Em uma linguagem recursiva como Pascal, os dados locais da sub-rotina também podem ser colocados na pilha, e estes dados também retornam quando necessários.

Por exemplo, a função `Expression()` contém uma variável local chamada `val`, que é preenchida pela chamada a `Term()`. Suponha, que na próxima chamada a `Term()` para o segundo argumento da expressão, `Term()` chama `Factor()`, que faz uma chamada recursiva a `Expression()`. Esta nova "instância" de `Expression()` obtém outro valor para sua própria cópia de `val`. O que aconteceu com o primeiro `val`? Resposta: ele ainda está na pilha, e estará lá outra vez quando retornarmos da nossa sequência de chamadas.

Em outras palavras, a razão pela qual as coisas pareceram tão fáceis até agora, é que nós fizemos o uso máximo dos recursos da linguagem. Os níveis hierárquicos e a árvore de análise estão lá, corretos, mas estão escondidos na estrutura do analisador, e são tratados pela ordem como as várias rotinas são chamadas. Agora que você já viu como fizemos, é difícil de imaginar qualquer outra forma de fazê-lo. Mas eu posso lhe dizer que levou muitos anos para os programadores de compiladores se darem conta disso. Os primeiros compiladores eram complexos demais para imaginar. Engraçado como as coisas ficam fáceis com um pouco de prática.

As razões pelas quais eu trouxe isto à tona são uma lição e um aviso. A lição: as coisas podem ser fáceis quando você as faz certas. O aviso: preste atenção no que você está fazendo. Se, quando você estiver pesquisando por si só, você começar a encontrar uma razão real para ter uma pilha ou uma estrutura de árvore separada, talvez seja hora de se perguntar se você está olhando as coisas da forma correta. Talvez você não esteja fazendo uso efetivo das facilidades da linguagem tão bem quanto poderia.

O próximo passo é adicionar nomes de variáveis. Agora, porém, temos um pequeno problema. Para o compilador, não tivemos problemas em tratar do nome das variáveis... apenas deixamos o problema dos nomes para o montador e deixamos o resto do programa alocar espaço de armazenamento para elas. Aqui, por outro lado, temos que buscar os valores das variáveis e retorná-los como valores de `Factor()`. Nós temos que criar um mecanismo para armazenar estas variáveis.

Nos primórdios da computação pessoal, vivia o "Tiny BASIC". Ele tinha um grande total de 26 variáveis possíveis: uma para cada letra do alfabeto. Isto encaixa-se bem no nosso conceito de tokens de um só caractere, então vamos usar o mesmo truque. No começo de nosso interpretador, após a declaração de `Look`, insira as seguintes declarações:

~~~c
#define MAXVAR 26
int VarTable[MAXVAR];
~~~

Também precisamos inicializar a matriz, então adicione isto:

~~~c
/* Inicializa variáveis */
void InitVar()
{
    int i;

    for (i = 0; i < MAXVAR; i++)
        VarTable[i] = 0;
}
~~~

Adicione uma chamada a `InitVar()` em `Init()`. Não esqueça de fazer isto, ou o resultado pode surpreender você!

Agora que temos uma matriz de variáveis, podemos modificar `Factor()` para usá-la.  Como ainda não temos uma forma de alterar as variáveis, `Factor()` sempre vai retornar zero para o valor delas, mas vamos fazê-lo de qualquer forma. Aqui está a nova versão:

~~~c
/* Avalia um fator */
int Factor()
{
    int val;

    if (Look == '(') {
        Match('(');
        val = Expression();
        Match(')');
    } else if (isalpha(Look))
        val = VarTable[GetName() - 'A'];
    else
        val = GetNum();

    return val;
}
~~~

Como sempre, compile e teste esta versão do programa. Mesmo que todas as variáveis sejam zeros, ao menos podemos analisar e completar expressões corretamente, assim como identificar expressões mal-formadas também.

Eu acho que você já adivinhou o próximo passo: precisamos implementar um comando de atribuição para podermos colocar alguma coisa nas variáveis. Por enquanto, vamos continuar com uma linha apenas, mas logo estaremos lidando com diversos comandos.

O comando de atribuição combina com o que fizemos antes:

~~~c
/* Avalia um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    VarTable[name - 'A'] = Expression();
}
~~~

Para testar isto eu adicionei um comando temporário no programa principal pra mostrar o valor da variável A. Então eu testei com várias atribuições.

É claro que um interpretador que só aceita uma única linha de programa não é de muito valor. Portando vamos querer tratar de múltiplos comandos. Devemos simplesmente colocar um laço de repetição em `Assignment()`. Então vamos fazer isto, mas qual deve ser o critério de saída do laço? Ainda bem que você perguntou, porque isto nos faz reparar em algo que pudemos ignorar até agora.

Uma das coisas mais complicadas de lidar em qualquer tradutor é como determinar quando sair de uma determinada construção e procurar por outra coisa. Isto não foi um problema até agora pois nós só permitimos um tipo de construção... uma expressão ou um comando de atribuição. Quando começamos a adicionar laços e outros tipos de construções, temos que tomar cuidado para que as coisas terminem apropriadamente. Se colocarmos nosso interpretador em um laço de repetição, precisamos de um método para interrompê-lo. Terminar numa nova linha não é bom, pois isso é o que nos faz ir para a próxima linha de código. Podemos também fazer com que um caractere desconhecido nos coloque pra fora, mas isso vai fazer com que cada execução termine com uma mensagem de erro, e isso não parece legal.

O que nós precisamos é de um caractere delimitador. Eu voto para o ponto final (".") do Pascal. Uma complicação menor é que a linguagem C termina cada linha com um caractere de fim-de-linha ("\n"). No fim de cada linha precisamos eliminar estes caracteres antes de processar a próxima. Uma maneira natural de fazer isso seria com a rotina "match", exceto que "match" emite uma mensagem de erro com o caractere esperado, o que não ficaria tão bom com um "\n". O que precisamos é de uma rotina especial pra isso, que nós vamos usar sem dúvida várias vezes. Aqui está ela:

~~~c
/* Captura um caractere de nova linha */
void NewLine()
{
    if (Look == '\n')
        NextChar();
}
~~~

Insira esta rotina em um lugar conveniente no seu programa. Agora reescreva o programa principal desta forma:

~~~c
/* Programa principal */
int main()
{
    Init();
    do {
        Assignment();
        NewLine();
    } while (Look != '.');

    return 0;
}
~~~

Note que o teste para fim de linha se foi e agora não há testes de erro na rotina `NewLine()`. Tudo bem, porém, todo caractere deixado de lado, por ser um caractere estranho, vai ser pego como erro no começo do próximo comando de atribuição.

Bem, agora nós temos um interpretador funcional. Não é lá muito útil porém, uma vez que não há como ler dados do usuário ou mostrá-los. Certamente iria ajudar se tivéssemos entrada e saída!

Vamos completar isso, adicionando rotinas de entrada e saída. Como estamos mantendo os tokens de um único caractere, vou usar "?" para um comando de entrada e "!" para a saída, com um caractere seguindo imediatamente funcionando como uma lista de parâmetros de um token. Aqui estão as rotinas:

~~~c
/* Interpreta um comando de entrada */
void Input()
{
    char name;
    char buffer[20];

    Match('?');
    name = GetName();
    printf("%c? ", name);
    fgets(buffer, 20, stdin);
    VarTable[name - 'A'] = atoi(buffer);
}

/* Interpreta um comando de saída */
void Output()
{
    char name;

    Match('!');
    name = GetName();
    printf("%c -> %d\n", name, VarTable[name - 'A']);
}
~~~

Eles não são muito bacanas, eu admito... mas eles fazem o trabalho.

As mudanças correspondentes no programa principal são mostradas abaixo. Note que estamos usando um truque comum de um comando switch baseado no caractere "lookahead" atual, para decidir o que fazer.

~~~c
/* Programa principal */
int main()
{
    Init();
    do {
        switch (Look) {
            case '?':
                Input();
                break;
            case '!':
                Output();
                break;
            default:
                Assignment();
                break;
        }
        NewLine();
    } while (Look != '.');

    return 0;
}
~~~

Você agora completou um verdadeiro interpretador funcional. É meio limitado, mas funciona assim como os "grandões". Ele inclui 3 tipos de comandos de programação (e consegue diferenciá-los!), 26 variáveis e comandos de entrada e saída. A única coisa que falta, realmente, são estruturas de controle, sub-rotinas, e algum tipo de função de edição de programas. A parte de edição de programas eu vou deixar passar. Afinal, não estamos aqui para construir um produto, mas para aprender as coisas. Dos comandos de controle nós vamos tratar no próximo capítulo, e as sub-rotinas em breve. Estou ansioso para começar com isso, então vamos deixar o interpretador como está por enquanto.

Espero que, por enquanto, você esteja convencido de que as limitações de nomes de apenas um caractere, e o processamento de espaços em branco são fáceis de contornar, como fizemos na lição anterior. Desta vez, se quiser fazer algumas extensões, fique à vontade... elas ficam como "lições de casa para os estudantes". Até a próxima.

O código completo até aqui:

~~~c
{% include_relative src/cap04-interp.c %}
~~~

{% include footer.md %}