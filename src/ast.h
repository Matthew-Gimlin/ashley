#ifndef AST_H
#define AST_H

#include "ashley.h"
#include "type.h"

typedef enum {
    AST_ERROR = 0,
    AST_BOOL,
    AST_BYTE,
    AST_INT,
    AST_FLOAT,
    AST_IDENTIFIER,
    AST_CONVERT,
    AST_CAST,
    AST_NEGATE,
    AST_ADD,
    AST_SUBTRACT,
    AST_MULTIPLY,
    AST_DIVIDE,
    AST_MODULO,
    AST_SHIFT_LEFT,
    AST_SHIFT_RIGHT,
    AST_LESS,
    AST_LESS_EQUAL,
    AST_GREATER,
    AST_GREATER_EQUAL,
    AST_EQUAL,
    AST_NOT_EQUAL,
    AST_BITWISE_AND,
    AST_BITWISE_OR,
    AST_BITWISE_XOR,
    AST_AND,
    AST_OR,
    AST_XOR,
    AST_ASSIGN,
    AST_EXPRESSION,
    AST_BLOCK,
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
        ash_bool_t _bool;
        ash_byte_t byte;
        ash_int_t _int;
        ash_float_t _float;
        struct { expression_t* right; } unary;
        struct { expression_t* left; expression_t* right; } binary;
    } as;
};

struct statement_s {
    ast_t ast;
    union {
        expression_t* expression;
    } as;
    statement_t* next;
};

#endif
