#ifndef NODEFCE_H
#define NODEFCE_H

#include <stdlib.h>
#include "struct.h"
#include "linkedlist.h"
#include "graphfce.h"

#ifdef DEBUG
#include "print.h"
#endif //DEBUG

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static inline node_t* nodeNew();
static inline void nodeDelete(node_t* node);

//-------------NODE---------------GRAPH------------------
static inline void nodeEmptyGraph(node_t * node);
static inline void nodeCopyGraph(node_t * to, node_t * from);
static inline bool compareGraph(node_t * a, node_t * b);

static inline bool nodeColorEdgeExist(node_t * node, int i, int j, color c);
static inline bool nodeEdgeExist(node_t * node, int i, int j);
static inline void nodeSetEdge(node_t * node, int i, int j, color color);
static inline void nodeLastEdge(node_t * node, int i, int j); //prehodily se souradnice posledni hrany
static inline int nodeDegree(node_t * node, int i, color c);

static inline void nodeChangeNodes(node_t * node, int a, int b);
static inline bool testK4(node_t * node, int i, int j, color color);
static inline bool testK4andFreeK4(node_t * node, int i, int j, color color, int * freeK4);
static inline bool nodeThreat(node_t * node, int i, int j, color color);
#ifdef DEBUG
//static inline bool nodeSimetric(node_t * a);
static inline bool nodeTurnChack(node_t * a);
#endif //DEBUG

//-------------NODE---------------DATA------------------
static inline bool nodeExpanded(node_t * node);
static inline void nodeSetExpanded(node_t * node, u32 expanded);

static inline nodeType_t nodeType(node_t * node);
static inline void nodeSetType(node_t * node, nodeType_t type);

static inline nodeValue_t nodeValue(node_t * node);
static inline void nodeSetValue(node_t * node, nodeValue_t value);

static inline u8 nodeTurn(node_t * node);
static inline void nodeSetTurn(node_t * node, u32 turn);

static inline u32 nodeHash(node_t * node);

static inline u32 nodeProof(node_t * node);
static inline void nodeSetProof(node_t * node, u32 proof);

static inline u32 nodeDisproof(node_t * node);
static inline void nodeSetDisproof(node_t * node, u32 disproof);

static inline u8 nodeLastEdgeI(node_t *node);
static inline u8 nodeLastEdgeJ(node_t *node);

//-------------------------------------------------------------
static inline u8 nodeLastEdgeI(node_t *node){
#ifdef DEBUG
	if( nodeTurn(node) == 0 )
		perror("neni predchozi tah");
#endif
	return node->last_i;
}
static inline u8 nodeLastEdgeJ(node_t *node){
#ifdef DEBUG
	if( nodeTurn(node) == 0 )
		perror("neni predchozi tah");
#endif
	return node->last_j;
}

static inline u8 setBit(u8 data, u8 bit, u8 value){
	return (data ^ (data & (1<<bit))) | (value<<bit);
}
static inline u8 getBit(u8 data, u8 bit){
	return !!(data & (1<<bit));
}

//expanded
static inline bool nodeExpanded(node_t * node){
	return getBit(node->data,0);
}
static inline void nodeSetExpanded(node_t * node, u32 expanded){
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

//turn
static inline u8 nodeTurn(node_t * node){
	return node->turn;
}
static inline void nodeSetTurn(node_t * node, u32 turn){
	node->turn = turn;
}

//hash
static inline u32 nodeHash(node_t * node){
	return node->hash;
}

//proof
static inline u32 nodeProof(node_t * node){
	return node->proof2;
}
static inline void nodeSetProof(node_t * node, u32 proof){
#ifdef DEBUG
#endif //DEBUG
	node->proof2 = MIN( proof, MAXPROOF);
}

//disproof
static inline u32 nodeDisproof(node_t * node){
	return node->disproof;
}
static inline void nodeSetDisproof(node_t * node, u32 disproof){
	node->disproof = MIN( disproof, MAXPROOF);
}

//delete
static inline void nodeDelete(node_t* node){ //nestara se o mazani deti ani rodicu
#ifdef DEBUG
	if ( nodeExpanded(node) || (!ll2Empty(&node->parents)) || (!ll2Empty(&node->children)) ){
		printf("tohle by se nemelo mazat\n");
		printNode(node);
	}
#endif //DEBUG
	free(node);
}

//new
static inline node_t* nodeNew(){
	node_t* node = malloc(sizeof(node_t));
#ifdef DEBUG
	if (node == NULL)
		perror("malloc node");
#endif //DEBUG
	ll2New( &node->parents );
	ll2New( &node->children );

#ifdef STATS
	node->set_stats = 0;
#endif //STATS

	nodeSetExpanded( node, false);
	return node;
}

#endif

