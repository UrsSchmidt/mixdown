MAKEFLAGS+=--no-builtin-rules

TITLE=mixdown
TARGET_EXEC=$(TITLE)
EXTENSION=.$(TITLE)

GEN=gen
OBJ=obj

# build target

SRC=src

CC=gcc
CFLAGS=-std=c18 -Wall -Wpedantic
LDLIBS=-lm

YACC=bison
YFLAGS=-d

LEX=flex
LFLAGS=

PARSERY=$(SRC)/$(TITLE).y
PARSERC=$(GEN)/y.tab.c
PARSERH=$(GEN)/y.tab.h

LEXERL=$(SRC)/$(TITLE).l
LEXERC=$(GEN)/lex.yy.c

SRCCFILES=$(wildcard $(SRC)/*.c)
SRCOFILES=$(SRCCFILES:$(SRC)/%.c=$(OBJ)/%.o)
GENCFILES=$(PARSERC) $(LEXERC)
GENOFILES=$(GENCFILES:$(GEN)/%.c=$(OBJ)/%.o)

$(TARGET_EXEC): $(SRCOFILES) $(GENOFILES)
	$(CC) -o $@ $^ $(LDLIBS)

$(GENOFILES): $(OBJ)/%.o: $(GEN)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCOFILES): $(OBJ)/%.o: $(SRC)/%.c $(GENCFILES)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(PARSERC): $(PARSERY)
	@mkdir -p $(dir $@)
	$(YACC) $(YFLAGS) -o $@ $^

$(LEXERC): $(LEXERL)
	@mkdir -p $(dir $@)
	$(LEX) $(LFLAGS) -o $@ $^

# install/uninstall targets

FILE_BIN=$(TARGET_EXEC)
FILE_MAN=$(TITLE).1
FILE_LANG=$(TITLE).lang
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
MIXDOWN=./$(TARGET_EXEC)
MIXDOWNFLAGS=-l 2 -w
MIXDOWNFILES=$(wildcard $(EXAMPLES_SRC)/*$(EXTENSION))
WAVFILES=$(MIXDOWNFILES:$(EXAMPLES_SRC)/%$(EXTENSION)=$(EXAMPLES_DST)/%.wav)

.PHONY: examples
examples: $(WAVFILES)

$(WAVFILES): $(EXAMPLES_DST)/%.wav: $(EXAMPLES_SRC)/%$(EXTENSION) $(TARGET_EXEC)
	@mkdir -p $(dir $@)
	$(MIXDOWN) $(MIXDOWNFLAGS) $< $@

# clean target

.PHONY: clean
clean:
	rm -f $(TARGET_EXEC) $(PARSERC) $(PARSERH) $(LEXERC)
	rm -rf $(EXAMPLES_DST)
	rm -rf $(OBJ)
	rm -rf $(GEN)
