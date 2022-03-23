Parte 12: Miscelâneas
=====================

> **Autor:** Jack W. Crenshaw, Ph.D. (05/06/1989)<br>
> **Tradução e adaptação:** Felipo Soranz (26/05/2002)

Este capítulo é sobre outra daquelas incursões laterais que não parecem se encaixar em outro lugar nesta série. Como eu mencionei da última vez, foi quando eu estava escrevendo este capítulo que percebi que mudanças deveriam ser feitas na estrutura do compilador. Portanto tive que desviar um pouco do nosso curso padrão para desenvolver a nova estrutura e mostrá-la pra você.

Agora que já passamos por isto, podemos trabalhar naquilo que eu havia mencionado. Isto não deve levar muito tempo, e logo podemos voltar ao nosso normal.

Muitas pessoas me perguntaram a respeito de coisas que outras linguagens tem, mas que eu ainda não tratei nesta série. As duas maiores são ponto-e-vírgula e comentários. Talvez você tenha pensado a respeito deles, também, e imaginou como as coisas mudariam se tivéssemos que tratar deles. Apenas para prosseguirmos com o que está por vir, sem nos preocuparmos com o incômodo sentimento de que alguma está faltanto, vamos tratar destas questões aqui.

Ponto-e-vírgula
---------------

Desde a introdução de Algol, o ponto-e-vírgula tem sido parte de quase toda linguagem moderna. Todos nos acostumamos com o lugar onde eles supostamente deveriam estar. Aliás, eu suspeito que mais erros de compilação ocorreram por causa de ponto-e-vírgula mal-posicionado ou omitido que qualquer outra causa. E se ganhássemos um centavo para cada tecla extra pressionada que os programadores se acostumaram a digitar, poderíamos pagar a dívida externa.

Tendo vindo da linguagem FORTRAN, levou tempo para que eu me acostumasse a usar ponto-e-vírgula, e pra dizer a verdade eu nunca soube muito bem por que eles eram necessários. Como eu programo em Pascal, e como o tratamento de ponto-e-vírgula de Pascal é particularmente complexo, este pequeno caracter ainda é uma das minhas maiores fontes de erro.

Quando eu comecei a desenvolver KISS, eu resolvi questionar CADA construção das outras linguagens, e tentar evitar a maioria dos problemas comuns que ocorrem nelas. Isto coloca o ponto-e-vírgula nas primeiras posições da minha lista.

Para entender o papel do ponto-e-vírgula, é preciso olharmos um pouco para a história.

As primeiras linguagens de programação era orientadas a linha. Em FORTRAN, por exemplo, diversas partes de um comando tinham colunas e campos específicos onde deveriam aparecer. Como alguns comandos eram longos demais para uma linha, um mecanismo de "cartão de continuação" foi provido para deixar o compilador decidir que um determinado cartão era ainda parte da linha anterior. O mecanismo sobrevive até hoje, mesmo que cartões perfurados sejam agora coisas de um passado distante.

Quando outras linguagens surgiram, elas também adotaram vários mecanismos para tratar de comandos de múltiplas linhas. BASIC é um bom exemplo. É importante reconhecermos, porém, que o mecanismo de FORTRAN não era tão necessário para a orientação por linha da linguagem, como também a orientação por coluna. Nas versões de FORTRAN onde entrada de forma livre é permitida, estas orientações não são mais necessárias.

Quando os pais de Algol introduziram a nova linguagem, eles queriam se livrar dos programas orientados a linha, como FORTRAN e BASIC, e permitir que a entrada tivesse uma forma livre. Isto inclui a possibilidade de colocar múltiplos comandos em sequência em uma única linha, como em:

    a=b; c=d; e=e+1;
    
Em casos como este, o ponto-e-vírgula é quase necessário. A mesma linha sem o uso de ponto-e-vírgula ficaria "esquisita":

    a=b c=d e=e+1
    
Eu suspeito que esta é a maior... talvez a ÚNICA... razão para o ponto-e-vírgula: evitar que os programas pareçam "esquisitos".

Mas a idéia de colocar diversos comandos em sequência juntos na mesma linha é bastante duvidosa na melhor das hipóteses. Não é um estilo de programação muito bom, e parece nos levar aos dias em que era considerado importante conservar cartões. Nestes dias de monitores coloridos e código indentado, a clareza dos programas é muito melhor alcançada mantendo os comandos separados. É claro que é bom ter a OPÇÃO de múltiplos comandos, mas parece ser uma vergonha manter os programadores escravizados com o ponto-e-vírgula, apenas para evitar que aquele caso raro pareça "esquisito".

Quando comecei com KISS, eu procurei manter a mente aberta. Eu decidi que eu usaria o ponto-e-vírgula quando ele se tornasse necessário para o analisador, mas não antes disso. Eu achei que isto aconteceria assim que eu adicionasse a habilidade de espalhar comandos em múltiplas linhas. Mas como você pôde ver, nunca aconteceu realmente. O compilador TINY está completamente feliz em analisar os comandos mais complicados, espalhados em diversas linhas, sem ponto-e-vírgula.

Porém, há pessoas que usaram o ponto-e-vírgula por tanto tempo, que se sentem nuas sem eles. Eu sou uma delas. Uma vez que eu defini KISS suficientemente bem, eu comecei a escrever alguns programas de exemplo na linguagem. Eu descobri, para meu completo horror, que eu continuava colocando o ponto-e-vírgula lá de qualquer forma. Então eu estou encarando uma NOVA onda de erros de compilação, causados por um ponto-e-vírgula INDESEJADO. Sinistro!

Talvez alguns leitores já estejam projetando suas próprias linguagens, que podem incluir ponto-e-vírgula, ou que desejam usar as técnicas deste tutorial para compilar linguagens convencionais como C. Em todo caso, temos que estar aptos a tratar do ponto-e-vírgula.

"Açúcar Sintático"
------------------

Toda esta discussão nos leva à questão de "açúcar sintático" (*syntactic sugar*)... construções que são adicionadas à linguagem, não por serem necessárias, mas para ajudar para que o programa pareça correto para o programador. Afinal, é bom term um compilador pequeno e simples, mas seria de pouco uso se a linguagem resultante fosse enigmática ou difícil de programar. A linguagem Forth vêm à mente (sim, eu estou ciente que talvez alguns de vocês reclamem). Se podemos adicionar características à linguagem que faz com que o programa seja mais fácil de ler e entender, e se estas características ajudam o programador a evitar os erros, então, devemos fazê-lo. Particularmente se as construções não adicionam muita complexidade à linguagem ou seu compilador.

O ponto-e-vírgula pode ser considerado um exemplo, mas existem muitos outros, como o THEN do comando IF, o DO, do comando WHILE, e mesmo o comando PROGRAM. Nenhum destes tokens adicionam muita coisa à sintaxe da linguagem... o compilador pode descobrir o que está acontecendo sem eles. Mas algumas pessoas acreditam que eles realmente melhoram a legibilidade dos programas, e isto pode ser muito importante.

Há duas escolas de pensamento neste assunto, que são bem representadas por duas das linguagens muito populares, C e Pascal.

Para os minimalistas, todo este açúcar deve ser deixada de fora. Eles argumentam que tudo isto abarrota a linguagem e obriga o programador a digitar mais do que o necessário. Talvez mais importante que isto, cada token extra ou palavra-chave representa uma armadilha esperando para pegar o programador desatento. Se você esquecer de um token, trocá-lo de lugar, ou escrevê-lo de forma errada, o compilador vai pegar você. Então estas pessoas acham que o melhor é se livrar deste tipo de coisa. Em geral eles tendem a gostar de linguagem C e suas descendentes, que procuram usar o mínimo necessário de palavras-chave e pontuação.

Aquelas da outra linha de pensamento geralmente gostam de Pascal. Eles argumentam que ter que escrever alguns caracteres a mais é um preço pequeno a ser pago por legibilidade. Afinal, os humanos tem que ler os programas, também. (E muitos de nós nunca lembram disso...) O melhor argumento é que cada construção é uma oportunidade de dizer ao compilador que você realmente quis dizer aquilo que você disse para ser feito. Os tokens de "açúcar" servem como demarcações úteis para nos ajudar a encontrar nosso caminho.

As diferenças são bem representadas pelas duas linguagens. A reclamação mais comum da linguagem C é que ela é muito permissiva. Quando você comete um erro em C, o código errado é frequentemente uma outra construção válida em C. Então o compilador simplesmente continua compilando, e deixa você encontrar o erro durante a depuração. Eu acho que é por isto que depuradores são tão populares entre os programadores C.

Por outro lado, se um programa Pascal compilar, você pode ter certeza de que o programa faz o que você queria que ele fizesse. Se houver um erro em tempo de execução, ele provavelmente será de projeto.

O melhor exemplo de "açúcar" é o próprio ponto-e-vírgula. Considere o seguinte fragmento:

    a=1+(2*b+c) b...

Como não há operadores conectando o token "b" com o resto do comando, o compilador conclui que a expressão terminou no ")", e que "b" é o princípio de um novo comando. Mas suponha que eu simplesmente tenha esquecido do operador desejado, e realmente quis dizer:

    a=1+(2*b_c)*b...
    
Neste caso, o compilador emitiria um erro, tudo bem, mas não seria muito compreensível, já que ele estaria esperando um operador "=" depois do "b" que realmente deveria estar lá.

Se, por outro lado, eu incluir um ponto-e-vírgula depois do "b", ENTÃO não haveria dúvida sobre onde eu quero que o comando termine. "Açúcar sintático", portanto, pode ter um propósito muito útil em adicionar alguma segurança para nos mantermos na linha.

Eu me encontro mais ou menos no meio de tudo isso. Em geral, eu gosto da visão dos programadores Pascal.. eu prefiro encontrar meus bugs em tempo de compilação ao invés de em tempo de execução. Mas também odeio incluir "falatório" sem razão aparente, como em COBOL. Até aqui eu estive deixando a maior parte do "açúcar" de lado em KISS/TINY. Mas certamente eu também não sou inflexível e não vejo nenhum mal em espalhar um pouco de "açúcar" aqui e ali apenas pela segurança extra que ele traz. Se você prefere esta última abordagem, saiba que coisas assim são simples de adicionar. Apenas lembre-se que, assim como o ponto-e-vírgula, cada item de "açúcar" é algo que pode potencialmente causar um erro de compilação com sua omissão.

Tratando do ponto-e-vírgula
---------------------------

Há duas formas distintas em que o ponto-e-vírgula pode ser usado em linguagens populares. Em Pascal, o ponto-e-vírgula é considerado como um SEPARADOR de comandos. Nenhum ponto-e-vírgula é necessário depois do último comando de um bloco. A sintaxe é:

    <block> ::= <statement> ( ';' <statement> ) *
    
    <statement> ::= <assignment> | <if> | <while> ... | null
    
(O comando "null" ou vazio é IMPORTANTE!)

Pascal também define alguns outros locais onde podem haver ponto-e-vírgula, como depois do comando PROGRAM.

Em C e Ada, por outro lado, o ponto-e-vírgula é considerado um TERMINADOR de comandos, e segue todos os comandos (com algumas exceções confusas e embaraçosas). A sintaxe para isto é simples:

    <block> ::= ( <statement> ';' )*
    
Das duas sintaxes, a do Pascal parece ser a mais racional, mas a experiência tem mostrada que ela permite algumas dificuldade estranhas. As pessoas se acostumam a digitar um ponto-e-vírgula depois de todo comando digitado e acabam fazendo isto no último comando de um bloco, também. Isto normalmente não causa nenhum mal... isto é simplesmente tratado como se houvesse um comando "null" em seguida. Muitos programadores Pascal, incluindo este aqui, fazem isto. Mas existe um lugar em que NÃO se deve digitar um ponto-e-vírgula, e é justamente antes de um ELSE. Este pequeno detalhe já me custou muitas compilações extra, particularmente quando o ELSE é adicionado a código existente. Portanto a escolha de C/Ada acaba sendo melhor. Aparentemente, Niklaus Wirth também acha isto: em Modula-2, ele abandonou a abordagem de Pascal.

Dada qualquer uma destas sintaxes, é simples (agora que reorganizamos o analisador!) adicionar estas características ao nosso compilador. Vamos tratar do último caso primeiro, já que é mais simples.

Pra começar, eu tornei as coisas mais fáceis, introduzindo um novo reconhecedor:

~~~c
/* Reconhece um ponto-e-vírgula */
void Semicolon()
{
    MatchString(";");
}
~~~

Esta rotina funciona de forma muito parecida com à antiga `Match()`. Ela insiste em achar um ponto e vírgula como o próximo token. Tendo encontrado, ela passa para o próximo token.

Como um ponto-e-vírgula segue um comando, a rotina `Block()` é praticamente a única que deve ser alterada:

~~~c
/* Analisa e traduz um bloco de comandos estilo "C/Ada" */
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
            case 'x':
                Assignment();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
        }
        if (!follow)
            Semicolon();
    } while (!follow);
}
~~~

Repare cuidadosamente na pequena alteração no comando switch. A chamada a `Assignment()` agora é garantida com um teste em token. Isto é para evitar a chamada a `Assignment()` quando o token é o ponto-e-vírgula (que pode acontecer se o comando for vazio).

Como as declarações são comandos também, também devemos adicionar uma chamada a `Semicolon()` dentro de `TopDeclarations()`:

~~~c
/* Analisa e traduz declarações */
void TopDeclarations()
{
    Scan();
    while (Token == 'v') {
        do {
            Declaration();
        } while (Token == ',');
        Semicolon();
        Scan();
    }
}
~~~

Finalmente, precisamos de mais uma para o comando PROGRAM.

~~~c
/* Programa principal */
int main()
{
    Init();

    MatchString("PROGRAM");
    Semicolon();
    AsmHeader();
    TopDeclarations();
    MatchString("BEGIN");
    AsmProlog();
    Block();
    MatchString("END");
    AsmEpilog();

    return 0;
}
~~~

É simples assim. Teste com uma cópia de TINY e veja se você gosta.

A versão Pascal é um pouco mais complicada, mas ainda assim ela requer apenas mudanças menores, e somente para a rotina `Block()` . Para manter as coisas o mais simples possível, vou separar a rotina em duas partes. A rotina seguinte trata de apenas um comando:

~~~c
/* Analisa e traduz um único comando */
void Statement()
{
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
        case 'x':
            Assignment();
            break;
    }
}
~~~

Usando a rotina, podemos agora reescrever `Block()` :

~~~c
/* Analiza e traduz um bloco de comandos estilo "Pascal" */
void Block()
{
    Statement();
    while (Token == ';') {
        NextToken();
        Statement();
    }
    Scan();
}
~~~

Isto certamente não machucou, certo? Agora podemos processar ponto-e-vírgula da mesma forma que em Pascal.

Um Meio-Termo
-------------

Agora que sabemos como tratar de ponto-e-vírgula, isto significa que vamos colocá-los em KISS/TINY? Bem, sim e não. Eu gosto do açúcar sintático e da segurança extra que acompanha o fato de sabermos com certeza onde o fim dos comandos estão. Mas eu não mudei a minha idéia sobre erros de compilação associados ao ponto-e-vírgula.

Então eu tenho algo em mente que parece ser um bom meio-termo: fazê-los OPCIONAIS!

Considere a seguinte versão de "semicolon":

~~~c
/* Reconhece um ponto-e-vírgula opcional */
void Semicolon()
{
    if (Token == ';')
        NextToken();
}
~~~

Esta rotina vai aceitar o ponto-e-vírgula sempre que for chamada, mas não vai insistir em encontrar um. Isto significa que quando você decidir usar ponto-e-vírgula, o compilador vai usar a informação extra para se manter na linha. Mas se você omitir um (ou omitir a todos) o compilador não vai reclamar. O melhor dos dois mundos.

Coloque esta nova rotina na primeira versão do seu programa (a com a sintaxe C/Ada), e você terá o princípio de [TINY Versão 1.2](src/cap12-tiny12.c).

Comentários
-----------

Até aqui eu estive cuidadosamente evitando a questão dos comentários. Você pensaria que isto deve ser uma questão fácil... afinal, o compilador em si não tem que tratar dos comentários; ele simplesmente deve ignorá-los. Bem, algumas vezes isto é verdade.

Comentários podem ser tão fáceis ou tão difíceis quanto você quiser fazê-los. Em um extremo, podemos fazer as coisas de forma que os comentários são interceptados quase que no mesmo instante que eles entram no compilador. No outro extremo, podemos tratá-los como elementos léxicos. As coisas começam a ficar interessantes, quando você considera coisas como delimitadores contidos dentro de strings entre aspas.

Delimitadores de Um Caracter
----------------------------

Aqui está um exemplo. Podemos usar o padrão Turbo Pascal e usar chaves para os comentários. Neste caso, temos delimitadores de um caracter, então o processamento é um pouco mais fácil.

Uma abordagem é eliminar os comentários no instante em que os encontramos na entrada; isto é, na rotina `NextChar()`. Para fazer isto, primeiro altere o nome de `NextChar()` para algo como `NextCharX()`. (**Aviso:** isto será uma mudança temporária, portanto é melhor não fazer isto com sua única cópia de TINY. Eu presumo que você entenda que todas estas experiências devem ser feitas em uma **cópia do programa**, e não no próprio.)

Agora, vamos precisar de uma rotina que pule os comentários. Então, entre com esta nova rotina:

~~~c
/* Pula um campo de comentário */
void SkipComment()
{
    while (Look != '}') {
        NextCharX();
    }
    NextCharX();
}
~~~

Evidentemente, o que está rotina faz é simplesmente ler e descartar caracteres da entrada, até encontrar uma chave direita ("}"). Então ele lê um caracter a mais e o coloca em "Look" como esperado.

Agora podemos escrever uma nova versão de `NextChar()` que usa "skipComment" para remover os comentários:

~~~c
/* Lê próximo caracter da entrada e pula quaisquer comentários */
void NextChar()
{
    NextCharX();
    if (Look == '{')
        SkipComment();
}
~~~

Entre com este código e faça um teste. Você vai descobrir que é possível colocar comentários de fato em qualquer lugar que você quiser. Os comentários nem sequer chegam ao analisador sinático... toda chamada a `NextChar()` retorna apenas caracteres que NÃO são parte de um comentário.

Na verdade, embora esta abordagem faça o trabalho corretamente, e talvez seja perfeitamente satisfatória pra você, ela faz o trabalho bem DEMAIS. Primeiro, a maioria das linguagens de programação especificam que o comentário deve ser tratado como um espaço, de forma que comentários não são permitidos digamos, no meio dos nomes de variávies (exemplo: "VAR{comentário}IÁVEL"). Esta versão atual simplesmente não liga para ONDE você coloca os comentários.

Segundo, já que o resto do analisador não recebe o caracter "{", você não vai poder colocá-lo futuramente em uma string entre aspas.

Antes que você torça o nariz para esta solução simplística, eu gostaria de falar que alguns compiladores de respeito não permitem o uso do delimitador de comentário dentro de uma string. E seria possível usar comentários no meio de um identificador. Eu não consigo imaginar porque alguém faria isso, então é um ponto meio duvidoso. Mas para 99% de todas as aplicações, o que eu lhe mostrei pode funcionar perfeitamente bem.

Mas se você preferir o tratamento convencional, então temos que mover o ponto de detecção um pouco abaixo.

Para fazer isto, volte à versão anterior de `NextChar()` da forma como ela era antes e altere a chamada em `SkipComment()`. Então, vamos adicionar a chave esquerda ("{") como um possível caracter de espaço, alterando a rotina `SkipWhite()`:

~~~c
/* Pula caracteres em branco */
void SkipWhite()
{
    while (isspace(Look) || Look == '{') {
        if (Look == '{')
            SkipComment();
        else
            NextChar();
    }
}
~~~

Repare que `skipWhite` foi escrito de forma que vamos pular qualquer combinação de caracteres de espaços e comentários, em uma chamada.

OK, teste este também. Você vai descobrir que é possível usar comentários para delimitar tokens também. Vale a pena mencionar que esta abordagem nos permite tratar de chaves dentro de strings, já que dentro das strings não há porque testar ou pular espaços em branco, e sim tratá-los literalmente.

Há mais um item a ser tratado: comentários aninhados. Alguns programadores gostam da idéia de aninhar comentários, já que isto permite comentar código que possui comentários durante a depuração. O código que eu mostrei aqui não permite isto, nem mesmo Pascal ou linguagem C.

Mas para arrumar isto é incrivelmente simples. Tudo o que precisamos fazer é tornar s `SkipComment()` recursivo:

~~~c
/* Pula um campo de comentário */
void SkipComment()
{
    while (Look != '}') {
        NextChar();
        if (Look == '{') /* Trata comentários aninhados */
            SkipComment();
    }
    NextChar();
}
~~~

Aí está. Um tratamento de comentários com um nível de sofistificação suficiente pra toda vida.

Delimitadores Multi-caracter
----------------------------

Tudo isto está muito bom para casos onde um comentário é delimitado por caracteres simples, mas e os casos como em C ou Pascal padrão, onde 2 caracteres são necessários? Bem, os princípios são os mesmos, mas temos que mudar nossa abordagem um pouco. Eu tenho certeza que você não ficaria surpreso de saber que as coisas são um pouco mais difíceis neste caso.

Para a situação multi-caracter, a coisa mais fácil a fazer é interceptar o delimitador esquerdo de volta no estágio `NextChar()`. É possível "tokenizá-lo" lá mesmo, trocando-o por um simples caracter.

Vamos assumir que estamos usando os delimitadores de C "/*" e "*/". Primeiro, temos que voltar a abordagem `NextCharX()`. Em outra cópia do compilador, renomeie `NextChar()` para `NextCharX()` e então entre com a nova versão de `NextChar()`:

~~~c
/* Lê próximo caracter e intercepta início de comentário*/
void NextChar()
{
    if (TempChar != ' ') {
        Look = TempChar;
        TempChar = ' ';
    } else {
        NextCharX();
        if (Look == '/') {
            TempChar = getchar();
            if (TempChar == '*') {
                Look = '{';
                TempChar = ' ';
            }
        }
    }
}
~~~

Como você pode perceber, o que este procedimento faz é interceptar toda ocorrência de "/". Então ele examina o PRÓXIMO caracter de entrada. Se o caracter é um "*", então encontramos o início de um comentário, e `NextChar()` vai retornar um substituto de um só caracter para ele. (Por simplicidade, estou usando o caracter "{" como eu fiz para os comentários da versão Pascal. Se você estiver escrevendo um compilador C, sem dúvida você iria preferir escolher algum outro caracter que não é usado em nenhum outro lugar em C. Escolhe qualquer um da sua preferência... mesmo que seja o caracter ASCII 255 por exemplo, qualquer coisa que seja única.)

Se o caracter após o "/" não for um "*", então `NextChar()` o coloca em "TempChar" e retorna o "/".

Repare que é preciso declarar esta nova variável global e inicializá-la com um espaço.

~~~c
char TempChar = ' ';
~~~

Agora, precisamos de uma nova versão de `SkipComment()`:

~~~c
/* Pula um campo de comentário */
void SkipComment()
{
    do {
        do {
            NextCharX();
        } while (Look != '*');
        NextCharX();
    } while (Look != '/');
    NextCharX();
}
~~~

Algumas coisas a notar: primeiro, a rotina "skipWhite" não precisa ser alterada, já que `NextChar()` continua retornando o token "{". Se você alterar este caracter de token, é claro que você também deve alterar o caracter nesta rotina.

Segundo, repare que "skipComment" não chama `NextChar()` em seu laço, e sim `NextCharX()`. Isto significa que o "/" não é interceptado e é visto por `SkipComment()`. Terceiro, apesar de `NextChar()` ser a rotina que faz o trabalho, ainda podemos tratar dos caracteres de comentário dentro de uma string entre aspas, chamando `NextCharX()` ao invés de `NextChar()` enquanto estivermos dentro da string. Finalmente, repare que podemos novamente prover comentários aninhados simplesmente adicionando uma chamada recursiva em `SkipComment()`, da mesma forma que antes.

Comentários de Uma Linha
------------------------

Até aqui eu mostrei como tratar de todo tipo de comentários delimitados pela esquerda e direita. Isto só deixa de lado os comentários de linha, como aqueles de linguagem Assembly (`;`), Ada (`--`) ou mesmo os de C++ (`//`), que são terminados pelo final da linha. De certa forma, este caso é até mais simples. A única rotina que deve ser alterada é `SkipComment()`, que agora deve terminar no final da linha:

~~~c
/* Pula um campo de comentário de uma só linha*/
void SkipComment()
{
    do {
        NextCharX();
    } while (Look != '\n');
    NextChar();
}
~~~

Se o caracter de início do comentário é único, como ";" de assembly, é praticamente isto. Basta alterar `SkipWhite()` para usá-lo. Repare que você não pode usar o ";" propriamente dito na nossa versão atual de TINY por razões óbvias. Como alternativa, podemos experimentar "#".

~~~c
/* Pula caracteres em branco */
void SkipWhite()
{
    while (isspace(Look) || Look == '#') {
        if (Look == '#')
            SkipComment();
        else
            NextChar();
    }
}
~~~

Se for um token de dois caracteres, como "--" de Ada, precisamos apenas alterar os testes dentro de `NextChar()`. Nos dois casos, é um problema mais simples que o caso balanceado.

Conclusão
---------

Neste ponto temos a habilidade de lidar com comentários e ponto-e-vírgula, como também outros tipos de "açúcar sintático". Eu lhe mostrei diversas formas de lidar com cada um, dependendo da convenção desejada. A única questão pendente é: qual destas convenções serão usadas em KISS/TINY?

Pelas razões dadas anteriormente, vou escolher o seguinte:

1. O ponto-e-vírgula é um TERMINADOR, não um separador
2. O ponto-e-vírgula é OPCIONAL
3. Comentários são delimitados por chaves
4. Comentários podem ser aninhados

Coloque o código correspondente a estes casos em sua cópia de TINY. Agora você tem TINY Versão 1.3.

Listagem completa:

~~~c
{% include_relative src/cap12-tiny13.c %}
~~~

> Download do [compilador "Tiny 1.3"](src/cap12-tiny13.c).

Agora que já tratamos destas questões paralelas, podemos finalmente voltar ao núcleo da série. No próximo capítulo, vamos falar sobre procedimentos e passagem de parâmetros, e vamos adicionar estas importantes características a TINY.

Vejo vocês lá.

{% include footer.md %}