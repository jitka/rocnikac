#ifndef STATS_H
#define STATS_H


#include <stdio.h>
#include "setting.h"

#ifdef STATS

typedef struct {
	int created;
	int created_true;
	int created_false;
} stats_t;


extern stats_t all_stats;
extern stats_t turn_stats[N*N];

void printStats(char * file_name);
//void printStats();

#endif //STATS

#endif //STATS_H
