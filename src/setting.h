#ifndef SETTING_H
#define SETTING_H

//pozor spojuju hrany -> pn search se muze zacyklit
#define MAXPROOF (INT_MAX/1000)
#define N 6 //funguje pro N 5..8 aby sla matice sousedu ulozit 
#define M ((N*(N-1))/2+1) 
//#define DEBUG
#define WEAK //jestli je klasicky nebo weak PN-search 
//#define NOVECHANGE //prehazuju sploupce i radky maskama POMALE, NEFUNGUJE
//#define STATS
#define NORM2
//#define HEURISTIC1 //listy se radi tak aby driv byly ti co maji vic volnych vyhernich pozic ZPOMALUJE
//#define NOFREEK4 //kdyz prvni hrac nema zadnou nezabranenou K4 hra je FALSE, NENI HOTOVE
#define TURNDDELETECHILDRENST 1 //do ktereho tahu se mazou vicenasobne deti

#define CACHE_SIZE (1<<26)
#define CACHE_PATIENCE 100 //kolik pozic za mistem kam patri se jeste muze nalezat 
#define MAXNODES 6000000 //kolik node se vleze do pameti

#define bool int
#define u8 unsigned char
#define u32 unsigned int
#define u64 unsigned long long int

#endif
