Parte 8: Um Pouco de Filosofia
==============================

- Autor: Jack W. Crenshaw, Ph.D. (02/04/1989)
- Tradução e adaptação: Felipo Soranz (21/05/2002)

Está vai ser uma seção de um tipo diferente das outras na nossa série de construção de analisadores e compiladores. Para esta seção, não haverá experimentos para codificar ou escrever. Apenas desta vez, eu gostaria de conversar com você por um instante. Será uma seção curta, e então poderemos continuar do ponto onde paramos, com vigor renovado, eu espero!

Quando eu estava na faculdade, eu soube que eu sempre poderia acompanhar a exposição do professor muito melhor se eu soubesse aonde ele queria chegar. Eu aposto como acontece o mesmo com você.

Então eu imagino que já é hora de eu dizer aonde vamos chegar com esta série: o que vai acontecer nos capítulos futuros, e em geral, qual o propósito disto tudo. Eu também pretendo compartilhar algumas idéias a respeito da utilidade do que estivemos fazendo.

O Caminho para Casa
-------------------

Até aqui, cobrimos a [análise](02_analise_expressoes) e tradução de [expressões aritméticas](03_mais_expressoes) e [booleanas](06_expressoes_booleanas), e combinações conectadas por operadores relacionais. Também fizemos o mesmo para [construções de controle](05_estruturas_controle). No decorrer de tudo isso nos inclinamos muito no uso de análise descendente recursiva top-down, definições BNF da sintaxe, e geração direta de código em linguagem assembly. Também aprendemos o valor de truques como tokens de um único caracter para nos ajuda a ver a floresta através das árvores. No [último capítulo](07_analise_lexica) tratamos de um analisador léxico, e eu mostrei uma forma simples, mas poderosa, de remover a barreira de caracteres simples.

Através de todo o estudo, eu enfatizei a filosofia KISS... Keep It Simple, Sidney (ou Stupid, depende da ênfase que você quer dar... :) e eu espero que você já tenha percebido quão simples a coisa pode ser de verdade. Enquanto pode haver áreas na teoria de compiladores que podem ser verdadeiramente intimidadoras, a mensagem mais importante desta série é que na prática você pode educadamente dar um passo de lado e sair da complicação de muitas destas áreas. Se a definição da linguagem cooperar ou, como nesta série, se você puder definir a linguagem conforme você avança, é possível escrever definições de linguagem em BNF de forma relativamente fácil. E, como já foi visto, é possível criar rotinas do analisador a partir da BNF tão rápido quanto você consegue digitar.

Conforme nosso compilador tomou forma, ele também ganhou mais partes, mas cada parte é pequena e simples, e muito parecida com todas as outras.

Neste ponto, temos muito conhecimento do processo de criação de um compilador real e prático. De fato, já temos tudo que é necessário para construir um compilador "de brinquedo" para uma linguagem tão poderosa quanto, digamos, ["Tiny BASIC"](https://en.wikipedia.org/wiki/Tiny_BASIC#Formal_grammar). Nos próximos capítulos, nós vamos definir esta linguagem.

Para completar de vez a série, temos alguns poucos itens a cobrir. Estes incluem:

- Chamadas de rotina, com e sem parâmetros
- Variáveis locais e globais
- Tipos básicos, como tipos caracter e inteiro
- Matrizes
- Strings
- Tipos definidos pelo usuário e estruturas
- Analisadores estruturados em árvores e linguagens intermediárias
- Otimização

Eu pretendo cobrir tudo isto em capítulos futuros. Quando terminarmos você terá todas as ferramentas necessárias para projetar e construir sua própria linguagem, e seus próprios compiladores para traduzí-las.

Eu não posso projetar estas linguagens pra você, mas eu posso fazer alguns comentários e recomendações. Eu já espalhei alguns nos capítulos anteriores. Você já viu, por exemplo, as estruturas de controle que eu prefiro.

Estas construções farão parte das linguagens que eu construir. Eu tenho três linguagens em mente neste ponto, duas das quais você verá nos capítulos a seguir:

TINY - Uma linguagem mínima mas usável na ordem de [Tiny BASIC](https://en.wikipedia.org/wiki/Tiny_BASIC#Formal_grammar) ou Tiny C. Não vai ser muito prática, mas vai ter poder o suficiente para permitir que você escreva e execute programas reais que façam algo de valor.

KISS - A linguagem que estou construindo para meu próprio uso. KISS deve ser uma linguagem de programação de sistemas. Ela não vai ter tipos fortemente definidos ou estruturas de dados fantasiosas, mas vai suportar a maioria das coisas que eu quero fazer com uma linguagem de alto nível, exceto escrever compiladores talvez.

Além disso, eu também estive brincando por anos com a idéia de um tipo de assembly de alto-nível, com construções de controle estruturadas e atribuições do tipo das linguagens de alto nível. Esta, de fato, foi o meu pontapé inicial por trás do meu interesse original em me aventurar na teoria dos compiladores. Pode até ser que eu nunca chegue a construí-la, simplesmente porque eu aprendi que é realmente mais simples implementar uma linguagem como KISS, que usa apenas um subconjunto das instruções da CPU. Como você sabe, a linguagem assembly pode ser bizarra e irregular ao extremo, e uma linguagem que mapeia as instruções uma-pra-uma pode ser realmente um desafio. De toda forma, eu sempre achei que a sintaxe dos montadores convencionais é boba... Diga-me por que

~~~
    MOV A, B
~~~

é melhor, ou mais simples de traduzir do que isto?

~~~
    A = B
~~~

Eu acho que seria um exercício interessante desenvolver um "compilador" que daria ao programador acesso e controle completo das instruções da CPU, e permitira que você criasse programas tão eficientes quanto em linguagem assembly, sem a dor de ter que aprender todo um conjunto de mnemônicos. Poderia ser feito? Eu não sei. A questão é: Seria a linguagem resultante mais fácil de escrever que a própria assembly? Se não fosse, não haveria nenhum objetivo para ela. Eu acho que é possível fazer, mas eu não estou completamente certo ainda de como a sintaxe deve ser.

Talvez você tenha algum comentário ou sugestão a respeito disso. Eu adoraria ouví-los.

Você não ficaria surpreso em saber que eu já trabalhei com a maioria das área que vamos cobrir. Eu tenho boas notícias: As coisas nunca ficam muito mais difíceis do que elas tem sido até aqui. É possível construir um compilador completo e funcional para uma linguagem real, usando nada além das mesmas técnicas que aprendemos até aqui. E isto nos leva a algumas perguntas interessantes.

Por que é tão simples?
----------------------

Antes de embarcar nesta série, eu sempre achei que compiladores eram naturalmente programas de computadores complexos... o desafio final! No entanto a maioria das coisas que fizemos até aqui normalmente se mostraram bem simples, algumas vezes até triviais!

Por um momento, eu pensei que era tudo muito simples porque eu não havia chegado ao núcleo do assunto. Eu havia coberto apenas as partes simples. Eu devo admitir sinceramente que quando eu comecei a série, eu não estava tão certo de quão longe poderíamos chegar antes que as coisas ficassem muito complexas de tratar da forma que fizemos até então. Mas neste ponto, eu já estive na estrada tempo o suficiente para ver o fim dela. Adivinhe então?

> NÃO HÁ PARTES DIFÍCEIS!

Então, eu pensei que talvez seja porque não estamos gerando código objeto muito bom. Aqueles de vocês que tem acompanhado a série e tentado compilações de teste sabem que, apesar do código gerado funcionar e ser à prova de falhas, sua eficiência é bem ruim. Eu percebi que se nos concentrássemos em gerar código otimizado iríamos em breve encontrar esta complexidade que faltava.

Até um certo ponto, isto é verdade. Em particular, minhas primeiras tentativas para tentar melhorar a eficiência introduziram complexidade numa taxa alarmante. Mas desde então eu estive trabalhando com algumas otimizações simples e encontrei algumas que resultam em código com qualidade muito respeitável, SEM adicionar um monte de complexidade.

Finalmente, talvez a natureza de "compilador de brinquedo" do nosso estudo seja a desculpa principal. Eu nunca prometi que um dia poderíamos construir um compilador para competir com a Borland ou a Microsoft. E novamente, conforme nos aprofundamos no assunto as diferenças começam a desaparecer.

Apenas para ter certeza de que você entendeu a mensagem, deixe-me colocá-la de uma forma bem direta:

> USANDO AS TÉCNICAS QUE USAMOS ATÉ AQUI, É POSSÍVEL CONSTRUIR UM COMPILADOR FUNCIONAL COM QUALIDADE DE PRODUÇÃO SEM ADICIONAR MUITA COMPLEXIDADE AO QUE JÁ FOI FEITO.

Desde que a série começou eu recebi muitos comentários. Muitos deles espelhavam os meus próprios pensamentos: "Isto é fácil! Por que os livros fazem parecer tão difícil?". Boa pergunta!

Recentemente, eu voltei e li alguns destes livros novamente, e até mesmo comprei alguns novos. Cada vez que eu fazia isso eu tinha a mesma sensação: estes caras fizeram com que parecesse muito difícil!

O que está acontecendo? Por que a coisa toda parece tão difícil nos textos, mas tão simples pra nós? Quer dizer que nós somos bem mais espertos do que Aho, Ullman, Brinch Hansen, e todo o resto?

Dificilmente. Mas nós estamos fazendo as coisas de um modo um pouco diferente, e cada vez mais eu aprecio o valor da nossa abordagem, e a maneira que ela simplifica as coisas. Além dos atalhos óbvios que descrevemos na Parte 1, como tokens mono-caracter e entrada/saída pelo console, assumimos implicitamente e fizemos algumas coisas diferentemente daqueles que projetaram compiladores no passado. Como ficou claro, nossa abordagem tornou a vida bem mais fácil.

Então porque todos aqueles caras não a usam?

Você deve lembrar o contexto de desenvolvimento de alguns dos compiladores primitivos. Aquelas pessoas estavam trabalhando em computadores de capacidade muito limitada. A memória era muito limitada, o conjunto de instruções da CPU era mínimo, e os programas eram executados em lote ("batch") ao invés de interativamente. Estas coisas causaram algumas decisões-chave que realmente complicaram os projetos. Até recentemente, eu não havia percebido quanto dos compiladores clássicos eram dirigidos pelo hardware disponível.

Mesmo em casos onde estas limitações não ocorrem mais, as pessoas tendem a estruturar seus programas da mesma forma, pois é a forma na qual eles aprenderam a fazer.

Em nosso caso, nós começamos com uma folha de papel em branco. Há um perigo aqui, é claro. Você pode acabar caindo em alguma armadilha que as outras pessoas já aprenderam faz tempo como evitar. Mas isto também permitiu que tivéssemos abordagens diferentes, parcialmente por causa do projeto, e parcialmente por pura sorte que nos permitiram ganhar simplicidade.

Aqui estão as áreas que eu acho que fizeram com que as coisas ficassem complexas no passado.


Memória limitada forçando múltiplas passagens
---------------------------------------------

Eu acabo de ler "Pascal Compilers" de Brinch Hansen (um livro excelente). Ele desenvolveu um compilador Pascal para um PC, mas ele começou em 1981 com um sistema de 64K de memória, e portanto toda decisão de projeto que ele fez foi baseada em fazer com que o compilador coubesse na memória RAM. Para fazer isto, seu compilador tinha 3 passagens, uma das quais era o analisador léxico. Não havia como ele usar um analisador léxico distribuído introduzido no último capítulo, porque a estrutura do programa não permitira. E foram necessárias não uma, mas duas linguagens intermediárias, para prover a comunicação entre as fases.

Todos os escritores de compiladores do passado tiveram de lidar com este problema: quebrar o compilador em partes suficientes para que ele caiba na memória. Quando você tem múltiplas passagens, você precisa adicionar estruturas para suportar a informação que cada passagem deixa para a próxima. Isto adiciona complexidade, e acaba dirigindo o "projeto". O livro de Lee, "The Anatomy of a Compiler", menciona um compilador FORTRAN desenvolvido para um IBM 1401. Ele tinha nada mais nada menos do que 63 passagens separadas!!! Não é necessário dizer que, em um compilador como este a separação em fases domina o projeto.

Mesmo em situações onde a memória é abundante, as pessoas tendem a usar as mesmas técnicas pois é com isso que estão familiarizadas. Com o tempo começou a ficar claro que é simples escrever um compilador se você começar com convenções diferentes.

Processamento em lote ("batch")
-------------------------------

No início, o processamento em lote era a única escolha... não havia computação interativa. Mesmo nos dias de hoje, os compiladores são executados essencialmente em lote, como uma operação de fundo.

Em um compilador de mainframe assim como também em muitos compiladores de micros, esforços consideráveis foram feitos para implementar recuperação de erros... isto pode consumir quase 30-40% do tempo de compilação e pode influenciar completamente o projeto. A idéia é evitar parar no primeiro erro, e ao invés disso, continuar a todo custo, de forma que é possível dizer ao programador o máximo a respeito dos erros no programa todo.

Tudo isso nos leva a pensar nos tempos dos primeiros mainframes, onde o tempo de resposta era medido em horas por dia, e era importante extrair o máximo possível de informações em cada execução.

Nesta série, eu estive evitando cuidadosamente o problema de recuperação de erros, ao invés disso nosso compilador simplesmente é interrompido com uma mensagem de erro ao encontrar o primeiro erro. Eu admito francamente que eu fiz isto principalmente porque eu queria pegar o caminho mais fácil e permanecer com as coisas simples. Mas esta abordagem, em que a Borland foi pioneira com o Turbo Pascal, também tem muita coisa boa em si. Além de manter o compilador simples, ela também combina muito bem com a idéia de um sistema interativo. Quando a compilação é rápida, e especialmente, quando você possui um editor, como o do Turbo Pascal por exemplo, que leva você diretamente ao ponto onde foi encontrado o erro, então faz sentido parar lá, e simplesmente recomeçar a compilação depois que o erro for corrigido.

Programas grandes
-----------------

Os primeiros compiladores foram projetados para tratar de programas grandes... essencialmente programas infinitos. Naqueles tempos não havia muita escolha; a idéia de bibliotecas de rotinas e compilação separada ainda pareciam distantes no futuro. Novamente, esta abordagem levou a projetos de múltiplas passagens e arquivos intermediários para conter os resultados do processamento parcial.

O objetivo declarado por Brinch Hansen era de que o compilador deveria ser capaz de compilar a si mesmo. Novamente, por causa da memória limitada, isto o levou a fazer um projeto de múltiplas passagens. Ele precisava do mínimo possível de código do compilador residente em memória, para que as tabelas necessárias e outras estruturas coubessem na memória.

Eu não havia comentado sobre isto antes, pois não havia necessidade... nós só lemos e escrevemos os dados diretamente, de qualquer forma. Mas apenas para constar, meus planos sempre foram que, em um compilador de produção, o código fonte e objeto devem coexistir da memória RAM com o compilador. É por isso que eu cuidadosamente mantive `nextChar()` e outras rotinas como rotinas separadas, apesar de seu tamanho reduzido. Vai ser fácil alterá-las para ler e gravar na memória.

Ênfase na eficiência
--------------------

John Backus declarou que, quando ele e seus colegas desenvolveram o compilador FORTRAN original, eles SABIAM que teriam que produzir código enxuto. Naqueles dias, havia um forte sentimento contra linguagens de alto-nível e em favor de linguagem assembly, e a razão era a eficiência. Se FORTRAN não produzisse código muito bom para os padrões do assembly, os usuários simplesmente iriam se recusar a usá-lo. Uma curiosidade: este compilador FORTRAN acabou sendo um dos compiladores mais eficientes já construídos, em termos de qualidade de código. Mas era MUITO complexo!

Hoje, temos capacidade de CPU e espaço de memória RAM para esbanjar, então a eficiência do código não é um problema tão grande. Ignorando cuidadosamente este problema, fomos de fato capazes de Manter a Coisa Simples (Keep It Simple, lembra?). Ironicamente, como eu já disse, eu descobri que há alguns tipos de otimização, que podemos adicionar à estrutura básica do compilador sem adicionar muita complexidade. Neste caso, podemos ter nosso bolo e comê-lo também: vamos acabar com um código de qualidade razoável de qualquer forma.

Conjunto de instruções limitado
-------------------------------

Os primeiros computadores tinham conjuntos de instruções primitivos. Coisas que tomamos por garantidas, como operações de pilha e endereçamento indireto, vieram apenas com grande dificuldade.

Por exemplo: Em muitos projetos de compilador, há uma estrutura de dados chamada "literal pool" (algo como poço de literais). O compilador tipicamente identifica todas as literais usadas no programa, e as coleta em estruturas de dados simples. Todas as referências a estas literais são feitas indiretamente através da "literal pool". No fim da compilação, o compilador emite comandos para conseguir armazenamento e inicializar a "literal pool".

Nós nunca tivemos que lidar com este problema. Quando queríamos carregar uma literal, nós simplesmente o fazíamos em linha, como:

~~~
    MOV AX, 3
~~~

Há algo a ser dito a respeito do uso da "literal pool", particularmente no caso de máquinas x86 onde os dados e o código podem ser separados. Ainda assim, a coisa todo adiciona um bocado de complexidade com pouca coisa em troca.

Logicamente, sem a pilha estaríamos perdidos. Em um micro, tanto as chamadas de sub-rotinas como o armazenamento temporário dependem muito da pilha, e nós a usamos até mais do que necessário para facilitar a análise das expressões.

Desejo por generalidade
-----------------------

Muito do conteúdo dos livros típicos sobre compiladores é sobre problemas que não discutimos aqui... coisas como tradução automática de gramáticas, ou geração de tabelas de análise LALR. Isto não é só porque os autores querem impressionar você. Há razões boas, e práticas para estes assuntos estarem ali.

Nós estivemos nos concentrando no uso de um analisador descendente-recursivo para analisar uma gramática determinística, isto é, uma gramática que não é ambígua e, portanto, pode ser analisada com apenas um nível de "lookahead". Eu não criei muito desta limitação, mas o fato é que isto representa um pequeno subconjunto das gramáticas possíveis. Na verdade, há um número infinito de gramáticas que não podemos analisar com nossas técnicas. A técnica LR é mais poderosa, e pode tratar de gramática que não podemos.

Em teoria de compiladores, é importante saber como lidar com estas outras gramáticas, e como transformá-las em gramáticas que são mais fáceis de tratar. Por exemplo, muitas (mas não todas) gramáticas ambíguas podem ser transformadas em gramáticas não ambíguas. A maneira como fazer isto nem sempre é óbvia, e é por isto que tantas pessoas dedicaram vários anos no desenvolvimento de formas de transformá-las automaticamente.

Na prática, estes problemas acabam sendo consideravelmente menos importantes. Linguagens modernas tendem a ser projetadas para ser facilmente analisadas, de qualquer forma. Esta foi uma motivação chave no projeto da linguagem Pascal por exemplo. Certamente, há gramáticas patológicas para as quais seria complicado escrever gramáticas BNF não ambíguas, mas no mundo real provavelmente a melhor resposta é evitar estas gramáticas!

No nosso caso, logicamente, nós deixamos a linguagem se desenvolver conforme avançamos e por isso não ficamos presos em nenhum canto até aqui. Talvez você não se deva dar sempre este privilégio. Mas com um pouco de cuidado é possível manter o analisador simples sem ter que recorrer a tradução automática de gramáticas.

Nós tomamos uma abordagem muito diferente nesta série. Começamos com uma folha de papel em branco, e desenvolvemos nossas técnicas que funcionam no contexto em que estamos; isto é, um PC de um só usuário com ampla capacidade de CPU e espaço de memória. Nos limitamos a gramáticas razoáveis que são simples de analisar, usamos o conjunto de instruções da CPU em nossa vantagem, e não nos preocupamos com eficiência. É por isso que tem sido fácil.

Isto significa que estaremos sempre condenados a conseguir construir somente compiladores "de brinquedo"? Não, eu acho que não. Como eu já disse, é possível adicionar certas otimizações sem alterar a estrutura do compilador. Se queremos processar arquivos grandes, podemos sempre adicionar buffers para eles. Este tipo de coisa não afeta o projeto geral do programa.

E eu acho que isto é um fator chave. Começando com casos pequenos e limitados, fomos capazes de nos concentrar numa estrutura para o compilador que é natural para o trabalho. Como a estrutura naturalmente se acomoda ao trabalho, está quase sempre limitada a ser simples e transparente. Adicionar novas capacidades não necessariamente implica em alterar a estrutura básica. Sempre podemos simplesmente expandir as coisas, como a estrutura de arquivos ou adicionar uma camada de otimização.  Eu acho que é isso que eu sinto, de volta ao tempo em que os recursos eram limitados, as estruturas que as pessoas acabaram construindo as levaram a trabalhar sob certas condições, e não eram estruturas ótimas para o problema que tinham em mãos.

Conclusão
---------

Este é o meu palpite a respeito de como conseguimos manter as coisas simples. Começamos com algo simples e deixamos que evoluísse naturalmente, sem forçar as coisas a seguir um molde tradicional.

Nós vamos continuar desta forma. Eu lhe dei uma lista das áreas que pretendo cobrir nos capítulos futuros. Com estes capítulos, você será capaz de construir compiladores completos e funcionais para quase qualquer ocasião, e construí-los de forma simples. Se você realmente quer construir um compilador com qualidade de produção, você será capaz de fazê-lo também.

Para aqueles de vocês que estão esperando mais um pouco de código para o analisador, eu devo me desculpar por este afastamento. Eu apenas pensei que você gostaria de ver as coisas com uma certa perspectiva um pouco. Da próxima vez, vamos voltar ao objetivo principal do tutorial.

Até aqui, estivemos olhando apenas para partes de um compilador, e embora conheçamos muito da construção de uma linguagem completa, ainda não falamos sobre como juntar tudo. Isto vai ser o assunto dos nossos próximos dois capítulos. Então vamos continuar com novos assuntos que eu listei no começo desta parte.

Até lá!

{% include footer.md %}