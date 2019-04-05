# Ferramentas

Alguns links de ferramentas que você pode achar útil enquanto estiver seguindo o tutorial (ou pra outras coisas também é claro).

> **Nota:** Estas recomendações só estão aqui como referência. Eram o que existia na época. Elas estão bem **desatualizadas** !!!

## Compiladores

Se você estiver usando Linux, BSD ou algum Unix, é quase certo que já possua um compilador ANSI C instalado (como o GCC). Experimente digitar cc ou gcc na linha de comando. Caso não possua é relativamente simples (para um usuário de Linux/Unix pelo menos XD) instalar um compilador e um bom IDE.

Caso esteja usando Windows, experimente uma destas sugestões:

[MingW](http://www.mingw.org/) - Se você só quer um compilador para Windows para utilizá-lo na linha de comando ou com alguma IDE como Eclipse, NetBeans, etc. É o GCC portado para Windows.

[Code::Blocks](http://www.codeblocks.org/) - Um IDE bastante prático e simples de usar. Você pode baixar a versão que já vem com o MingW. É o que estou usando.

[Visual C++ Express](http://www.microsoft.com/express) - A Microsoft disponibiliza gratuitamente a versão Express de seu IDE/compilador. É suficiente para compilar os códigos. (Obs: Os códigos não foram todos testados com este compilador! Se tiver problemas, faça perguntas nos comentários. Tentarei revisar os códigos na medida do possível e testá-los no VC++)

[Dev-C++](http://www.bloodshed.net/devcpp.html) - Foi o IDE que eu usei na criação dos fontes para este tutorial. É relativamente fácil de usar e já vem com o MingW. Porém, não está recebendo mais atualizações e eu não o recomendo mais. 

[DJGPP](http://www.delorie.com/djgpp/) - Se você estiver usando DOS (!), talvez seja sua única alternativa. (Obs: Eu coloquei isto pois na época que eu traduzi o tutorial, eu programava uma parte do código em um 486 rodando DOS e a outra parte em outra máquina rodando Windows c/ Dev-C++).

## Referência

[libc.a reference](http://www.delorie.com/djgpp/doc/libc-2.02/) - Referência da biblioteca C (mais especificamente, a do DJGPP, mas vale pra todos os compiladores praticamente).

[Let's Build a Compiler](http://compilers.iecc.com/crenshaw/) - Versão original em inglês deste tutorial, escrita por Jack W. Crenshaw, usando nos exemplos Pascal e assembly Motorola 68000. É possível fazendo uma pesquisa numa boa ferramenta de busca para encontrar versões HTML, PDF, em Russo (!), etc. Este link é para a versão original MESMO.

[Compiler Construction](http://www.ethoberon.ethz.ch/) - Livro disponível gratuitamente em que Niklaus Wirth (o criador das linguagens Pascal, Modula-2, Oberon, etc.) implementa um compilador completo para uma versão simplificada da linguagem Oberon (muito parecida com Pascal). Certamente é um ótimo livro para quem seguiu este tutorial e quer ir além.

[The Compilers resources page](http://www.bloodshed.net/compilers/index.html) - Diversos links para interessados em compiladores e programação em geral.
