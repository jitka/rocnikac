#include "stats.h"

#ifdef STATS

stats_t all_stats;
stats_t turn_stats[N*N];

void printStats(char * file_name){
	FILE* f = fopen(file_name,"w");
	fprintf(f,"stats\n");
	fclose(f);
}

#endif //STATS
