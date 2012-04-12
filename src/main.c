#include <stdio.h>
#include "pnsearch.h"
#include "linkedlist.h"
#include "start.h"
#include "print.h"
#include "norm.h"
#include "tabs.h"

// TODO HNED
// herni strom 
// 	- novy soubory na ukladani, vypis statictik
// 	- nastavovatko STATS 
// 		- update;
// 		- select;
//              - patro - vytvorenych; 
//                      - vytvorenech true; 
//                      - false; 
//                      - vynucenych tahu; 
//                      - otcu pri vypocitani;
//  			- cetnost pousteni set pred urcenim
// 			- cetnost pousteni set pred 
// 	- soucet cekove
// 	- haskell novym srovnat stary ...
// 	- cele pustit pro N 5..7 s/bez WEAK
// 	- zkusit strcit co calc
// 	- vyrobit grafy (vsechny varianty)
// 	 	- celkove; mych; az na izo;
// googlit
// testovat jestli ma prvni na alespon jedne hrane alespon 2 volne sousedy jinak false
// heuristika podle volnych vrcholu
//
// TODO NEKDY
// paralelizace 
// vhodne CACHE_PATIENCE, velikost cache 
// radit listy podle heuristik
// norm hasovanani
// kdyz nucene tahy DB-search, nejspis vyhodne az pozdeji... po nejakem stupni? tahu?
// normalizace i podle vedlejsich stupnu
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

int main(){

	tabsInit();

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
	return 0;
}
