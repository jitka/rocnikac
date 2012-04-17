#ifndef STATS_H
#define STATS_H


#include <stdio.h>
#include "setting.h"

#ifdef STATS

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define SELECT_STATS_MAX 15
#define UPDATE_STATS_MAX 46
#define SET_STATS_MAX 10
#define STATS_MAX  MAX( SET_STATS_MAX, MAX( UPDATE_STATS_MAX, SET_STATS_MAX ) )


typedef struct histagram_T histagram_t;
typedef struct stats_T stats_t;

struct histagram_T {
	int values[STATS_MAX];
	int maxVal; //maximalni velikost pole
	int max; //nejvyssi hodnota
	int more; // kolik hodnot je vyssych nez MAX
	char* name;
};

struct stats_T {
	int created;
	int created_true;
	int created_false;
	int finished;
	int finished_true;
	int finished_false;
	int threat;

	histagram_t setDel;
};


extern stats_t all_stats;
extern stats_t turn_stats[M+1];
extern int interations_stats;
extern histagram_t updateStats;
extern histagram_t selectStats;

void printStats(char * file_name);
void statsInit();
void histagramAdd( histagram_t* h, int value );

#endif //STATS

#endif //STATS_H
