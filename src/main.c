#include <stdio.h>
#include "pnsearch.h"
#include "linkedlist.h"
#include "start.h"
#include "print.h"
#include "norm.h"
#include "tabs.h"
#include "stats.h"
#include "cache.h"

// TODO BAKALARKA
// defaultni hodne ukecany stats
// ifdef DEBUG HEURISTIC1
// uplne smazat nove change
// smazat maxnodes
// update ancestors vycistit
// lechce mensi node_t
// printf smazat 
// precist cele
// doplnit komentare 
//
// cistci predavani pozic
// konstanty 
// 	- vhodne CACHE_PATIENCE, 
// 	- TURNDELETECHILDRENST 
// 	- velikost cache 
// 	- velikost stomu 
// 	- pocitat s sizeof
// hasovaci funkce z graph
//
// smazat spojak uplne
// pokud je vic hrozeb tak prohra
// heuristika jit tam kde ma protihrac nejvic nejvetsich(co do poctu plnych hran2^) hrozeb
// heuristika misto 1 1
// N 8  generovani tahu - jak moc musim predgenerovat?
// kdyz nucene tahy DB-search, nejspis vyhodne az pozdeji... po nejakem stupni? tahu?
// do cache: graph_t + ukazatel/true/false 
// mensi node_t 
// 	- (brum prvocisla,ukazatele)
// 	- blue/or spojit dohromady
// 1+epsilon
// PN-set

void printSetting(){

	printf("N %d M %d\n",N,M);
	printf("CACHE_SIZE %d, CACHE_PATIENCE %d, MAXNODES %d\n", CACHE_SIZE, CACHE_PATIENCE, MAXNODES);
#ifdef DEBUG 
	printf("DEBUG ");
#endif

#ifdef WEAK 
	printf("WEAK ");
#endif
#ifdef STATS
	printf("STATS ");
#endif
#ifdef HEURISTIC1
	printf("HEURISTIC1 ");
#endif
#ifdef NORM2
	printf("NORM2 ");
#endif
	printf("TURNDDELETECHILDRENST %d ",TURNDDELETECHILDRENST);
#ifdef DFPN
	printf("DFPN ");
#endif
#ifdef DFPNUPDATE
	printf("DFPNUPDATE ");
#endif
#ifdef NOFREEK4
	printf("NOFREEK4 ");
#endif
#ifdef UPDATEANCESTORS2
	printf("UPDATEANCESTORS2 ");
#endif
#ifdef DELETEUSELESSPARENTS
	printf("DELETEUSELESSPARENTS ");
#endif

	printf("\n---------------------------------------------------------\n");
}

int main(){

#ifdef STATS
	statsInit();
#endif //STATS
	tabsInit();
	cacheInit();

	printSetting();


//	printf("%d\n",sizeof(node_t));
	//1.varianta
	node_t* root = getRoot();
	nodeValue_t value = proofNumberSearch(root);
	printValue(value);
/*
	node_t* root = readNode();
	printNode(root);
	norm(root);
	printNode(root);
*/
/*
	//2.varianta
	ll2_t nodes = readNodes();
	ll2FStart(&nodes); 
	for (node_t* node; (node = ll2FGet(&nodes)) != NULL; ll2FNext(&nodes)){
		printNode(node);
//		nodeValue_t v = proofNumberSearch(node);
//		if (v != FALSE)
//			printValue(v);
	}
	printf("\nhotovo\n");

*/
#ifdef STATS
	printf("---------------------------------------------------------\n");
	printStats( );
#endif //STATS
	return 0;
}
