#include "parse.h"
#include "common.h"
#include "state.h"
#include "token.h"
#include "lex.h"
#include "type.h"
#include <stdlib.h>
#include <stdbool.h>

typedef expression_t* (*parse_expr_fn)(ash_state_t*);

static expression_t* parse_expression(ash_state_t* a);

static expression_t* new_atom(ash_state_t* a, ast_node_t node) {
    expression_t* atom = arena_alloc(a->arena, sizeof(expression_t));
    atom->ast.node = node;
    atom->ast.type = NULL;
    atom->ast.begin = a->begin;
    atom->ast.end = a->end;
    if (lex(a) != 0) {
        return NULL;
    }
    return atom;
}

static expression_t* new_unary(ash_state_t* a, ast_node_t node,
        parse_expr_fn parse_right) {
    expression_t* unary = arena_alloc(a->arena, sizeof(expression_t));
    unary->ast.node = node;
    unary->ast.type = NULL;
    unary->ast.begin = a->begin;
    if (lex(a) != 0) {
        return NULL;
    }
    if ((unary->as.unary.right = parse_right(a)) == NULL) {
        return NULL;
    }
    unary->ast.end = a->end;
    return unary;
}

static expression_t* new_binary(ash_state_t* a, ast_node_t node,
        expression_t* left, parse_expr_fn parse_right) {
    expression_t* binary = arena_alloc(a->arena, sizeof(expression_t));
    binary->ast.node = node;
    binary->ast.type = NULL;
    binary->ast.begin = left->ast.begin;
    binary->as.binary.left = left;
    if (lex(a) != 0) {
        return NULL;
    }
    if ((binary->as.binary.right = parse_right(a)) == NULL) {
        return NULL;
    }
    binary->ast.end = binary->as.binary.right->ast.end;
    return binary;
}

static expression_t* new_compound_assignment(ash_state_t* a,
        ast_node_t suboperator, expression_t* left, parse_expr_fn parse_right) {
    expression_t* assignment = arena_alloc(a->arena, sizeof(expression_t));
    assignment->ast.node = AST_ASSIGN;
    assignment->ast.type = NULL;
    assignment->ast.begin = left->ast.begin;
    assignment->as.binary.left = left;
    if ((assignment->as.binary.right =
                new_binary(a, suboperator, left, parse_right)) == NULL) {
        return NULL;
    }
    assignment->ast.end = assignment->as.binary.right->ast.end;
    return assignment;
}

static bool accept(ash_state_t* a, token_t token) {
    if (a->token == token) {
        lex(a);
        return true;
    }
    return false;
}

static expression_t* parse_atom(ash_state_t* a) {
    if (a->token == TOKEN_INT) {
        expression_t* atom = new_atom(a, AST_INT);
        atom->as._int = strtol(&a->src[atom->ast.begin], NULL, 10);
        atom->ast.type = table_get(&a->types, "int", 3);
        return atom;
    } else if (a->token == TOKEN_FLOAT) {
        expression_t* atom = new_atom(a, AST_FLOAT);
        atom->as._float = strtod(&a->src[atom->ast.begin], NULL);
        atom->ast.type = table_get(&a->types, "float", 5);
        return atom;
    } else if (a->token == TOKEN_IDENTIFIER) {
        expression_t* atom = new_atom(a, AST_IDENTIFIER);
        return atom;
    } else if (accept(a, TOKEN_LEFT_PARENTHESIS)) {
        expression_t* e = parse_expression(a);
        if (!accept(a, TOKEN_RIGHT_PARENTHESIS)) {
            return NULL;
        }
        return e;
    }
    return NULL;
}

static expression_t* parse_unary(ash_state_t* a) {
    if (a->token == TOKEN_PLUS) {
        lex(a);
        return parse_unary(a);
    } else if (a->token == TOKEN_MINUS) {
        return new_unary(a, AST_NEGATE, parse_unary);
    }
    return parse_atom(a);
}

static expression_t* parse_cast(ash_state_t* a) {
    expression_t* e = parse_unary(a);
    while (e) {
        if (a->token == TOKEN_AS) {
            e = new_binary(a, AST_CAST, e, parse_unary);
        } else {
            break;
        }
    }
    return e;
}

static expression_t* parse_multiplicative(ash_state_t* a) {
    expression_t* e = parse_cast(a);
    while (e) {
        if (a->token == TOKEN_STAR) {
            e = new_binary(a, AST_MULTIPLY, e , parse_cast);
        } else if (a->token == TOKEN_SLASH) {
            e = new_binary(a, AST_DIVIDE, e, parse_cast);
        } else if (a->token == TOKEN_PERCENT) {
            e = new_binary(a, AST_MODULO, e, parse_cast);
        } else {
            break;
        }
    }
    return e;
}

static expression_t* parse_additive(ash_state_t* a) {
    expression_t* e = parse_multiplicative(a);
    while (e) {
        if (a->token == TOKEN_PLUS) {
            e = new_binary(a, AST_ADD, e, parse_multiplicative);
        } else if (a->token == TOKEN_MINUS) {
            e = new_binary(a, AST_SUBTRACT, e, parse_multiplicative);
        } else {
            break;
        }
    }
    return e;
}

static expression_t* parse_shift(ash_state_t* a) {
    expression_t* e = parse_additive(a);
    while (e) {
        if (a->token == TOKEN_LESS_LESS) {
            e = new_binary(a, AST_SHIFT_LEFT, e, parse_additive);
        } else if (a->token == TOKEN_GREATER_GREATER) {
            e = new_binary(a, AST_SHIFT_RIGHT, e, parse_additive);
        } else {
            break;
        }
    }
    return e;
}

static expression_t* parse_comparison(ash_state_t* a) {
    expression_t* e = parse_shift(a);
    while (e) {
        if (a->token == TOKEN_LESS) {
            e = new_binary(a, AST_LESS, e, parse_shift);
        } else if (a->token == TOKEN_LESS_EQUAL) {
            e = new_binary(a, AST_LESS_EQUAL, e, parse_shift);
        } else if (a->token == TOKEN_GREATER) {
            e = new_binary(a, AST_GREATER, e, parse_shift);
        } else if (a->token == TOKEN_GREATER_EQUAL) {
            e = new_binary(a, AST_GREATER_EQUAL, e, parse_shift);
        } else {
            break;
        }
    }
    return e;
}

static expression_t* parse_equality(ash_state_t* a) {
    expression_t* e = parse_comparison(a);
    while (e) {
        if (a->token == TOKEN_EQUAL_EQUAL) {
            e = new_binary(a, AST_EQUAL, e, parse_comparison);
        } else if (a->token == TOKEN_BANG_EQUAL) {
            e = new_binary(a, AST_NOT_EQUAL, e, parse_comparison);
        } else {
            break;
        }
    }
    return e;
}

static expression_t* parse_bitwise_and(ash_state_t* a) {
    expression_t* e = parse_equality(a);
    while (e && a->token == TOKEN_AMPERSAND) {
        e = new_binary(a, AST_BITWISE_AND, e, parse_equality);
    }
    return e;
}

static expression_t* parse_bitwise_or(ash_state_t* a) {
    expression_t* e = parse_bitwise_and(a);
    while (e && a->token == TOKEN_BAR) {
        e = new_binary(a, AST_BITWISE_OR, e, parse_bitwise_and);
    }
    return e;
}

static expression_t* parse_bitwise_xor(ash_state_t* a) {
    expression_t* e = parse_bitwise_or(a);
    while (e && a->token == TOKEN_CARROT) {
        e = new_binary(a, AST_BITWISE_XOR, e, parse_bitwise_or);
    }
    return e;
}

static expression_t* parse_and(ash_state_t* a) {
    expression_t* e = parse_bitwise_xor(a);
    while (a->token == TOKEN_AND) {
        e = new_binary(a, AST_AND, e, parse_bitwise_xor);
    }
    return e;
}

static expression_t* parse_or(ash_state_t* a) {
    expression_t* e = parse_and(a);
    while (a->token == TOKEN_OR) {
        e = new_binary(a, AST_OR, e, parse_and);
    }
    return e;
}

static expression_t* parse_xor(ash_state_t* a) {
    expression_t* e = parse_or(a);
    while (a->token == TOKEN_XOR) {
        e = new_binary(a, AST_XOR, e, parse_or);
    }
    return e;
}

static expression_t* parse_assignment(ash_state_t* a) {
    expression_t* e = parse_xor(a);
    while (e) {
        if (a->token == TOKEN_EQUAL) {
            e = new_binary(a, AST_ASSIGN, e, parse_expression);
        } else if (a->token == TOKEN_PLUS_EQUAL) {
            e = new_compound_assignment(a, AST_ADD, e, parse_expression);
        } else if (a->token == TOKEN_MINUS_EQUAL) {
            e = new_compound_assignment(a, AST_SUBTRACT, e, parse_expression);
        } else if (a->token == TOKEN_STAR_EQUAL) {
            e = new_compound_assignment(a, AST_MULTIPLY, e, parse_expression);
        } else if (a->token == TOKEN_SLASH_EQUAL) {
            e = new_compound_assignment(a, AST_DIVIDE, e, parse_expression);
        } else if (a->token == TOKEN_PERCENT_EQUAL) {
            e = new_compound_assignment(a, AST_MODULO, e, parse_expression);
        } else if (a->token == TOKEN_AMPERSAND_EQUAL) {
            e = new_compound_assignment(a, AST_BITWISE_AND, e, parse_expression);
        } else if (a->token == TOKEN_CARROT_EQUAL) {
            e = new_compound_assignment(a, AST_BITWISE_XOR, e, parse_expression);
        } else if (a->token == TOKEN_BAR_EQUAL) {
            e = new_compound_assignment(a, AST_BITWISE_OR, e, parse_expression);
        } else if (a->token == TOKEN_LESS_LESS_EQUAL) {
            e = new_compound_assignment(a, AST_SHIFT_LEFT, e, parse_expression);
        } else if (a->token == TOKEN_GREATER_GREATER_EQUAL) {
            e = new_compound_assignment(a, AST_SHIFT_RIGHT, e, parse_expression);
        } else {
            break;
        }
    }
    return e;
}

static expression_t* parse_expression(ash_state_t* a) {
    return parse_assignment(a);
}

int parse(ash_state_t* a) {
    if (!a) {
        return 1;
    }
    a->ast = (ast_t*)parse_expression(a);
    return 0;
}
