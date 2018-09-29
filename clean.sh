#!/bin/bash

examplesdir='examples_wave'
if [ -d "$examplesdir" ]; then
    rm $examplesdir/*.wav
    rmdir "$examplesdir"
fi

if [ -f 'src/lex.yy.c' ]; then
    rm 'src/lex.yy.c'
fi

if [ -f 'src/y.tab.c' ]; then
    rm 'src/y.tab.c'
fi

if [ -f 'src/y.tab.h' ]; then
    rm 'src/y.tab.h'
fi

if [ -f 'mixdown' ]; then
    rm 'mixdown'
fi
