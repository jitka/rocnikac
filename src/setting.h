#ifndef SETTING_H
#define SETTING_H

#define MAXPROOF (INT_MAX/1000)
#define N 6 //funguje pro N 5..8 aby sla matice sousedu ulozit 
#define DEBUG
#define WEAK //jestli je klasicky nebo weak PN-search 

#define CACHE_SIZE (1<<24)
#define CACHE_PATIENCE 100 //kolik pozic za mistem kam patri se jeste muze nalezat 
#define MAXNODES 1000000 //kolik node se vleze do pameti

#define u8 char
#define u32 unsigned int
#define u64 unsigned long long int

#endif
