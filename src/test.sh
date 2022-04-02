#!/bin/bash
pushd ../bin > /dev/null

echo
echo "================="
echo "Testes de Sucesso"
echo "================="
echo

set -v

echo ""| ./cap01-craddle

echo "0"| ./cap02-expr
echo "-1"| ./cap02-expr
echo "2+3"| ./cap02-expr
echo "4-5"| ./cap02-expr
echo "6*7"| ./cap02-expr
echo "8/9"| ./cap02-expr
echo "2+3*4"| ./cap02-expr
echo "1+2-3*4/5"| ./cap02-expr
echo "2*(3+4)"| ./cap02-expr
echo "1+(2-(3+(4-5)))"| ./cap02-expr
echo "(1+2)/((3+4)+(5-6))"| ./cap02-expr
echo "-(3-2)"| ./cap02-expr

echo "d=b*b-4*a*c"| ./cap03-single
echo "x=f()+g()"| ./cap03-single

echo "nota=(prova1+prova2*2)/3"| ./cap03-multi
echo "nota = (prova1 + prova2 * 2) / 3"| ./cap03-multi
echo "x = 2 * y + 3"| ./cap03-multi
echo "x = x + 3 - 2 - (5 - 4)"| ./cap03-multi
echo "delta  =  (b * b  )  -(  4*a*c   )"| ./cap03-multi
echo "resultado = funcao () + outra(  )"| ./cap03-multi

echo "a=1
b=2
c=3
d=b*b-4*a*c
!d
."| ./cap04-interp

echo "iee"| ./cap05-control
echo "AiBeCe"| ./cap05-control
echo "AiBiCeDeFe"| ./cap05-control
echo "AiBlCeDe"| ./cap05-control
echo "wXee"| ./cap05-control
echo "pXee"| ./cap05-control
echo "rXue"| ./cap05-control
echo "fI=ee"| ./cap05-control
echo "AfI=BeCe"| ./cap05-control
echo "dee"| ./cap05-control
echo "pABCbDEFee"| ./cap05-control
echo "pwXepYbZeiAlberCfI=Deuee"| ./cap05-control

echo "a*(-b)"| ./cap06-bool1
echo "a-(-b)"| ./cap06-bool1
echo "a&!b"| ./cap06-bool1
echo "T"| ./cap06-bool1
echo "F"| ./cap06-bool1
echo "!T"| ./cap06-bool1
echo "T&F"| ./cap06-bool1
echo "T|F"| ./cap06-bool1
echo "T~F"| ./cap06-bool1
echo "1>2"| ./cap06-bool1
echo "3<4"| ./cap06-bool1
echo "5=6"| ./cap06-bool1
echo "5#6"| ./cap06-bool1
echo "x()"| ./cap06-bool1
echo "((x()+1)>(5*y()))|(a#b&c<d)"| ./cap06-bool1

echo "iA=BX=1lX=2ee"| ./cap06-bool2
echo "X=1fA=1t9Y=2eZ=3e"| ./cap06-bool2

echo "1 23 456 a bc def = # . : ; ! ? + - * / if else endif end"| ./cap07-lex-enum
echo "1 23 456 a bc def = # . : ; ! ? + - * / if else endif end"| ./cap07-lex-char
echo "ia=1lb=2ee "| ./cap07-subset
echo "if a = 1 else b = 2 endif end "| ./cap07-lex-subset

echo "pXbe."| ./cap09-top-pascal
echo "pXlcltcvtpvfvlbXYZe."| ./cap09-top-pascal

echo ""| ./cap09-top-c
echo "iI,J;g(){}uU;iS;f(){}cC,D;"| ./cap09-top-c
echo "auiX,Y;xiZ;"| ./cap09-top-c

echo "pbe."| ./cap10-tiny01
echo "pvXvYvZbe."| ./cap10-tiny01
echo "pvX,Y,ZvM,NvObe."| ./cap10-tiny01
echo "pvX=1,Y=2,Z=3vM,NvO=9be."| ./cap10-tiny01
echo "pvX=-1,Y=23,Z=456vM,NvO=-7890be."| ./cap10-tiny01
echo "pbXYZe."| ./cap10-tiny01
echo "pvX=1,Y=2,ZbZ=X-Y+(-X*Y)/10e."| ./cap10-tiny01
echo "pvX,Y,ZbX=Z>Ye."| ./cap10-tiny01
echo "pvXbX=0wX<9X=X+1ee."| ./cap10-tiny01
echo "pvX,YbX=1iX=1Y=Y-1lY=Y+2ee."| ./cap10-tiny01

echo "Program Begin End ."| ./cap10-tiny10
echo "Program Var x Var y Var z Begin End ."| ./cap10-tiny10
echo "Program Var x,Y,z Var M , N Var O Begin End ."| ./cap10-tiny10
echo "Program Var x = 1 , y = 2 , z = 3 Var M , N Var O = 9 Begin End ."| ./cap10-tiny10
echo "Program Var x = - 1 , y = 23 , z = 456 Var M , N Var O = - 7890 Begin End ."| ./cap10-tiny10
echo "Program Var x = 1 , y = 2 , z Begin z = x - y + ( - x * y ) / 10 End ."| ./cap10-tiny10
echo "Program Var x , y , z Begin x = z >= y End ."| ./cap10-tiny10
echo "Program Var x Begin x = 0 While x <= 9 x = x + 1 EndWhile End ."| ./cap10-tiny10
echo "Program Var x , y Begin x = 1 If x = 1 y = y - 1 Else y = y + 2 EndIf End ."| ./cap10-tiny10
echo "Program Var alfa, beta Begin alfa = 1 If alfa <> 1 beta = beta - 1 Else beta = beta + 2 EndIf End ."| ./cap10-tiny10
echo "Program Var alfa, beta Begin Read(alfa, beta) If alfa >= 1 beta = beta - 1 Else beta = beta + 2 EndIf Write(alfa, beta) End . "| ./cap10-tiny10

echo "a bc def 1 23 456 + - * / = >= <= ."| ./cap11-lex

echo "Program Begin End ."| ./cap11-tiny11
echo "Program Var x Var y Var z Begin End ."| ./cap11-tiny11
echo "Program Var x,Y,z Var M , N Var O Begin End ."| ./cap11-tiny11
echo "Program Var x , y , z Var M , N Var O Begin End ."| ./cap11-tiny11
echo "Program Var x , y , z Var M , N Var O Begin End ."| ./cap11-tiny11
echo "Program Var x , y , z Begin z = x - y + ( - x * y ) / 10 End ."| ./cap11-tiny11
echo "Program Var x , y , z Begin x = z >= y End ."| ./cap11-tiny11
echo "Program Var x Begin x = 0 While x <= 9 x = x + 1 EndWhile End ."| ./cap11-tiny11
echo "Program Var x , y Begin x = 1 If x = 1 y = y - 1 Else y = y + 2 EndIf End ."| ./cap11-tiny11
echo "Program Var alfa, beta Begin alfa = 1 If alfa <> 1 beta = beta - 1 Else beta = beta + 2 EndIf End ."| ./cap11-tiny11
echo "Program Var alfa, beta Begin Read(alfa, beta) If alfa >= 1 beta = beta - 1 Else beta = beta + 2 EndIf Write(alfa, beta) End . "| ./cap11-tiny11

echo "Program; Var x, y; Begin x = 0; y = 10; While !(x = 10) x = x + 1; y = y - 1; EndWhile; End ."| ./cap12-tiny12

echo "{x}Program{x};{x}{}Var{x} alfa, beta; Begin{x}   Read(alfa,{x} beta);   If alfa {x}>= 1{x}     beta {x}= beta - 1  {x{y}z} Else     beta{x} = beta + 2;{ }  EndIf{x};   Write(alfa{x}, beta){ }End{x}"| ./cap12-tiny13

echo "va vb vc b a=b c=b e."| ./cap13-base
echo "va px(m) b a=m e vb py(m,n) b b=m n=a x(b) e vc pz(m,n,o) b n=b c=m y(b,n) e Pp b x(a) a=b y(a,b) c=b z(a,b,c) e."| ./cap13-byval
echo "va px(m) b a=m e vb py(m,n) b b=m n=a x(b) e vc pz(m,n,o) b n=b c=m y(b,n) e Pp b x(a) a=b y(a,b) c=b z(a,b,c) e."| ./cap13-byref
echo "va px(m) b a=m e vb py(m,n) vi b b=m n=a i=b x(i) e vc pz(m,n,o) vi vj b n=b c=m y(b,n) i=j e Pp b x(a) a=b y(a,b) c=b z(a,b,c) e."| ./cap13-locals

echo ""| ./cap14-base
echo "ba wb lc B . "| ./cap14-types
echo "ba wb lc B a=a a=b a=c b=a b=b b=c c=a c=b c=c . "| ./cap14-types
echo "ba wb lc B a=42 b=1999 c=999999 a=888888 b=77 c=666 . "| ./cap14-types
echo "ba wb lc B a=a+a a=a-a b=b+b b=b-b c=c+c c=c-c . "| ./cap14-types
echo "ba wb lc B a=b+c a=c+b b=a+c b=c+b c=a+b c=b+a . "| ./cap14-types
echo "ba wb lc B a=b-c a=c-b b=a-c b=c-b c=a-b c=b-a . "| ./cap14-types
echo "ba wb lc B a=a*a a=a/a b=b*b b=b/b c=c*c c=c/c . "| ./cap14-types
echo "ba wb lc B a=b*c a=c*b b=a*c b=c*b c=a*b c=b*a . "| ./cap14-types
echo "ba wb lc B a=b/c a=c/b b=a/c b=c/b c=a/b c=b/a . "| ./cap14-types
echo "ba wb lc ld B d=(b*b)-(4*a*c) . "| ./cap14-types

echo "1 "| ./cap15-single
echo "x "| ./cap15-single

echo "12345 "| ./cap15-multi
echo "alfa "| ./cap15-multi

echo "a=-1 "| ./cap16-single
echo "a=-x "| ./cap16-single
echo "d=b*b-4*a*c/2 "| ./cap16-single
echo "s=o|n~s "| ./cap16-single
echo "b=(a|b)&(c~d) "| ./cap16-single
echo "x=a&!b|!a&b "| ./cap16-single

echo "a=-123 "| ./cap16-multi
echo "a=-alfa "| ./cap16-multi
echo "delta=beta*beta-412*alfa*cappa/2121 "| ./cap16-multi
echo "ser=ou|nao~ser "| ./cap16-multi
echo "bool=(a|b)&(c~d) "| ./cap16-multi
echo "myxor=a&!b|!a&b "| ./cap16-multi

set +v

echo
echo "================="
echo "Testes de Falha"
echo "================="
echo

set -v

echo ""| ./cap02-expr
echo "x"| ./cap02-expr
echo "1+"| ./cap02-expr
echo "*1"| ./cap02-expr

echo "1+2 3+4"| ./cap03-single
echo "x=f("| ./cap03-single

echo ""| ./cap03-multi

echo ""| ./cap04-interp

echo ""| ./cap05-control

echo ""| ./cap06-bool1
echo "a*-b"| ./cap06-bool1
echo "a--b"| ./cap06-bool1

echo ""| ./cap06-bool2
echo ""| ./cap07-lex-subset
echo ""| ./cap07-subset
echo ""| ./cap09-top-pascal

echo "."| ./cap10-tiny01
echo "p"| ./cap10-tiny01
echo "pb"| ./cap10-tiny01
echo "pbe"| ./cap10-tiny01
echo "pe."| ./cap10-tiny01
echo "pvavavabe."| ./cap10-tiny01

echo "? "| ./cap15-single
echo "? "| ./cap15-multi
echo "999999 "| ./cap15-multi
echo "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx "| ./cap15-multi

set +v

popd > /dev/null