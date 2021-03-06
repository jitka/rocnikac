#ifndef SETTING_H
#define SETTING_H

//#define N 6 // definuje se v Makefile funguje pro N 5..8 aby sla matice sousedu ulozit 
#define M ((N*(N-1))/2+1) 
#define MAXPROOF (INT_MAX/M)
#define CACHE_SIZE (1<<24)
#define CACHE_PATIENCE 100 //kolik pozic za mistem kam patri se jeste muze nalezat 
#define MAXTREEWIDTH 100000 //velikost pole pro updateAncestors

//tito se zapinaji v makefile
//#define WEAK //jestli je klasicky nebo weak PN-search 
//#define STATS
//#define NORM2
//#define UPDATEANCESTORS2
//#define HEURISTIC1 //listy se radi tak aby driv byly ti co maji vic volnych vyhernich pozic ZPOMALUJE, aktualne nefunguje
//#define NOFREEK4 //kdyz prvni hrac nema zadnou nezabranenou K4 hra je FALSE
//#define TURNDDELETECHILDRENST 0 //do ktereho tahu se mazou vicenasobne deti
//#define DELETEUSELESSPARENTS //velmi vhodne pokud se vyhazuje z cache, jinak nasobnosti pretece pamet
//#define NODEDELETE //kdyz neni misto v cachy a node neni nutne potreba je zahozen
//#define DFPN
//#define DFPNUPDATE //jestli je pri DFPN zaply update ancestors NEFUNGUJE

#define MAXCHILD(TURN) MAX((M-(TURN)),3)

#define bool unsigned int
#define u8 unsigned char
#define u32 unsigned int
#define u64 unsigned long long int

#endif
