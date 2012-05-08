#include <stdio.h>
#include "pnsearch.h"
#include "stats.h"
#include "setting.h"
#include "print.h"
#include "linkedlist.h"
#include "nodefce.h"
#include "norm.h"
#include "cache.h"

// or node... na tahu je prvni hrac
int numberOfNodes = 0; //abych vedela kolik zeru pameti - to co je mimo cache
node_t* currentPath[M];
int currentNode = 0; //kde je posledni prvek, uklaza _ZA_ nej
u32 updateN = 0; //kolikaty probehl update
int parentMiss = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setTrue(node_t* node);
static inline void setFalse(node_t* node);
static inline void setUnknown(node_t* node);
static inline void setProofAndDisproofNubers(node_t* node);
static inline void setValue(node_t* node, bool fullK4);
static inline node_t* createChild(node_t* node, int i, int j);
static inline void insertChild(node_t* node, node_t* child);
static inline void repairNode(node_t* node);
static inline void developNode(node_t* node);
static inline void updateAncestors();
static inline void selectMostProving();
static inline node_t* selectMostProving2(node_t* node, u32* secondProof, u32* secondDisproof);
nodeValue_t proofNumberSearch(node_t* root);

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setTrue(node_t* node){
	nodeSetValue(node, TRUE);
	nodeSetProof(node,0);
	nodeSetDisproof(node,MAXPROOF);
#ifdef STATS
	all_stats.finished++;
	turn_stats[nodeTurn(node)].finished++;
	all_stats.finished_true++;
	turn_stats[nodeTurn(node)].finished_true++;
	histogramAdd ( &all_stats.setFin, node->set_stats);
	histogramAdd ( &turn_stats[nodeTurn(node)].setFin, node->set_stats);
	histogramAdd ( &all_stats.parFin, nodeParentsN(node));
	histogramAdd ( &turn_stats[nodeTurn(node)].parFin, nodeParentsN(node));
#endif //STATS
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setFalse(node_t* node){
	nodeSetValue(node, FALSE);
	nodeSetProof(node,MAXPROOF);
	nodeSetDisproof(node,0);
#ifdef STATS
	int t = nodeTurn(node);
	all_stats.finished++;
	turn_stats[t].finished++;
	all_stats.finished_false++;
	turn_stats[t].finished_false++;
	histogramAdd ( &all_stats.setFin, node->set_stats);
	histogramAdd ( &turn_stats[nodeTurn(node)].setFin, node->set_stats);
	histogramAdd ( &all_stats.parFin, nodeParentsN(node));
	histogramAdd ( &turn_stats[nodeTurn(node)].parFin, nodeParentsN(node));
#endif //STATS
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setUnknown(node_t* node){
	nodeSetValue(node, UNKNOWN);
	nodeSetProof(node,1);
	nodeSetDisproof(node,1);
#ifdef DEBUG
	if (nodeExpanded(node))
		perror("tohle se teprve tvori");
#endif //DEBUG
}
///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setProofAndDisproofNubers(node_t* node){
#ifdef STATS
	node->set_stats++;
#endif //STATS
	
#ifdef DEBUG
	if (nodeValue(node) != UNKNOWN){
		if (nodeProof(node) != 0 && nodeProof(node) != MAXPROOF){
			printf("au set maxproof %d\n",MAXPROOF);
		}
		if (nodeDisproof(node) != 0 && nodeDisproof(node) != MAXPROOF)
			printf("au set");
		//printf("zbytecne\n");
		return; 
	}
	if (!nodeExpanded(node)){
		printf("neexpandove\n");
	}
#endif //DEBUG	

	bool missing = false;	
	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
		//pokud se smazal syn
		if ( child == NULL){
			missing = true;
		}
	}

	if (missing)
		repairNode(node);

	missing = false;	
	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
		//pokud se smazal syn
		if ( child == NULL){
			missing = true;
		}
	}

	if (missing)
		printf("TODO tohle nechci resit\n");


	if (nodeType(node) == OR ) {
		u32 min = MAXPROOF;
#ifdef WEAK
		u32 max = 0;
#else //WEAK
		u32 sum = 0;
		bool infinity = false;
#endif //WEAK
			
		int to = 0;
		for (int i = 0; i < nodeChildrenN(node); i++) {
			node_t* child = cacheFind(&node->children[i]);
#ifdef DEBUG
			//nepodarilo se vytvorit vsechny deti/navzajem se vyhazuji
			if (child == NULL){
				printf("TODO smazano dite %d %d \n",nodeChildrenN(node),i);
			}
#endif //DEBUG
			//zahodim zbytecne
			if ( nodeValue(child) == FALSE ){
				continue;
			} else {
				node->children[to] = node->children[i];
				to++;
			}

			//pocitam ze synu
			min = MIN(min,nodeProof(child));
#ifdef WEAK 			
			max = MAX( max, nodeDisproof(child) );
#else //WEAK
			sum += nodeDisproof(child);
			if (nodeDisproof(child) == MAXPROOF)
				infinity = true;
#endif //WEAK
		}
		nodeSetChildrenN(node,to);
		nodeSetProof( node, min);
#ifdef WEAK
		if (nodeChildrenN(node) == 0){
			nodeSetDisproof( node, 0);
		} else if (max == MAXPROOF) {
			nodeSetDisproof( node, MAXPROOF);
		} else { 
			nodeSetDisproof( node, max + nodeChildrenN(node) - 1);
		}
#else //WEAK
		if (infinity) {
			nodeSetDisproof( node, MAXPROOF);
		} else if (sum >= MAXPROOF) {
			printf("dosazeno nekonecna\n");
		} else {
			nodeSetDisproof( node, sum);
		}
#endif //WEAK
#ifdef DEBUG
		if (nodeProof(node) == MAXPROOF && nodeDisproof(node) == MAXPROOF){
			printf("dve nekonecna %d %d\n",min,max);
			printNode(node);
			printChildren(node);
		}
#endif //DEBUG
	} else { //(nodeType(node) == OR)
		u32 min = MAXPROOF;
#ifdef WEAK
		u32 max = 0;
#else //WEAK
		u32 sum = 0;
		bool infinity = false;
#endif //WEAK
		int to = 0;
		for (int i = 0; i < nodeChildrenN(node); i++) {
			node_t* child = cacheFind(&node->children[i]);
#ifdef DEBUG
			if (child == NULL){
				printf("TODO smazano dite %d %d \n",nodeChildrenN(node),i);
				printNode(node);
				printChildren(node);
			}
#endif //DEBUG
			//zahodim zbytecne
			if ( nodeValue(child) == TRUE ){
				continue;
			} else {
				node->children[to] = node->children[i];
				to++;
			}

			//pocitam ze synu
#ifdef WEAK
			max = MAX(max,nodeProof(child));
#else //WEAK
			sum += nodeProof(child);
			if (nodeDisproof(child) == MAXPROOF)
				infinity = true;
#endif //WEAK
			min = MIN( min, nodeDisproof(child) );
		}
		nodeSetChildrenN(node,to);
#ifdef WEAK
		if (nodeChildrenN(node) == 0){
			nodeSetProof( node, 0 );
		} else if (max == MAXPROOF) {
			nodeSetProof( node, MAXPROOF);
		} else { 
			nodeSetProof( node, max + nodeChildrenN(node) - 1 );
		}
#else //WEAK
		if (infinity) {
			nodeSetProof( node, MAXPROOF);
		} else if (sum >= MAXPROOF) {
			printf("dosazeno nekonecna\n");
		} else {
			nodeSetProof( node, sum);
		}
#endif //WEAK
		nodeSetDisproof( node, min);

	}
	if (nodeProof(node) == 0){
		setTrue(node);
	} 
	if (nodeDisproof(node) == 0){
		setFalse(node);
	}
#ifdef DEBUG
	if (nodeProof(node) == MAXPROOF && nodeDisproof(node) == MAXPROOF){
		printf("dve nekonecna\n");
		printNode(node);
		printChildren(node);
	}
#endif //DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setValue(node_t* node, bool fullK4){
	if (fullK4){
		switch (nodeType(node)) {
		case OR:	
			setTrue(node);
			break;
		case AND: 
			setFalse(node);
			break;
		}
	} else if ( nodeTurn(node) == (N*(N-1))/2 ){
		setFalse(node);
	} else {
		setUnknown(node);
	}
}
static inline node_t* createChild(node_t* node, int i, int j){
	//vytvori potomka obarvenim hrany i,j
	
	//-----vytvorim vrchol
	node_t* child = nodeNew(nodeTurn(node)+1);

	graphCopy(nodeGraph(child),nodeGraph(node));

	switch (nodeType(node)) {
	case OR: //hraje prvni hrac
		nodeSetType(child, AND);
		graphSetEdge(nodeGraph(child),i,j,RED);
		break;
	case AND: //hraje druhy
		nodeSetType(child, OR);
		graphSetEdge(nodeGraph(child),i,j,BLUE);
		break;
	}
	nodeSetLastEdge(child,i,j);
	nodeSetExpanded(child,false);
	
	//-----znormuju
	norm(child);
	return child;

}

static inline void insertChild(node_t* node, node_t* child){
	//zapoji vrchol do stromu
	node_t* n = cacheFind(nodeGraph(child));
	if ( n != NULL ) { 
		//je v cachy
		nodeAddParent(n,nodeGraph(node));
		nodeAddChild(node,nodeGraph(n));
		nodeDelete(child);
	} else {
		//neni v cachy	
#ifdef STATS
		statsNewNode(child);
#endif //STATS
		nodeAddChild(node,nodeGraph(child));
		nodeAddParent(child,nodeGraph(node));
		cacheInsert(child);
	}
}

static inline node_t** generateChildren(node_t* node, int *childrenN){
	node_t** children = malloc(sizeof(node_t*)*M);
#ifdef DEBUG
	assert(children != NULL);
	int tmp = numberOfNodes;
#endif //DEBUG
#ifdef NOFREEK4
	bool possible = false;
#endif //NOFREEK4
#ifdef HEURISTIC1
	int free[M];
#endif //HEURISTIC1


	//vyroby deti a normalizuje
	*childrenN = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < i; j++)
			if ( ! graphEdgeExist(nodeGraph(node), i, j) ) 
				//ij je hrana ktera jeste nema barvu
				children[(*childrenN)++] = createChild(node,i,j);
#ifdef DEBUG
	assert(tmp+(*childrenN) == numberOfNodes);
	assert(*childrenN > 0);
#endif //DEBUG
	//maze dvojcata	
	if (nodeTurn(node) < TURNDDELETECHILDRENST) {
		for (int i = 0; i < *childrenN; i++){
			for (int j = i+1; j < *childrenN; j++){
				if (nodeHash(children[i]) < nodeHash(children[j]) ){
					node_t * tmp = children[i];
					children[i] = children[j];
					children[j] = tmp;
				}
			}
		}
		node_t* last = children[0];
		int where = 1;
		for (int i = 1; i < *childrenN; i++){
			if (nodeHash(children[i]) != nodeHash(last)){
				children[where++]=children[i];
				last = children[i];
			} else {
				//jestli se nestane ze dva ruzni synove maji stejnou hash
				assert(graphCompare( nodeGraph(last), nodeGraph(children[i]) ));
				nodeDelete(children[i]);
			}
		}
		*childrenN = where;
	}
#ifdef DEBUG
	assert(tmp+(*childrenN) == numberOfNodes);
#endif //DEBUG

	//vyhodnocuje deti	
	for (int v = 0; v < *childrenN; v++){
		int freeK4;
		bool fullK4; 
		testK4andFreeK4(nodeGraph(children[v]), 
				nodeLastEdgeI(children[v]),nodeLastEdgeJ(children[v]),
				(nodeTurn(children[v]) % 2 == 1) ? RED : BLUE, //kterou barvou byl nakreslet posledni tah
				&freeK4, &fullK4);

#ifdef NOFREEK4
		if (freeK4 > 0)
			possible = true;
#endif //NOFREEK4
#ifdef HEURISTIC1
		free[v]=freeK4;
#endif //HEURISTIC1
		setValue(children[v],fullK4);

	}
#ifdef NOFREEK4
	if (possible == false && nodeType(node)==OR ){
//		printf("prvni hrac nema moznost vyhrat %d\n",nodeTurn(node));
		//	printNode(node);
		setFalse(node);
		updateAncestors(node); //TODO tady taky prepinac
	}
#endif //NOFREEK4
#ifdef HEURISTIC1
	//tridi deti aby nejdriv byly ty s vyce moznostmi
	for (int i = 0; i < *childrenN; i++){
		for (int j = i+1; j < *childrenN; j++){
			if (free[i] < free[j]){
				{int tmp = free[i]; free[i] = free[j]; free[j] = tmp;}
				{node_t* tmp = children[i]; children[i] = children[j]; children[j] = tmp;}
			}
		}	
	}
#endif //HEURISTIC1


	assert(tmp+(*childrenN) == numberOfNodes);
	return children;
}

int kolik = 0;

static inline void repairNode(node_t* node){
#ifdef DEBUG
	assert(nodeExpanded(node));
#endif //DEBUG

	//zahodim prazdne
	int to = 0;
	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
		if (child == NULL){
			continue;
		} else {
			node->children[to] = node->children[i];
			nodeSetCurrentChild(child);
			to++;
		}

	}
	nodeSetChildrenN(node,to);

	//vytvorim znova
	int childrenN;
	node_t** children = generateChildren(node,&childrenN);

	//pridam chybejici
	for (int i = 0; i < childrenN; i++){
		node_t* child = cacheFind(nodeGraph(children[i]));
		if ( child == NULL){
#ifdef DEBUG
			assert(children[i] !=NULL); //ma deti
			assert(cacheFind(&node->children[nodeChildrenN(node)-1]) != NULL); //posledni dite existuje
			assert(children[i]->parentsN == 1);
#endif //DEBUG
			nodeSetCurrentChild(children[i]);
				insertChild(node,children[i]);
		} else {
			nodeDelete(children[i]);
		}
	}
	free(children);

	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
#ifdef DEBUG
		assert(child!=NULL);
#endif //DEBUG
		nodeUnsetCurrentChild(child);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void developNode(node_t* node){

#ifdef DEBUG
	assert(!nodeExpanded(node));
#endif //DEBUG

	int childrenN;
	node_t** children = generateChildren(node,&childrenN);
	for (int v = 0; v < childrenN; v++){ 
		insertChild(node,children[v]);
	}
	free(children);

	nodeSetExpanded(node,true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void updateAncestors(){ //po hladinach

#ifdef STATS
	int updateS = 0;
#endif //STATS
#ifdef DFPN
	updateN++; //kolikaty probyha update
#endif //DFPN

	ll2_t ancestors;
       	ll2New(&ancestors);
	ll2AddNodeEnd( &ancestors, currentPath[currentNode]);
//	printf("%d: ",nodeTurn(currentPath[currentNode]));

	while ( ! ll2Empty(&ancestors) ){

		node_t* node = ll2FirstNode(&ancestors);
		ll2DelFirst(&ancestors);
#ifdef DFPN
		if (nodeUpdated(node, updateN))
			continue;
		nodeUpdate(node, updateN);
#endif //DFPN
#ifdef STATS
		updateS++;
#endif //STATS

		if ( graphCompare(nodeGraph(node), nodeGraph(currentPath[nodeTurn(node)])) ){
			currentNode = MIN (currentNode, nodeTurn(node) );
		}

		u32 oldProof = nodeProof(node);
		u32 oldDisproof = nodeDisproof(node);

		setProofAndDisproofNubers(node);

		int changed = (oldProof != nodeProof(node)) || (oldDisproof != nodeDisproof(node));
		if (!changed)
			continue;

		//pridat vsechny predky co je potreba updatetovat
		for (int i = 0; i < nodeParentsN(node); i++){
			node_t * parent = cacheFind(&node->parents[i]);
#ifdef DEBUG
			if (parent == NULL){
				perror("TODO neni rodic");
				parentMiss++;
			}
#endif //DEBUG
		}

	}
#ifdef STATS
	histogramAdd( &updateStats, updateS);
#endif //STATS
}
///////////////////////////////////////////////////////////////////////////////////////////////
static inline void selectMostProving(){
	node_t * node = currentPath[currentNode];
#ifdef STATS
	int select = 0;
#endif //STATS
	while (nodeExpanded(node)){
#ifdef STATS
		select++;
#endif //STATS
#ifdef DEBUG
		assert(nodeValue(node)==UNKNOWN);
		assert(nodeProof(node)!= 0);
	        assert(nodeDisproof(node)!=0);
		u8 turn = nodeTurn(node);
#endif //DEBUG
		switch (nodeType(node)) {
		case OR: 
			for (int i = 0; i < nodeChildrenN(node); i++) {
				node_t* child = cacheFind(&node->children[i]);
				if ( nodeProof(node) == nodeProof(child) ){
					node = child;
					break;
				}
			}
			break;
		case AND: 
			for (int i = 0; i < nodeChildrenN(node); i++) {
				node_t* child = cacheFind(&node->children[i]);
				if ( nodeDisproof(node) == nodeDisproof(child) ){
					node = child;
					break;
				}
			}
			break;
		}
#ifdef DEBUG
		assert(turn<nodeTurn(node));
#endif //DEBUG
		currentPath[++currentNode] = node;
	}
#ifdef STATS
	histogramAdd( &selectStats, select);
#endif //STATS
}

static inline node_t* selectMostProving2(node_t* node, u32* secondProof, u32* secondDisproof){
	//vybere ze synu toho nevhodnejsiho 
	node_t* best = NULL;
	u32 bestProof; u32 bestDisproof; 
	
#ifdef DEBUG
	assert(nodeExpanded(node));
	assert(nodeValue(node) == UNKNOWN);
	assert(nodeProof(node) != 0);
        assert(nodeDisproof(node) != 0);
	assert(nodeChildrenN(node)>0);
#endif //DEBUG

	switch (nodeType(node)) {
	case OR: 
		bestProof = MAXPROOF;
		bestDisproof = 0;
		*secondProof = MAXPROOF;
		*secondDisproof = 0;
		for (int i = 0; i < nodeChildrenN(node); i++) {
			node_t* child = cacheFind(&node->children[i]);
			if ( nodeProof(child) < bestProof ){
				bestProof = nodeProof(child);
				bestDisproof = nodeDisproof(child);
				best = child;
			} else if ( nodeProof(child) == bestProof ){
			} else if ( nodeProof(child) < *secondProof ) {
				*secondProof = nodeProof(child);
				*secondDisproof = nodeDisproof(child);
			}
		}
		break;
	case AND: 
		bestProof = 0;
		bestDisproof = MAXPROOF;
		*secondProof = 0;
		*secondDisproof = MAXPROOF;
		for (int i = 0; i < nodeChildrenN(node); i++) {
			node_t* child = cacheFind(&node->children[i]);
			if ( nodeDisproof(child) < bestDisproof ){
				bestProof = nodeProof(child);
				bestDisproof = nodeDisproof(child);
				best = child;
			} else if ( nodeDisproof(child) == bestDisproof ){
			} else if ( nodeDisproof(child) < *secondDisproof ) {
				*secondProof = nodeProof(child);
				*secondDisproof = nodeDisproof(child);
			}
		}
		break;
	}

	return best;
}

///////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DFPN
nodeValue_t proofNumberSearch(node_t* root){

	
	currentPath[0] = root;
	currentNode = 0;

#ifdef DEBUG
	int counter = 0;
#endif //DEBUG
#ifdef UNLOOP1 
	node_t* oldMostProovingNode = NULL;
#endif
	while (nodeProof(root) > 0 && nodeDisproof(root) > 0 && numberOfNodes < MAXNODES ){
#ifdef STATS
		interations_stats++;
#endif //STATS

//		printf("1\n");
		selectMostProving();
		node_t* mostProovingNode = currentPath[currentNode];
		
//		printf("2\n");
		developNode(mostProovingNode);

//		printf("3\n");
		updateAncestors(); 
//		printf("4\n");
	
#ifdef DEBUG
		counter++;
		//if (counter % 1000 == 0){
		//if (true){
		if (false){
			//printNode(mostProovingNode);
			printf("hotov node (%u) %u %u\n",nodeHash(mostProovingNode),nodeProof(mostProovingNode),nodeDisproof(mostProovingNode));
			//printNode(mostProovingNode);
			printf("nodes %d ",numberOfNodes);
			printf("root %u %u\n",nodeProof(root),nodeDisproof(root));
			//printChildren(mostProovingNode);
		}
#endif //DEBUG
	}

#ifdef DEBUG
	printf("nodes %d\n",numberOfNodes);
	printf("cache miss %d\n",cacheMiss);
//	extern int TMP;	printf("norm %d\n",TMP);
#endif //DEBUG


	return nodeValue(root);
}

#else  //DFPN

nodeValue_t proofNumberSearch(node_t* root){

	currentPath[0] = root;
	currentNode = 0;

	nodeSetTh(root,MAXPROOF,MAXPROOF);
#ifdef DEBUG
	int counter = 0;
#endif //DEBUG
	

	while (nodeValue(root) == UNKNOWN) {
		node_t * node = currentPath[currentNode];
//		printf("1\n");
#ifdef STATS
		interations_stats++;
#endif //STATS
#ifdef DEBUG
		if (false){
			printf("cN %d\n",currentNode);
			printf("th %d %d p/d %d %d\n",nodeThProof(node),nodeThDisproof(node),nodeProof(node),nodeDisproof(node));
			printNode(node);
		}
#endif //DEBUG
		
		if (nodeThProof(node) <= nodeProof(node) || nodeThDisproof(node) <= nodeDisproof(node) ){
			nodeUnsetCurrent(node);
			currentNode--;
			continue;
		}
//		printf("2\n");
		if ( !nodeExpanded(node) )
			developNode(node);
//		printf("3\n");
		setProofAndDisproofNubers(node);
// 		updateAncestors(node);
//		printf("4\n");

		if (nodeThProof(node) <= nodeProof(node) || nodeThDisproof(node) <= nodeDisproof(node) ){
			nodeUnsetCurrent(node);
			currentNode--;
			continue;
		}
//		printf("5\n");
	
		u32 secondProof; u32 secondDisproof;
		node_t * child = selectMostProving2(node,&secondProof,&secondDisproof);

		if (nodeType(child) == OR){
			nodeSetTh( 	child, 
					nodeThProof(node) - nodeProof(node) + nodeProof(child),
					MIN( nodeThDisproof(node), secondDisproof )
				 );
		} else {
			nodeSetTh( 	child, 
					MIN( nodeThProof(node), secondProof ),
					nodeThDisproof(node) - nodeDisproof(node) + nodeDisproof(child)
				 );
		}
		
		currentNode++;
		currentPath[currentNode] = child;
		nodeSetCurrent(child);

#ifdef DEBUG
		counter++;
		node_t * mostProovingNode = currentPath[currentNode];
		if (counter % 100000 == 0){
		//if (true){
		//if (false){
			//printNode(mostProovingNode);
			printf("hotov node (%u) %u \n",nodeHash(mostProovingNode),nodeTurn(mostProovingNode));
			//printNode(mostProovingNode);
			printf("nodes %d ",numberOfNodes);
			printf("cache miss %d ",cacheMiss);
			printf("parent miss %d ",parentMiss);
			printf("interace %d\n",counter);
			//printChildren(mostProovingNode);
		}
#endif //DEBUG
	}
#ifdef DEBUG
	printf("nodes %d ",numberOfNodes);
	printf("cache miss %d ",cacheMiss);
	printf("parent miss %d\n",parentMiss);
//	extern int TMP;	printf("norm %d\n",TMP);
#endif //DEBUG


	return nodeValue(root);
}
#endif  //DFPN
