#include <stdio.h>
#include "pnsearch.h"
#include "linkedlist.h"
#include "start.h"
#include "print.h"
#include "norm.h"
#include "tabs.h"
#include "stats.h"

// TODO HNED
// herni strom 
// 	- cele pustit pro N 5..7 s/bez WEAK,
// 	- zkusit strcit co calc
// 	- tisk
// 	- ucesat vypis, ulozit pro budouci generace
// googlit, stare clanky od Toma
// testovat jestli ma prvni na alespon jedne hrane alespon 2 volne sousedy jinak false
// heuristika podle volnych vrcholu
// other color
//
// TODO NEKDY
// testovat vsechny binarni veci
// vadi svapovani? 
// nemit vic stejnych deti 
// 	- prepinac DELETECHILDS
//      - otcu pri vypocitani statiskka
// updatete jednoho predka vickrat?
// paralelizace 
// vhodne CACHE_PATIENCE, velikost cache 
// radit listy podle heuristik
// norm-rychlejsi hasovanani
// norm-lepsi fce
// 	- normalizace i podle vedlejsich stupnu
// absolutni norm = vyber reprezentanta, knihovna
// kdyz nucene tahy DB-search, nejspis vyhodne az pozdeji... po nejakem stupni? tahu?
// pri vkladani co cache skusit permutovat vrcholy se stejnou ohodnocovacich fci
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
// norm a hasovani (prohazovani radku/tabulka)
// nekonecno... nesmim se tam dostat a nekonecno plus konstanta je nekonecno

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
