#include "api.h"
#include <stdlib.h>
#include <string.h>

ash_state_t* ash_state_new(void) {
    ash_state_t* a = malloc(sizeof(ash_state_t));
    memset(a, 0, sizeof(ash_state_t));
    return a;
}

void ash_state_free(ash_state_t* a) {
    free(a);
}
