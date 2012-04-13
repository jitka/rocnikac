#ifndef GRAPHFCE_H
#define GRAPHFCE_H

#include "tabs.h"

#ifdef DEBUG
#include "print.h"
#endif //DEBUG

//todo je jen prodlouzeni nodefce.h
//jsou tu implementovane fce pro praci s grafem tak aby 
//nikdo mimo nodefce.h nemusel znat spusob ulozeni

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

#ifdef PERM 

static inline u32 nodeNeighbour(node_t * node, int i, color color){
	//vrati masku kde je 1 tam kde vede hrana
	return (node->graph[color] >> ((node->perm[i])*N)) & N1s; 
}

static inline bool nodeColorEdgeExist(node_t * node, int i, int j, color c){
	return (!!(node->graph[c] & (1ULL<<((node->perm[i])*N+(node->perm[j])))));
}

static inline bool nodeEdgeExist(node_t * node, int i, int j){
	return nodeColorEdgeExist(node,i,j,RED) || nodeColorEdgeExist(node,i,j,BLUE);
}

static inline void nodeSetEdge(node_t * node, int i, int j, color color){
	node->graph[color] |= 1ULL<<((node->perm[i])*N+(node->perm[j]));
	node->graph[color] |= 1ULL<<((node->perm[j])*N+(node->perm[i]));
	node->hash ^= hashNumbers[color][node->perm[i]][node->perm[j]];
}

static inline int nodeDegree(node_t * node, int i, color c){
	return count[ nodeNeighbour(node,(node->perm[i]),c) ];
}

static inline void nodeChangeNodes(node_t * node, int a, int b){
	u8 tmp = node->perm[a];
	node->perm[a] = node->perm[b];
	node->perm[b] = tmp;
}

static inline bool testK4(node_t * node, int i, int j, color color){
	//otestuje jestli po pridani hrany ij nevznikla K4
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana
	tr = nodeNeighbour(node,(node->perm[i]),color) & nodeNeighbour(node,node->perm[j],color);
	for (int s = 0; s < N; s++)
		if (tr & (1<<s))
			for (int t = 0; t < N; t++)
				if (tr & (1<<t)){
					//staci overit jestli mezi s a t vede hrana
					if ( nodeEdgeExist( node, s, t, color) )
						return true;

				}
	return false;
}

#else //PERM

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
			for (int t = 0; t < N; t++)
				if (tr & (1<<t)){
					//staci overit jestli mezi s a t vede hrana
					if ( nodeColorEdgeExist( node, s, t, color) )
						return true;

				}
	return false;
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
			for (int t = 0; t < N; t++)
				if (tr & (1<<t)){
					// i, j, t jsou troj
					if ( !nodeEdgeExist( node, s, t) )
						//s t je hrozba
						return true;

				}
	//troj na tron
	for (int s = 0; s < N; s++)
		if (tr & (1<<s)){
			//i,j,s jsou trojuhelnik
			u32 tr2 = nodeNeighbour(node,i,color) & nodeNeighbour(node,s,color);
			for (int t = 0; t < N; t++)
				if (tr2 & (1<<t)){
					//i,s,t jsou trojhulenik
					if ( !nodeEdgeExist( node, j, t) ){
						//j t je hrozba
						return true;
					}
				}
			u32 tr3 = nodeNeighbour(node,j,color) & nodeNeighbour(node,s,color);
			for (int t = 0; t < N; t++)
				if (tr3 & (1<<t)){
					//j,s,t jsou trojhulenik
					if ( !nodeEdgeExist( node, i, t) ){
						//i t je hrozba
						return true;
					}
				}
		}
	return false;
}

/*
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
#endif
*/
#endif //PERM

#endif //GRAPHFECE_H
