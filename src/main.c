#include <stdio.h>
#include "pnsearch.h"
#include "linkedlist.h"
#include "start.h"
#include "print.h"
#include "norm.h"
#include "tabs.h"
#include "stats.h"

// TODO HNED
// vylepsovaky nahrat, cist
// googlit klikovou hru
// #define NOFREEK4 //kdyz prvni hrac nema zadnou nezabranenou K4 hra je FALSE, NENI HOTOVE
// napsat df (ε trik)
// 	- nejdriv bez nasobnych deti
// 	- zarazky
// 	- nekonecno
// 	- pri probublani bocnimy otci navrch zastavit
// 	- hasovaci tabulka misto stromu
// konstanty 
// 	- vhodne CACHE_PATIENCE, 
// 	- TURNDELETECHILDRENST 
// 	- velikost cache 
// 	- velikost stomu 
// 	- pocitat s sizeof
// updatete jednoho predka vickrat? 
// 	- do vrcholu psat cislo update
// 	- kolikrat v tom update tam bylo
// 	- statistika
// node - mensi
//
// TODO NEKDY
// hasovaci funkce z graph
// testovat vsechny binarni veci
// paralelizace 
// radit listy podle heuristik
// norm-rychlejsi hasovanani
// norm-lepsi fce, vic fci
// absolutni norm = vyber reprezentanta, knihovna
// kdyz nucene tahy DB-search, nejspis vyhodne az pozdeji... po nejakem stupni? tahu?
// pri vkladani co cache zkusit permutovat vrcholy se stejnou ohodnocovacich fci
// oprofile
// davat dohromady vysledky z ruznych pozic
// cistejsi predavani stavu mezi haskelem a timhle
//
// MOZNE ZRYCHLENI
// spojak do pole a cely strom do pole
// do cache data + ukazatel/true/false
// nepristupovat pres ukazatel ale zkusit cele node_t zkopirovat
// rychlejsi normalizace -malloc, trizeni
// mensi node_t (brum prvocisla,ukazatele)
// rychlejsi predani tupu blue/or spojit dohromady
// rekurze v delete
// jak casto se bude pri jednom update ancessor jeden node updatetovat vickrat?
//
// DOTAZY
// tema+co vsechno napsat
// proc je update ancesor tak vysoke? proc nepomohlo mazani?
// funguje df-PN na vice otcu odrekurzene?
// klikova hra - preklad

void printSetting(){

	printf("N %d M %d\n",N,M);
	printf("CACHE_SIZE %d, CACHE_PATIENCE %d, MAXNODES %d\n", CACHE_SIZE, CACHE_PATIENCE, MAXNODES);
#ifdef DEBUG 
	printf("DEBUG\n");
#endif

#ifdef WEAK 
	printf("WEAK\n");
#endif
#ifdef STATS
	printf("STATS\n");
#endif
#ifdef HEURISTIC1
	printf("HEURISTIC1\n");
#endif
#ifdef NORM2
	printf("NORM2\n");
#endif
	printf("TURNDDELETECHILDRENST %d\n",TURNDDELETECHILDRENST);
	printf("---------------------------------------------------------\n");
}

int main(){

#ifdef STATS
	statsInit();
#endif //STATS
	tabsInit();

	printSetting();

	node_t* root = getRoot();
	nodeValue_t value = proofNuberSearch(root);
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
	printStats( );
#endif //STATS
	return 0;
}
