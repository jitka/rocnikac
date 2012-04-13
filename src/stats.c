#include "stats.h"

#ifdef STATS
#endif //STATS

#ifdef STATS

stats_t all_stats;
stats_t turn_stats[N*N];

void printStats(char * file_name){
	FILE* f = fopen(file_name,"w");
	fprintf(f,"stats\n");
	fprintf(f,"%d; %d; %d;\n",
			all_stats.created,
			all_stats.created_true,
			all_stats.created_false
			);
	fprintf(f,"tah: vsech; true; false;\n");
	for (int turn = 0; turn < (N*(N-1))/2; turn++){
		fprintf(f,"%d: %d; %d; %d;\n",
				turn,
				turn_stats[turn].created,
				turn_stats[turn].created_true,
				turn_stats[turn].created_false
				);
	}
	fclose(f);
}

#endif //STATS
