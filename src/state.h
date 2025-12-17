#ifndef API_H
#define API_H

#include "ashley.h"
#include "common.h"
#include "token.h"
#include "ast.h"

struct ash_state_s {
    const char* filename;
    const char* src;

    // lexer state
    unsigned begin;
    unsigned end;
    token_t token;

    // parser state
    ast_t* ast;

    arena_t* arena;
    table_t symbols;
    table_t types;
};

#endif
