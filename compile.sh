#!/bin/sh
gcc -std=c2x -Wall -O3 fail.c util.c vec3.c sphere.c main.c -o craytracer -lSDL2 -lSDL2main
