#ifndef LEX_H
#define LEX_H

#include "ashley.h"

void lex_init(ash_state_t* a, const char* filename, const char* src);
int lex(ash_state_t* a);

#endif
