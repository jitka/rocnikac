#ifndef NODEFCE_H
#define NODEFCE_H

#include <stdlib.h>
#include "struct.h"
#include "linkedlist.h"
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

//-------------NODE---------------GRAPH------------------
static inline void nodeSetEdge(node_t * node, int i, int j, color color){
	node->graph[color] |= 1ULL<<(i*N+j);
	node->graph[color] |= 1ULL<<(j*N+i);
	node->hash ^= hashNumbers[color][i][j];
}
static inline uint nodeNeighbour(node_t * node, int i, color color){
	return (node->graph[color] >> (i*N)) & ((1ULL<<N)-1ULL); 
}
static inline void nodeCopyData(node_t * to, node_t * from){
	to->graph[0] = from->graph[0];
	to->graph[1] = from->graph[1];
	to->hash = from->hash;
}
static inline void nodeEmptyGraph(node_t * node){
	node->graph[0] = 0ULL;
	node->graph[1] = 0ULL;
	node->hash = 0;
}
static inline int nodeEdge(node_t * node, int i, int j){
	return (node->graph[0] & (1ULL<<(i*N+j)) || (node->graph[1] & (1ULL<<(i*N+j))));
}
static inline int nodeEdgeColor(node_t * node, int i, int j, color c){
	return node->graph[c] & (1ULL<<(i*N+j));
}
static inline int compareGraph(node_t * a, node_t * b){
	return a->graph[0] == b->graph[0] && a->graph[1] == b->graph[1];
}
static inline int nodeSimetric(node_t * a){
	for (int u = 0; u < N; u++){
		for (int v = 0; v < N; v++){
			for (int c = 0; c < 2; c++){
				if ( (!!(a->graph[c] & (1ULL<<(u*N+v)))) != (!!(a->graph[c] & (1ULL<<(v*N+u)))) ){
//					printf("%d %d %d %llu %llu \n",u,v,c,a->graph[c] & (1ULL<<(u*N+v)), a->graph[c] & (1ULL<<(v*N+u)) );
					return false;
				}
			}
		}
	}
	return true;
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

//proof
static inline uint nodeProof(node_t * node){
	return node->proof2;
}
static inline void nodeSetProof(node_t * node, uint proof){
	node->proof2 = MIN( proof, MAXPROOF);
}

//disproof
static inline uint nodeDisproof(node_t * node){
	return node->disproof;
}
static inline void nodeSetDisproof(node_t * node, uint disproof){
	node->disproof = MIN( disproof, MAXPROOF);
}

//-------------NODE---------------DELETE------------------
static inline void nodeDelete(node_t* node){ //nestara se o mazani deti ani rodicu
	if (nodeExpanded(node))
		free(node->childs);
	llDeletell(node->parents); //zbytecne tohle se vola jen kdyz rodice nejsou
	free(node);
}


#endif
