#ifndef NODEFCE_H
#define NODEFCE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "struct.h"
#include "tabs.h"
#include "cache.h"

#include <stdio.h>
#include "print.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static inline node_t* nodeNew(u8 turn);
static inline void nodeDelete(node_t * node);

//-------------NODE---------------GRAPH------------------ 
static inline void graphEmpty(graph_t * graph);
static inline void graphCopy(graph_t * to, graph_t * from);
static inline bool graphIdentical(graph_t * a, graph_t * b);
static inline bool graphCompare(graph_t * a, graph_t * b); //vrati true pokud je a alfabeticky mensi nez b
static inline u32 graphHash(graph_t * graph);

static inline bool graphColorEdgeExist(graph_t * graph, int i, int j, color c);
static inline bool graphEdgeExist(graph_t * graph, int i, int j);
static inline void graphSetEdge(graph_t * graph, int i, int j, color color);
static inline int graphDegree(graph_t * graph, int i, color c);

static inline bool graphTestK4(graph_t * graph, int i, int j, color color);
static inline void graphTestK4andFreeK4(graph_t * graph, int i, int j, color color, int * freeK4, bool * fullK4);
static inline bool graphThreat(graph_t * graph, int i, int j, color color);
//static inline void nodeChangeNodes(node_t * node, int a, int b); //na rychlejsi norm
//static inline bool nodeSimetric(node_t * a);
//static inline bool nodeTurnChack(node_t * a);

//-------------NODE---------------DATA------------------
static inline bool nodeTestK4(node_t * node);
static inline void nodeTestK4andFreeK4(node_t * node,int * freeK4, bool * fullK4);

static inline u8 nodeChildrenN(node_t * node);
static inline void nodeSetChildrenN(node_t * node, u8 childrenN);
static inline void nodeAddChild(node_t * node, graph_t * child);
static inline u8 nodeParentsN(node_t * node);
static inline void nodeSetParentN(node_t * node, u32 parentsN);
static inline void nodeAddParent(node_t * node, graph_t * parent);

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

static inline void nodeSetLastEdge(node_t * node, int i, int j); //prehodily se souradnice posledni hrany
static inline u8 nodeLastEdgeI(node_t *node);
static inline u8 nodeLastEdgeJ(node_t *node);

#ifdef UPDATEANCESTORS2
static inline void nodeUpdate(node_t * node, u32 update);
static inline bool nodeUpdated(node_t * node, u32 update);
#endif //UPDATEANCESTORS2

#ifdef DFPN
static inline void nodeSetTh(node_t * node, u32 thProof, u32 thDisproof);
static inline u32 nodeThProof(node_t * node);
static inline u32 nodeThDisproof(node_t * node);
#endif //DFPN

#ifdef NODEDELETE
static inline void nodeSetCurrent(node_t *node); //oznacuje vrcholy v current path, ktere nemohou byt smazane
static inline void nodeUnsetCurrent(node_t *node);
static inline bool nodeCurrent(node_t *node); 
static inline void nodeSetCurrentChild(node_t *node); //oznacuje syny ktere se nemuzou smazat kvuli repair
static inline void nodeUnsetCurrentChild(node_t *node);
static inline void nodeSetCurrentChild(node_t *node);
#endif //NODEDELETE

static inline graph_t* nodeGraph(node_t *node);
//-------------------------------------------------------------


//--------------------------------------------------------------
// samotne fce bordel
//--------------------------------------------------------------

static inline bool nodeTestK4(node_t * node){
	return graphTestK4( &node->graph, node->last_i, node->last_j, (node->turn%2==1)?RED:BLUE);
}

static inline void nodeTestK4andFreeK4(node_t * node, int * freeK4, bool * fullK4){
	graphTestK4andFreeK4( &node->graph, node->last_i, node->last_j, (node->turn%2==1)?RED:BLUE, freeK4, fullK4);
}

#ifdef NODEDELETE
static inline void nodeSetCurrent(node_t *node){
	node->current = true;
}
static inline void nodeUnsetCurrent(node_t *node){
	node->current = false;
}
static inline bool nodeCurrent(node_t *node){ 
	return node->current;
}

static inline void nodeSetCurrentChild(node_t *node){
	node->currentChild = true;
}
static inline void nodeUnsetCurrentChild(node_t *node){
	assert(node != NULL);
	node->currentChild = false;
}
static inline bool nodeCurrentChild(node_t *node){ 
	return node->currentChild;
}
#endif //NODEDELETE

static inline graph_t* nodeGraph(node_t *node){
	return &(node->graph);
}
static inline void nodeSetLastEdge(node_t * node, int i, int j){
	node->last_i = i;
	node->last_j = j;
}
static inline u8 nodeLastEdgeI(node_t *node){
	if( nodeTurn(node) == 0 )
		perror("neni predchozi tah");
	return node->last_i;
}
static inline u8 nodeLastEdgeJ(node_t *node){
	if( nodeTurn(node) == 0 )
		perror("neni predchozi tah");
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
	return node->graph.hash;
}

//proof
static inline u32 nodeProof(node_t * node){
	return node->proof;
}
static inline void nodeSetProof(node_t * node, u32 proof){
	if (proof > MAXPROOF)
		perror("velky proof");
	node->proof = proof;
}

//disproof
static inline u32 nodeDisproof(node_t * node){
	return node->disproof;
}
static inline void nodeSetDisproof(node_t * node, u32 disproof){
	if (disproof > MAXPROOF)
		perror("velky disproof");
	node->disproof = disproof;
}


#ifdef UPDATEANCESTORS2
static inline void nodeUpdate(node_t * node, u32 update){
	if (update > MAXPROOF)
		perror("moc update, pretece");
	node->update = update;
}

static inline bool nodeUpdated(node_t * node, u32 update){
	return node->update == update;
}
#endif //UPDATEANCESTORS2
#ifdef DFPN
static inline void nodeSetTh(node_t * node, u32 thProof, u32 thDisproof){
	node->thProof = thProof;
	node->thDisproof = thDisproof;
}

static inline u32 nodeThProof(node_t * node){
	return node->thProof;
}

static inline u32 nodeThDisproof(node_t * node){
	return node->thDisproof;
}
#endif //DFPN


//new
extern int numberOfNodes; //abych vedela kolik zeru pameti
static inline node_t* nodeNew(u8 turn){
	node_t* node = malloc(sizeof(node_t));
	graph_t * parents2 = malloc( sizeof(graph_t) * 2 );
	graph_t * children2 = malloc( sizeof(graph_t) * MAXCHILD(turn) );
	numberOfNodes++;
	if (node == NULL || parents2 == NULL || children2 == NULL)
		perror("malloc node");

#ifdef STATS
	node->set_stats = 0;
#endif //STATS
#ifdef DFPN
	node->update = 0;
#endif //DFPN

	node->childrenN = 0;
	node->parentsN = 0;
	node->parentsMAX = 2;
	node->parents = parents2;
	node->children = children2;
#ifdef NODEDELETE
	node->current = false;
	node->currentChild = false;
#endif //NODEDELETE
	node->turn = turn;
	graphEmpty(&node->graph);

	nodeSetExpanded( node, false);

	return node;
}

static inline void nodeDelete(node_t * node){
	free(node->parents);
	free(node->children);
	free(node);
	numberOfNodes--;
}

static inline u8 nodeChildrenN(node_t * node){
	return node->childrenN;
}

static inline void nodeSetChildrenN(node_t * node, u8 childrenN){
	node->childrenN = childrenN;
}

static inline void nodeAddChild(node_t * node, graph_t * child){
	assert( node->childrenN < MAXCHILD(nodeTurn(node)) );
	assert( child != NULL );
	assert( node != NULL );
	assert( node->children != NULL );
	memcpy( &node->children[node->childrenN++], child, sizeof(graph_t) );
}

static inline u8 nodeParentsN(node_t * node){
	return node->parentsN;
}
static inline void nodeSetParentN(node_t * node, u32 parentsN){
	node->parentsN = parentsN;
}
static inline void nodeAddParent(node_t * node, graph_t * parent){
#ifdef DELETEUSELESSPARENTS
	//smrsknu 
	int where=0; 
	bool exist = false;
	for (u32 i = 0; i < node->parentsN; i++){
		if ( cacheFind(&node->parents[i]) == NULL ) 
			continue;
		if ( graphIdentical(nodeGraph(cacheFind(&node->parents[i])), parent) ) 
			exist = true;
		node->parents[where++] = node->parents[i];
	}
	node->parentsN = where;
	if (exist){
		assert(node->parentsN > 0);
		return;
	}
#endif //DELETEUSELESSPARENTS
	//pripadne zvetsim pole
	if (node->parentsN >= node->parentsMAX){
		assert(node->parentsMAX < M*2); //kdyby bylo moc rodicu je to jen divne ne nutne blble
		node->parentsMAX *= 2;
		graph_t * parents = malloc( sizeof(graph_t) * node->parentsMAX);
		assert(parents != NULL);
		for (u32 i = 0; i < node->parentsN; i++){
			parents[i] = node->parents[i];
		}
		free(node->parents);
		node->parents = parents;
	}
	//pridam
	memcpy( &node->parents[node->parentsN++], parent, sizeof(graph_t) );
	assert(node->parentsN > 0);
}


static inline void graphEmpty(graph_t * graph){
	graph->graph[0] = 0ULL;
	graph->graph[1] = 0ULL;
	graph->hash = 0;
}

static inline void graphCopy(graph_t * to, graph_t * from){
	memcpy(to,from,sizeof(graph_t));
}

static inline u32 graphHash(graph_t * graph){
	return graph->hash;
}

static inline bool graphIdentical(graph_t * a, graph_t * b){
	assert( (a!=NULL) && (b!=NULL) );
	return ((a->graph[0] == b->graph[0]) && (a->graph[1] == b->graph[1]));
}

static inline bool graphCompare(graph_t * a, graph_t * b){
       	//vrati true pokud je a alfabeticky mensi nez b
	assert( (a!=NULL) && (b!=NULL) );
	if (a->hash <= b->hash)
		return true;
	if (a->graph[0] <= b->graph[0])
		return true;
	if (a->graph[1] <= b->graph[1])
		return true;
	return false;
}

static inline u32 graphNeighbour(graph_t * graph, int i, color color){
	//vrati masku kde je 1 tam kde vede hrana
	return (graph->graph[color] >> (i*N)) & N1s; 
}
static inline bool graphColorEdgeExist(graph_t * graph, int i, int j, color c){
	return (!!(graph->graph[c] & (1ULL<<(i*N+j))));
}

static inline bool graphEdgeExist(graph_t * graph, int i, int j){
	return graphColorEdgeExist(graph,i,j,RED) || graphColorEdgeExist(graph,i,j,BLUE);
}

static inline void graphSetEdge(graph_t * graph, int i, int j, color color){
	graph->graph[color] |= 1ULL<<(i*N+j);
	graph->graph[color] |= 1ULL<<(j*N+i);
	graph->hash ^= hashNumbers[color][i][j];
}


static inline int graphDegree(graph_t * graph, int i, color c){
	return count[ graphNeighbour(graph,i,c) ];
}
/*
static inline void nodeChangeNodes(node_t * node, int a, int b){
	for (int c = 0; c < 2; c++){
		//zahodim vahy starych hran
		node->graph.hash ^= hashNumbers2[c][a][nodeNeighbour(node,a,c)];
		node->graph.hash ^= hashNumbers2[c][b][nodeNeighbour(node,b,c)];

		//vezmu radky a b
		u64 sa = ( node->graph.graph[c] & (N1s<<(a*N)) ); 
		u64 sb = ( node->graph.graph[c] & (N1s<<(b*N)) ); 
		//odstranim radky a b
		node->graph.graph[c] =  node->graph.graph[c] ^ sa; 
		node->graph.graph[c] =  node->graph.graph[c] ^ sb; 
		//pridam radky prehozene
		node->graph.graph[c] =  node->graph.graph[c] ^ ((sa>>(a*N))<<(b*N)); 
		node->graph.graph[c] =  node->graph.graph[c] ^ ((sb>>(b*N))<<(a*N));
		//vezmu sloupce a b
		u64 ra = ( node->graph.graph[c] & (R1s<<a) ); 
		u64 rb = ( node->graph.graph[c] & (R1s<<b) ); 
		//odstranim sloupce a b
		node->graph.graph[c] =  node->graph.graph[c] ^ ra; 
		node->graph.graph[c] =  node->graph.graph[c] ^ rb; 
		//pridam sloupce prehozene
		node->graph.graph[c] =  node->graph.graph[c] ^ ((ra>>a)<<b); 
		node->graph.graph[c] =  node->graph.graph[c] ^ ((rb>>b)<<a); 

		//pridam vahy hran
		node->graph.hash ^= hashNumbers2[c][a][nodeNeighbour(node,a,c)];
		node->graph.hash ^= hashNumbers2[c][b][nodeNeighbour(node,b,c)];
	}
}
*/
static inline bool graphTestK4(graph_t * graph, int i, int j, color color){
	//otestuje jestli po pridani hrany ij nevznikla K4
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana
	tr = graphNeighbour(graph,i,color) & graphNeighbour(graph,j,color);
	for (int s = 0; s < N; s++)
		if (tr & (1<<s))
			for (int t = s+1; t < N; t++)
				if (tr & (1<<t)){
					//staci overit jestli mezi s a t vede hrana
					if ( graphColorEdgeExist( graph, s, t, color) )
						return true;

				}
	return false;
}

static inline void graphTestK4andFreeK4(graph_t * graph, int i, int j, color color, int * freeK4, bool * fullK4){
	//otestuje jestli po pridani hrany ij nevznikla K4
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana

	*freeK4=0;
	
	//trojuhelniky kam nevede souperova hrana
	tr = (~graphNeighbour(graph,i,otherColor(color))) & (~graphNeighbour(graph,j,otherColor(color))); 
	for (int s = 0; s < N; s++)
		if ((tr & (1<<s)) && s != i && s != j)
			for (int t = s+1; t < N; t++)
				if ((tr & (1<<t)) && t != i && t !=j ){
					//staci overit jestli mezi s a t nevede souperova hrana
					if ( !graphColorEdgeExist( graph, s, t, otherColor(color)) ){
						(*freeK4)++;
					}
				}

	//trojuhelniky kam vedou moje hrany
	tr = graphNeighbour(graph,i,color) & graphNeighbour(graph,j,color);
	for (int s = 0; s < N; s++)
		if ((tr & (1<<s)) && s != i && s != j)
			for (int t = s+1; t < N; t++)
				if ((tr & (1<<t)) && t != i && t !=j ){
					//staci overit jestli mezi s a t vede hrana
					if ( graphColorEdgeExist( graph, s, t, color) ){
						*fullK4 = true;
						return;
					}

				}
	*fullK4 = false;
	return;

}

static inline bool graphThreat(graph_t * graph, int i, int j, color color){
	//otestuje jestli po pridani hrany ij nevznikla hrozba
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana
	tr = graphNeighbour(graph,i,color) & graphNeighbour(graph,j,color);
	//dva troj bez hrany
	for (int s = 0; s < N; s++)
		if (tr & (1<<s))
			// i, j, s jsou troj
			for (int t = s+1; t < N; t++)
				if (tr & (1<<t)){
					// i, j, t jsou troj
					if ( !graphEdgeExist( graph, s, t) ){
						//s t je hrozba
						return true;
					}

				}
	//troj na tron
	for (int s = 0; s < N; s++)
		if (tr & (1<<s)){
			//i,j,s jsou trojuhelnik
			u32 tr2 = graphNeighbour(graph,i,color) & graphNeighbour(graph,s,color);
			for (int t = s+1; t < N; t++)
				if ((tr2 & (1<<t)) && (t != j)){
					//i,s,t jsou trojhulenik
					if ( !graphEdgeExist( graph, j, t) ){
						//j t je hrozba
						return true;
					}
				}
			u32 tr3 = graphNeighbour(graph,j,color) & graphNeighbour(graph,s,color);
			for (int t = 0; t < N; t++)
				if ((tr3 & (1<<t)) && (t != i)){
					//j,s,t jsou trojhulenik
					if ( !graphEdgeExist( graph, i, t) ){
						//i t je hrozba
						return true;
					}
				}
		}
	return false;
}

#endif

