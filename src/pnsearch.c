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

void deleteChild(node_t* node, node_t* child){
	ll2Delete( &node->children, child );
	ll2Delete( &child->parents, node);

	if ( ll2Empty( &child->parents ) ){
		histogramAdd ( &turn_stats[nodeTurn(child)].setDel, child->set_stats);
		histogramAdd ( &all_stats.setDel, child->set_stats);
		deleteChildren(child);
		cacheDelete(child);
		nodeDelete(child);
		numberOfNodes--;
	}

}

void deleteChildren(node_t* node){
	if (nodeExpanded(node)){	
		while ( ! ll2Empty(&node->children) ){
			node_t* child = ll2FirstNode(&node->children);
			deleteChild( node, child);
		}
		nodeSetExpanded(node,false);
	}
}

static inline void setProofAndDisproofNubers(node_t* node){
#ifdef STATS
	node->set_stats++;
#endif //STATS
	switch (nodeValue(node)) {
	case UNKNOWN:
		if (nodeExpanded(node)){
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
#ifdef DEBUG
/*					if ( nodeDisproof(child) + 1000 > MAXPROOF){
						printf("to uz ma by false %d\n",MAXPROOF);
						printNode(child);
						printChildren(child);
					}
					*/
#endif //DEBUG
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
				nodeSetValue(node, TRUE);
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
			if (nodeDisproof(node) == 0){
				nodeSetValue(node, FALSE);
				nodeSetProof(node,MAXPROOF);
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
#ifdef DEBUG
			if (nodeProof(node) == MAXPROOF && nodeDisproof(node) == MAXPROOF){
				printf("dve nekonecna\n");
				printNode(node);
				printChildren(node);
			}
#endif //DEBUG
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

static inline node_t* createChild(node_t* node, int i, int j){
	//vytvori potomka obarvenim hrany i,j
	node_t* child = nodeNew();

	nodeSetTurn(child, nodeTurn(node)+1 );
#ifdef STATS
	color color;
#endif //STATS

	switch (nodeType(node)) {
	case OR: //hraje prvni hrac
#ifdef STATS
		color=RED;
#endif //STATS
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
#ifdef STATS
		color=BLUE;
#endif //STATS
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
#ifdef STATS
	bool threat = nodeThreat(child, i, j, color);
#endif //STATS


	norm(child);

	node_t* n = cacheFind(child);
	if ( n != NULL ) { //je v cachy?
		ll2AddNodeEnd( &n->parents, node);
		nodeDelete(child);
		return n;
	} else {
#ifdef STATS
		all_stats.created++;
		turn_stats[nodeTurn(child)].created++;
		if (nodeValue(child) == TRUE){
			all_stats.created_true++;
			turn_stats[nodeTurn(child)].created_true++;
		}
		if (nodeValue(child) == FALSE){
			all_stats.created_false++;
			turn_stats[nodeTurn(child)].created_false++;
		}

		if (threat){
			if (nodeTurn(child) == 7)
				printNode(child);
			all_stats.threat++;
			turn_stats[nodeTurn(child)].threat++;
		}

#endif //STATS

		numberOfNodes++;
		ll2AddNodeEnd( &child->parents, node);
		setProofAndDisproofNubers( child );    
		cacheInsert(child);
		return child;
	}
}

static inline void developNode(node_t* node){
	//vytvori a ohodnoti potomky

#ifdef DEBUG
	if (nodeExpanded(node))
		perror("uz je");
#endif //DEBUG

	for (int i = 0; i < N; i++)
		for (int j = 0; j < i; j++)
			if ( ! nodeEdgeExist(node, i, j) ){ 
				//ij je hrana ktera jeste nema barvu
				node_t* child =  createChild(node,i,j);
				ll2AddNodeBegin( &node->children, child );
			}
	nodeSetExpanded(node,true);
}

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
#endif //STATS
}

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
#endif //STATS
}

nodeValue_t proofNuberSearch(node_t* root){

	ll2New(&currentPath);
	ll2AddNodeBegin(&currentPath,root);

#ifdef DEBUG
	int counter = 0;
#endif //DEBUG
	while (nodeProof(root) > 0 && nodeDisproof(root) > 0 && numberOfNodes < MAXNODES ){
#ifdef STATS
		interations_stats++;
#endif //STATS

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

