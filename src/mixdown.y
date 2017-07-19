%{
#include <stddef.h>
#include "ast.h"
#include "logger.h"

int yylex();
int yyparse();

void yyerror(struct ast_node** root, const char* str) {
    error(str, NULL);
}

int yywrap() {
    return 1;
}
%}

%parse-param { struct ast_node** root }

%start mixdown

%union {
    struct ast_node* ast;
    char* identifier;
    double number;
}

%token BROPEN BRCLOSE COMMA SEMICOLON
%token <identifier> IDENTIFIER
%token <number> NUMBER

%left ADD SUB
%left MUL DIV MOD
%right POW

%type <ast> mixdown expression argument_list argument

%%
mixdown
    : expression SEMICOLON { *root = $1; }
    ;

expression
    : expression ADD expression { $$ = new_ast_binary_node(NT_ADD, $1, $3); }
    | expression SUB expression { $$ = new_ast_binary_node(NT_SUB, $1, $3); }
    | expression MUL expression { $$ = new_ast_binary_node(NT_MUL, $1, $3); }
    | expression DIV expression { $$ = new_ast_binary_node(NT_DIV, $1, $3); }
    | expression MOD expression { $$ = new_ast_binary_node(NT_MOD, $1, $3); }
    | SUB expression %prec MUL { $$ = new_ast_unary_node(NT_NEG, $2); }
    | expression POW expression { $$ = new_ast_binary_node(NT_POW, $1, $3); }
    | BROPEN expression BRCLOSE { $$ = $2; }
    | NUMBER { $$ = new_ast_number_node($1); }
    | IDENTIFIER { $$ = new_ast_identifier_node($1); }
    | IDENTIFIER BROPEN BRCLOSE { $$ = new_ast_function_call_node($1); }
    | IDENTIFIER BROPEN argument_list BRCLOSE { $$ = new_ast_function_call_argument_list_node($1, $3); }
    ;

argument_list
    : argument
    | argument COMMA argument_list { $$ = new_ast_argument_list_node($1, $3); }
    ;

argument
    : expression
    ;
