Gerando um executável
=====================

> **Autor:** Felipo Soranz (24/03/2022)

Ao lidar diretamente com assembly, a forma de interagir com o sistema operacional varia muito entre arquiteturas (16, 32, 64-bits) e de um SO para o outro.

Aqui estão algumas indicações de como é possível começar a gerar algum código executável para as saídas assembly do compilador.

16-bits?
--------

Mesmo o código gerado sendo assembly para 8086 16-bits, é possível executá-lo em CPUs de 32 e 64-bits devido à retrocompatibilidade das CPUs Intel/AMD.

Se preferir, não é muito difícil converter as instruções geradas para 32-bits (ou 64-bits), mas isto fica como *"lição de casa"*.

Entrada/Saída
-------------

Enquanto não for implementada entrada/saída, a única forma de **VER** a execução do programa é através de um depurador, executando as instruções passo a passo e examinar os registradores e endereços de memória. Pode até parecer muito "baixo nível", mas não foi por isso que você começou a estudar sobre compiladores afinal?!?! :)

Algumas sugestões:

- DOS: DEBUG.COM, Turbo Debugger
- Windows: WinDBG, OllyDbg, Visual Studio
- Outros: GDB

> Instruções de como usar um depurador estão fora do escopo deste artigo!

Código-fonte
------------

O primeiro passo é salvar a saída do compilador em um arquivo com extensão ASM. Você pode "copiar e colar" do console ou redirecionar a entrada (`exemplo.tiny`) e saída (`exemplo.asm`) conforme abaixo:

    seu_compilador_tiny.exe < exemplo.tiny > exemplo.asm

Você deve ajustar a saída do seu compilador para incluir algumas instruções extras para a plataforma-alvo, ou editar o código assembly seguindo as instruções abaixo.

Variáveis
---------

Se o programa utiliza variáveis, mas você ainda não chegou na parte do tutorial que trata de declaração/alocação você precisa declará-las manualmente.

Acrescente na seção apropriada algo do tipo:

    section .data
    A dw 4
    B dw -1
    C dw 3
    D dw 0

(com o nome das variáveis do seu programa e os valores iniciais desejados, é claro!)

Montador (Assembler)
--------------------

O montador indicado é o *Netwide Assembler* que é gratuito, possui código-aberto, é multiplataforma, tem uma sintaxe simples e gera saída em diversos formatos.

Pode ser obtido em <https://nasm.us/> ou pelo sistema de pacotes da sua distro Linux.

É possível utilizar outro montador, mas a sintaxe pode ter de ser ajustada.

O formato dos parâmetros é:

    nasm.exe -f FORMATO ARQUIVO.ASM -o ARQUIVO.OBJ

Os exemplos assumem que o NASM esteja configurado no PATH do sistema.

MS-DOS ou compatível
--------------------

Use o exemplo abaixo como [modelo](src/exec/exemplo_dos.asm):

~~~asm
{% include_relative src/exec/exemplo_dos.asm %}
~~~

> Você precisa indicar ao sistema operacional que o programa terminou, ou o código vai seguir executando qualquer "lixo" que esteja na memória após o final do seu programa!

Na linha de comando, rode o NASM para gerar um arquivo no formato .COM do DOS (não é necessário ligador/linker).

    nasm.exe -f bin exemplo_dos.asm -o exemplo.com

O arquivo pode ser executado em um computador (ou máquina virtual, ex: Bochs, Qemu) com DOS ou emulador (ex: DOSBOX).

> Devido à natureza do modelo de memória de 16-bits e da simplicidade do nosso compilador, o tamanho do programa não pode exceder 64kb de memória (limitação do formato .COM). É possível escrever programas maiores para formato .EXE, mas isto está fora do escopo deste tutorial!

Windows (32 ou 64-bits)
-----------------------

Use o exemplo abaixo como [modelo](src/exec/exemplo_win.asm):

~~~asm
{% include_relative src/exec/exemplo_win.asm %}
~~~

Se estiver usando ferramentas Microsoft (ex: Visual Studio), garanta que as variáveis de ambiente estejam configuradas. Ajuste o comando abaixo conforme seu sistema:

    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

Para Windows 32-bits (repare no `start`!):

    nasm -f win32 exemplo_win.asm -o exemplo.obj
    link /subsystem:windows /entry:start exemplo.obj

Para Windows 64-bits (repare no `_start`!):

    nasm -f win64 exemplo_win.asm -o exemplo.obj
    link /subsystem:windows /entry:_start exemplo.obj
 
Se estiver usando ferramentas GNU (ex: MinGW), troque o `link` por:

    ld -e _start exemplo.obj -o exemplo.exe

Linux
-----

Use um dos modelos abaixo, conforme a arquitetura e a versão do kernel.

Modelo para kernel 32-bits ([elf32](src/exec/exemplo_elf32.asm)):

~~~asm
{% include_relative src/exec/exemplo_elf32.asm %}
~~~

Modelo para kernel 64-bits ([elf64](src/exec/exemplo_elf64.asm)):

~~~asm
{% include_relative src/exec/exemplo_elf64.asm %}
~~~

> Você precisa indicar ao sistema operacional que o programa terminou, ou o código vai seguir executando qualquer "lixo" que esteja na memória após o final do seu programa!

Na linha de comando, rode **UM** dos comandos para montar em um arquivo objeto (.o) no formato 32 ou 64-bits, conforme o seu kernel.

    nasm.exe -f elf32 exemplo_elf32.asm -o exemplo.o
    nasm.exe -f elf64 exemplo_elf64.asm -o exemplo.o

Para gerar o executável, é necessário o linker:

    ld -e _start exemplo.o -o exemplo

Se a versão 32-bits não funcionar, experimente:

    ld -m elf_i386 -e _start exemplo.o -o exemplo
