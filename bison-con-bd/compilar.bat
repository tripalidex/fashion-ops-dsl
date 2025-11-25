@echo off
REM Script de compilación para Windows

echo Compilando FashionOpsDSL con SQLite...

REM Generar parser con Bison
echo [1/4] Generando parser con Bison...
bison -d fashion-ops-bison.y
if errorlevel 1 (
    echo Error al generar parser
    exit /b 1
)

REM Generar lexer con Flex
echo [2/4] Generando lexer con Flex...
flex fashion-ops-dsl.l
if errorlevel 1 (
    echo Error al generar lexer
    exit /b 1
)

REM Compilar con GCC
echo [3/4] Compilando con GCC...
gcc -o fashion-ops.exe fashion-ops-bison.tab.c lex.yy.c -lsqlite3
if errorlevel 1 (
    echo Error en compilacion
    exit /b 1
)

echo [4/4] Compilacion exitosa!
echo.
echo Para ejecutar: fashion-ops.exe test.dsl
echo.
