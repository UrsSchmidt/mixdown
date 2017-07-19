%{
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

int fileno(FILE *stream);

char *strdup(const char *s);
%}

%option noinput
%option nounput

%%
"("                         return BROPEN;
")"                         return BRCLOSE;
","                         return COMMA;
";"                         return SEMICOLON;
"+"                         return ADD;
"-"                         return SUB;
"*"                         return MUL;
"/"                         return DIV;
"%"                         return MOD;
"^"                         return POW;
[A-Z_a-z][#0-9A-Z_a-z]*     yylval.identifier=strdup(yytext); return IDENTIFIER;
[0-9]+("."[0-9]+)?          yylval.number=atof(yytext); return NUMBER;
"/*"([^*]|\*+[^*/])*\*+"/"  /* skip */
[\t\n\r ]+                  /* skip */
%%
