#ifndef PNSEARCH_H
#define PNSEARCH_H

#include <limits.h>
#define uint unsigned int //32-bit
#define ull unsigned long long int //64-bit 
#define MAXPROOF (INT_MAX/1000)
#define true 1
#define false 0
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define N 5 

typedef enum { AND, OR } nodeType_t;
typedef enum { TRUE=1, FALSE=0, UNKNOWN=2 } nodeValue_t;
typedef enum { RED=0, BLUE=1 } color;

typedef struct node node_t;
struct node{
	uint proof;
	uint disproof;

	int expanded2;
	int turn2; //kolikaty je to tah = kolik je prave obarveno hran
	nodeType_t type2;
	nodeValue_t value2;
	uint childsNumber;

	uint hash;

	node_t** childs;
	node_t* parent;

	ull data2[N*2];
};

void proofNuberSearch(node_t* node);
void hashInit();

//--------------------------NODE----------------------

//data
static inline void nodeSetEdge(node_t * node, int i, int j, color color){
	node->data2[i+N*color] += 1<<j;
	node->data2[j+N*color] += 1<<i;
}
static inline uint nodeNeighbour(node_t * node, int i, color color){
	return node->data2[i+N*color]; 
}
static inline void nodeCopyData(node_t * to, node_t * from){
	for (int x = 0; x < N*2; x++){
		to->data2[x] = from->data2[x];
	}
}
static inline void nodeEmptyData(node_t * node){
	for (int i = 0; i < N*2; i++)
		node->data2[i] = 0;
}
static inline int nodeEdge(node_t * node, int i, int j){
	return (node->data2[i] & (1<<j)) || (node->data2[N+i] & (1<<j));
}

//type
static inline nodeType_t nodeType(node_t * node){
	return node->type2;
}
static inline void nodeSetType(node_t * node, nodeType_t type){
	node->type2 = type;
}

//value
static inline nodeValue_t nodeValue(node_t * node){
	return node->value2;
}
static inline void nodeSetValue(node_t * node, nodeValue_t value){
	node->value2 = value;
}

//expanded
static inline uint nodeExpanded(node_t * node){
	return node->expanded2;
}
static inline void nodeSetExpanded(node_t * node, uint expanded){
	node->expanded2 = expanded;
}

//turn
static inline uint nodeTurn(node_t * node){
	return node->turn2;
}
static inline void nodeSetTurn(node_t * node, uint turn){
	node->turn2 = turn;
}

//hash
static inline uint nodeHash(node_t * node){
	return node->hash;
}
static inline void nodeSetHash(node_t * node, uint hash){
	node->hash = hash;
}

//childsNumber
static inline uint nodeChildsN(node_t * node){
	return node->childsNumber;
}
static inline void nodeSetChildsN(node_t * node, uint v){
	node->childsNumber = v;
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
