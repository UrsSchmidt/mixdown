#include "ast.h"

#include <stdlib.h>

struct ast_node *new_ast_unary_node(NODE_TYPE nt, struct ast_node *right) {
    struct unary_node *ast_node = malloc(sizeof(struct unary_node));
    ast_node->node_type = nt;
    ast_node->right = right;
    return (struct ast_node*) ast_node;
}

struct ast_node *new_ast_binary_node(NODE_TYPE nt, struct ast_node *left, struct ast_node *right) {
    struct binary_node *ast_node = malloc(sizeof(struct binary_node));
    ast_node->node_type = nt;
    ast_node->left = left;
    ast_node->right = right;
    return (struct ast_node*) ast_node;
}

struct ast_node *new_ast_function_call_node(char *identifier) {
    struct function_call_node *ast_node = malloc(sizeof(struct function_call_node));
    ast_node->node_type = NT_FUNCTION_CALL;
    ast_node->identifier = identifier;
    return (struct ast_node*) ast_node;
}

struct ast_node *new_ast_function_call_argument_list_node(char *identifier, struct ast_node *argument_list) {
    struct function_call_argument_list_node *ast_node = malloc(sizeof(struct function_call_argument_list_node));
    ast_node->node_type = NT_FUNCTION_CALL_ARGUMENT_LIST;
    ast_node->identifier = identifier;
    ast_node->argument_list = argument_list;
    return (struct ast_node*) ast_node;
}

struct ast_node *new_ast_argument_list_node(struct ast_node *argument, struct ast_node *argument_list) {
    struct argument_list_node *ast_node = malloc(sizeof(struct argument_list_node));
    ast_node->node_type = NT_ARGUMENT_LIST;
    ast_node->argument = argument;
    ast_node->argument_list = argument_list;
    return (struct ast_node*) ast_node;
}

struct ast_node *new_ast_identifier_node(char *identifier) {
    struct identifier_node *ast_node = malloc(sizeof(struct identifier_node));
    ast_node->node_type = NT_IDENTIFIER;
    ast_node->identifier = identifier;
    return (struct ast_node*) ast_node;
}

struct ast_node *new_ast_number_node(double number) {
    struct number_node *ast_node = malloc(sizeof(struct number_node));
    ast_node->node_type = NT_NUMBER;
    ast_node->number = number;
    return (struct ast_node*) ast_node;
}
