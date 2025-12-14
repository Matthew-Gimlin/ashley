#ifndef AST_H
#define AST_H

#include "ashley.h"
#include "type.h"

typedef enum {
    AST_ERROR = 0,
    AST_INT,
    AST_FLOAT,
    AST_CAST,
    AST_NEGATE,
    AST_ADD,
    AST_SUBTRACT,
    AST_MULTIPLY,
    AST_DIVIDE,
    AST_MODULO,
    AST_BITWISE_AND,
    AST_BITWISE_XOR,
    AST_BITWISE_OR,
} ast_node_t;

typedef struct {
    ast_node_t node;
    type_t* type;
    unsigned begin;
    unsigned end;
} ast_t;

typedef struct expression_s expression_t;
typedef struct statement_s statement_t;

#define UNARY_RIGHT(e) ((e)->as.unary.right)
#define BINARY_LEFT(e) ((e)->as.binary.left)
#define BINARY_RIGHT(e) ((e)->as.binary.right)

struct expression_s {
    ast_t ast;
    union {
        ash_int_t _int;
        ash_float_t _float;
        struct { expression_t* right; } unary;
        struct { expression_t* left; expression_t* right; } binary;
    } as;
};

struct statement_s {
    ast_t ast;
    union {
        expression_t* expr;
    } as;
    statement_t* next;
};

#endif
