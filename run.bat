@echo off
cd /d "%~dp0\code"
echo Compilando biblioteca roaring...
gcc -c -O2 -o bibliotecas/roaring.o bibliotecas/roaring.c
if %ERRORLEVEL% neq 0 (
    echo Erro na compilação da biblioteca roaring!
    pause
    exit /b 1
)
echo Compilando projeto principal...
g++ -std=c++17 -O2 -pg -c -o main.o main.cpp
if %ERRORLEVEL% neq 0 (
    echo Erro na compilação do main.cpp!
    pause
    exit /b 1
)

echo Linkando projeto...
g++ -o main.exe main.o bibliotecas/roaring.o -lpsapi
if %ERRORLEVEL% equ 0 (
    echo Compilação bem-sucedida! Executando...
    main.exe
) else (
    echo Erro na compilação!
    pause
)