#include <stdio.h>
#include "pnsearch.h"
#include "stats.h"
#include "setting.h"
#include "print.h"
#include "nodefce.h"
#include "norm.h"
#include "cache.h"

node_t* currentPath[M];
int currentNode = 0; //kde je posledni prvek, uklada se _ZA_ nej

#ifdef NODEDELETE
int parentMiss = 0;
int childMiss = 0;
#endif //NODEDELETE

int numberOfNodes = 0; //abych vedela kolik zeru pameti - to co je mimo cache

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setTrue(node_t* node);
static inline void setFalse(node_t* node);
static inline void setUnknown(node_t* node);
static inline void setProofAndDisproofNubers(node_t* node);
static inline void setValue(node_t* node, bool fullK4);
static inline node_t* createChild(node_t* node, int i, int j);
static inline void insertChild(node_t* node, node_t* child);
#ifdef NODEDELETE
static inline void repairNode(node_t* node);
#endif //NODEDELETE
#ifdef NOFREEK4
static inline bool developNode(node_t* node); //vrati jestli je node prohrane
#else //NOFREEK4
static inline void developNode(node_t* node);
#endif //NOFREEK4
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
	assert(!nodeExpanded(node));
}
///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setProofAndDisproofNubers(node_t* node){
#ifdef STATS
	node->set_stats++;
#endif //STATS
	
	//assert(nodeParentsN(node) > 0 || nodeTurn(node) == 0); //TODO odkomentovat tohle nema smysl myt ani kdyz rodice muzou mizet
	if (nodeValue(node) != UNKNOWN){
		assert( (nodeProof(node) == 0 || nodeProof(node) == MAXPROOF) );
		assert( (nodeDisproof(node) == 0 || nodeDisproof(node) == MAXPROOF) );
		//printf("zbytecne\n");
		return; 
	}
#ifndef NODEDELETE
	assert(nodeExpanded(node));
#endif //NODEDELETE

#ifdef NODEDELETE
	bool missing = false;	
	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
		//pokud se smazal syn
		if ( child == NULL){
			missing = true;
		}
	}

	if (missing){
		repairNode(node);
		childMiss++;
	}
#endif //NODEDELETE

	bool missing2 = false;	
	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
		//pokud se smazal syn
		if ( child == NULL){
			missing2 = true;
		}
	}
	assert(!missing2);


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
			assert(child != NULL);
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
			printf("dosazeno nekonecna or\n");
		} else {
			nodeSetDisproof( node, sum);
		}
#endif //WEAK
		if (nodeProof(node) == MAXPROOF && nodeDisproof(node) == MAXPROOF){
//			printf("dve nekonecna %d %d\n",min,max);
			printNode(node);
			printChildren(node);
		}
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
			assert(child != NULL);
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
			if (nodeProof(child) == MAXPROOF)
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
			printf("dosazeno nekonecna %d\n",MAXPROOF);
			printNode(node);
			printChildren(node);
			scanf("%d",&infinity);
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
	assert(nodeProof(node) != MAXPROOF || nodeDisproof(node) != MAXPROOF);
	assert( nodeProof(node)!=0 || nodeDisproof(node) != 0 );
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
///////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void insertChild(node_t* node, node_t* child){
	//zapoji vrchol do stromu
	node_t* n = cacheFind(nodeGraph(child));
	if ( n != NULL ) { 
		//je v cachy
		nodeAddParent(n,nodeGraph(node));
		nodeAddChild(node,nodeGraph(n));
		
		assert(nodeParentsN(n) > 0);
		nodeDelete(child);
	} else {
		//neni v cachy	
#ifdef STATS
		statsNewNode(child);
#endif //STATS
		nodeAddChild(node,nodeGraph(child));
		nodeAddParent(child,nodeGraph(node));
		cacheInsert(child);
		assert(nodeParentsN(child) > 0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline node_t** generateChildren(node_t* node, int *childrenN){
	node_t** children = malloc(sizeof(node_t*)*M);
	assert(children != NULL);
	int numberOfNodesOld = numberOfNodes;
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
	assert(numberOfNodesOld+(*childrenN) == numberOfNodes);
	assert(*childrenN > 0);
	return children;
}

///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODEDELETE
static inline void repairNode(node_t* node){
	assert(nodeExpanded(node));

	//zahodim prazdne
	int to = 0;
	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
		if (child == NULL){
			continue;
		} else {
			node->children[to] = node->children[i];
			nodeSetCurrentChild(child);
			assert(nodeParentsN(child) > 0 || nodeTurn(child) == 0);
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
			//pridavam dite
			child = children[i];
			assert(child !=NULL); //ma deti
			assert(cacheFind(&node->children[nodeChildrenN(node)-1]) != NULL); //posledni dite existuje
			assert(child->parentsN == 0);
			nodeSetCurrentChild(children[i]);
			insertChild(node,children[i]);
			setValue(child, nodeTestK4(child));
			assert(nodeParentsN(children[i]) > 0 || nodeTurn(children[i]) == 0);
		} else {
			nodeDelete(children[i]);
		}
	}
	free(children);

	for (int i = 0; i < nodeChildrenN(node); i++) {
		node_t* child = cacheFind(&node->children[i]);
		assert(child!=NULL);
		nodeUnsetCurrentChild(child);
	}
	assert( nodeProof(node)!=0 || nodeDisproof(node) != 0 );
}
#endif //NODEDELETE

///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NOFREEK4
static inline bool developNode(node_t* node){
#else //NOFREEK4
static inline void developNode(node_t* node){
#endif //NOFREEK4

	assert(!nodeExpanded(node));

	int childrenN;
	node_t** children = generateChildren(node,&childrenN);

	//maze dvojcata	
	if (nodeTurn(node) < TURNDDELETECHILDRENST) {
		for (int i = 0; i < childrenN; i++){
			for (int j = i+1; j < childrenN; j++){
				if (nodeHash(children[i]) < nodeHash(children[j]) ){
					node_t * tmp = children[i];
					children[i] = children[j];
					children[j] = tmp;
				} else if (nodeHash(children[i]) == nodeHash(children[j]) &&
						graphCompare( nodeGraph(children[i]), nodeGraph(children[j]) ) ){
					node_t * tmp = children[i];
					children[i] = children[j];
					children[j] = tmp;
				}
			}
		}
		node_t* last = children[0];
		int where = 1;
		for (int i = 1; i < childrenN; i++){
			if (nodeHash(children[i]) != nodeHash(last) ||
					( !graphIdentical(nodeGraph(last),nodeGraph(children[i])) )){
				children[where++]=children[i];
				last = children[i];
			} else {
				assert(graphIdentical( nodeGraph(last), nodeGraph(children[i]) ));
				nodeDelete(children[i]);
			}
		}
		childrenN = where;
	}

	//vyhodnocuje deti	
#ifdef NOFREEK4
	bool possible = false;
#endif //NOFREEK4
	for (int v = 0; v < childrenN; v++){
		int freeK4;
		bool fullK4; 
		nodeTestK4andFreeK4( children[v], &freeK4, &fullK4);

#ifdef NOFREEK4
		if (freeK4 > 0)
			possible = true;
#endif //NOFREEK4
#ifdef HEURISTIC1
		free[v]=freeK4;
#endif //HEURISTIC1
		setValue(children[v],fullK4);
		assert( nodeProof(children[v])!=0 || nodeDisproof(children[v]) != 0 );
	}
#ifdef NOFREEK4
	if (possible == false && nodeType(node)==OR ){
//		printf("prvni hrac nema moznost vyhrat %d\n",nodeTurn(node));
		//	printNode(node);
		setFalse(node);
	} else {
		possible = true;
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

	for (int v = 0; v < childrenN; v++){ 
		insertChild(node,children[v]);
	}
	free(children);

	nodeSetExpanded(node,true);

	for (int i = 0; i < nodeChildrenN(node); i++){
		assert(nodeParentsN(cacheFind(&node->children[i])) > 0);
	}
	assert( nodeProof(node)!=0 || nodeDisproof(node) != 0 );
#ifdef NOFREEK4
	return !possible;
#endif //NOFREEK4

}

///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef UPDATEANCESTORS2
u32 updateN = 0; //kolikaty probehl update
#endif //UPDATEANCESTORS2
static inline void updateAncestors(){ //po hladinach

#ifdef STATS
	int updateS = 0;
#endif //STATS
#ifdef UPDATEANCESTORS2
	updateN++; //kolikaty probyha update
#endif //UPDATEANCESTORS2

	node_t* ancestors[MAXTREEWIDTH]; 	
	ancestors[0] = currentPath[currentNode];
	assert(cacheFind(nodeGraph(     ancestors[0]      ))!= NULL);
	assert(ancestors[0] != NULL);
	int ancestorsCurrent = 0; //odkut ctu
	int ancestorsLast = 1; //kam budu psat nasledujiciho

	while ( ancestorsCurrent != ancestorsLast ){

		//na zacatek kontrola
/*		bool problem = false;
		for (int i = ancestorsCurrent; i != ancestorsLast; i = (i+1)%MAXTREEWIDTH ){
			if (cacheFind(nodeGraph(ancestors[i])) == NULL)
				problem = true;
		}
		if(updateN == 1800){
			printf("update %d poradi %d current  %d last %d \n",updateN,updateS,ancestorsCurrent, ancestorsLast);
			for (int i = ancestorsCurrent; i != ancestorsLast; i = (i+1)%MAXTREEWIDTH ){
				printNode((ancestors[i]));
			}
		}
*/		for (int i = ancestorsCurrent; i != ancestorsLast; i = (i+1)%MAXTREEWIDTH ){
			assert(cacheFind(nodeGraph(ancestors[i])) != NULL);
		}

		node_t* node = ancestors[ancestorsCurrent];
#ifndef NODEDELETE
		assert(nodeParentsN(node) > 0 || nodeTurn(node) == 0);
#endif //NODEDELETE
		ancestorsCurrent = (ancestorsCurrent+1) % MAXTREEWIDTH;

#ifdef UPDATEANCESTORS2
		assert(node!= NULL);
		if (nodeUpdated(node, updateN)){
			continue;
		}
		nodeUpdate(node, updateN);
#endif //UPDATEANCESTORS2
#ifdef STATS
		updateS++;
#endif //STATS
		assert(node!= NULL);
		assert(cacheFind(nodeGraph(node))!= NULL);
		assert(currentPath[nodeTurn(node)] != NULL);
		assert(cacheFind(nodeGraph(currentPath[nodeTurn(node)])) != NULL);
		if ( graphIdentical(nodeGraph(node), nodeGraph(currentPath[nodeTurn(node)])) ){
			assert(nodeTurn(node) <= currentNode);
#ifdef NODEDELETE
			//TODO tohle testovat
			for (int i = currentNode; i > nodeTurn(node); i--){
				nodeUnsetCurrent(node);
			}
#endif //NODEDELETE
			currentNode = nodeTurn(node);
//			printf("deje se %d %d\n");
		}

		u32 oldProof = nodeProof(node);
		u32 oldDisproof = nodeDisproof(node);

#ifndef NODEDELETE
		assert(nodeExpanded(node));
#endif //NODEDELETE
		setProofAndDisproofNubers(node);

		int changed = (oldProof != nodeProof(node)) || (oldDisproof != nodeDisproof(node)) || (nodeValue(node) != UNKNOWN);
		if (!changed)
			continue;

/*		if(updateN == 1800){
			printf("pridavam puvodni pocet %d, rodicu %d ",ancestorsLast,nodeParentsN(node));
		}
*/		//pridat vsechny predky co je potreba updatetovat
		for (int i = 0; i < nodeParentsN(node); i++){
			node_t * parent = cacheFind(&node->parents[i]);
#ifdef NODEDELETE
			if (parent == NULL){
				parentMiss++;
//				printf("stalo se\n");
				continue;
			}
#else //NODEDELETE
			assert(nodeExpanded(parent));
#endif //NODEDELETE
			assert(parent != NULL);
			assert(cacheFind(nodeGraph(parent))!= NULL);
			ancestors[ancestorsLast] = parent;
			ancestorsLast = (ancestorsLast+1) % MAXTREEWIDTH;
			assert( ancestorsCurrent != ancestorsLast ); //obsazene je cele pole
		}
/*		if(updateN == 1800){
			printf("novy pocet %d\n",ancestorsLast);
		}
*/
	}
/*		node_t* tmp = currentPath[currentNode];
			if(nodeValue(currentPath[currentNode]) != UNKNOWN){
				printNode(tmp);
			}
*/			
	assert(nodeValue(currentPath[currentNode]) == UNKNOWN || currentNode == 0);
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
		assert(nodeValue(node)==UNKNOWN);
		assert(nodeProof(node)!= 0);
	        assert(nodeDisproof(node)!=0);
		u8 turn = nodeTurn(node);
#ifdef NODEDELETE
		bool missing = false;	
		for (int i = 0; i < nodeChildrenN(node); i++) {
			if ( cacheFind(&node->children[i]) == NULL){
				//pokud se smazal syn
				missing = true;
			}
		}

		if (missing){
			repairNode(node);
			childMiss++;
		}

		assert(nodeChildrenN(node)>0);
		node_t* best = cacheFind(&node->children[0]);
		assert(best!=NULL);
		
		switch (nodeType(node)) {
		case OR: 
			for (int i = 1; i < nodeChildrenN(node); i++) {
				node_t* child = cacheFind(&node->children[i]);
				if ( nodeProof(child) < nodeProof(best) ){
					best = child;
					break;
				}
			}
			break;
		case AND: 
			for (int i = 1; i < nodeChildrenN(node); i++) {
				node_t* child = cacheFind(&node->children[i]);
				if ( nodeDisproof(child) < nodeDisproof(best) ){
					best = child;
					break;
				}
			}
			break;
		}
		node = best;

#else //NODEDELETE

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
#endif //NODEDELETE
		assert(turn<nodeTurn(node)); //exstuje best node
#ifdef NODEDELETE
		nodeSetCurrent(node);
#endif //NODEDELETE
		currentPath[++currentNode] = node;
	}
#ifdef STATS
	histogramAdd( &selectStats, select);
#endif //STATS
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline node_t* selectMostProving2(node_t* node, u32* secondProof, u32* secondDisproof){
	//vybere ze synu toho nevhodnejsiho 
	node_t* best = NULL;
	u32 bestProof; u32 bestDisproof; 
	
	assert(nodeExpanded(node));
	assert(nodeValue(node) == UNKNOWN);
	assert(nodeProof(node) != 0);
        assert(nodeDisproof(node) != 0);
	assert(nodeChildrenN(node)>0);

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
nodeValue_t proofNumberSearch(node_t* root){

	
		int tmp=0;
	int counter = 0;
	currentPath[0] = root;
	cacheInsert(root);
	currentNode = 0;

#ifndef DFPN //-------------------------------------------

	while (nodeProof(root) > 0 && nodeDisproof(root) > 0 ){
//		printf("1\n");
		if(nodeTurn(currentPath[currentNode]) == 0)
			tmp++;
		selectMostProving();
		node_t* mostProovingNode = currentPath[currentNode];
//		printf("2\n");
		developNode(currentPath[currentNode]);
//		printf("3\n");
		updateAncestors(); 
//		printf("4\n");

#else  //DFPN -------------------------------------------
	

	nodeSetTh(root,MAXPROOF,MAXPROOF);
	while (nodeValue(root) == UNKNOWN) {
		node_t * node = currentPath[currentNode];
//		printf("1\n");
		if (nodeThProof(node) <= nodeProof(node) || nodeThDisproof(node) <= nodeDisproof(node) ){
#ifdef NODEDELETE
			nodeUnsetCurrent(node);
#endif //NODEDELETE
			currentNode--;
			continue;
		}
//		printf("2\n");
		bool zadnaK4 = false;
		if ( !nodeExpanded(node) ){
#ifdef NOFREEK4
			if (developNode(node)){
//				printf("konec\n");
				assert(nodeTurn(node) > 1);
				currentNode--;
				node = currentPath[currentNode];
				zadnaK4 = true;
//				printf("zacatek\n");
			}
#else //NOFREEK4
			developNode(node);
#endif //NOFREEK4
		}
//		printf("3\n");
		setProofAndDisproofNubers(node);
//		printf("4\n");
#ifdef DFPNUPDATE
 		updateAncestors(node);
#endif //DFPNUPDATE
//		printf("5\n");

		if (nodeThProof(node) <= nodeProof(node) || nodeThDisproof(node) <= nodeDisproof(node) ){
			//TODO tady by se to melo chytit a nechyti zkontrolovat jestli je nodeTh nastavene
			if (zadnaK4)
//				printf("ano\n");
#ifdef NODEDELETE
			nodeUnsetCurrent(node);
#endif //NODEDELETE
			currentNode--;
			continue;
		}
			if (zadnaK4)
				printf("ne\n");
	
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
#ifdef NODEDELETE
		nodeSetCurrent(child);
#endif //NODEDELETE

		node_t * mostProovingNode = currentPath[currentNode];


#endif  //DFPN ----------------------------------------------------

#ifdef STATS
		interations_stats++;
#endif //STATS
		counter++;
		if (counter % 100000 == 0){
		//if (true){
		//if (false){
			//printNode(mostProovingNode);
			printf("hotov node (%u) %u \n",nodeHash(mostProovingNode),nodeTurn(mostProovingNode));
			//printNode(mostProovingNode);
			printf("nodes %d ",numberOfNodes);
			printf("cache miss %d ",cacheMiss);
#ifdef NODEDELETE
			printf("child miss %d ",childMiss);
			printf("parent miss %d ",parentMiss);
#endif //NODEDELETE
			printf("interace %d ",counter);
			printf("\n");
			//printChildren(mostProovingNode);
		}
	}
	printf("nodes %d ",numberOfNodes);
	printf("root update %d ",tmp);
	printf("cache miss %d ",cacheMiss);
#ifdef NODEDELETE
	printf("child miss %d ",childMiss);
	printf("parent miss %d ",parentMiss);
#endif //NODEDELETE
	printf("\n");
//	extern int TMP;	printf("norm %d\n",TMP);


	return nodeValue(root);
}
