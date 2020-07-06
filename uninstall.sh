#!/bin/bash

if [ -f '/usr/local/bin/mixdown' ]; then
    rm '/usr/local/bin/mixdown'
fi

if [ -f '/usr/share/gtksourceview-4/language-specs/mixdown.lang' ]; then
    rm '/usr/share/gtksourceview-4/language-specs/mixdown.lang'
fi

if [ -f '/usr/local/share/man/man1/mixdown.1' ]; then
    rm '/usr/local/share/man/man1/mixdown.1'
    mandb -q
fi
