#ifndef NODEFCE_H
#define NODEFCE_H

#include <stdlib.h>
#include "struct.h"
#include "linkedlist.h"
#include "print.h"
#include "pnsearch.h"
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

//-------------NODE---------------GRAPH------------------
extern u64 N1s; //0000..00000000000000011111
extern u64 R1s; //0000..00100001000100010001

static inline void nodeSetEdge(node_t * node, int i, int j, color color){
	node->graph[color] |= 1ULL<<(i*N+j);
	node->graph[color] |= 1ULL<<(j*N+i);
	node->hash ^= hashNumbers[color][i][j];
}

static inline u32 nodeNeighbour(node_t * node, int i, color color){
	//vrati masku kde je 1 tam kde vede hrana
	return (node->graph[color] >> (i*N)) & N1s; 
}
static inline void nodeCopyGraph(node_t * to, node_t * from){
	to->graph[0] = from->graph[0];
	to->graph[1] = from->graph[1];
	to->hash = from->hash;
}
static inline void nodeEmptyGraph(node_t * node){
	node->graph[0] = 0ULL;
	node->graph[1] = 0ULL;
	node->hash = 0;
}
static inline int nodeEdgeExist(node_t * node, int i, int j, color c){
	return (!!(node->graph[c] & (1ULL<<(i*N+j))));
}
static inline int compareGraph(node_t * a, node_t * b){
	return a->graph[0] == b->graph[0] && a->graph[1] == b->graph[1];
}
static inline void nodeChangeNodes(node_t * node, int a, int b){
	for (int c = 0; c < 2; c++){
		//zahodim vahy starych hran
		node->hash ^= hashNumbers2[c][a][nodeNeighbour(node,a,c)];
		node->hash ^= hashNumbers2[c][b][nodeNeighbour(node,b,c)];

		//vezmu radky a b
		u64 sa = ( node->graph[c] & (N1s<<(a*N)) ); 
		u64 sb = ( node->graph[c] & (N1s<<(b*N)) ); 
		//odstranim radky a b
		node->graph[c] =  node->graph[c] ^ sa; 
		node->graph[c] =  node->graph[c] ^ sb; 
		//pridam radky prehozene
		node->graph[c] =  node->graph[c] ^ ((sa>>(a*N))<<(b*N)); 
		node->graph[c] =  node->graph[c] ^ ((sb>>(b*N))<<(a*N));
		//vezmu sloupce a b
		u64 ra = ( node->graph[c] & (R1s<<a) ); 
		u64 rb = ( node->graph[c] & (R1s<<b) ); 
		//odstranim sloupce a b
		node->graph[c] =  node->graph[c] ^ ra; 
		node->graph[c] =  node->graph[c] ^ rb; 
		//pridam sloupce prehozene
		node->graph[c] =  node->graph[c] ^ ((ra>>a)<<b); 
		node->graph[c] =  node->graph[c] ^ ((rb>>b)<<a); 

		//pridam vahy hran
		node->hash ^= hashNumbers2[c][a][nodeNeighbour(node,a,c)];
		node->hash ^= hashNumbers2[c][b][nodeNeighbour(node,b,c)];
	}
}

#ifdef DEBUG
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
#endif

//-------------NODE---------------DATA------------------
static inline u8 setBit(u8 data, u8 bit, u8 value){
	return (data ^ (data & (1<<bit))) | (value<<bit);
}
static inline u8 getBit(u8 data, u8 bit){
	return !!(data & (1<<bit));
}

//expanded
static inline u32 nodeExpanded(node_t * node){
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

//-------------NODE---------------OTHER------------------
//turn
static inline u32 nodeTurn(node_t * node){
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
/*#ifdef DEBUG
	if (proof > MAXPROOF){
		printf("moc %d %d \n",proof, MAXPROOF);
		printNode(node);
//		printChilds(node);
	}
	node->proof2 = proof;
#else
*/	node->proof2 = MIN( proof, MAXPROOF);
//#endif
}

//disproof
static inline u32 nodeDisproof(node_t * node){
	return node->disproof;
}
static inline void nodeSetDisproof(node_t * node, u32 disproof){
	node->disproof = MIN( disproof, MAXPROOF);
}

//-------------NODE---------------DELETE------------------
static inline void nodeDelete(node_t* node){ //nestara se o mazani deti ani rodicu
	if (nodeExpanded(node))
		free(node->childs);
#ifdef DEBUG
	if (!ll2Empty(&node->parents))
		perror("tohle by se nemelo mazat");
#endif //DEBUG
	free(node);
}


#endif
