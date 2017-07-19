#!/bin/bash

if [ -f '/usr/local/bin/mixdown' ]; then
    sudo rm '/usr/local/bin/mixdown'
fi

if [ -f '/usr/share/gtksourceview-3.0/language-specs/mixdown.lang' ]; then
    sudo rm '/usr/share/gtksourceview-3.0/language-specs/mixdown.lang'
fi

if [ -f '/usr/local/share/man/man1/mixdown.1' ]; then
    sudo rm '/usr/local/share/man/man1/mixdown.1'
    sudo mandb
fi
