#ifndef NODE_H
#define NODE_H

#include <limits.h>
#include "setting.h"
#define true 1
#define false 0

typedef enum { AND, OR } nodeType_t;
typedef enum { TRUE=1, FALSE=0, UNKNOWN=2 } nodeValue_t;
typedef enum { RED=0, BLUE=1 } color;
#define otherColor( x ) (((x)+1)%2)

typedef struct node node_t;

typedef struct linkedListNode ll_t;
struct linkedListNode{
	node_t* data;
	ll_t* next;
	ll_t* prev;
};

typedef struct {
	ll_t * first;
	ll_t * last;
	ll_t * current;
} ll2_t;

typedef struct {
	u64 graph[2];
	u32 hash;
} graph_t;
	
struct node{
	graph_t graph;
	
	u32 proof;
	u32 disproof;

	u8 data;// bity:
	           // 0 expanded
		   // 1 type
		   // 2,3 value
//	u8 expanded:1;	u8 type:1;	u8 value:2;
#ifdef NODEDELETE
	u8 current:1;
	u8 currentChild:1;
#endif //NODEDELETE

	u8 last_i;
        u8 last_j;

	u8 turn; //kolikaty je to tah = kolik je prave obarveno hran

	u8 childrenN;
	u32 parentsN;
	u32 parentsMAX;
	graph_t * children;
	graph_t * parents;

#ifdef STATS
	u32 set_stats; //kolikrat byl na tento node pusten setProofAndDisproofNubers
#endif //stats

#ifdef UPDATEANCESTORS2
	u32 update;
#endif //UPDATEANCESTORS2
#ifdef DFPN
	u32 thProof; //zarazky
	u32 thDisproof;
#endif //DFPN
};

extern u32 hashNumbers[2][N][N]; //pro kazdou parvu a hranuo

#endif
