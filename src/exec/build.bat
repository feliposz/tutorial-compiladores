nasm -f bin exemplo_dos.asm -o exemplo.com

nasm -f elf32 exemplo_elf32.asm -o exemplo_elf32.o
ld -m elf_i386 -e _start exemplo_elf32.o -o exemplo_elf32

nasm -f elf64 exemplo_elf64.asm -o exemplo_elf64.o
ld -e _start exemplo_elf64.o -o exemplo_elf64

nasm -f win32 exemplo_win.asm -o exemplo_win32.obj
link /subsystem:windows /entry:start exemplo_win32.obj

nasm -f win64 exemplo_win.asm -o exemplo_win64.obj
link /subsystem:windows /entry:_start exemplo_win64.obj
