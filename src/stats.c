#include "stats.h"

#ifdef STATS
#endif //STATS

#ifdef STATS

stats_t all_stats;
stats_t turn_stats[N*N];

void printStats(char * file_name){
	FILE* f = fopen(file_name,"w");
	fprintf(f,"stats\n");
	fprintf(f,"%d;\n",all_stats.created);
	for (int turn = 0; turn < (N*(N-1))/2; turn++){
		fprintf(f,"%d: %d;\n",turn,turn_stats[turn].created);
	}
	fclose(f);
}

#endif //STATS
