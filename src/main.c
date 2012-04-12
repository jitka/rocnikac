#include <stdio.h>
#include "pnsearch.h"
#include "linkedlist.h"
#include "start.h"
#include "print.h"
#include "norm.h"
#include "tabs.h"

// TODO HNED
// testovat norm
// rychlejsi norm select sort --proc je to tak pomale?? nefunguje s permutacni tabulkou testovat fce
// permutacni tabulka
// git
// vetsi testy, N 7
// vhodne CACHE_PATIENCE, velikost cache 
// paralelizace 
// radit listy podle heuristik
//
//
// TODO NEKDY
// uklid nodefce
// kdyz nucene tahy DB-search
// oprofile
// vhodnejsi cisla na zacatku - testovat!
// davat dohromady vysledky z ruznych pozic
// cistejsi predavani stavu mezi haskelem a timhle
//
// CIST
//
// MOZNE ZRYCHLENI
// spojak do pole? a cely strom do pole?
// do cache data + ukazatel/true/false
// nepristupovat pres ukazatel ale zkusit cele node_t zkopirovat
// rychlejsi normalizace -malloc, trizeni
// normalizace i podle vedlejsich stupnu
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
// proc je norm pomaly, pro to nezbehne pro 6 bez predvypocitani hran
// proc je pomale prohazovani hran?
// nekonecno... nesmim se tam dostat a nekonecno plus konstanta je nekonecno
// Gavento - hasovani

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
