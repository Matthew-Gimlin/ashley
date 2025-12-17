#ifndef ASH_COMMON_H
#define ASH_COMMON_H

#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>

#define ASH_UNUSED(x) (void)(x)

#ifdef __GNUC__
#    define ASH_FALLTHROUGH() __attribute((fallthrough))
#    define ASH_UNREACHABLE() __builtin_unreachable()
#else
#    define ASH_FALLTHROUGH() do {} while (0)
#    define ASH_UNREACHABLE() do {} while (0)
#endif

#define ARENA_SIZE 1024

typedef struct arena_s {
    alignas(max_align_t) char data[ARENA_SIZE];
    size_t size;
    struct arena_s* next;
} arena_t;

arena_t* arena_new(void);
void arena_free(arena_t* arena);
void* arena_alloc(arena_t* arena, size_t n);

#define FNV_1A_OFFSET 2166136261u
#define FNV_1A_PRIME 16777619u

uint32_t hash(const char* key, size_t key_size);

#define TABLE_INIT_CAPACITY 128
#define TABLE_MAX_LOAD 0.75
#define TABLE_TOMBSTONE (void*)1

typedef struct {
    const char* key;
    size_t key_size;
    size_t hash;
    void* value;
} entry_t;

typedef struct {
    entry_t* entries;
    size_t size;
    size_t capacity;
} table_t;

void table_init(table_t* table);
void table_free(table_t* table);
void table_set(table_t* table, const char* key, size_t key_size, void* value);
void* table_get(table_t* table, const char* key, size_t key_size);

#endif
