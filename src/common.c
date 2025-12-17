#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

arena_t* arena_new(void) {
    arena_t* arena = malloc(sizeof(arena_t));
    if (!arena) {
        return NULL;
    }
    memset(arena, 0, sizeof(arena_t));
    return arena;
}

void arena_free(arena_t* arena) {
    arena_t* next;
    while (arena) {
        next = arena->next;
        free(arena);
        arena = next;
    }
}

static inline size_t align(size_t n) {
    return (n + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1);
}

void* arena_alloc(arena_t* arena, size_t n) {
    assert(n < ARENA_SIZE);
    if (arena == NULL || n == 0) {
        return NULL;
    }
    size_t i = align(arena->size);
    while (i + n > ARENA_SIZE) {
        if (arena->next == NULL) {
            if ((arena->next = arena_new()) == NULL) {
                return NULL;
            }
        }
        arena = arena->next;
        i = align(arena->size);
    }
    void* p = &arena->data[i];
    arena->size = i + n;
    return p;
}

void table_init(table_t* table) {
    table->entries = calloc(TABLE_INIT_CAPACITY, sizeof(entry_t));
    table->size = 0;
    table->capacity = TABLE_INIT_CAPACITY;
}

void table_free(table_t* table) {
    free(table->entries);
    table->entries = NULL;
    table->size = 0;
    table->capacity = 0;
}

uint32_t hash(const char* key, size_t key_size) {
    uint32_t hash = FNV_1A_OFFSET;
    for (size_t i = 0; i < key_size; i++) {
        hash ^= (unsigned char)key[i];
        hash *= FNV_1A_PRIME;
    }
    return hash;
}

static entry_t* table_find(entry_t* entries, size_t capacity, const char* key,
        size_t key_size) {
    entry_t* tombstone = NULL;
    size_t i = hash(key, key_size) % capacity;
    while (true) {
        entry_t* entry = &entries[i];
        if (entry->key == NULL) {
            if (entry->value != TABLE_TOMBSTONE) {
                return tombstone != NULL ? tombstone : entry;
            } else if (tombstone == NULL) {
                tombstone = entry;
            }
        } else if (entry->key_size == key_size &&
                memcmp(entry->key, key, key_size) == 0) {
            return entry;
        }
        i = (i + 1) % capacity;
    }
    ASH_UNREACHABLE();
}

static void table_resize(table_t* table, size_t new_capacity) {
    entry_t* new_entries = malloc(new_capacity * sizeof(entry_t));
    table->size = 0;
    for (size_t i = 0; i < table->capacity; i++) {
        entry_t* old_entry = &table->entries[i];
        if (old_entry->key == NULL) {
            continue;
        }
        entry_t* new_entry = table_find(new_entry, new_capacity, old_entry->key,
                old_entry->key_size);
        *new_entry = *old_entry;
        table->size++;
    }
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
}

void table_set(table_t* table, const char* key, size_t key_size, void* value) {
    if (table->size + 1 > table->capacity * TABLE_MAX_LOAD) {
        table_resize(table, table->capacity * 2);
    }
    entry_t* entry = table_find(table->entries, table->capacity, key, key_size);
    if (entry->key == NULL && entry->value != TABLE_TOMBSTONE) {
        table->size++;
    }
    entry->key = key;
    entry->key_size = key_size;
    entry->value = value;
}

void* table_get(table_t* table, const char* key, size_t key_size) {
    if (table->size == 0) {
        return NULL;
    }
    entry_t* entry = table_find(table->entries, table->capacity, key, key_size);
    if (entry->key == NULL) {
        return NULL;
    }
    return entry->value;
}
