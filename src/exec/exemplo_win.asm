section .data

    ; dados inicializados estaticamente

    ; =================================================
    ; DECLARE AQUI AS VARIAVEIS GLOBAIS DO SEU PROGRAMA
    ; =================================================

section .bss

    ; dados nao inicializados 
    ; (esta secao pode ser omitida nos nossos exemplos)

section .text

    global _start   ; ponto de entrada do seu programa
_start:

    ; ============================================
    ; SUBSTITUA O EXEMPLO ABAIXO PELO SEU PROGRAMA
    ; ============================================

    CALL READ      ; Recebe um valor inteiro em AX
    NEG AX         ; Inverte o sinal
    CALL WRITE     ; Exibe o valor de AX

    RET            ; retorna ao runtime

%include "tinyrtl_win.inc"