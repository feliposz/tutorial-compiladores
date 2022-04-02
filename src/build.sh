#!/bin/bash
echo Compilando programas
mkdir -p ../bin
gcc cap01-craddle.c -o ../bin/cap01-craddle
gcc cap02-expr.c -o ../bin/cap02-expr
gcc cap03-multi.c -o ../bin/cap03-multi
gcc cap03-single.c -o ../bin/cap03-single
gcc cap04-interp.c -o ../bin/cap04-interp
gcc cap05-control.c -o ../bin/cap05-control
gcc cap06-bool1.c -o ../bin/cap06-bool1
gcc cap06-bool2.c -o ../bin/cap06-bool2
gcc cap07-lex-char.c -o ../bin/cap07-lex-char
gcc cap07-lex-enum.c -o ../bin/cap07-lex-enum
gcc cap07-lex-subset.c -o ../bin/cap07-lex-subset
gcc cap07-subset.c -o ../bin/cap07-subset
gcc cap09-top-c.c -o ../bin/cap09-top-c
gcc cap09-top-pascal.c -o ../bin/cap09-top-pascal
gcc cap10-tiny01.c -o ../bin/cap10-tiny01
gcc cap10-tiny10.c -o ../bin/cap10-tiny10
gcc cap11-lex.c -o ../bin/cap11-lex
gcc cap11-tiny11.c -o ../bin/cap11-tiny11
gcc cap12-tiny12.c -o ../bin/cap12-tiny12
gcc cap12-tiny13.c -o ../bin/cap12-tiny13
gcc cap13-base.c -o ../bin/cap13-base
gcc cap13-byval.c -o ../bin/cap13-byval
gcc cap13-byref.c -o ../bin/cap13-byref
gcc cap13-locals.c -o ../bin/cap13-locals
gcc cap14-base.c -o ../bin/cap14-base
gcc cap14-types.c -o ../bin/cap14-types
gcc cap15/SINGLE/*.c -o ../bin/cap15-single
gcc cap15/MULTI/*.c -o ../bin/cap15-multi
gcc cap16/SINGLE/*.c -o ../bin/cap16-single
gcc cap16/MULTI/*.c -o ../bin/cap16-multi