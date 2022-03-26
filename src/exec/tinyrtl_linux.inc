; ==========================================================
; TINYRTL (Versao Linux)
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
; ROTINAS EXTERNAS
; **********************************************************

section .text

; ==========================================================
; WRITE -- Exibe um numero inteiro com sinal
; Entradas: AX = valor a ser exibido
; Saidas: nenhuma
WRITE:
    ; Implementar

    ret

; ==========================================================
; READ -- Recebe um numero inteiro com sinal
; Entradas: nenhuma
; Saidas: AX = valor recebido
READ:
    ; Implementar
    mov ax, 12345
    ret