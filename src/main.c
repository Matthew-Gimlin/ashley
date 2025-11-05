#include "ashley.h"

int main() {
    ash_state_t* a = ash_state_new();
    ash_state_free(a);
    return 0;
}
