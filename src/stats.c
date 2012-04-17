#include "stats.h"
#ifdef STATS


stats_t all_stats;
stats_t turn_stats[M+1];
int interations_stats = 0;

histagram_t updateStats;
histagram_t selectStats;

void statsInit(){
	updateStats.maxVal = UPDATE_STATS_MAX;
	updateStats.name = "updateAncestors";
	selectStats.maxVal = SELECT_STATS_MAX;
	selectStats.name = "selectMostProving";
	all_stats.set.maxVal = SET_STATS_MAX;
	all_stats.set.name = "setPD-Numbers";
	for (int t = 0; t <= M; t++){
		turn_stats[t].set.maxVal = SET_STATS_MAX;
		turn_stats[t].set.name = "setPD-Numbers";
	}
}

void histagramAdd( histagram_t* h, int value ){
	h->max = MAX( h->max, value);
	if ( value >= h->maxVal)
		h->more++;
	else 
		h->values[value]++;
}

void histagramPrint( FILE* f,  histagram_t* h){
	fprintf(f,"%s max: %d; more then %d: %d\n",h->name,h->max,h->maxVal,h->more);
	for (int s = 0; s < h->maxVal; s++){
		fprintf(f,"%d-%d; ",s,h->values[s]);
	} fprintf(f,"\n");
}

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
	fprintf(f,"interations %d\n",interations_stats);
	histagramPrint( f, &all_stats.set );
	histagramPrint( f, &updateStats );
	histagramPrint( f, &selectStats );
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
		histagramPrint( f, &turn_stats[turn].set );
	}
	fclose(f);
}

#endif //STATS
