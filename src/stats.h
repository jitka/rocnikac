#ifndef STATS_H
#define STATS_H


#include <stdio.h>
#include "setting.h"

#ifdef STATS

#define SELECT_STATS_MAX 15
#define UPDATE_STATS_MAX 4651
#define SET_STATS_MAX 10

typedef struct {
	int created;
	int created_true;
	int created_false;
	int finished;
	int finished_true;
	int finished_false;
	int threat;
	int set_stats_max;
	int set_stats_more_then_max;
	int set[SET_STATS_MAX];
} stats_t;


extern stats_t all_stats;
extern stats_t turn_stats[N*N];
extern int update_stats_max;
extern int update_stats[UPDATE_STATS_MAX];
extern int select_stats_max;
extern int select_stats[SELECT_STATS_MAX];
extern int interations_stats;

void printStats(char * file_name);
//void printStats();

#endif //STATS

#endif //STATS_H
