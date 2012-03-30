#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "pnsearch.h"
#include "print.h"
#include "linkedlist.h"
#include "nodefce.h"

// or node... na tahu je prvni hrac

//--------------------------CACHE-----------------------
#define CACHE_SIZE (1<<20)
#define CACHE_PATIENCE 100 //kolik pozic za mistem kam patri se jeste muze nalezat 

uint hashNumbers[2][N][N]; //pro kazdou barvu a hranu
node_t* cache[CACHE_SIZE];
int cacheMiss = 0;
void hashInit(){
	for (int i = 0; i < N; i++){
		for (int j = 0; j < i; j++){
			hashNumbers[0][i][j] = random() % CACHE_SIZE;
			hashNumbers[0][j][i] = hashNumbers[0][i][j];
			hashNumbers[1][i][j] = random() % CACHE_SIZE;
			hashNumbers[1][j][i] = hashNumbers[1][i][j];
		}
	}
}

void cacheInsert(node_t* node){
	for (uint i = 0; i < CACHE_PATIENCE; i++){
		uint where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] != NULL)
			continue;
		cache[where] = node;
//		printf("hash %d\n",nodeHash(node));
		return;
	}
	cacheMiss++;
}

node_t* cacheFind(node_t* node){ //vrati ukazatel na stejny graf nebo NULL pokud tam neni
	for (uint i = 0; i < CACHE_PATIENCE; i++){
		uint where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			return NULL;
		if ( compareGraph( cache[where], node) )
			return cache[where];
	}
	return NULL;
}

//--------------------------PN-SEARCH-----------------------
int numberOfNodes = 1; //abych vedela kolik zeru pameti
ll_t* currentPath;


static inline void deleteChild(node_t* node){
	//printf("hui\n");
	if (nodeExpanded(node)){
	//	printf("hui\n");
		for (uint i = 0; i < nodeChildsN(node); i++){
			node_t* child = node->childs[i];
			llDelete(&child->parents,node);
			if (child->parents == NULL){
				deleteChild(child);
				nodeDelete(node);
				numberOfNodes--;
			}
		}
	}
}

static inline void setProofAndDisproofNubers(node_t* node){
//nejdriv overit jestli neni value true nebo false
//pak pocitat ze synu
//a pak dat 1 1
	switch (nodeValue(node)) {
	case UNKNOWN:
		if (nodeExpanded(node)){
			uint min = MAXPROOF;
			uint sum = 0;
			switch (nodeType(node)) {
			case OR:
				for (uint i = 0; i < nodeChildsN(node); i++){
					min = MIN(min,nodeProof(node->childs[i]));
				}
				nodeSetProof( node, min);

				for (uint i = 0; i < nodeChildsN(node); i++){
					sum += nodeDisproof(node->childs[i]);
				}
				nodeSetDisproof( node, sum);
				break;

			case AND:
				for (uint i = 0; i < nodeChildsN(node); i++){
					sum += nodeProof(node->childs[i]);
				}
				nodeSetProof( node, sum);

				for (uint i = 0; i < nodeChildsN(node); i++){
					min = MIN(min,nodeDisproof(node->childs[i]));
				}
				nodeSetDisproof( node, min);
				break;
			
			}
			if (nodeProof(node) == 0){
				nodeSetValue(node, TRUE);
				//deleteChild(node);
			} 
			if (nodeDisproof(node) == 0){
				nodeSetValue(node, FALSE);
				//deleteChild(node);
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
	uint tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
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
	node_t* child = malloc(sizeof(node_t));
	if (child == NULL)
		perror("malloc child");
	numberOfNodes++;
	nodeSetExpanded(child, false);
	nodeSetTurn(child, nodeTurn(node)+1 );
	child->parents = llNew();
	llAddNode(&child->parents, node);

	switch (nodeType(node)) {
	case OR: //hraje prvni hrac
		nodeSetType(child, AND);
		nodeCopyData(child,node);
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
		nodeCopyData(child,node);
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

	node_t* n = cacheFind(child);
	if ( n != NULL ) { //je v cachy?
		llAddNode(&(n->parents),node);
		free(child);
		numberOfNodes--;
		return n;
	} else {
		cacheInsert(child);
		return child;
	}
}

static inline void developNode(node_t* node){
	//vytvori a ohodnoti potomky
	//TODO otestovat vypis viteztvi+proher, zbytek hotovo

	node_t* childs[N*N];
	int childsN=0;

	for (int i = 0; i < N; i++)
		for (int j = 0; j < i; j++)
			if ( !nodeEdge(node, i, j) ){ //ij je hrana ktera jeste nema barvu
				childs[childsN] = createChild(node,i,j);
				setProofAndDisproofNubers(childs[childsN]);
				childsN++;
			}

	//umistim potomky do stromu
	node->childs = malloc(sizeof(node_t*) * childsN);
	for (int i = 0; i < childsN; i++){
		node->childs[i] = childs[i];
	}
	nodeSetChildsN( node, childsN);
	
	nodeSetExpanded(node,true);
}

static inline void updateAncestors(){
	node_t* node = llLastNode(&currentPath);
	if (node==NULL) perror("nesmysl");

	ll_t* ancestors = llNew();

	//nejdrive predky po te linii kudy se dostalo k mostProvingNode
	while (true){
		uint oldProof = nodeProof(node);
		uint oldDisproof = nodeDisproof(node);

		setProofAndDisproofNubers(node);

		int changed = (oldProof != nodeProof(node)) || (oldDisproof != nodeDisproof(node));
		if (!changed)
			break;
		
		//pridat vsechny predky udatate
		llAddll(&ancestors, node->parents);

		//jit vzhuru po linii
		node_t* previousNode = node;
		llGetNode(&currentPath);
		node = llLastNode(&currentPath);
		if (node == NULL){ //pokud to jde
			llAddNode(&currentPath,previousNode);
			break;
		}
	}

	//update zbytku
	node_t* ancestor;
	while ((ancestor = llGetNode(&ancestors)) != NULL){
		if (ancestor == NULL)
			continue;
		uint oldProof = nodeProof(ancestor);
		uint oldDisproof = nodeDisproof(ancestor);

		setProofAndDisproofNubers(ancestor);

		if ((oldProof != nodeProof(ancestor)) || (oldDisproof != nodeDisproof(ancestor)))
			llAddll(&ancestors, ancestor->parents);

	}
}

static inline void selectMostProving(){
	node_t * node = llLastNode(&currentPath);
	while (nodeExpanded(node)){
		uint turn = nodeTurn(node);
		switch (nodeType(node)) {
		case OR: 
			for (uint i = 0; i < nodeChildsN(node); i++){
				if (nodeProof(node) == nodeProof(node->childs[i])){
					node = node->childs[i];
					break;
				}
			}
			break;
		case AND: 
			for (uint i = 0; i < nodeChildsN(node); i++){
				if (nodeDisproof(node) == nodeDisproof(node->childs[i])){
					node = node->childs[i];
					break;
				}
			}
			break;
		}
		if (turn == nodeTurn(node)){
			printf("minimalni (dis)proof numer neni\n");
		}
		llAddNode(&currentPath,node);
	}
}

nodeValue_t proofNuberSearch(node_t* root){

	currentPath = llNew();
	llAddNode(&currentPath,root);

	int counter = 0;
	while (nodeProof(root) > 0 && nodeDisproof(root) > 0){
	
		selectMostProving();
		node_t* mostProovingNode = llLastNode(&currentPath);
		
		developNode(mostProovingNode);

		updateAncestors(); 
	
		//TESTOVACI CAST	
		counter++;
		//if (counter % 1000 == 0){
		//if (true){
		if (false){
			printNode(mostProovingNode);
			printf("hotov node (%u) %u %u\n",nodeTurn(mostProovingNode),nodeProof(mostProovingNode),nodeDisproof(mostProovingNode));
			printf("root %u %u\n",nodeProof(root),nodeDisproof(root));
			printChilds(mostProovingNode);
		}
	}

	printf("nodes %d\n",numberOfNodes);
	printf("cache miss %d\n",cacheMiss);
	return nodeValue(root);
}

