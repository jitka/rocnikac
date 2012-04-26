#ifndef NODEFCE_H
#define NODEFCE_H

#include <stdlib.h>
#include "struct.h"
#include "linkedlist.h"
#include "tabs.h"

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
static inline void testK4andFreeK4(node_t * node, int * freeK4, bool * fullK4);
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


static inline void nodeEmptyGraph(node_t * node){
	node->graph[0] = 0ULL;
	node->graph[1] = 0ULL;
	node->hash = 0;
}

static inline void nodeCopyGraph(node_t * to, node_t * from){
	to->graph[0] = from->graph[0];
	to->graph[1] = from->graph[1];
	to->hash = from->hash;
}

static inline bool compareGraph(node_t * a, node_t * b){
	return a->graph[0] == b->graph[0] && a->graph[1] == b->graph[1];
}
static inline u32 nodeNeighbour(node_t * node, int i, color color){
	//vrati masku kde je 1 tam kde vede hrana
	return (node->graph[color] >> (i*N)) & N1s; 
}

static inline bool nodeColorEdgeExist(node_t * node, int i, int j, color c){
	return (!!(node->graph[c] & (1ULL<<(i*N+j))));
}

static inline bool nodeEdgeExist(node_t * node, int i, int j){
	return nodeColorEdgeExist(node,i,j,RED) || nodeColorEdgeExist(node,i,j,BLUE);
}

static inline void nodeSetEdge(node_t * node, int i, int j, color color){
	node->graph[color] |= 1ULL<<(i*N+j);
	node->graph[color] |= 1ULL<<(j*N+i);
	node->hash ^= hashNumbers[color][i][j];
	node->last_i = i;
	node->last_j = j;
}

static inline void nodeLastEdge(node_t * node, int i, int j){
	node->last_i = i;
	node->last_j = j;
}

static inline int nodeDegree(node_t * node, int i, color c){
	return count[ nodeNeighbour(node,i,c) ];
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

static inline bool testK4(node_t * node, int i, int j, color color){
	//otestuje jestli po pridani hrany ij nevznikla K4
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana
	tr = nodeNeighbour(node,i,color) & nodeNeighbour(node,j,color);
	for (int s = 0; s < N; s++)
		if (tr & (1<<s))
			for (int t = s+1; t < N; t++)
				if (tr & (1<<t)){
					//staci overit jestli mezi s a t vede hrana
					if ( nodeColorEdgeExist( node, s, t, color) )
						return true;

				}
	return false;
}

static inline void testK4andFreeK4(node_t * node, int * freeK4, bool * fullK4){
	//otestuje jestli po pridani hrany ij nevznikla K4
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana

	*freeK4=0;
	int i = nodeLastEdgeI(node);
	int j = nodeLastEdgeJ(node);
	color color = (nodeTurn(node) % 2 == 1) ? RED : BLUE; //kterou barvou byl nakreslet posledni tah
	
	//trojuhelniky kam nevede souperova hrana
	tr = (~nodeNeighbour(node,i,otherColor(color))) & (~nodeNeighbour(node,j,otherColor(color))); 
	for (int s = 0; s < N; s++)
		if ((tr & (1<<s)) && s != i && s != j)
			for (int t = s+1; t < N; t++)
				if ((tr & (1<<t)) && t != i && t !=j ){
					//staci overit jestli mezi s a t nevede souperova hrana
					if ( !nodeColorEdgeExist( node, s, t, otherColor(color)) ){
//						printf("hrana %d %d\n", s, t);
						(*freeK4)++;
					}
				}
//		printf("%d %d %d\n",i,j,*freeK4);
//		printNode(node);


	//trojuhelniky kam vedou moje hrany
	tr = nodeNeighbour(node,i,color) & nodeNeighbour(node,j,color);
	for (int s = 0; s < N; s++)
		if ((tr & (1<<s)) && s != i && s != j)
			for (int t = s+1; t < N; t++)
				if ((tr & (1<<t)) && t != i && t !=j ){
					//staci overit jestli mezi s a t vede hrana
					if ( nodeColorEdgeExist( node, s, t, color) ){
						*fullK4 = true;
						return;
					}

				}
	*fullK4 = false;
	return;

}

static inline bool nodeThreat(node_t * node, int i, int j, color color){
	//otestuje jestli po pridani hrany ij nevznikla hrozba
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana
	tr = nodeNeighbour(node,i,color) & nodeNeighbour(node,j,color);
	//dva troj bez hrany
	for (int s = 0; s < N; s++)
		if (tr & (1<<s))
			// i, j, s jsou troj
			for (int t = s+1; t < N; t++)
				if (tr & (1<<t)){
					// i, j, t jsou troj
					if ( !nodeEdgeExist( node, s, t) ){
						//s t je hrozba
						return true;
					}

				}
	//troj na tron
	for (int s = 0; s < N; s++)
		if (tr & (1<<s)){
			//i,j,s jsou trojuhelnik
			u32 tr2 = nodeNeighbour(node,i,color) & nodeNeighbour(node,s,color);
			for (int t = s+1; t < N; t++)
				if ((tr2 & (1<<t)) && (t != j)){
					//i,s,t jsou trojhulenik
					if ( !nodeEdgeExist( node, j, t) ){
						//j t je hrozba
						return true;
					}
				}
			u32 tr3 = nodeNeighbour(node,j,color) & nodeNeighbour(node,s,color);
			for (int t = 0; t < N; t++)
				if ((tr3 & (1<<t)) && (t != i)){
					//j,s,t jsou trojhulenik
					if ( !nodeEdgeExist( node, i, t) ){
						//i t je hrozba
						return true;
					}
				}
		}
	return false;
}


#ifdef DEBUG
static inline bool nodeSimetric(node_t * a){
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

static inline bool nodeTurnChack(node_t * a){
	u8 turn = 0;
	for (int u = 0; u < N; u++){
		for (int v = u+1; v < N; v++){
			for (int c = 0; c < 2; c++){
				if ( nodeColorEdgeExist(a,u,v,c) ){
					turn++;
				}
			}
		}
	}
//	printf("turn %d %d\n",turn,a->turn);
	return turn == nodeTurn(a);

}
#endif



#endif

