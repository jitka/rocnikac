#ifndef CACHE_H_
#define CACHE_H_

#include "config.h"

extern uint hash_numbers[2][(N*(N+1))/2];

int get_from_cache(luint m[4], int hash);
void put_into_cache(luint m[4], int hash, int winner);

#endif
