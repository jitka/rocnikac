#include "pnsearch.h"
#include "start.h"

// TODO HNED
// novy pristup k node -
//                     - ukazatele
//                     - data
//                     - testuj print
// mensi node
// 	- testovat-vypisy pri expandovani
// otcove v poli-rekurze nahoru
// hasovani - pridani do cache -hned
//          - koukani do cache pred tim = spolecne ukazatele k synum
// nacitani startovnich pozic
// vhodnejsi cisla na zacatku - testovat!
//
// TODO NEKDY
// radit listy podle heuristik
// normalizace
// mazat zbytecne nody, pocet otcu-snizovat pri delete, 
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

int main(){

	hashInit();
	node_t* root = getRoot();
	proofNuberSearch(root);

	return 0;
}
