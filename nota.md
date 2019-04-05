# Nota

Esta série tem uma abordagem muito diferente da maioria dos textos sobre compiladores. Ela trata dos aspectos práticos de sua construção, sem poluir o texto com excesso de teoria.

Não que a teoria não seja importante, pelo contrário, ela é muito importante. Mas eu creio que você terá um aproveitamento muito melhor dos textos teóricos, depois de conhecer a maior parte dos aspectos práticos da implementação de um compilador e, quem sabe, depois de ter criado um ou mais de sua própria autoria.

Eu comecei a tradução deste tutorial como um projeto pessoal, apenas para obter um conhecimento mais aprofundado do assunto. Enquanto eu pesquisava, percebi que é muito difícil achar material em português (+/- 2002), especialmente material introdutório, que não seja para especialistas, sobre compiladores. Principalmente na Web. Por isso tomei a decisão de publicá-lo aqui.

Algumas pessoas podem achar que o tutorial está um pouco "desatualizado" (o autor começou a escrevê-lo em 1988 e interrompeu seu trabalho em 1995), principalmente agora que temos tanta tecnologia de orientação a objeto, ferramentas automatizadas para construção de compiladores, etc.

Em parte isto é verdade. Muitos dos compiladores modernos são escritos usando-se técnicas bem mais avançadas do que as usadas aqui. Mas também são mais complexas, o que pode confundir e desencorajar aqueles que estão começando. Além disso, conhecendo o básico da construção de compiladores, fica muito mais fácil o estudo das técnicas modernas.

Fiz poucas alterações no conteúdo do texto em si. Algumas partes são de minha própria autoria e poucas foram cortadas. Quase todas as alterações durante a tradução, tem a ver com a adaptação do texto. O original fazia uso da linguagem Pascal e a plataforma alvo do compilador era um computador com processador Motorola 68000.

Eu decidi adaptar o tutorial para usar linguagem C e processadores 80x86, pois C é uma linguagem bem mais popular (em especial na criação de compiladores) e a plataforma 80x86 é a mesma da maioria dos PCs encontrados hoje em dia.

Da mesma forma que eu fui capaz de adaptar o código de Pascal/Motorola 68000 para C/80x86 você deve ser capaz de adaptá-lo para a linguagem e/ou processador da sua preferência, mesmo sem conhecer a linguagem assembly do 80x86.

Infelizmente, o doutor Crenshaw precisou interromper o tutorial no capítulo 16, antes de ser capaz de tratar de alguns tópicos mais avançados como vetores, registros, E/S com arquivos, aritmética de ponto-flutuante, otimização, etc. Mas eu tenho certeza que vocês serão capazes de implementar isto em seus compiladores com um pouco de estudo adicional.

Espero que os leitores deste tutorial façam bom uso do conhecimento contido nele.
