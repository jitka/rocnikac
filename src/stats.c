#include "stats.h"
int cacheMiss = 0;

#ifdef STATS

#include "nodefce.h"

stats_t all_stats;
stats_t turn_stats[M+1];
int interations_stats = 0;

histogram_t updateStats;
histogram_t selectStats;

void statsInit(){
	updateStats.maxVal = UPDATE_STATS_MAX;
	updateStats.name = "updateAncestors";
	selectStats.maxVal = SELECT_STATS_MAX;
	selectStats.name = "selectMostProving";
	all_stats.setFin.maxVal = SET_STATS_MAX;
	all_stats.setFin.name = "setPD-NumbersHotove";
	all_stats.setDel.maxVal = SET_STATS_MAX;
	all_stats.setDel.name = "setPD-NumbersSmazane";
	all_stats.parFin.maxVal = PAR_STATS_MAX;
	all_stats.parFin.name = "parentsHotove";
	all_stats.parDel.maxVal = PAR_STATS_MAX;
	all_stats.parDel.name = "parentsSmazane";

	for (int t = 0; t <= M; t++){
		turn_stats[t].setFin.maxVal = SET_STATS_MAX;
		turn_stats[t].setFin.name = "setPD-NumbersHotove";
		turn_stats[t].setDel.maxVal = SET_STATS_MAX;
		turn_stats[t].setDel.name = "setPD-NumbersSmazane";
		turn_stats[t].parFin.maxVal = PAR_STATS_MAX;
		turn_stats[t].parFin.name = "parentsHotove";
		turn_stats[t].parDel.maxVal = PAR_STATS_MAX;
		turn_stats[t].parDel.name = "parentsSmazane";
	}
}

void statsNewNode(node_t* node){
	int i = nodeLastEdgeI(node);
	int j = nodeLastEdgeJ(node);
	all_stats.created++;
	turn_stats[nodeTurn(node)].created++;
	if (nodeValue(node) == TRUE){
		all_stats.created_true++;
		turn_stats[nodeTurn(node)].created_true++;
	}
	if (nodeValue(node) == FALSE){
		all_stats.created_false++;
		turn_stats[nodeTurn(node)].created_false++;
	}

	color color; //ktera se obarvila kdyz se slo do tohohle stavu
	if (nodeType(node)==OR) 
		color = BLUE;
	else
		color = RED;
	if (graphThreat(nodeGraph(node), i, j, color)){
		all_stats.threat++;
		turn_stats[nodeTurn(node)].threat++;
	}

}

void histogramAdd( histogram_t* h, int value ){
	h->max = MAX( h->max, value);
	if ( value >= h->maxVal)
		h->more++;
	else 
		h->values[value]++;
}

void histogramPrint( histogram_t* h){
	printf("%s max: %d; more then %d: %d\n",h->name,h->max,h->maxVal,h->more);
	for (int s = 0; s < h->maxVal; s++){
		printf("%d-%d; ",s,h->values[s]);
	} printf("\n");
}

u64 choose(int n, int k){
	if (n < k || k < 0 || n < 0)
		perror("komibanci cislo");
	if (k == 0)
		return 1;
	else 
		return (n * choose(n-1,k-1) ) / k;
}

u64 differentNodesOnTurn(int turn){
	int red = turn/2;
	int blue = turn - red;
	return choose( M,red) * choose( M-red, blue);
}

u64 differentNodes(){
	u64 sum = 0;
	for (int turn = 0; turn <= M; turn++)
		sum += differentNodesOnTurn(turn);
	return sum;
}

void printStats(){

	printf("interace %d\n",interations_stats);
	histogramPrint( &all_stats.setFin );
	histogramPrint( &all_stats.parFin );
	histogramPrint( &all_stats.setDel );
	histogramPrint( &updateStats );
	histogramPrint( &selectStats );
	printf("\n");
	printf("%llu, %d; %d; %d; %d;; %d; %d; %d;\n",
			differentNodes(),
			all_stats.created,
			all_stats.created_true,
			all_stats.created_false,
			all_stats.threat,
			all_stats.finished,
			all_stats.finished_true,
			all_stats.finished_false
			);
	printf("tah: moznych; vytvorenych; true; false; threats;; vypocitanych; true; false;\n");
	for (int turn = 0; turn <= M; turn++){
		printf("%d: %llu; %d; %d; %d; %d;; %d; %d; %d; \n",
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
//		histogramPrint( &turn_stats[turn].setFin );
//		histogramPrint( &turn_stats[turn].parFin );
//		histogramPrint( &turn_stats[turn].setDel );
//		printf("\n");
	}

}

#endif //STATS
