@echo off
cbundl .\src\main.c .\bundle.c
gcc -std=c2x -Wall -Wno-unknown-pragmas -O3 .\bundle.c -o craytracer.exe -lSDL2 -lSDL2main -lm
