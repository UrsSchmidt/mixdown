%{
#include "y.tab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* suppress compiler warnings */
int fileno(FILE *stream);

/* see: https://stackoverflow.com/a/9427286 */
/* suppress compiler warnings */
char *strdup(const char *s);
%}

/* suppress compiler warnings */
%option noinput
%option nounput

%%
[\t\n\r ]+                  /* skip */
"%"                         return MOD;
"("                         return BROPEN;
")"                         return BRCLOSE;
"*"                         return MUL;
"+"                         return ADD;
","                         return COMMA;
"-"                         return SUB;
"/*"([^*]|\*+[^*/])*\*+"/"  /* skip */
"/"                         return DIV;
[0-9]+("."[0-9]+)?          yylval.number = atof(yytext); return NUMBER;
";"                         return SEMICOLON;
[A-Z_a-z][#0-9A-Z_a-z]*     yylval.identifier = strdup(yytext); return IDENTIFIER;
"^"                         return POW;
%%
