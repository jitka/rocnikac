#include <stdio.h>
#include "pnsearch.h"
#include "linkedlist.h"
#include "start.h"
#include "print.h"
#include "norm.h"
#include "tabs.h"
#include "stats.h"

// TODO HNED
// obsah
// zeptat Pavel, stroje
// existuje knihovna na zdilenou cache? google, Tom, Brum, Pavel
// googlit klikovou hru clique game, hight symetri, low branching factor
// oprofile
// N 8 
// jeden rodic vicekrat
// divne padani na cachesize 12
//
// TODO NEKDY
// heuristika misto 1 1
// node - mensi
// updatete jednoho predka vickrat? 
// 	- do vrcholu psat cislo update
// 	- kolikrat v tom update tam bylo
// 	- statistika
// 	- na kazdem levelu vyhodit duplicity?
// konstanty 
// 	- vhodne CACHE_PATIENCE, 
// 	- TURNDELETECHILDRENST 
// 	- velikost cache 
// 	- velikost stomu 
// 	- pocitat s sizeof
// hasovaci funkce z graph
// testovat vsechny binarni veci
// paralelizace 
// norm-rychlejsi hasovanani
// norm-lepsi fce, vic fci
// kdyz nucene tahy DB-search, nejspis vyhodne az pozdeji... po nejakem stupni? tahu?
// pri vkladani co cache zkusit permutovat vrcholy se stejnou ohodnocovacich fci
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
// proc udpte ancesor vubec nepomuze?
// co se slepimi cesty?

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
#define TURNDDELETECHILDRENST 1 //do ktereho tahu se mazou vicenasobne deti


	printf("\n---------------------------------------------------------\n");
}

int main(){

#ifdef STATS
	statsInit();
#endif //STATS
	tabsInit();

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
