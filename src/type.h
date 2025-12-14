#ifndef TYPE_H
#define TYPE_H

typedef enum {
    TYPE_ERROR = 0,
    TYPE_BOOL,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_FUNCTION,
} type_type_t;

typedef struct {
    type_type_t type;
    unsigned size;
    unsigned align;
} type_t;

#endif
