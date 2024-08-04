# Installation

## Needed Libraries

* `argp.h`
* `bison`
* `flex`
* `gcc`
* `math.h`

## Building

Run `make` or `make mixdown`.

## Installation

On Ubuntu you can run `sudo make install`. Otherwise just copy the generated executable, the man page and the gedit language file manually to the appropriate directories.

## Other make targets

On Ubuntu you can run `sudo make uninstall` to uninstall mixdown.

`make examples` will generate all sounds in the examples directory.

`make clean` will clean the working directory.
