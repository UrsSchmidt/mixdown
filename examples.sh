#!/bin/bash

if [ -d 'examples_wave' ]; then
    rm examples_wave/*.wav
    rmdir 'examples_wave'
fi

if [ ! -f 'mixdown' ]; then
    ./build.sh
fi

if [ -f 'mixdown' ]; then
    examplesdir='examples_wave'
    if [ -d "$examplesdir" ]; then
        rm $examplesdir/*.wav
    else
        mkdir "$examplesdir"
    fi
    for path in examples/*.mixdown; do
        filename=$(basename "$path" '.mixdown')
        ./mixdown -l '2' -w "$path" "$examplesdir/$filename.wav"
    done
fi
