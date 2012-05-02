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
	u32 proof2;
	u32 disproof;

	u8 data;// bity:
	           // 0 expanded
		   // 1 type
		   // 2,3 value
//	u8 expanded:1;	u8 type:1;	u8 value:2;

	u8 last_i;
        u8 last_j;

	u8 turn; //kolikaty je to tah = kolik je prave obarveno hran

	u32 hash;

	ll2_t parents;
	ll2_t children;

	u8 childrenN;
	graph_t * children2;

#ifdef STATS
	u32 set_stats; //kolikrat byl na tento node pusten setProofAndDisproofNubers
#endif //stats

#ifdef UPDATE2
	u32 update;
	u32 thProof; //zarazky
	u32 thDisproof;
#endif //UPDATE2
	u64 graph[2];
};

extern u32 hashNumbers[2][N][N]; //pro kazdou parvu a hranuo

#endif
