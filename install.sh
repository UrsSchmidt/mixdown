#!/bin/bash

if [ ! -f 'mixdown' ]; then
    ./build.sh
fi

if [ -f 'mixdown' ]; then
    if [ -d '/usr/local/bin/' ]; then
        sudo cp 'mixdown' '/usr/local/bin/'
    fi
fi

if [ -d '/usr/share/gtksourceview-3.0/language-specs/' ]; then
    sudo cp 'mixdown.lang' '/usr/share/gtksourceview-3.0/language-specs/'
fi

if [ -d '/usr/local/share/man/man1/' ]; then
    sudo cp 'mixdown.1' '/usr/local/share/man/man1/'
    sudo mandb -q
fi
