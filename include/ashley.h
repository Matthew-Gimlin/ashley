#ifndef ASHLEY_H
#define ASHLEY_H

#ifdef __cplusplus
extern "C" {
#endif

#define ASH_VERSION_MAJOR 0
#define ASH_VERSION_MINOR 1
#define ASH_VERSION_PATCH 0

typedef struct ash_state_s ash_state_t;

ash_state_t* ash_state_new(void);
void ash_state_free(ash_state_t* a);

#ifdef __cplusplus
}
#endif

#endif
