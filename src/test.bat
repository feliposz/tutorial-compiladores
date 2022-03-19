@prompt TEST: 
@echo,
@echo =================
@echo Testes de Sucesso
@echo =================
@echo,

cap01-craddle <nul

echo 0| cap02-expr
echo -1| cap02-expr
echo 2+3| cap02-expr
echo 4-5| cap02-expr
echo 6*7| cap02-expr
echo 8/9| cap02-expr
echo 2+3*4| cap02-expr
echo 1+2-3*4/5| cap02-expr
echo 2*(3+4)| cap02-expr
echo 1+(2-(3+(4-5)))| cap02-expr
echo (1+2)/((3+4)+(5-6))| cap02-expr
echo -(3-2)| cap02-expr

echo d=b*b-4*a*c| cap03-single
echo x=f()+g()| cap03-single

echo nota=(prova1+prova2*2)/3| cap03-multi
echo nota = (prova1 + prova2 * 2) / 3| cap03-multi
echo x = 2 * y + 3| cap03-multi
echo x = x + 3 - 2 - (5 - 4)| cap03-multi
echo delta  =  (b * b  )  -(  4*a*c   )| cap03-multi
echo resultado = funcao () + outra(  )| cap03-multi

echo a=1^
b=2^
c=3^
d=b*b-4*a*c^
!d^
.| cap04-interp

echo iee| cap05-control
echo AiBeCe| cap05-control
echo AiBiCeDeFe| cap05-control
echo AiBlCeDe| cap05-control
echo wXee| cap05-control
echo pXee| cap05-control
echo rXue| cap05-control
echo fI=ee| cap05-control
echo AfI=BeCe| cap05-control
echo dee| cap05-control
echo pABCbDEFee| cap05-control
echo pwXepYbZeiAlberCfI=Deuee| cap05-control

echo a*(-b)| cap06-bool1
echo a-(-b)| cap06-bool1
echo a^^^&!b| cap06-bool1
echo T| cap06-bool1
echo F| cap06-bool1
echo !T| cap06-bool1
echo T^^^&F| cap06-bool1
echo T^^^|F| cap06-bool1
echo T^^^~F| cap06-bool1
echo 1^^^>2| cap06-bool1
echo 3^^^<4| cap06-bool1
echo 5=6| cap06-bool1
echo 5#6| cap06-bool1
echo x()| cap06-bool1
echo ((x()+1)^^^>(5*y()))^^^|(a#b^^^&c^^^<d)| cap06-bool1

echo iA=BX=1lX=2ee| cap06-bool2
echo X=1fA=1t9Y=2eZ=3e| cap06-bool2

echo 1 23 456 a bc def = # . : ; ! ? + - * / if else endif end| cap07-lex-enum
echo 1 23 456 a bc def = # . : ; ! ? + - * / if else endif end| cap07-lex-char
echo ia=1lb=2ee | cap07-subset
echo if a = 1 else b = 2 endif end | cap07-lex-subset

echo pXbe.| cap09-top-pascal
echo pXlcltcvtpvfvlbXYZe.| cap09-top-pascal

echo,| cap09-top-c
echo iI,J;g(){}uU;iS;f(){}cC,D;| cap09-top-c
echo auiX,Y;xiZ;| cap09-top-c

@echo,
@echo =================
@echo Testes de Falha
@echo =================
@echo,

cap02-expr <nul
echo x| cap02-expr
echo 1+| cap02-expr
echo *1| cap02-expr

echo 1+2 3+4| cap03-single
echo x=f(| cap03-single

cap03-multi <nul

cap04-interp <nul

cap05-control <nul

cap06-bool1 <nul
echo a*-b| cap06-bool1
echo a--b| cap06-bool1

cap06-bool2 <nul
cap07-lex-subset <nul
cap07-subset <nul
cap09-top-pascal <nul
@prompt