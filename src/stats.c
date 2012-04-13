#include "stats.h"

#ifdef STATS
#endif //STATS

#ifdef STATS

stats_t all_stats;
stats_t turn_stats[N*N];
int update_stats_max = 0;
int update_stats[UPDATE_STATS_MAX];
int select_stats_max = 0;
int select_stats[SELECT_STATS_MAX];
int interations_stats = 0;

void printStats(char * file_name){
	FILE* f = fopen(file_name,"w");
	fprintf(f,"stats\n");
	fprintf(f,"%d; %d; %d; %d;; %d; %d; %d;\n",
			all_stats.created,
			all_stats.created_true,
			all_stats.created_false,
			all_stats.threat,
			all_stats.finished_true,
			all_stats.finished_false,
			all_stats.finished
			);
	fprintf(f,"interations %d\n",interations_stats);
	fprintf(f,"update max %d;\n",update_stats_max);
	//for (int s = 0; s < UPDATE_STATS_MAX; s++){
	//	fprintf(f,"%d-%d; ",s,update_stats[s]);
	//} fprintf(f,"\n");
	fprintf(f,"select max %d;",select_stats_max);
	for (int s = 0; s < SELECT_STATS_MAX; s++){
		fprintf(f,"%d-%d; ",s,select_stats[s]);
	} fprintf(f,"\n");
	fprintf(f,"tah: vytvorenych; true; false; threats;; vypocitanych; true; false;\n");
	for (int turn = 0; turn < (N*(N-1))/2; turn++){
		fprintf(f,"%d: %d; %d; %d; %d;; %d; %d; %d; \n",
				turn,
				turn_stats[turn].created,
				turn_stats[turn].created_true,
				turn_stats[turn].created_false,
				turn_stats[turn].threat,
				turn_stats[turn].finished,
				turn_stats[turn].finished_true,
				turn_stats[turn].finished_false
				);
	}
	fclose(f);
}

#endif //STATS
