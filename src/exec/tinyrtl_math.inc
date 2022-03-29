; ==========================================================
; TINYRTL - Modulo matematico
; 
; Biblioteca de tempo de execucao para a linguagem TINY, 
; implementada no tutorial "Vamos Construir Um Compilador".
;
; https://feliposz.github.io/tutorial-compiladores/
; 
; Autor: Felipo Soranz
; Data: 28/03/2022
;
; Este código é de livre distribuição e uso.
; ==========================================================

section .text

; ==========================================================
; MUL32 -- Multiplica dois inteiros de 32-bits com resultado de 32-bits (descarta qualquer overflow)
; Entradas: DX:AX = primeiro fator, CX:BX = segundo fator
; Saidas: DX:AX = resultado
; Efeitos: Preserva os demais registradores
;
;       DX:AX  - Fator A (AHI:ALO)
;       CX:BX  - Fator B (BHI:BLO)
;
;       DX, CX : 16-bits mais significativos (HI)
;       AX, BX : 16-bits menos significativos (LO)
;
;              ALO * BLO
;        AHI * BLO
; +      BHI * ALO
; -----------------------
;
MUL32:
    push si
    mov si, dx  ; Guarda DX
    or dx, cx   ; Testa os 16-bits mais significativos (AHI, BHI)
    jnz .full   ; Se nao for zero, faz multiplicacao completa
    mul bx      ; Se for zero, só precisa multiplicar os 16-bits menos significativos (ALO * BLO)
    pop si
    ret

.full:
    mov dx, si  ; Restaura DX

    push ax     ; Guarda ALO
    push dx     ; Guarda AHI

    mul cx      ; DX:AX = ALO * BHI
    mov si, ax  ; Salva resultado em SI, ignora DX

    pop ax      ; Carrega AHI
    mul bx      ; DX:AX = AHI * BLO
    add si, ax  ; Combina com SI já salvo, ignora DX

    pop ax      ; Carrega ALO
    mul bx      ; DX:AX = ALO * BLO
    add dx, si  ; Combina DX com SI (DX + ALO*BHI + AHI*BLO)

    pop si

    ret

; ==========================================================
; DIV32 -- Divide dois inteiros com sinal de 32-bits com resultado de 32-bits desprezando o resto
; Entradas: DX:AX = dividendo, CX:BX = divisor
; Saidas: DX:AX = quociente
; Efeitos: Preserva os demais registradores
;
DIV32:

    push di
    push si
    push bx
    push cx

; Verifica sinal dos operadores e ajusta para positivo se necessário

    mov di,1            ; DI = 1 (resultado positivo)

    or dx,dx            ; verifica se o dividendo DX:AX é positivo
    jge short .skipa    ; pula o resto se já for
    neg di              ; inverte sinal do resultado
    neg dx              ; torna DX:AX positivo
    neg ax
    sbb dx, 0
.skipa:
    or cx,cx            ; verifica se o divisor CX:BX é positivo
    jge short .skipb    ; pula o resto se já for
    neg di              ; inverte sinal do resultado
    neg cx              ; torna CX:BX positivo
    neg bx
    sbb cx,0
.skipb:

; Se divisor < 65536, aplica divisao simples das duas partes do dividendo (DX:AX) pelo divisor (BX).

    or cx,cx            ; verifica se divisor < 65536 (CX = 0)
    jnz short .full     ; se nao, faz a divisao completa
    push ax             ; guarda ALO
    mov ax,dx           ; carrega AHI
    xor dx,dx
    div bx              ; 0:AHI / BLO -> DX: resto da parte alta (usado abaixo), AX: quociente da parte alta
    mov cx,ax           ; salva quociente da parte alta
    pop ax              ; carrega ALO
    div bx              ; resto da parte alta:ALO / BLO -> DX: resto da parte baixa (ignorado), AX: quociente da parte baixa
    mov dx,cx           ; restaura quociente da parte alta
    jmp short .sign     ; ajusta sinal do resultado

; Salva dividendo (AHI:ALO) e divisor (BHI:BLO) para usar na verificacao abaixo

.full:
    push ax             ; ALO
    push dx             ; AHI
    push bx             ; BLO
    push cx             ; BHI

; Enquanto divisor >= 65536, deslocar divisor e dividendo a direita (equivale a dividir ambos por 2)

.shift:
    shr cx,1            ; desloca divisor (CX:BX) 1 bit a direita
    rcr bx,1
    shr dx,1            ; desloca dividendo (DX:AX) 1 bit a direita
    rcr ax,1
    or cx,cx
    jnz short .shift    ; repete ate divisor < 65536

; Aplica a divisao dos valores ajustados

    div bx              ; DX:AX / BX -> DX: resto, AX: quociente 
    mov si,ax           ; salva quociente em SI, ignora resto

; Para verificar o resultado, multiplicamos o quociente (SI) pelo divisor (BHI:BLO)

    pop ax              ; restaura BHI 
    mul si              ; quociente * BHI
    mov cx,ax           ; salva parcial

    pop ax              ; restaura BLO 
    mul si              ; quociente * BLO
    add dx,cx           ; DX:AX = quociente (SI) * divisor (BHI:BLO)

    pop cx              ; restaura AHI
    pop bx              ; restaura ALO

    jc short .decquot   ; se houve "vai-um" acima, significa que o quociente precisa ser ajustado

; Se (quociente * divisor) <= dividendo, o quociente esta correto
; senao divisor esta 1 maior do que deveria e deve ser ajustado

    cmp dx,cx           ; compara AHI com parte alta do resultado
    ja short .decquot   ; se > AHI, ajustar
    jb short .quotient  ; se < AHI, esta correto
    cmp ax,bx           ; se igual, prossegue comparando ALO com parte baixa do resultado
    jbe short .quotient ; se <= esta correto, senao ajustar
.decquot:
    dec si              ; ajustar quociente para baixo
.quotient:
    xor dx,dx           ; dx:ax <- quociente
    mov ax,si

; Ajusta o sinal do resultado

.sign:
    cmp di, 1           ; verifica sinal
    je short .result    ; se DI = 1, positivo
    neg dx              ; senao, inverte sinal de DX:AX
    neg ax
    sbb dx,0

.result:

    pop cx
    pop bx
    pop si
    pop di

    ret