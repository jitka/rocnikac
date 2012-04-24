#ifndef SETTING_H
#define SETTING_H

//pozor spojuju hrany -> pn search se muze zacyklit
#define MAXPROOF (INT_MAX/1000)
#define N 7 //funguje pro N 5..8 aby sla matice sousedu ulozit 
#define M ((N*(N-1))/2) 
#define DEBUG
#define WEAK //jestli je klasicky nebo weak PN-search 
//#define NOVECHANGE //prehazuju sploupce i radky maskama POMALE, NEFUNGUJE
#define STATS
//#define HEURISTIC1 //listy se radi tak aby driv byly ti co maji vic volnych vyhernich pozic NEFUNGUJE
#define NORM2

#define CACHE_SIZE (1<<24)
#define CACHE_PATIENCE 100 //kolik pozic za mistem kam patri se jeste muze nalezat 
#define MAXNODES 6000000 //kolik node se vleze do pameti

#define bool int
#define u8 unsigned char
#define u32 unsigned int
#define u64 unsigned long long int

#endif
