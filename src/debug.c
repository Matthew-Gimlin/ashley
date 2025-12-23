#include "debug.h"
#include "state.h"
#include <stdio.h>

void dump_ast(ash_state_t* a, ast_t* ast) {
    if (!ast) {
        printf("<null>");
        return;
    }
    expression_t* e = (expression_t*)ast;
    switch (ast->node) {
        case AST_INT:
            printf("%ld", e->as._int);
            break;

        case AST_FLOAT:
            printf("%lf", e->as._float);
            break;

        case AST_IDENTIFIER:
            printf("%.*s", (int)(ast->end - ast->begin), &a->src[ast->begin]);
            break;

        case AST_CAST:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" as ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_NEGATE:
            printf("(- ");
            dump_ast(a, &UNARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_ADD:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" + ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_SUBTRACT:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" - ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_MULTIPLY:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" * ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_DIVIDE:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" / ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_MODULO:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" %% ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_SHIFT_LEFT:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" << ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_SHIFT_RIGHT:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" >> ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_LESS:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" < ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_LESS_EQUAL:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" <= ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_GREATER:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" > ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_GREATER_EQUAL:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" >= ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_EQUAL:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" == ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_NOT_EQUAL:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" != ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_BITWISE_AND:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" & ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_BITWISE_OR:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" | ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_BITWISE_XOR:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" ^ ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_AND:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" and ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_OR:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" or ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_XOR:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" xor ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;

        case AST_ASSIGN:
            printf("(");
            dump_ast(a, &BINARY_LEFT(e)->ast);
            printf(" = ");
            dump_ast(a, &BINARY_RIGHT(e)->ast);
            printf(")");
            break;
            break;

        default:
            printf("<ast %d>", ast->node);
            break;
    }
}
