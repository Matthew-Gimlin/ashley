#include "state.h"
#include "type.h"
#include "lex.h"
#include "parse.h"
#include "analyze.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void set_default_types(ash_state_t* a) {
    type_t* bool_type = arena_alloc(a->arena, sizeof(type_t));
    *bool_type = (type_t){TYPE_BOOL, sizeof(ash_bool_t), alignof(ash_bool_t)};
    table_set(&a->types, "bool", 4, bool_type);

    type_t* byte_type = arena_alloc(a->arena, sizeof(type_t));
    *byte_type = (type_t){TYPE_BYTE, sizeof(ash_byte_t), alignof(ash_byte_t)};
    table_set(&a->types, "byte", 4, byte_type);

    type_t* int_type = arena_alloc(a->arena, sizeof(type_t));
    *int_type = (type_t){TYPE_INT, sizeof(ash_int_t), alignof(ash_int_t)};
    table_set(&a->types, "int", 3, int_type);

    type_t* float_type = arena_alloc(a->arena, sizeof(type_t));
    *float_type = (type_t){TYPE_FLOAT, sizeof(ash_float_t), alignof(ash_float_t)};
    table_set(&a->types, "float", 5, float_type);
}

static void set_default_symbols(ash_state_t* a) {
    // TODO
    UNUSED(a);
}

ash_state_t* ash_state_new(void) {
    ash_state_t* a = malloc(sizeof(ash_state_t));
    if (!a) {
        return NULL;
    }
    memset(a, 0, sizeof(ash_state_t));
    a->arena = arena_new();
    table_init(&a->types);
    table_init(&a->symbols);
    set_default_types(a);
    set_default_symbols(a);
    return a;
}

void ash_state_free(ash_state_t* a) {
    free(a);
}

static int do_string(ash_state_t* a, const char* filename, const char* string) {
    lex_init(a, filename, string);
    if (lex(a) != 0) {
        return 1;
    }
    if (parse(a) != 0) {
        return 1;
    }
    if (analyze(a) != 0) {
        return 1;
    }
    dump_ast(a, a->ast);
    printf("\n");
    return 0;
}

int ash_do_file(ash_state_t* a, const char* filename) {
    if (!a || !filename) {
        return 1;
    }
    a->filename = filename;
    FILE* f = fopen(filename, "rb");
    if (!f) {
        return 1;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        return 1;
    }
    long n = ftell(f);
    if (n < 0) {
        return 1;
    }
    rewind(f);
    char* src = malloc(n + 1);
    if (src == NULL) {
        return 1;
    }
    if (fread(src, sizeof(*src), n, f) != (size_t)n) {
        fclose(f);
        free(src);
        return 1;
    }
    src[n] = '\0';
    fclose(f);
    int rv = do_string(a, filename, src);
    free(src);
    return rv;
}

int ash_do_string(ash_state_t* a, const char* string) {
    if (!a || !string) {
        return 1;
    }
    return do_string(a, "<stdin>", string);
}
