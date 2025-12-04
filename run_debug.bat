@echo off
cd /d "%~dp0\code"

echo ============================================
echo   Compilando em modo DEBUG
echo ============================================
echo.

echo Compilando projeto com símbolos de debug...
g++ -std=c++17 -g -O0 -DDEBUG -Wall -Wextra -o main_debug.exe main.cpp bibliotecas/roaring.c -lpsapi
if %ERRORLEVEL% equ 0 (
    echo [OK] Compilação bem-sucedida!
    echo.
    echo ============================================
    echo   Executando em modo DEBUG
    echo ============================================
    echo.
    main_debug.exe
    echo.
    echo ============================================
    echo   Programa finalizado
    echo ============================================
) else (
    echo [ERRO] Falha na compilação do projeto!
    echo.
    pause
)
