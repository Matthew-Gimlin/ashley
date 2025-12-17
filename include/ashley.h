#ifndef ASHLEY_H
#define ASHLEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ASH_VERSION_MAJOR 0
#define ASH_VERSION_MINOR 1
#define ASH_VERSION_PATCH 0

typedef int64_t ash_int_t;
typedef double ash_float_t;

typedef struct ash_state_s ash_state_t;

ash_state_t* ash_state_new(void);
void ash_state_free(ash_state_t* a);

int ash_do_file(ash_state_t* a, const char* filename);
int ash_do_string(ash_state_t* a, const char* string);

#ifdef __cplusplus
}
#endif

#endif
