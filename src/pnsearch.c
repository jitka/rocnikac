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
ll2_t currentPath;

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
static inline void setValue(node_t* node){
	//zjisti hodnotu hry a vrati kolik volnych K4 pouziva posledni hranu
	int i = nodeLastEdgeI(node);
	int j = nodeLastEdgeJ(node);
	switch (nodeType(node)) {
	case OR:	
		//nevyhral prvni hrac?
		if (testK4(node,i,j,0)){
			setTrue(node);
		} else if ( nodeTurn(node) == (N*(N-1))/2 ){
			setFalse(node);
		} else {
			setUnknown(node);
		}
		break;
	case AND: 
		//neprohral prvni hrac?
		if (testK4(node,i,j,1)){
			setFalse(node);
		} else if ( nodeTurn(node) == (N*(N-1))/2 ){
			setFalse(node);
		} else {
			setUnknown(node);
		}
		break;
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
	
	//vyhodnocuje deti	
	for (int v = 0; v < childrenN; v++){
		int freeK4;
		bool fullK4;
		color c = (nodeTurn(node) % 2 == 0) ? RED : BLUE;	
//		testK4andFreeK4(children[v], i, j, c, &freeK4, &fullK4);
/*
#ifdef NOFREEK4
		if (freeK4 > 0)
			possible = true;
#endif //NOFREEK4
#ifdef HEURISTIC1
		free[childsN]=freeK4;
#endif //HEURISTIC1
*/		setValue(children[v]);

	}
/*#ifdef NOFREEK4
	if (possible == false && nodeType(node)==OR ){
		//printf("prvni hrac nema moznost vyhrat %d\n",nodeHash(node));
		//printNode(node);
		setFalse(node);
	}
#endif //NOFREEK4
#ifdef HEURISTIC1
	//tridi deti aby nejdriv byly ty s vyce moznostmi
	for (int i = 0; i < childsN; i++){
		for (int j = i+1; j < childsN; j++){
			if (free[i] < free[j]){
				{int tmp = free[i]; free[i] = free[j]; free[j] = tmp;}
				{node_t* tmp = childs[i]; childs[i] = childs[j]; childs[j] = tmp;}
			}
		}	
	}
#endif //HEURISTIC1
*/
	for (int v = 0; v < childrenN; v++){ 
		insertChild(node,children[v]);
	}
	nodeSetExpanded(node,true);


}

///////////////////////////////////////////////////////////////////////////////////////////////
static inline void updateAncestors(){ //po hladinach

#ifdef STATS
	int update = 0;
#endif //STATS

	ll2_t ancestors;
       	ll2New(&ancestors);
	ll2AddNodeEnd( &ancestors, ll2FirstNode(&currentPath));
	while ( ! ll2Empty(&ancestors) ){

#ifdef STATS
		update++;
#endif //STATS
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
#ifdef STATS
	histogramAdd( &updateStats, update);
//	printf("update %d\n",update);
#endif //STATS
}
///////////////////////////////////////////////////////////////////////////////////////////////
static inline void selectMostProving(){
	node_t * node = ll2FirstNode(&currentPath);
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
		ll2AddNodeBegin(&currentPath,node);
	}
#ifdef STATS
	histogramAdd( &selectStats, select);
//	printf("select %d\n",select);
#endif //STATS
}

///////////////////////////////////////////////////////////////////////////////////////////////
nodeValue_t proofNuberSearch(node_t* root){

	ll2New(&currentPath);
	ll2AddNodeBegin(&currentPath,root);

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
		node_t* mostProovingNode = ll2FirstNode(&currentPath);
		
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

