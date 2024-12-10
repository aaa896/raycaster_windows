@echo off

mkdir ..\build
pushd ..\build
cl /Zi /nologo /EHsc ..\code\main.cpp /link user32.lib kernel32.lib Gdi32.lib
popd

if %errorlevel% equ 0 (
    echo Compilation succeeded
    ) else (
    echo Compilation failed.
)
pause

