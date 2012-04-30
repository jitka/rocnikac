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
int numberOfNodes = 1; //abych vedela kolik zeru pameti
node_t* currentPath[M];
int currentNode = 0; //kde je posledni prvek, uklaza _ZA_ nej
u32 updateN = 0; //kolikaty probehl update

void deleteChildren(node_t* node);

///////////////////////////////////////////////////////////////////////////////////////////////
void deleteChild(node_t* node, node_t* child){
	ll2Delete( &node->children, child );
	ll2Delete( &child->parents, node);

	if ( ll2Empty( &child->parents ) ){
#ifdef STATS
		histogramAdd ( &turn_stats[nodeTurn(child)].setDel, child->set_stats);
		histogramAdd ( &all_stats.setDel, child->set_stats);
#endif //STATS
		deleteChildren(child);
		cacheDelete(child);
		nodeDelete(child);
		numberOfNodes--;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
void deleteChildren(node_t* node){
	if (nodeExpanded(node)){	
		while ( ! ll2Empty(&node->children) ){
			node_t* child = ll2FirstNode(&node->children);
			deleteChild( node, child);
		}
		nodeSetExpanded(node,false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setTrue(node_t* node){
	nodeSetValue(node, TRUE);
	nodeSetProof(node,0);
	nodeSetDisproof(node,MAXPROOF);
	deleteChildren(node);
#ifdef STATS
	all_stats.finished++;
	turn_stats[nodeTurn(node)].finished++;
	all_stats.finished_true++;
	turn_stats[nodeTurn(node)].finished_true++;
	int s = node->set_stats;
	histogramAdd ( &all_stats.setFin, s);
	histogramAdd ( &turn_stats[nodeTurn(node)].setFin, s);
#endif //STATS

#ifdef DEBUG
	if (nodeExpanded(node))
		perror("au1");
#endif //DEBUG

}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void setFalse(node_t* node){
	nodeSetValue(node, FALSE);
	nodeSetProof(node,MAXPROOF);
	nodeSetDisproof(node,0);
	deleteChildren(node);
#ifdef STATS
	int t = nodeTurn(node);
	all_stats.finished++;
	turn_stats[t].finished++;
	all_stats.finished_false++;
	turn_stats[t].finished_false++;
	int s = node->set_stats;
	histogramAdd ( &all_stats.setFin, s);
	histogramAdd ( &turn_stats[t].setFin, s);
#endif //STATS

#ifdef DEBUG
	if (nodeExpanded(node))
		perror("au2");
#endif //DEBUG
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
	if (nodeHash(node) ==  2138471 ){
			printf("chyceno\n");
			printNode(node);
			if ( ll2Empty( &node->parents) ){
				printf("nejsou rodice\n");
			}
			node_t * parent = ll2FirstNode( &node->parents );
			printNode(parent);
			printChildren(node);
	}
	if (nodeValue(node) != UNKNOWN){
#ifdef DEBUG
		if (!nodeTurnChack(node)){
			printf("turn\n");
			printNode(node);
			printChildren(node);
		}
		if (nodeProof(node) != 0 && nodeProof(node) != MAXPROOF){
			printf("au set maxproof %d\n",MAXPROOF);
			nodeTurnChack(node);
//			printNode(node);
		}
		if (nodeDisproof(node) != 0 && nodeDisproof(node) != MAXPROOF)
			printf("au set");
#endif //DEBUG
		return; //TODO tohle by slo odstanit, predek se nemusi updatovat tak mockrat
	}
	
#ifdef DEBUG
	if (!nodeExpanded(node)){
		printf("neexpandove\n");
	}
#endif //DEBUG

	u32 min = MAXPROOF;
	u32 childrenN = 0;
#ifdef WEAK
	u32 max = 0;
#else //WEAK
	u32 sum = 0;
#endif //WEAK
	switch (nodeType(node)) {
	case OR:
		ll2FStart(&node->children); 
		for (node_t* child; (child = ll2FGet(&node->children)) != NULL; ){
			childrenN++;
			if ( nodeValue(child) == FALSE ){
				deleteChild( node, child);
				childrenN--;
				continue;
			}
			min = MIN(min,nodeProof(child));
#ifdef WEAK 			
			max = MAX( max, nodeDisproof(child) );
#else //WEAK
			sum += nodeDisproof(child);
#endif //WEAK
			ll2FNext(&node->children);
		}
		nodeSetProof( node, min);
#ifdef WEAK
		if (childrenN == 0)
			nodeSetDisproof( node, 0);
		else 
			nodeSetDisproof( node, max + childrenN - 1);
#else //WEAK
		nodeSetDisproof( node, sum);
#endif //WEAK
#ifdef DEBUG
		if (nodeProof(node) == MAXPROOF && nodeDisproof(node) == MAXPROOF){
			printf("dve nekonecna %d %d\n",min,max);
			printNode(node);
			printChildren(node);
		}
#endif //DEBUG
		break;
	case AND:
		ll2FStart(&node->children); 
		for (node_t* child; (child = ll2FGet(&node->children)) != NULL; ){
			childrenN++;
			if ( nodeValue(child) == TRUE ){
				deleteChild( node, child);
				childrenN--;
				continue;
			}
#ifdef WEAK
			max = MAX(max,nodeProof(child));
#else //WEAK
			sum += nodeProof(child);
#endif //WEAK
			min = MIN( min, nodeDisproof(child) );

			ll2FNext(&node->children);
		}
#ifdef WEAK
		if (childrenN == 0)
			nodeSetProof( node, 0 );
		else 
			nodeSetProof( node, max + childrenN - 1 );
#else //WEAK
		nodeSetProof( node, sum);
#endif //WEAK
		nodeSetDisproof( node, min);

		break;
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
	node_t* child = nodeNew();

	nodeSetTurn(child, nodeTurn(node)+1 );
	nodeCopyGraph(child,node);

	switch (nodeType(node)) {
	case OR: //hraje prvni hrac
		nodeSetType(child, AND);
		nodeSetEdge(child,i,j,RED);
		break;
	case AND: //hraje druhy
		nodeSetType(child, OR);
		nodeSetEdge(child,i,j,BLUE);
		break;
	}
	nodeSetExpanded(node,false);
	
	//-----znormuju
//	printNode(child);
	norm(child);
//	printNode(child);
//	printf("-----------------------\n");
	return child;

}

static inline void insertChild(node_t* node, node_t* child){
	//zapoji vrchol do stromu

	node_t* n = cacheFind(child);
	if ( n != NULL ) { 
		//je v cachy
		ll2AddNodeEnd( &n->parents, node);
		ll2AddNodeEnd( &node->children, n );
		nodeDelete(child);
	} else {
		//neni v cachy	
#ifdef STATS
		statsNewNode(child);
#endif //STATS
		numberOfNodes++;
		ll2AddNodeEnd( &child->parents, node);
		ll2AddNodeEnd( &node->children, child );
		cacheInsert(child);
	if ( ll2Empty( &child->parents) ){
			printf("nejsou rodice2\n");
			printNode(node);
		}

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void developNode(node_t* node){
	//vytvori a ohodnoti potomky


#ifdef DEBUG
	if (nodeExpanded(node))
		perror("uz je");
#endif //DEBUG

#ifdef NOFREEK4
	bool possible = false;
#endif //NOFREEK4
#ifdef HEURISTIC1
	int free[M];
#endif //HEURISTIC1


	//vyroby deti a normalizuje
	node_t* children[M];
	int childrenN = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < i; j++)
			if ( ! nodeEdgeExist(node, i, j) ) 
				//ij je hrana ktera jeste nema barvu
				children[childrenN++] = createChild(node,i,j);
	//maze dvojcata	
	if (nodeTurn(node) < TURNDDELETECHILDRENST) {
		for (int i = 0; i < childrenN; i++){
			for (int j = i+1; j < childrenN; j++){
				if (nodeHash(children[i]) < nodeHash(children[j]) ){
					node_t * tmp = children[i];
					children[i] = children[j];
					children[j] = tmp;
				}
			}
		}
		u32 last = 0;
		int where = 0;
		for (int i = 0; i < childrenN; i++){
			if (nodeHash(children[i]) != last){
				children[where++]=children[i];
				last = nodeHash(children[i]);
			}
		} 
		childrenN = where;
/*		for (int i = 0; i < childrenN; i++){
			printf("%d ",nodeHash(children[i]));
		} printf("\n");*/
	}

	//vyhodnocuje deti	
	for (int v = 0; v < childrenN; v++){
		int freeK4;
		bool fullK4; 
		testK4andFreeK4(children[v], &freeK4, &fullK4);
 
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
	//	printf("prvni hrac nema moznost vyhrat %d\n",nodeHash(node));
	//	printNode(node);
		//setFalse(node);
	}
#endif //NOFREEK4
#ifdef HEURISTIC1
	//tridi deti aby nejdriv byly ty s vyce moznostmi
	for (int i = 0; i < childrenN; i++){
		for (int j = i+1; j < childrenN; j++){
			if (free[i] < free[j]){
				{int tmp = free[i]; free[i] = free[j]; free[j] = tmp;}
				{node_t* tmp = children[i]; children[i] = children[j]; children[j] = tmp;}
			}
		}	
	}
/*		for (int i = 0; i < childrenN; i++){
			printf("%d ",free[i]);
		} printf("\n");*/
#endif //HEURISTIC1
	for (int v = 0; v < childrenN; v++){ 
		insertChild(node,children[v]);
	}
	nodeSetExpanded(node,true);


}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void updateAncestors(){ //po hladinach

#ifdef STATS
	int updateS = 0;
#endif //STATS
#ifdef UPDATE2
	updateN++; //kolikaty probyha update
#endif //UPDATE2

	ll2_t ancestors;
       	ll2New(&ancestors);
	ll2AddNodeEnd( &ancestors, currentPath[currentNode]);
//	printf("%d: ",nodeTurn(currentPath[currentNode]));

	while ( ! ll2Empty(&ancestors) ){

		node_t* node = ll2FirstNode(&ancestors);
		ll2DelFirst(&ancestors);
#ifdef UPDATE2
		if (nodeUpdated(node, updateN))
			continue;
		nodeUpdate(node, updateN);
#endif //UPDATE2
#ifdef STATS
		updateS++;
#endif //STATS

		if ( compareGraph(node, currentPath[nodeTurn(node)]) ){
			currentNode = MIN (currentNode, nodeTurn(node) );
		}

		u32 oldProof = nodeProof(node);
		u32 oldDisproof = nodeDisproof(node);

		setProofAndDisproofNubers(node);

		int changed = (oldProof != nodeProof(node)) || (oldDisproof != nodeDisproof(node));
		if (!changed)
			continue;

		//pridat vsechny predky co je potreba updatetovat
		ll2AddNodesEnd( &ancestors, &node->parents);

	}
//	printf("\n");
#ifdef STATS
	histogramAdd( &updateStats, updateS);
//	printf("update %d\n",update);
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
		if (nodeValue(node)!=UNKNOWN){
			printf("tady ne\n");
			printNode(node);
		}
		if (nodeProof(node) == 0 || nodeDisproof(node) == 0){
			printf("tady nee\n");
		}
		u8 turn = nodeTurn(node);
#endif //DEBUG
		switch (nodeType(node)) {
		case OR: 
			ll2FStart(&node->children); 
			for (node_t* child; (child = ll2FGet(&node->children)) != NULL; ll2FNext(&node->children)){
				if ( nodeProof(node) == nodeProof(child) ){
					node = child;
					break;
				}
			}
			break;
		case AND: 
			ll2FStart(&node->children); 
			for (node_t* child; (child = ll2FGet(&node->children)) != NULL; ll2FNext(&node->children)){
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
			printNode(node);
			printChildren(node);
		}
#endif //DEBUG
		currentPath[++currentNode] = node;
	}
#ifdef STATS
	histogramAdd( &selectStats, select);
//	printf("select %d\n",select);
#endif //STATS
}

static inline node_t* selectMostProving2(node_t* node, u32* bestProof, u32* bestDisproof, u32* secondProof, u32* secondDisproof){
	//vybere ze synu toho nevhodnejsiho 
	
#ifdef DEBUG
	if ( !nodeExpanded(node) )
		printf("au\n");
	if (nodeValue(node)!=UNKNOWN){
		printf("tady ne\n");
		printNode(node);
	}
	if (nodeProof(node) == 0 || nodeDisproof(node) == 0){
		printf("tady nee\n");
	}
#endif //DEBUG

	node_t* best;

	switch (nodeType(node)) {
	case OR: 
		*bestProof = MAXPROOF;
		*bestDisproof = 0;
		*secondProof = MAXPROOF;
		*secondDisproof = 0;
		ll2FStart(&node->children); 
		for (node_t* child; (child = ll2FGet(&node->children)) != NULL; ll2FNext(&node->children)){
			if ( nodeProof(child) < *bestProof ){
				*bestProof = nodeProof(child);
				*bestDisproof = nodeDisproof(child);
				best = child;
			} else if ( nodeProof(child) < *secondProof ) {
				*secondProof = nodeProof(child);
				*secondDisproof = nodeDisproof(child);
			}
		}
		break;
	case AND: 
		*bestProof = 0;
		*bestDisproof = MAXPROOF;
		*secondProof = 0;
		*secondDisproof = MAXPROOF;
		ll2FStart(&node->children); 
		for (node_t* child; (child = ll2FGet(&node->children)) != NULL; ll2FNext(&node->children)){
			if ( nodeDisproof(child) < *bestDisproof ){
				*bestProof = nodeProof(child);
				*bestDisproof = nodeDisproof(child);
				best = child;
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
nodeValue_t proofNuberSearch(node_t* root){

	
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

