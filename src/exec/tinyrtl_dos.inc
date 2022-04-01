; ==========================================================
; TINYRTL (Versao DOS)
; 
; Biblioteca de tempo de execucao para a linguagem TINY, 
; implementada no tutorial "Vamos Construir Um Compilador".
;
; https://feliposz.github.io/tutorial-compiladores/
; 
; Autor: Felipo Soranz
; Data: 25/03/2022
;
; Este código é de livre distribuição e uso.
; ==========================================================

; **********************************************************
; Constantes
; **********************************************************

DOS_SYSCALL     equ 21h
DOS_WRITE_STR   equ 09h
DOS_READ_STR    equ 0ah

; **********************************************************
; ROTINAS EXTERNAS
; **********************************************************

section .text

; ==========================================================
; WRITE -- Exibe um numero inteiro com sinal
; Entradas: AX = valor a ser exibido
; Saidas: nenhuma
; Efeitos: Nao preserva os registradores!
WRITE:
    call __sint_to_string
    mov dx, ax
    mov ah, DOS_WRITE_STR
    int DOS_SYSCALL

    mov dx, __newline_string
    mov ah, DOS_WRITE_STR
    int DOS_SYSCALL

    ret

; ==========================================================
; READ -- Recebe um numero inteiro com sinal
; Entradas: nenhuma
; Saidas: AX = valor recebido
; Efeitos: Nao preserva os registradores!
READ:
    mov dx, __prompt_string   ; exibe prompt
    mov ah, DOS_WRITE_STR
    int DOS_SYSCALL

    mov dx, __input_buffer    ; le entrada do usuario
    mov ah, DOS_READ_STR
    int DOS_SYSCALL

    mov dx, __newline_string
    mov ah, DOS_WRITE_STR
    int DOS_SYSCALL

    xor bx, bx
    mov bl, [__input_count]          ; bx = caracteres lidos
    mov byte [__input_data + bx], 0  ; coloca terminador nulo no fim do buffer

    mov si, __input_data
    call __string_to_sint
    ret

; **********************************************************
; ROTINAS INTERNAS
; **********************************************************

; ==========================================================
; __string_to_sint -- Converte string para valor inteiro 16-bits com sinal
; Entradas: SI = endereco da string (max 6 caracteres, '-32768' ate '+32767')
; Saidas: AX = valor inteiro com sinal
; Efeitos: Nao preserva os registradores!
__string_to_sint:
    mov al, [si]             ; verifica se comeca com '-'
    cmp al, '-'
    je .negative
    cmp al, '+'
    jne .positive
    inc si                   ; pula o caractere '+'
.positive:
    call __string_to_int
    ret
.negative:
    inc si                   ; pula o caractere '-' e converte como se fosse positivo
    call __string_to_int
    neg ax                   ; inverte o sinal apos conversao
    ret

; ==========================================================
; __string_to_int -- Converte string para valor inteiro 16-bits sem sinal
; Entradas: SI = endereco da string (max 5 caracteres, '0' ate '65536')
; Saidas: AX = valor inteiro sem sinal
; Efeitos: Nao preserva os registradores!
__string_to_int:
    mov cx, 5       ; maximo de digitos que serao lidos
    mov bx, 10      ; base 10
    xor di, di
.next_digit:
    xor ax, ax
    lodsb
    cmp al, '0'
    jb .done
    cmp al, '9'
    ja .done
    sub al, '0'
    xchg ax, di
    mul bx
    add ax, di
    xchg ax, di
    loop .next_digit
.done:
    xchg ax, di
    ret
    
; ==========================================================
; __sint_to_string -- Converte inteiro com sinal para string
; Entradas: AX = inteiro com sinal
; Saidas: AX = endereco da string convertida (terminador $)
; Efeitos: Nao preserva os registradores!
__sint_to_string:
    cmp ax, 0
    jl .negative
    call __int_to_string
    ret
.negative:
    neg ax
    call __int_to_string
    mov ax, __neg_string
    ret

; ==========================================================
; __int_to_string -- Converte inteiro sem sinal para string
; Entradas: AX = inteiro sem sinal
; Saidas: AX = endereco da string convertida (terminador $)
; Efeitos: Nao preserva os registradores!
__int_to_string:
    mov di, __int_string
    or ax, ax
    jnz .not_zero
    mov dx, '0'
    mov cx, 1
    push dx
    jmp .pop_digits
.not_zero:
    mov bx, 10
    mov cx, 0
.push_digits:
    xor dx, dx
    or ax, ax
    jz .pop_digits
    div bx
    add dl, '0'
    push dx
    inc cx
    jmp .push_digits
.pop_digits:
    pop dx
    mov [di], dl
    inc di
    dec cx
    or cx, cx
    jnz .pop_digits
.done:
    mov byte [di], '$'           ; acrescenta terminador ao final da string
    mov ax, __int_string
    ret

; **********************************************************
; Variaveis para uso da biblioteca
; **********************************************************

section .data

__input_buffer    db 7             ; tamanho maximo do buffer
__input_count     db 0             ; caracteres lidos
__input_data      times 7 db ' '   ; dados recebidos

__neg_string      db '-'           ; inicio da string para numero negativo
__int_string      times 7 db 0     ; inicio da string convertida
__prompt_string   db '? $'         ; prompt exibido no comando READ
__newline_string  db 13, 10, '$'   ; avanco de linha
