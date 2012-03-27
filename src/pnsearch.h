#ifndef PNSEARCH_H
#define PNSEARCH_H

#include <limits.h>
#define uint unsigned int
#define ull unsigned long long int 
#define MAXPROOF (INT_MAX/1000)
#define true 1
#define false 0
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define N 5 

typedef enum { AND, OR } nodeType_t;
typedef enum { TRUE=1, FALSE=0, UNKNOWN=2 } nodeValue_t;

typedef struct node node_t;
struct node{
	uint proof;
	uint disproof;

	int expanded2;
	int turn2; //kolikaty je to tah = kolik je prave obarveno hran
	nodeType_t type;
	nodeValue_t value;
	uint childsNumber;

	uint hash;

	node_t** childs;
	node_t* parent;

	ull data[N*2];
};

void proofNuberSearch(node_t* node);
void hashInit();

//--------------------------NODE----------------------

static inline nodeType_t nodeType(node_t * node){
	return node->type;
}
static inline void nodeSetType(node_t * node, nodeType_t type){
	node->type = type;
}

static inline nodeValue_t nodeValue(node_t * node){
	return node->value;
}
static inline void nodeSetValue(node_t * node, nodeValue_t value){
	node->value = value;
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
