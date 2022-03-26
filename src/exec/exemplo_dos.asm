org 100h    ; endereco de memoria inicial de um programa .COM

section .data

    ; dados inicializados estaticamente

    ; =================================================
    ; DECLARE AQUI AS VARIAVEIS GLOBAIS DO SEU PROGRAMA
    ; =================================================

section .bss

    ; dados nao inicializados 
    ; (esta secao pode ser omitida nos nossos exemplos)

section .text

    ; ============================================
    ; SUBSTITUA O EXEMPLO ABAIXO PELO SEU PROGRAMA
    ; ============================================

    CALL READ      ; Recebe um valor inteiro em AX
    NEG AX         ; Inverte o sinal
    CALL WRITE     ; Exibe o valor de AX

    MOV AX, 4C00H  ; AH=4C: indica fim do programa  AL=00: sucesso
    INT 21H        ; 21: chamada de sistema do DOS

%include "tinyrtl_dos.inc"
