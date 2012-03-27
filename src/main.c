#include "pnsearch.h"

// TODO HNED
// pn-search poustet na node
// nacitani startovnich pozid
// novy pristup k node
// min intu v node
// syny v poli
// otcove v poli-rekurze nahoru, smazt delete
// vhodnejsi cisla na zacatku - testovat!
// hasovani - pridani do cache -hned
//          - koukani do cache pred tim
//          - spolecne ukazatele k synum
//
// TODO NEKDY
// radit listy podle heuristik
// normalizace
// mazat zbytecne nody
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

int main(){

	hashInit();
	proofNuberSearch();

	return 0;
}
