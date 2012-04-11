#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "setting.h"
#include "pnsearch.h"
#include "print.h"
#include "linkedlist.h"
#include "nodefce.h"
#include "graphfce.h"
#include "norm.h"

// or node... na tahu je prvni hrac

//--------------------------CACHE-----------------------

u32 hashNumbers[2][N][N]; //pro kazdou barvu a hranu
u32 hashNumbers2[2][N][1<<N]; //pro kazdou barvu, vrchol a _mensi_ sousedy
node_t* cache[CACHE_SIZE];
int cacheMiss = 0;
u64 N1s;
u64 R1s;

void hashInit(){
	for (int i = 0; i < N; i++){
		for (int j = 0; j < i; j++){
			hashNumbers[0][i][j] = random() % CACHE_SIZE;
			hashNumbers[0][j][i] = hashNumbers[0][i][j];
			hashNumbers[1][i][j] = random() % CACHE_SIZE;
			hashNumbers[1][j][i] = hashNumbers[1][i][j];
		}
	}
	for (int i = 0; i < N; i++){
		for (u32 s = 0; s < (1<<N); s++){
			int hash0 = 0;
			int hash1 = 0;
			for (int j = 0; j < i; j++){
				if ( s & (1<<j) ){
					hash0 ^= hashNumbers[0][i][j];
					hash1 ^= hashNumbers[1][i][j];
				}
			}
			hashNumbers2[0][i][s] = hash0;
			hashNumbers2[1][i][s] = hash1;
		}
	}
	N1s = ((1ULL<<N)-1ULL);
	R1s = 0;
	for (int i = 0; i < N; i++){
		R1s ^= (1<<i*N);
	}
}

static inline void cacheInsert(node_t* node){
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] != NULL)
			continue;
		cache[where] = node;
		return;
	}
	cacheMiss++;
	printf("neni kam dat %d\n",nodeHash(node));
}

static inline node_t* cacheFind(node_t* node){ //vrati ukazatel na stejny graf nebo NULL pokud tam neni
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		if ( compareGraph( cache[where], node) )
			return cache[where];
	}
	return NULL;
}

static inline void cacheDelete(node_t* node){
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		if ( compareGraph( cache[where], node) ){
			cache[where] = NULL;
			return;
		}
	}
	perror("neni v cachy");
}
//--------------------------PN-SEARCH-----------------------
int numberOfNodes = 1; //abych vedela kolik zeru pameti
ll2_t currentPath;

static inline void deleteChild(node_t* node){
	if (nodeExpanded(node)){	
		while ( ! ll2Empty(&node->childs) ){
			node_t* child = ll2FirstNode(&node->childs);
			ll2DelFirst(&node->childs);

			ll2Delete(&(child->parents),node);
			if ( ll2Empty( &child->parents ) ){
				deleteChild(child);
				cacheDelete(child);
				nodeDelete(child);
				numberOfNodes--;
			}
		}
		nodeSetExpanded(node,false);
	}
}

static inline void setProofAndDisproofNubers(node_t* node){

	switch (nodeValue(node)) {
	case UNKNOWN:
		if (nodeExpanded(node)){
			u32 min = MAXPROOF;
#ifdef WEAK
			u32 max = 0;
#else //WEAK
			u32 sum = 0;
#endif //WEAK
			switch (nodeType(node)) {
			case OR:
				ll2FStart(&node->childs); 
				for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
					min = MIN(min,nodeProof(child));
				}
				nodeSetProof( node, min);
#ifdef WEAK
				ll2FStart(&node->childs); 
				for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
					max = MAX(max,nodeDisproof(childs));
				}
				nodeSetDisproof( node, max+nodeChildsN(node)-1);

#else //WEAK
				ll2FStart(&node->childs); 
				for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
					sum += nodeDisproof(child);
				}
				nodeSetDisproof( node, sum);
#endif //WEAK
				break;
			case AND:
#ifdef WEAK
				ll2FStart(&node->childs); 
				for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
					max = MAX(max,nodeProof(child));
				}
				nodeSetProof( node, max+nodeChildsN(node)-1);
#else //WEAK
				ll2FStart(&node->childs); 
				for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
					sum += nodeProof(child);
				}
				nodeSetProof( node, sum);
#endif //WEAK

				ll2FStart(&node->childs); 
				for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
					min = MIN( min, nodeDisproof(child) );
				}
				nodeSetDisproof( node, min);
			
				break;
			}
			if (nodeProof(node) == 0){
				nodeSetValue(node, TRUE);
				nodeSetDisproof(node,MAXPROOF);
				deleteChild(node);
#ifdef DEBUG
				if (nodeExpanded(node))
					perror("au1");
#endif //DEBUG
			} 
			if (nodeDisproof(node) == 0){
				nodeSetValue(node, FALSE);
				nodeSetProof(node,MAXPROOF);
				deleteChild(node);
#ifdef DEBUG
				if (nodeExpanded(node))
					perror("au2");
#endif //DEBUG
			}
		} else {
			nodeSetProof(node,1);
			nodeSetDisproof(node,1);
		}
		break;
	case TRUE:
		nodeSetProof(node,0);
		nodeSetDisproof(node,MAXPROOF);
		break;
	case FALSE:
		nodeSetProof(node,MAXPROOF);
		nodeSetDisproof(node,0);
		break;
	}
}

static inline int testK4(node_t * node, int i, int j, color color){
	//otestuje jestli po pridani hrany ij nevznikla K4
	u32 tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana
	tr = nodeNeighbour(node,i,color) & nodeNeighbour(node,j,color);
	for (int s = 0; s < N; s++)
		if (tr & (1<<s))
			for (int t = 0; t < N; t++)
				if (tr & (1<<t)){
					//staci overit jestli mezi s a t vede hrana
					if ( nodeNeighbour(node,s,color) & (1<<t) )
						return true;

				}
	return false;
}

static inline node_t* createChild(node_t* node, int i, int j){
	//vytvori potomka obarvenim hrany i,j
	node_t* child = nodeNew();

	nodeSetTurn(child, nodeTurn(node)+1 );

	switch (nodeType(node)) {
	case OR: //hraje prvni hrac
		nodeSetType(child, AND);
		nodeCopyGraph(child,node);
		nodeSetEdge(child,i,j,RED);
		//nevyhral prvni hrac?
		if (testK4(node,i,j,0)){
			nodeSetValue(child, TRUE);
		} else if ( nodeTurn(child) == (N*(N-1))/2 ){
			nodeSetValue(child, FALSE);
		} else {
			nodeSetValue(child, UNKNOWN);
		}
		break;
	case AND: //hraje druhy
		nodeSetType(child, OR);
		nodeCopyGraph(child,node);
		nodeSetEdge(child,i,j,BLUE);
		//neprohral prvni hrac?
		if (testK4(node,i,j,1)){
			nodeSetValue(child, FALSE);
		} else if ( nodeTurn(child) == (N*(N-1))/2 ){
			nodeSetValue(child, FALSE);
		} else {
			nodeSetValue(child, UNKNOWN);
		}
		break;
	}

//	if (nodeTurn(child) == 10)
//		norm(child);

	node_t* n = cacheFind(child);
	if ( n != NULL ) { //je v cachy?
		ll2AddNodeEnd( &n->parents, node);
		nodeDelete(child);
		return n;
	} else {
		ll2AddNodeEnd( &child->parents, node);
		numberOfNodes++;
		cacheInsert(child);
		return child;
	}
}

static inline void developNode(node_t* node){
	//vytvori a ohodnoti potomky
	//TODO otestovat vypis viteztvi+proher, zbytek hotovo

#ifdef DEBUG
	if (nodeExpanded(node))
		perror("uz je");
#endif //DEBUG

	for (int i = 0; i < N; i++)
		for (int j = 0; j < i; j++)
			if ( ! ( nodeEdgeExist(node, i, j, 0) || nodeEdgeExist(node, i, j, 1) ) ){ 
				//ij je hrana ktera jeste nema barvu
				node_t* child =  createChild(node,i,j);
				setProofAndDisproofNubers( child );    
				ll2AddNodeBegin( &node->childs, child );
			}
	nodeSetExpanded(node,true);
}

static inline void updateAncestors(){ //po hladinach

	ll2_t ancestors;
       	ll2New(&ancestors);
	ll2AddNodeEnd( &ancestors, ll2FirstNode(&currentPath));

	while ( ! ll2Empty(&ancestors) ){
	
		node_t* node = ll2FirstNode(&ancestors);
		ll2DelFirst(&ancestors);

		u32 oldProof = nodeProof(node);
		u32 oldDisproof = nodeDisproof(node);

		setProofAndDisproofNubers(node);

		int changed = (oldProof != nodeProof(node)) || (oldDisproof != nodeDisproof(node));
		if (!changed)
			continue;

		//pridat vsechny predky co je potreba updatetovat
		ll2AddNodesEnd( &ancestors, &node->parents);

		//jit vzhuru po linii pokud nejsem na konci
		node_t* previousNode = ll2FirstNode(&currentPath);
		ll2DelFirst(&currentPath);
		if ( ll2FirstNode(&currentPath) == NULL){
			ll2AddNodeBegin(&currentPath,previousNode);
		}

	}
}

static inline void selectMostProving(){
	node_t * node = ll2FirstNode(&currentPath);
	int tmp=0;
	while (nodeExpanded(node)){
		tmp++;
#ifdef DEBUG
		if (nodeValue(node)!=UNKNOWN){
			printf("tady ne %d\n",tmp);
			printNode(node);
		}
		if (nodeProof(node) == 0 || nodeDisproof(node) == 0){
			printf("tady nee\n");
		}
		u32 turn = nodeTurn(node);
#endif //DEBUG
		switch (nodeType(node)) {
		case OR: 
			ll2FStart(&node->childs); 
			for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
				if ( nodeProof(node) == nodeProof(child) ){
					node = child;
					break;
				}
			}
			break;
		case AND: 
			ll2FStart(&node->childs); 
			for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
				if ( nodeDisproof(node) == nodeDisproof(child) ){
					node = child;
					break;
				}
			}
			break;
		}
#ifdef DEBUG
		if (turn == nodeTurn(node)){
			printf("minimalni (dis)proof numer neni\n");
		}
#endif //DEBUG
		ll2AddNodeBegin(&currentPath,node);
	}
}

nodeValue_t proofNuberSearch(node_t* root){

	ll2New(&currentPath);
	ll2AddNodeBegin(&currentPath,root);

#ifdef DEBUG
	int counter = 0;
#endif //DEBUG
	while (nodeProof(root) > 0 && nodeDisproof(root) > 0 && numberOfNodes < MAXNODES ){
	
		selectMostProving();
		node_t* mostProovingNode = ll2FirstNode(&currentPath);
		
		developNode(mostProovingNode);

		updateAncestors(); 
	
#ifdef DEBUG
		counter++;
		//if (counter % 1000 == 0){
		//if (true){
		if (false){
			//printNode(mostProovingNode);
			printf("hotov node (%u) %u %u\n",nodeHash(mostProovingNode),nodeProof(mostProovingNode),nodeDisproof(mostProovingNode));
			printNode(mostProovingNode);
			printf("nodes %d ",numberOfNodes);
			printf("root %u %u\n",nodeProof(root),nodeDisproof(root));
			//printChilds(mostProovingNode);
		}
#endif //DEBUG
	}

//	printf("nodes %d\n",numberOfNodes);
//	printf("cache miss %d\n",cacheMiss);
//	extern int TMP;	printf("norm %d\n",TMP);


	return nodeValue(root);
}

