#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "ashley.h"
#include "ast.h"

#define DEBUG_PRINTF(s, ...) printf("DEBUG %s: %s:%d: " s "\n", __func__, \
        __FILE__, __LINE__, ##__VA_ARGS__)

void dump_ast(ash_state_t* a, ast_t* ast);

#endif
