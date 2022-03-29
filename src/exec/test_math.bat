gcc -o gen_test_mul32.exe gen_test_mul32.c
gen_test_mul32.exe > test_mul32.asm 
nasm -f bin test_mul32.asm -o tstmul32.com

gcc -o gen_test_div32.exe gen_test_div32.c
gen_test_div32.exe > test_div32.asm 
nasm -f bin test_div32.asm -o tstdiv32.com
