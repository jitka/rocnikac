#include <stdio.h>
#include "pnsearch.h"
#include "linkedlist.h"
#include "start.h"
#include "print.h"
#include "norm.h"
#include "tabs.h"
#include "stats.h"
#include "cache.h"

// TODO HNED
// googlit klikovou hru clique game, hight symetri, low branching facto strong, positional, časopis combinoro.. Beck combinatorial games
// oprofile
// jine varianty - prepinace
//	- ne DF ne DEL
// 	- NOFREEK4 DF 
// 	- DFPN -updateAncestor 
// 		- prepinac jestli pouzit
// 		- posouvat current node vys jen pokud preroste zarazky (jak casto se deje?)
// psat - probublani bokem
// psat - moje programovici cast
//
// TODO NEKDY MOZNA
// heuristika misto 1 1
// heuristika na vyhazovani z cache
// heuristika jit tam kde ma protihrac nejvic nejvetsich(co do poctu plnych hran2^) hrozeb
// N 8 
// konstanty 
// 	- vhodne CACHE_PATIENCE, 
// 	- TURNDELETECHILDRENST 
// 	- velikost cache 
// 	- velikost stomu 
// 	- pocitat s sizeof
// hasovaci funkce z graph
// testovat vsechny binarni veci
// norm - rychlejsi hasovanani
// 	- rychlejsi trizeni
// 	- lepsi fce, vic fci
// kdyz nucene tahy DB-search, nejspis vyhodne az pozdeji... po nejakem stupni? tahu?
// generovani tahu 
// 	- dokazat ze je blbost
// 	- smazat?
// 	- davat dohromady vysledky z ruznych pozic
// 	- cistejsi predavani stavu mezi haskelem a timhle
// 	- smazat spojak uplne
// do cache: graph_t + ukazatel/true/false 
// mensi node_t 
// 	- (brum prvocisla,ukazatele)
// 	- rychlejsi predani tupu blue/or spojit dohromady
// 1+epsilon
// PN-set
//
// DOTAZY
// proc udpte ancesor vubec nepomuze?

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
	ll2_t nodes = readNodes();
	ll2FStart(&nodes); 
	for (node_t* node; (node = ll2FGet(&nodes)) != NULL; ll2FNext(&nodes)){
//		printNode(node);
		nodeValue_t v = proofNuberSearch(node);
//		if (v != FALSE)
			printValue(v);
	}
	printf("\nhotovo\n");
*/

#ifdef STATS
	printf("---------------------------------------------------------\n");
	printStats( );
#endif //STATS
	return 0;
}
