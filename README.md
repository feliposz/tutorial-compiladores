# Vamos construir um compilador

Tradução e adaptação do tutorial *Let's Build A Compiler* de Jack Crenshaw.

## Publicação

O tutorial está disponível online em: <https://feliposz.github.io/tutorial-compiladores/>

Se estiver lendo uma cópia local ou no GitHub, comece pela [Apresentação](index.md).

## Exemplos

A versão final dos programas de exemplo construídos em cada capítulo estão no diretório `src`.

> Estes programas são uma implementação de referência apenas. O ideal, é que o leitor siga o tutorial, implemente e teste cada funcionalidade conforme ela é apresentada, recorrendo aos exemplos somente em caso de dúvida ou se ficar preso em algum ponto específico do tutorial.

Para compilar os programas é necessário ter instalado um compilador C (por exemplo, o GCC) acessível no PATH do sistema.

A partir da linha de comando execute:

```
build.bat
```

> Se usar um compilador diferente do GCC, edite o script `build.bat`!

Para limpar as saídas compiladas:

```
clean.bat
```

Para executar testes e conferir as saídas com o arquivo de referência `test.cmp`:

```
check.bat
```

> Se não tiver o utilitário `diff` instalado, edite `check.bat` para usar `fc` ou outro de sua preferência.

Para ver a execução dos testes:

```
test.bat
```

> Estes são alguns testes mínimos para garantir que os exemplos estão funcionando. O leitor pode adaptar os scripts de compilação e testes para serem usados com seus próprios programas e testes.

Para limpar as saídas, recompilar os programas, rodar os testes e conferir o resultado, execute:

```
all.bat
```
