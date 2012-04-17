#ifndef STATS_H
#define STATS_H


#include <stdio.h>
#include "setting.h"

#ifdef STATS

#define SELECT_STATS_MAX 15
#define UPDATE_STATS_MAX 46
#define SET_STATS_MAX 10
#define STATS_MAX 500 //max z toho nahore


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

typedef struct {
	int values[STATS_MAX];
	int maxVal; //maximalni velikost pole
	int max; //nejvyssi hodnota
	int more; // kolik hodnot je vyssych nez MAX
	char* name;
} histagram_t;

extern stats_t all_stats;
extern stats_t turn_stats[N*N];
extern int select_stats_max;
extern int select_stats[SELECT_STATS_MAX];
extern int interations_stats;
extern histagram_t updateStats;

void printStats(char * file_name);
void statsInit();
void histagramAdd( histagram_t* h, int value );

#endif //STATS

#endif //STATS_H
