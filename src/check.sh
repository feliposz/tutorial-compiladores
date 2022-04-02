#!/bin/bash
echo Executando testes
./test.sh > ../bin/test.out 2>&1
echo Comparando resultado
diff test.sh.cmp ../bin/test.out