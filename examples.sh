#!/bin/bash

# not necessary, since build.sh calls this as well
#./clean.sh
./build.sh

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
