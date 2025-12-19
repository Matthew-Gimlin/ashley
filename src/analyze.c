#include "analyze.h"
#include "state.h"
#include "ast.h"
#include "common.h"
#include <stdbool.h>

#include "debug.h"
#include <stdio.h>

static inline bool is_type(ast_t* ast, type_type_t type) {
    return ast->type != NULL && ast->type->type == type;
}

static inline bool is_number_type(ast_t* ast) {
    return is_type(ast, TYPE_INT) || is_type(ast, TYPE_FLOAT);
}

static expression_t* convert(ash_state_t* a, expression_t* e, type_t* type) {
    if (!e || e->ast.type == type) {
        return e;
    }
    expression_t* c = arena_alloc(a->arena, sizeof(expression_t));
    if (!c) {
        return NULL;
    }
    c->ast.node = AST_CONVERT;
    c->ast.type = type;
    c->ast.begin = e->ast.begin;
    c->ast.end = e->ast.end;
    UNARY_RIGHT(c) = e;
    return c;
}

static int tag_ast(ash_state_t* a, ast_t* ast) {
    if (!ast) {
        return 1;
    }
    expression_t* e = (expression_t*)ast;
    type_t* float_type = table_get(&a->types, "float", 5);
    switch (ast->node) {
        case AST_ERROR:
            return 1;

        case AST_NEGATE:
            if (tag_ast(a, &UNARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            if (!is_number_type(&UNARY_RIGHT(e)->ast)) {
                return 1;
            }
            ast->type = UNARY_RIGHT(e)->ast.type;
            return 0;

        case AST_ADD:
        case AST_SUBTRACT:
        case AST_MULTIPLY:
        case AST_DIVIDE:
        case AST_LESS:
        case AST_LESS_EQUAL:
        case AST_GREATER:
        case AST_GREATER_EQUAL:
            if (tag_ast(a, &BINARY_LEFT(e)->ast) != 0) {
                return 1;
            }
            if (tag_ast(a, &BINARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            if (!is_number_type(&BINARY_LEFT(e)->ast) ||
                    !is_number_type(&BINARY_RIGHT(e)->ast)) {
                return 1;
            }
            if (BINARY_LEFT(e)->ast.type != BINARY_RIGHT(e)->ast.type) {
                BINARY_LEFT(e) = convert(a, BINARY_LEFT(e), float_type);
                BINARY_RIGHT(e) = convert(a, BINARY_RIGHT(e), float_type);
            }
            ast->type = BINARY_LEFT(e)->ast.type;
            return 0;

        case AST_MODULO:
        case AST_SHIFT_LEFT:
        case AST_SHIFT_RIGHT:
        case AST_BITWISE_AND:
        case AST_BITWISE_OR:
        case AST_BITWISE_XOR:
            if (tag_ast(a, &BINARY_LEFT(e)->ast) != 0) {
                return 1;
            }
            if (tag_ast(a, &BINARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            if (!is_type(&BINARY_LEFT(e)->ast, TYPE_INT) ||
                    !is_type(&BINARY_RIGHT(e)->ast, TYPE_INT)) {
                return 1;
            }
            ast->type = BINARY_LEFT(e)->ast.type;
            return 0;

        case AST_ASSIGN:
            if (tag_ast(a, &BINARY_LEFT(e)->ast) != 0) {
                return 1;
            }
            if (tag_ast(a, &BINARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            return 0;

        default:
            return 0;
    }
    UNREACHABLE();
}

static ash_int_t eval_binary_i(expression_t* binary) {
    ash_int_t l = BINARY_LEFT(binary)->as._int;
    ash_int_t r = BINARY_RIGHT(binary)->as._int;
    switch (binary->ast.node) {
        case AST_ADD:           return l + r;
        case AST_SUBTRACT:      return l - r;
        case AST_MULTIPLY:      return l * r;
        case AST_DIVIDE:        return l / r;
        case AST_MODULO:        return l % r;
        case AST_SHIFT_LEFT:    return l << r;
        case AST_SHIFT_RIGHT:   return l >> r;
        case AST_BITWISE_AND:   return l & r;
        case AST_BITWISE_OR:    return l | r;
        case AST_BITWISE_XOR:   return l ^ r;
        default:                return 0;
    }
    UNREACHABLE();
}

static ash_float_t eval_binary_f(expression_t* binary) {
    ash_float_t l = BINARY_LEFT(binary)->as._float;
    ash_float_t r = BINARY_RIGHT(binary)->as._float;
    switch (binary->ast.node) {
        case AST_ADD:           return l + r;
        case AST_SUBTRACT:      return l - r;
        case AST_MULTIPLY:      return l * r;
        case AST_DIVIDE:        return l / r;
        default:                return 0;
    }
    UNREACHABLE();
}

static int fold_ast(ash_state_t* a, ast_t* ast) {
    if (!ast) {
        return 1;
    }
    expression_t* e = (expression_t*)ast;
    switch (ast->node) {
        case AST_ERROR:
            return 1;

        case AST_CONVERT:
            if (fold_ast(a, &UNARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            if (ast->type == UNARY_RIGHT(e)->ast.type) {
                *e = *UNARY_RIGHT(e);
            } else if (UNARY_RIGHT(e)->ast.node == AST_FLOAT &&
                    is_type(ast, TYPE_INT)) {
                e->as._int = (ash_float_t)UNARY_RIGHT(e)->as._float;
                ast->node = AST_INT;
            } else if (UNARY_RIGHT(e)->ast.node == AST_INT &&
                    is_type(ast, TYPE_FLOAT)) {
                e->as._float = (ash_float_t)UNARY_RIGHT(e)->as._int;
                ast->node = AST_FLOAT;
            }
            return 0;

        case AST_NEGATE:
            if (fold_ast(a, &UNARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            if (UNARY_RIGHT(e)->ast.node == AST_INT) {
                e->as._int = -UNARY_RIGHT(e)->as._int;
                ast->node = AST_INT;
            } else if (UNARY_RIGHT(e)->ast.node == AST_FLOAT) {
                e->as._float = -UNARY_RIGHT(e)->as._float;
                ast->node = AST_FLOAT;
            }
            return 0;

        case AST_ADD:
        case AST_SUBTRACT:
        case AST_MULTIPLY:
        case AST_DIVIDE:
        case AST_LESS:
        case AST_LESS_EQUAL:
        case AST_GREATER:
        case AST_GREATER_EQUAL:
            if (fold_ast(a, &BINARY_LEFT(e)->ast) != 0) {
                return 1;
            }
            if (fold_ast(a, &BINARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            if (BINARY_LEFT(e)->ast.node == AST_INT) {
                e->as._int = eval_binary_i(e);
                ast->node = AST_INT;
            } else if (BINARY_RIGHT(e)->ast.node == AST_FLOAT) {
                e->as._float = eval_binary_f(e);
                ast->node = AST_FLOAT;
            }
            return 0;

        case AST_MODULO:
        case AST_SHIFT_LEFT:
        case AST_SHIFT_RIGHT:
        case AST_BITWISE_AND:
        case AST_BITWISE_OR:
        case AST_BITWISE_XOR:
            if (fold_ast(a, &BINARY_LEFT(e)->ast) != 0) {
                return 1;
            }
            if (fold_ast(a, &BINARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            if (BINARY_LEFT(e)->ast.node == AST_INT) {
                e->as._int = eval_binary_i(e);
                ast->node = AST_INT;
            }
            return 0;

        case AST_ASSIGN:
            if (fold_ast(a, &BINARY_LEFT(e)->ast) != 0) {
                return 1;
            }
            if (fold_ast(a, &BINARY_RIGHT(e)->ast) != 0) {
                return 1;
            }
            return 0;

        default:
            return 0;
    }
    UNREACHABLE();
}

int analyze(ash_state_t* a) {
    if (tag_ast(a, a->ast) != 0) {
        return 1;
    }
    if (fold_ast(a, a->ast) != 0) {
        return 1;
    }
    return 0;
}
