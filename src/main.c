#include <stdio.h>
#include "pnsearch.h"
#include "start.h"
#include "print.h"

// TODO HNED
// smazat vsechno po pnsearch 
// testovat cteni vice node
// normalizace
// kdyz nucene tahy DB-search
//
// TODO NEKDY
// prepsat spojak - do pole? a cely strom do pole?
// radit listy podle heuristik
// vhodnejsi cisla na zacatku - testovat!
// davat dohromady vysledky z ruznych pozic
// cistejsi predavani stavu mezi haskelem a timhle
// vhodne CACHE_PATIENCE  
//
// CIST
// DB-search
// weak PN-search
//
// MOZNE ZRYCHLENI
// rychlejsi predani tupu blue/or spojit dohromady
// rekurze v delete
// test v K4
// nejdriv se koukat do cache a pak teprve testovat
// zmensit data v node - brum prvocisla
// zmensit i ukazatele v node
// proc je sys tak velike? malloc? ptat se jineho time
// pole na syny mit globalni?
// odstranit vypisy chyb co nikdy nenastanou
// mazat true deti v and a mazat false deti v or
// jak casto se bude pri jednom update ancessor jeden node updatetovat vickrat?
// spojaky v poli
// cely strom v poli
// posladni stavy (uz dohrano) zabiraji moc mista v cachy
//
// DOTAZY
// Gavento - hasovani

int main(){

	hashInit();
	node_t* root = getRoot();
	nodeValue_t value = proofNuberSearch(root);
	printValue(value);

//	node_t* root = readNode();
//	printNode(root);

	/*
	ll_t* nodes = readNodes();
	node_t* node;
//	node = llGetNode(&nodes); printNode(node);
//	printValue(proofNuberSearch(node));
	while ((node = llGetNode(&nodes)) != NULL){
		printNode(node);
		printValue(proofNuberSearch(node));
	}
*/
	return 0;
}
