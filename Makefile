MAKEFLAGS+=--no-builtin-rules

DST=mixdown

# build target

SRC=src

CC=gcc
CFLAGS=-std=c18 -Wall -Wpedantic
LDLIBS=-lm

YACC=bison
YFLAGS=-d

LEX=flex
LFLAGS=

PARSERY=$(SRC)/mixdown.y
PARSERC=$(SRC)/y.tab.c
PARSERH=$(SRC)/y.tab.h

LEXERL=$(SRC)/mixdown.l
LEXERC=$(SRC)/lex.yy.c

CFILES=$(filter-out $(PARSERC) $(LEXERC), $(wildcard $(SRC)/*.c))
HFILES=$(filter-out $(PARSERH), $(wildcard $(SRC)/*.h))

$(DST): $(CFILES) $(HFILES) $(PARSERC) $(PARSERH) $(LEXERC)
	$(CC) $(CFLAGS) -o $(DST) $(CFILES) $(PARSERC) $(LEXERC) $(LDLIBS)

$(PARSERC) $(PARSERH): $(PARSERY)
	$(YACC) $(YFLAGS) -o $(PARSERC) $(PARSERY)

$(LEXERC): $(LEXERL)
	$(LEX) $(LFLAGS) -o $(LEXERC) $(LEXERL)

# install/uninstall targets

FILE_BIN=$(DST)
FILE_MAN=mixdown.1
FILE_LANG=mixdown.lang
INSTALL_DIR_BIN=/usr/local/bin/
INSTALL_DIR_MAN=/usr/local/share/man/man1/
INSTALL_DIR_LANG=/usr/share/gtksourceview-4/language-specs/
INSTALL_PATH_BIN=$(INSTALL_DIR_BIN)$(FILE_BIN)
INSTALL_PATH_MAN=$(INSTALL_DIR_MAN)$(FILE_MAN)
INSTALL_PATH_LANG=$(INSTALL_DIR_LANG)$(FILE_LANG)

.PHONY: install
install: $(FILE_BIN) $(FILE_MAN) $(FILE_LANG)
ifneq ($(wildcard $(INSTALL_DIR_BIN).),)
	cp $(FILE_BIN) $(INSTALL_PATH_BIN)
else
	@echo directory not found $(INSTALL_DIR_BIN)
endif
ifneq ($(wildcard $(INSTALL_DIR_MAN).),)
	cp $(FILE_MAN) $(INSTALL_PATH_MAN)
	mandb -q
else
	@echo directory not found $(INSTALL_DIR_MAN)
endif
ifneq ($(wildcard $(INSTALL_DIR_LANG).),)
	cp $(FILE_LANG) $(INSTALL_PATH_LANG)
else
	@echo directory not found $(INSTALL_DIR_LANG)
endif

.PHONY: uninstall
uninstall:
ifneq ($(wildcard $(INSTALL_PATH_BIN)),)
	rm -f $(INSTALL_PATH_BIN)
endif
ifneq ($(wildcard $(INSTALL_PATH_MAN)),)
	rm -f $(INSTALL_PATH_MAN)
	mandb -q
endif
ifneq ($(wildcard $(INSTALL_PATH_LANG)),)
	rm -f $(INSTALL_PATH_LANG)
endif

# examples target

EXAMPLES_SRC=examples
EXAMPLES_DST=examples_wave
MIXDOWN=./$(DST)
MIXDOWNFLAGS=-l 2 -w
MIXDOWNFILES=$(wildcard $(EXAMPLES_SRC)/*.mixdown)
WAVFILES=$(addprefix $(EXAMPLES_DST)/, $(notdir $(patsubst $(EXAMPLES_SRC)/%.mixdown, %.wav, $(MIXDOWNFILES))))

.PHONY: examples
examples: $(WAVFILES)
	@echo done

$(WAVFILES): $(EXAMPLES_DST)/%.wav: $(EXAMPLES_SRC)/%.mixdown $(DST) $(EXAMPLES_DST)
	$(MIXDOWN) $(MIXDOWNFLAGS) $< $@

$(EXAMPLES_DST):
	mkdir -p $(EXAMPLES_DST)

# clean target

.PHONY: clean
clean:
	rm -f $(DST) $(PARSERC) $(PARSERH) $(LEXERC)
	rm -rf $(EXAMPLES_DST)
