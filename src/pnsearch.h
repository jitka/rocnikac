#ifndef PNSEARCH_H
#define PNSEARCH_H

#include <limits.h>
#define uchar unsigned int //8-bit
#define uint unsigned int //32-bit
#define ull unsigned long long int //64-bit 
#define MAXPROOF (INT_MAX/1000)
#define true 1
#define false 0
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define N 6 //funguje pro N 5..8 aby sla matice sousedu ulozit 

typedef enum { AND, OR } nodeType_t;
typedef enum { TRUE=1, FALSE=0, UNKNOWN=2 } nodeValue_t;
typedef enum { RED=0, BLUE=1 } color;

typedef struct node node_t;
struct node{
	uint proof;
	uint disproof;

	uchar data;// bity:
	           // 0 expanded
		   // 1 type
		   // 2,3 value

	uchar turn; //kolikaty je to tah = kolik je prave obarveno hran
	uchar childsNumber;
	uchar parentsNumber;

	uint hash;

	node_t** childs;
	node_t* parent;

	ull graph[2];
};

void proofNuberSearch(node_t* node);
void hashInit();

//-------------NODE---------------GRAPH------------------
static inline void nodeSetEdge(node_t * node, int i, int j, color color){
	node->graph[color] += 1<<(i*N+j);
	node->graph[color] += 1<<(j*N+i);
}
static inline uint nodeNeighbour(node_t * node, int i, color color){
	return (node->graph[color] >> (i*N)) & ((1<<N)-1); 
}
static inline void nodeCopyData(node_t * to, node_t * from){
	to->graph[0] = from->graph[0];
	to->graph[1] = from->graph[1];
}
static inline void nodeEmptyGraph(node_t * node){
	node->graph[0] = 0;
	node->graph[1] = 0;
}
static inline int nodeEdge(node_t * node, int i, int j){
	return (node->graph[0] & (1<<(i*N+j)) || (node->graph[1] & (1<<(i*N+j))));
}

//-------------NODE---------------DATA------------------
static inline uchar setBit(uchar data, uchar bit, uchar value){
	return (data ^ (data & (1<<bit))) | (value<<bit);
}
static inline uchar getBit(uchar data, uchar bit){
	return !!(data & (1<<bit));
}

//expanded
static inline uint nodeExpanded(node_t * node){
	return getBit(node->data,0);
}
static inline void nodeSetExpanded(node_t * node, uint expanded){
	node->data = setBit(node->data,0,expanded);
}

//type
static inline nodeType_t nodeType(node_t * node){
	if (getBit(node->data,1))
		return AND;
	else
		return OR;
}
static inline void nodeSetType(node_t * node, nodeType_t type){
	if (type == AND)
		node->data = setBit(node->data,1,1);
	else
		node->data = setBit(node->data,1,0);
}

//value
static inline nodeValue_t nodeValue(node_t * node){		
	if (getBit(node->data,3)) {
		return UNKNOWN;
	} else {
		if (getBit(node->data,2)) {
			return TRUE;
		} else {
			return FALSE;
		}
	}
}
static inline void nodeSetValue(node_t * node, nodeValue_t value){
	switch (value) {
	case UNKNOWN:
		node->data = setBit(node->data,2,0);
		node->data = setBit(node->data,3,1);
		break;
	case TRUE:
		node->data = setBit(node->data,2,1);
		node->data = setBit(node->data,3,0);
		break;
	case FALSE:
		node->data = setBit(node->data,2,0);
		node->data = setBit(node->data,3,0);
		break;
	}

}

//-------------NODE---------------CHILDS------------------
static inline uchar nodeChildsN(node_t * node){
	return node->childsNumber;
}
static inline void nodeSetChildsN(node_t * node, uchar v){
	node->childsNumber = v;
}

//-------------NODE---------------PARENTS------------------

//-------------NODE---------------OTHER------------------
//turn
static inline uchar nodeTurn(node_t * node){
	return node->turn;
}
static inline void nodeSetTurn(node_t * node, uint turn){
	node->turn = turn;
}

//hash
static inline uint nodeHash(node_t * node){
	return node->hash;
}
static inline void nodeSetHash(node_t * node, uint hash){
	node->hash = hash;
}

//proof
static inline uint nodeProof(node_t * node){
	return node->proof;
}
static inline void nodeSetProof(node_t * node, uint proof){
	node->proof = MIN( proof, MAXPROOF);
}

//disproof
static inline uint nodeDisproof(node_t * node){
	return node->disproof;
}
static inline void nodeSetDisproof(node_t * node, uint disproof){
	node->disproof = MIN( disproof, MAXPROOF);
}
#endif
