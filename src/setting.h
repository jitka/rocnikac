#ifndef SETTING_H
#define SETTING_H

#define N 6 //funguje pro N 5..8 aby sla matice sousedu ulozit 
#define M ((N*(N-1))/2+1) 
#define MAXPROOF (INT_MAX/M)
#define CACHE_SIZE (1<<23)
#define CACHE_PATIENCE 100 //kolik pozic za mistem kam patri se jeste muze nalezat 
#define MAXNODES 6000000 //kolik node se vleze do pameti NEFUNGUJE
#define MAXTREEWIDTH 100000 //velikost pole pro updateAncestors

#define DEBUG
#define WEAK //jestli je klasicky nebo weak PN-search 
//#define NOVECHANGE //prehazuju sploupce i radky maskama POMALE, NEFUNGUJE
#define STATS
#define NORM2
#define UPDATEANCESTORS2
//#define HEURISTIC1 //listy se radi tak aby driv byly ti co maji vic volnych vyhernich pozic ZPOMALUJE
//#define NOFREEK4 //kdyz prvni hrac nema zadnou nezabranenou K4 hra je FALSE TESTOVAT
#define TURNDDELETECHILDRENST M //do ktereho tahu se mazou vicenasobne deti
#define DELETEUSELESSPARENTS //velmi vhodne pokud se vyhazuje z cache, jinak nasobnosti pretece pamet
//#define NODEDELETE //kdyz neni misto v cachy a node neni nutne potreba je zahozen
//#define DFPN

#define MAXCHILD(TURN) MAX((M-(TURN)),3)

#define bool unsigned int
#define u8 unsigned char
#define u32 unsigned int
#define u64 unsigned long long int

#endif
