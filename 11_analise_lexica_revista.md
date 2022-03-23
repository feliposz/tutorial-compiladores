Parte 11: Análise Léxica Revista
================================

> **Autor:** Jack W. Crenshaw, Ph.D. (03/06/1989)<br>
> **Tradução e adaptação:** Felipo Soranz (25/05/2002)

Eu tenho algumas notícias boas e ruins. A ruim é que este capítulo não vai ser a respeito do que eu havia prometido. E o que é pior, nem o próximo.

A boa notícia é também a razão para esta seção: eu descobri uma forma de simplificar e melhorar a parte da análise léxica do compilador. Deixe-me explicar.

Conceitos
---------

Se vocês se lembram, já andamos muito no tópico de analisadores sintáticos na [Parte 7](07_analise_lexica.md), e eu deixei vocês com um projeto para um analisador sintático distribuído que parecia tão simples quanto possível... mais do que a maioria dos que eu já vi. Nós usamos esta idéia na [Parte 10](10_apresentando_tiny.md). A [estrutura resultante do compilador](src/cap10-tiny10.c) era simples, e fazia bem o trabalho.

Recentemente, porém, eu comecei a ter problemas, e este é o tipo de mensagem que diz que talvez você esteja fazendo algo errado.

Tudo começou quando eu tentei tratar do problema do ponto-e-vírgula (;). Muitas pessoas me perguntaram a respeito, e se KISS teria ou não ponto-e-vírgula para separar os comandos. A minha intenção era não usá-los, simplesmente porque eu não gosto deles, e como você já viu, é provado que eles não são necessários.

Mas eu sei que muitos de vocês, como eu, se acostumaram com eles, e portanto eu resolvi escrever um capítulo curto para mostrar como eles podem ser facilmente adicionados, se você os quer tanto.

Bem, acontece que começou a parecer que eles não eram tão fáceis de adicionar. Na verdade parecia bem difícil.

Eu acho que eu deveria ter percebido que algo estava errado, por causa do problema das quebras de linha. Nos últimos capítulos nós tratamos deste problema, e eu mostrei como tratar de quebras de linha com uma rotina chamada, apropriadamente, `NewLine()`. Em TINY Versão 1.0, eu espalhei chamadas a esta rotina em pontos estratégicos do código.

Mas parece que cada vez que eu trato do problema de quebras de linha, ele parece ser complicado, e o analisador resultante acaba ficando meio frágil... algo adicionado aqui ou ali e as coisas começavam a dar errado. Olhando novamente para o problema, eu percebi que havia uma mensagem ali que eu não estava prestando atenção.

Quando eu tentei adicionar ponto-e-vírgula sobre quebras-de-linha é que a coisa complicou. Eu acabei com uma solução complexa demais. Eu percebi que algo fundamental tinha que mudar.

Portanto, de certa forma, este capítulo vai nos fazer dar um passo para trás e rever o problema de análise léxica novamente. Peço desculpas por isto. Este é o preço pago por me observar fazer as coisas em "tempo real". Mas a nova versão é definitivamente uma melhoria, e vai ser bastante útil para o que vem em seguida.

Como eu disse, o analisador léxico usado na parte 10 era tão simples quanto possível. Mas tudo pode ser melhorado. O novo analisador é mais parecido com um analisador léxico clássico, e não é tão simples quanto antes. Mas a estrutura geral do compilador é até mais simples do que antes. Também é mais robusta, e fácil de aumentar e/ou modificar. Eu acho que vai valer a pena gastarmos um tempo nesta melhoria. Portanto neste capítulo eu vou mostrar a nova estrutura. Sem dúvida você vai gostar de saber que, apesar das mudanças afetarem diversas rotinas, elas não são muito profundas e portanto não vamos perder quase nada do que já foi feito.

Ironicamente, o novo analisador léxico é muito mais convencional do que o antigo, e é muito parecido com um analisador genérico que eu mostrei anteriormente na [parte 7](07_analise_lexica.md). Então eu comecei a tentar ser mais esperto, e a minha esperteza quase me atirou pra fora do caminho. Talvez um dia eu aprenda: Keep It Simple, Stupid! (Mantenha Simples, Estúpido!)

O Problema
----------

O problema começa a se apresentar na rotina `Block()`, que eu reproduzi abaixo:

~~~c
/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    do {
        Scan();
        switch (Token) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'R':
                DoRead();
                break;
            case 'W':
                DoWrite();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                Assignment();
                break;
        }
    } while (!follow);
}
~~~

Como você pode ver, `Block()` é orientado a comandos individuais do programa. A cada passagem do laço, sabemos que estamos no início de um novo comando. Nós saímos do bloco quando encontramos um END ou um ELSE.

Mas suponha que um ponto-e-vírgula seja encontrado. A rotina, como ela está agora, não é capaz de tratar dele, pois a rotina `Scan()` espera, e só consegue aceitar, tokens que começam com uma letra.

Eu fiquei pensando por um tempo no problema e tentando arrumar alguma solução. Eu achei muitas abordagens possíveis, mas nenhuma era muito satisfatória. Eu finalmente descobri a razão.

Lembre-se que quando começamos com nosso analisador sintático de caracteres simples, adotamos uma convenção de que o caracter **lookahead** seria sempre pré-carregado. Isto é, teríamos o caracter que corresponde à nossa posição atual na entrada, carregado na variável global `Look`, de forma que poderíamos examiná-lo tantas vezes quanto necessário. A regra que adotamos era que CADA reconhecedor, tendo encontrado seu token alvo, avançaria `Look` para o próximo caracter na entrada.

Esta convenção fixa e simples nos foi muito útil quando tínhamos tokens de um caracter, e ainda é. Faria sentido aplicar a mesma regra a tokens multi-caracter.

Mas quando chegamos em análise sintática, eu comecei a violar aquela regra simples. O analisador sintático da [parte 10](10_apresentando_tiny.md) de fato avançava para o próximo token se encontrasse um identificador ou palavra-chave, mas NÃO fazia isto se encontrasse um retorno de linha, um caracter de espaço, ou um operador.

Agora, este tipo de operação "misturada" nos causa sérios problemas na rotina `Block()`, pois o fato da entrada ter avançado ou não depende do tipo de token encontrado. Se for uma palavra-chave ou o alvo de um comando de atribuição, o "cursor", conforme definido pelo conteúdo de `Look`, avança para o próximo token OU para o começo de um espaço em branco. Se, por outro lado, o token é um ponto-e-vírgula, ou se emitimos uma quebra de linha, o cursor NÃO avança.

É desnecessário dizer que podemos adicionar lógica necessária para nos manter na mesma linha. Mas é complicado, e faz o analisador todo ficar muito frágil.

Há um jeito muito melhor, que é adotar a mesma regra que funcionou tão bem antes, aplicar a TOKENS o mesmo que a caracteres simples. Em outras palavras, vamos pré-carregar os tokens da mesma forma que sempre fizemos com caracteres. Parece tão óbvio depois que você já pensou nisso.

É interessante que, se fizermos as coisas desta forma, o problema que tivemos com retornos de linha desaparece. Podemos simplesmente tratá-los como caracteres de espaço, o que significa que o tratamento deles se torna trivial, e MUITO menos suscetível a erros do que a forma que tivemos de tratar anteriormente.

A Solução
---------

Vamos começar a arrumar o problema reintroduzindo as duas rotinas:

~~~c
/* Recebe o nome de um identificador ou palavra-chave */
void GetName()
{
    int i;

    SkipWhite();
    if (!isalpha(Look))
        Expected("Identifier or Keyword");
    for (i = 0; isalnum(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = toupper(Look);
        NextChar();
    }
    TokenText[i] = '\0';
    Token = 'x';
}

/* Recebe um número inteiro */
void GetNum()
{
    int i;

    SkipWhite();
    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = Look;
        NextChar();
    }
    TokenText[i] = '\0';
    Token = '#';
}
~~~

Estes dois procedimentos são funcionalmente quase idênticos aos que eu mostrei na [Parte 7](07_analise_lexica.md). Cada um carrega o token corrente, tanto um identificador como um número, na string global `TokenText`. Eles também alteram `Token` para o código apropriado. A entrada é deixada com o caracter `Look` contendo o primeiro caracter que NÃO é parte do token.

Podemos fazer o mesmo para operadores, mesmo multi-caracter, com uma rotina como:

~~~c
/* Analisa e traduz um operador */
void GetOp()
{
    int i;

    Token = Look;
    for (i = 0; !isalnum(Look) && !isspace(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = Look;
        nextchar();
    }
    TokenText[i] = '\0';
}
~~~

Repare que `GetOp()` retorna, como seu token codificado, o PRIMEIRO caracter do operador. Isto é importante, pois significa que podemos usar este caracter para orientar o analisador, ao invés de `Look`.

Temos que juntar estas rotinas em uma rotina única que trata dos três casos. A rotina seguinte lê qualquer um dos três tipos e sempre deixa a entrada posicionada depois do token:

~~~c
/* Pega o próximo token de entrada */
void NextToken()
{
    SkipWhite();
    if (isalpha(Look))
        GetName();
    else if (isdigit(Look))
        GetNum();
    else
        GetOp();
}
~~~

(Repare que eu coloquei `SkipWhite()` ANTES das chamadas ao invés de depois. Isto significa que, em geral, a variável `Look` NÃO vai conter um valor muito útil, e portanto NÃO devemos usá-la como um valor de teste na análise, como temos feito até aqui. Está é a grande diferença em relação à nossa abordagem normal.)

Agora, lembre-se que antes eu estava cuidadosamente NÃO tratando a quebra de linha como um caracter de espaço. Isto porque, com `SkipWhite()` sendo chamado por último no analisador léxico, o encontro com o retorno de linha iria gerar mais um comando de leitura. Se estivéssemos na última linha do programa, não poderíamos sair até entrar com uma nova linha com no mínimo um caracter. É por isso que precisávamos da segunda rotina, `NewLine()`, para tratar das quebras de linha.

Mas agora, com a chamada a `SkipWhite()` no início, é exatamente o comportamento que queremos. O compilador deve saber que há outro token em seguida ou ele não vai chamar `NextChar()`. Em outras palavras, se ele não encontrou o END final ainda, vamos insistir em ler mais dados até encontrar algo.

Isto significa que podemos simplificar muito o programa e os conceitos, tratando a quebra de linha como um caracter de espaço, e eliminando `NewLine()`. Apenas trocamos o teste em `SkipWhite()`:

~~~c
/* Pula caracteres em branco */
void SkipWhite()
{
    while (isspace(Look))
        NextChar();
}
~~~

Já testamos rotinas similares na [Parte 7](07_analise_lexica.md), mas você pode tentar estas novas.

Se quiser fazer o teste:

- Comece com uma [cópia do "berço"](src/cap01-craddle.c) 
- Adiciona as rotinas acima
- Declare as variáveis globais:

~~~c
#define MAXTOKEN 16
char Token; /* Código do token atual */
char TokenText[MAXTOKEN+1]; /* Texto do token atual */
~~~

- Por último, chame `NextToken()` com o seguinte programa:

~~~c
/* Programa principal */
int main()
{
    Init();
    do {
        NextToken();
        printf("Token: %c Value: %s\n", Token, TokenText);
    } while (Token != '.');
}
~~~

Compile e verifique que é possível separar um programa em uma série de tokens, e que é possível obter a codificação correta para cada token.

Isto QUASE funciona, mas não totalmente. Há dois problemas potenciais: Primeiro, em KISS/TINY quase todo operador é de um só caracter. As únicas exceções são os operadores >=, <= e <>. Parece uma vergonha tratar todo operador como strings e fazer comparação de strings, quando apenas uma comparação de caracter seria quase sempre suficiente. Segundo, e mais importante, a coisa não FUNCIONA quando dois operadores aparecem juntos, como em (a+b)*(c+d). Aqui a string depois de "b" seria interpretada como um único operador ")*(".

É possível resolver isto. Por exemplo, poderíamos dar a `GetOp()` uma lista dos caracteres válidos, e poderíamos tratar parênteses como tipos de operadores diferentes dos outros. Mas a coisa começa a virar bagunça.

Felizmente, há uma forma melhor de resolver todos estes problemas. Como quase todo operador é de um único caracter, vamos simplesmente tratá-los desta forma, e permitir que `GetOp()` pegue apenas um caracter no momento. Isto não só simplifica `GetOp()`, mas também acelera as coisas um pouco. Ainda temos o problema dos operadores relacionais, mas estamos tratando deles como casos especiais de qualquer maneira.

Aqui está a versão final de `GetOp()`:

~~~c
/* Analisa e traduz um operador */
void GetOp()
{
    SkipWhite();
    Token = Look;
    TokenText[0] = Look;
    TokenText[1] = '\0';
    NextChar();
}
~~~

Repare que eu ainda atribuo um valor a `TokenText`. Se você estiver muito preocupado com eficiência, é possível remover isto (embora vá fazer uma diferença muito pequena realmente). Quando esperamos um operador, vamos testar apenas `Token` de qualquer maneira, então o valor não tem tanta importância. Mas para mim parece ser uma boa prática colocar algum valor lá, só por garantia.

Tente esta nova versão com algum código realístico. Deve ser possível separar qualquer programa em seus tokens individuais, com a diferença que operadores relacionais de dois caracteres serão reconhecidos como tokens separados. Mas tudo bem... vamos tratá-los desta forma.

A listagem completa do analisador léxico até aqui:

~~~c
{% include_relative src/cap11-lex.c %}
~~~

> Download do [analisador léxico](src/cap11-lex.c).

Na [parte 7](07_analise_lexica.md) a função de `NextToken()` estava combinada com a rotina `Scan()`, que também verificava cada identificador com uma lista de palavras-chave e codificava cada uma que fosse encontrada. Como eu havia mencionado no momento, a última coisa que gostaríamos de fazer é usar tal rotina em locais onde palavras-chave não deveriam aparecer, como em expressões. Se tivéssemos feito isto, a lista de palavras-chave seria comparada com cada identificador no código. Nada bom.

A maneira correta de tratar disto é simplesmente separar as funções de capturar tokens e procurar por palavras-chave. A versão de `Scan()` mostrada abaixo não faz NADA a não ser verificar palavras-chave. Repare que ela opera no token corrente e NÃO avança na entrada.

~~~c
/* Analisador léxico */
void Scan()
{
    int kw;

    if (Token == 'x') {
        kw = Lookup(TokenText, KeywordList, KEYWORDLIST_SIZE);
        if (kw >= 0)
            Token = KeywordCode[kw];
    }
}
~~~

Há um último detalhe. No compilador há alguns lugares onde temos que verificar o valor do token. Normalmente, isto é feito para diferenciar entre os diferentes ENDs, mas há mais alguns locais. (Eu devo lembrar que podemos sempre eliminar a necessidade de comparar caracteres END codificando cada um deles com um caracter diferente. Neste momento estamos definitivamente sendo preguiçosos.)

A seguinte versão de `MatchString()` toma o lugar da versão caracter. Note que, como em `Match()`, ela AVANÇA na entrada.

~~~c
/* Compara string com texto do token atual */
void MatchString(char *s)
{
    if (strcmp(TokenText, s) != 0)
        Expected(s);
    NextToken();
}
~~~

Arrumando o Compilador
----------------------

Armados com estas novas rotinas de análise léxica, podemos começar a arrumar o [compilador](src/cap10-tiny10.c) para usá-las apropriadamente. As mudanças são bem pequenas, mas há alguns lugares em que mais mudanças são necessárias. Ao invés de mostrar cada lugar, vou dar uma idéia geral e então mostrar o produto completo.

Em primeiro lugar, o código para a rotina `Block()` não muda, mas sua função sim:

~~~c
/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    do {
        Scan();
        switch (Token) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'R':
                DoRead();
                break;
            case 'W':
                DoWrite();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                Assignment();
                break;
        }
    } while (!follow);
}
~~~

Lembre-se que a nova versão de `Scan()` não avança na entrada, apenas procura por palavras-chave. A entrada deve ser avançada por cada rotina que `Block()` chama.

Em geral, temos que trocar todo teste em `Look` por um similar em `Token`. Por exemplo:

~~~c
/* Analisa e traduz uma expressão booleana */
void BoolExpression()
{
    BoolTerm();
    while (IsOrOp(Token)) {
        AsmPush();
        switch (Token) {
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

Em rotinas como `Add()`, não temos mais que usar `Match()`. Só temos que chamar `NextToken()` para avançar na entrada:

~~~c
/* Reconhece e traduz uma adição */
void Add()
{
    NextToken();
    Term();
    AsmPopAdd();
}
~~~

As estruturas de controle são na verdade mais simples. Simplesmente chamamos `NextToken()` para avançar nas palavras-chave de controle:

~~~c
/* Analiza e traduz um comando IF-ELSE-ENDIF */
void DoIf()
{
    int l1, l2;

    NextToken();
    BoolExpression();
    l1 = NewLabel();
    l2 = l1;
    AsmBranchFalse(l1);
    Block();
    if (Token == 'l') {
        NextToken();
        l2 = NewLabel();
        AsmBranch(l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    MatchString("ENDIF");
}
~~~

Esta é a extensão das mudanças NECESSÁRIAS. Na listagem de TINY Versão 1.1 abaixo, eu também fiz algumas outras "melhorias" que não são na verdade necessárias. Deixe-me explicá-las brevemente:

1. Removi as rotinas `Program()` e `MainBlock()`, e combinei suas funções no programa principal. Elas não pareciam estar ajudando na compreensão... na verdade parecia que elas estavam complicando as coisas um pouco.

2. Removi as palavras-chave PROGRAM e BEGIN da lista. Elas ocorrem apenas em um lugar, então não é necessário procurar por elas.

3. Tendo sido atacado por uma overdose de esperteza, eu me lembrei que TINY deveria ser um programa minimalista. Portanto, eu troquei o tratamento fantasioso do menos unário pelo mais simples que eu consegui. Um grande passo para trás na qualidade do código, mas uma grande simplificação do compilador. KISS é o lugar certo para usar a outra versão.

4. Adicionei algumas rotinas de checagem de erro como `CheckTable()` e `CheckDuplicate()`, e troquei o código "em linha" por chamadas a elas. Isto faz uma limpeza em diversas rotinas.

5. Retirei a checagem de erro da rotinas de geração de código `AsmStore()`, e coloquei-a no analisador, que é o lugar em que ela deve estar. Veja `Assignment()`, por exemplo.

6. Adicionei uma nova tabela (`SymbolType`) para os tipos dos identificadores. Isto será útil para mais tarde. Eu poderia ter criado uma estrutura `symbol` e combinar o nome e o tipo na mesma estrutura. Mas teríamos que construir uma função `Lookup()` separada pra símbolos e outra para palavras-chave. Deixemos assim por enquanto.

7. A rotina `AddEntry()` agora tem dois parâmetros, que faz com que as coisas fiquem mais modulares.

8. Repare na maneira que estou tratando operadores multi-caracter em `Relation()`. É essencialmente a mesma. Apenas trocando `Match()` por `NextToken()` onde apropriado.

9. Corrigi o erro na rotina `DoRead()`... a anterior não verificava se o nome da variável era válido.

10. Removi o tratamento dos inicializadores na declaração de variáveis, pois isto não acrescenta muito à linguagem, já que não há como usar expressões completas. Além disso estou tentando manter TINY simples por enquanto e isto iria complicar um pouco o código. Se você acha que isto é dar um passo atrás, sinta-se livre para manter o tratamento de constantes numéricas.

Conclusão
---------

O compilador resultante para TINY é dado abaixo. Ele compila (praticamente) a mesma linguagem que antes. Só está um pouco mais "limpo", e mais importante, está consideravelmente mais robusto. Eu me sinto bem com ele.

O [próximo capítulo](12_miscelaneas.md) vai ser outro desvio do nosso rumo: a discussão sobre ponto-e-vírgula e outras coisas que me fizeram bagunçar as coisas anteriormente. ENTÃO partiremos para procedimentos e tipos. Contine comigo. A adição destas características vai ser uma grande melhoria fazendo com que KISS saia da categoria de "linguagem de brinquedo". Estamos chegando muito perto de estar aptos a escrever um compilador sério.

Listagem completa de TINY Versão 1.1:

~~~c
{% include_relative src/cap11-tiny11.c %}
~~~

> Download do [compilador "Tiny 1.1"](src/cap11-tiny11.c).

{% include footer.md %}