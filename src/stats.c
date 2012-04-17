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

int choose(int n, int k){
	if (n < k || k < 0 || n < 0)
		perror("komibanci cislo");
	if (k == 0)
		return 1;
	else 
		return (n * choose(n-1,k-1) ) / k;
}

int differentNodesOnTurn(int turn){
	int red = turn/2;
	int blue = turn - red;
	return choose( M,red) * choose( M-red, blue);
}

int differentNodes(){
	int sum = 0;
	for (int turn = 0; turn <= M; turn++)
		sum += differentNodesOnTurn(turn);
	return sum;
}

void printStats(char * file_name){
	FILE* f = fopen(file_name,"w");
	fprintf(f,"stats\n");
	fprintf(f,"%d, %d; %d; %d; %d;; %d; %d; %d;\n",
			differentNodes(),
			all_stats.created,
			all_stats.created_true,
			all_stats.created_false,
			all_stats.threat,
			all_stats.finished,
			all_stats.finished_true,
			all_stats.finished_false
			);

	fprintf(f,"set-finished max: %d more then %d: %d \n",
			all_stats.set_stats_max, 
			SET_STATS_MAX, 
			all_stats.set_stats_more_then_max);
	for (int s = 0; s < SET_STATS_MAX; s++){
		fprintf(f,"%d-%d; ",s,all_stats.set[s]);
	} fprintf(f,"\n");
	fprintf(f,"interations %d\n",interations_stats);
	fprintf(f,"update max %d;\n",update_stats_max);
	//for (int s = 0; s < UPDATE_STATS_MAX; s++){
	//	fprintf(f,"%d-%d; ",s,update_stats[s]);
	//} fprintf(f,"\n");
	fprintf(f,"select max %d;",select_stats_max);
	for (int s = 0; s < SELECT_STATS_MAX; s++){
		fprintf(f,"%d-%d; ",s,select_stats[s]);
	} fprintf(f,"\n");
	fprintf(f,"tah: moznych; vytvorenych; true; false; threats;; vypocitanych; true; false;\n");
	for (int turn = 0; turn <= M; turn++){
		fprintf(f,"%d: %d; %d; %d; %d; %d;; %d; %d; %d; \n",
				turn,
				differentNodesOnTurn(turn),
				turn_stats[turn].created,
				turn_stats[turn].created_true,
				turn_stats[turn].created_false,
				turn_stats[turn].threat,
				turn_stats[turn].finished,
				turn_stats[turn].finished_true,
				turn_stats[turn].finished_false
		       );
		fprintf(f,"set-finished max: %d more then %d: %d \n",
				turn_stats[turn].set_stats_max, 
				SET_STATS_MAX, 
				turn_stats[turn].set_stats_more_then_max
			);
		for (int s = 0; s < SET_STATS_MAX; s++){
			fprintf(f,"%d-%d; ",s,turn_stats[turn].set[s]);
		} fprintf(f,"\n");
	}
	fclose(f);
}

#endif //STATS
