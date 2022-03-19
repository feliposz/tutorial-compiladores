@echo Executando testes
@call test.bat > test.out 2>&1
@echo Comparando resultado
@diff test.out test.cmp