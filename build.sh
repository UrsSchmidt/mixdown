#!/bin/bash

./clean.sh

flex -o 'src/lex.yy.c' 'src/mixdown.l'
bison -d -o 'src/y.tab.c' 'src/mixdown.y'
gcc -std=c11 -Wall -Wpedantic -o 'mixdown' src/*.c -lm
