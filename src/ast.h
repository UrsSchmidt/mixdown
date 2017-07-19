#ifndef AST_HEADER_FILE
#define AST_HEADER_FILE

/* see: https://efxa.org/2014/05/25/how-to-create-an-abstract-syntax-tree-while-parsing-an-input-stream/ */

typedef enum {
    NT_NEG,
    NT_ADD,
    NT_SUB,
    NT_MUL,
    NT_DIV,
    NT_MOD,
    NT_POW,
    NT_FUNCTION_CALL,
    NT_FUNCTION_CALL_ARGUMENT_LIST,
    NT_ARGUMENT_LIST,
    NT_IDENTIFIER,
    NT_NUMBER
} NODE_TYPE;

struct ast_node {
    NODE_TYPE node_type;
};

struct unary_node {
    NODE_TYPE node_type;
    struct ast_node* right;
};

struct binary_node {
    NODE_TYPE node_type;
    struct ast_node* left;
    struct ast_node* right;
};

struct function_call_node {
    NODE_TYPE node_type;
    char* identifier;
};

struct function_call_argument_list_node {
    NODE_TYPE node_type;
    char* identifier;
    struct ast_node* argument_list;
};

struct argument_list_node {
    NODE_TYPE node_type;
    struct ast_node* argument;
    struct ast_node* argument_list;
};

struct identifier_node {
    NODE_TYPE node_type;
    char* identifier;
};

struct number_node {
    NODE_TYPE node_type;
    double number;
};

struct ast_node* new_ast_unary_node(NODE_TYPE nt, struct ast_node* right);

struct ast_node* new_ast_binary_node(NODE_TYPE nt, struct ast_node* left, struct ast_node* right);

struct ast_node* new_ast_function_call_node(char* identifier);

struct ast_node* new_ast_function_call_argument_list_node(char* identifier, struct ast_node* argument_list);

struct ast_node* new_ast_argument_list_node(struct ast_node* argument, struct ast_node* argument_list);

struct ast_node* new_ast_identifier_node(char* identifier);

struct ast_node* new_ast_number_node(double number);

#endif
