#include <stdio.h>
#include "pnsearch.h"
#include "start.h"
#include "print.h"

// TODO HNED
// vice predku -
//             - update ancesor dalsi predci do spojaku
//             - update dalsich predku
//             - uklidit
// hasovani - pridani do cache -hned
//          - koukani do cache pred tim = spolecne ukazatele k synum
// vhodnejsi cisla na zacatku - testovat!
//
// TODO NEKDY
// radit listy podle heuristik
// normalizace
// mazat zbytecne nody, pocet otcu-snizovat pri delete, zmensit pocet nodu
// smazat vsechno po pnsearch 
// davat dohromady vysledky z ruznych pozic
// cistejsi predavani stavu mezi haskelem a timhle
// kdyz nucene tahy DB-search
//
// CIST
// DB-search
// weak PN-search
//
// MOZNE ZRYCHLENI
// rekurze v delete
// test v K4
// zmensit data v node
// zmensit i ukazatele v node
// proc je sys tak velike? malloc?
// pole na syny mit globalni?
// odstranit vypisy chyb co nikdy nenastanou
// mazat true deti v and a mazat false deti v or
// jak casto se bude pri jednom update ancessor jeden node updatetovat vickrat?
//
// DOTAZY
// jsou ty velikosti jak maji byt? vyjde to?
// jsou bitove operace ok?

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
