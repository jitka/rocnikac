#include <stdio.h>
#include "nodefce.h"
#include "print.h"
#include "cache.h"

void printGraph(graph_t* graph){
	printf("hash %u\n",graphHash(graph));

	printf(" | ");
	for (int j = 0; j < N; j++){
		printf("%d ",j);
	}
	printf("|\n");
	printf("---");
	for (int j = 0; j < N; j++){
		printf("--");
	}
	printf("|\n");
	for (int i = 0; i < N; i++){
		printf("%d| ",i);
		for (int j = 0; j < N; j++){
			bool print = false;
			for (int c = 0; c < 2; c++){
				if ( graphColorEdgeExist(graph,i,j,c)){
					printf("%d ",c);
					print = true;
				}
			}
			if (!print){
				printf("  ");
			}
		}
		printf("|\n");
	}
	printf("\n");
	
}

void printNode(node_t* node){

	if (node == NULL){
		printf("(null)\n");
		return;
	}

	printf("turn %d, proof %d, disproof %d\nexpanded ",nodeTurn(node),nodeProof(node),nodeDisproof(node));
	if (nodeExpanded(node))
		printf("yes, value ");
	else 
		printf("no, value ");
	if (nodeValue(node)==TRUE) {
		printf("TRUE, type ");
	} else if (nodeValue(node)==FALSE) {
		printf("FALSE, type ");
	} else {
		printf("UNKNOWN, type ");
	}
	if (nodeType(node)==OR)
		printf("OR, ");
	else 
		printf("AND, ");

	printGraph(nodeGraph(node));	
}

void printValue(nodeValue_t value){
	switch (value) {
	case UNKNOWN:
		printf("nevim\n");
		break;
	case TRUE:
		printf("prvni vyhraje\n");
		break;
	case FALSE:
		printf("druhy neprohraje\n");
		break;
	}
}

void printChildren(node_t* node){

	if (!nodeExpanded(node)){
		printf("nejsou deti\n");
		return;
	}
	printf("potomci %d:\n",nodeChildrenN(node));
	for (int i = 0; i < nodeChildrenN(node); i++){
		node_t * child = cacheFind(&node->children[i]);
		printNode(child);
		if (child == NULL)
			printGraph(&node->children[i]);
	}
	printf("potomci konec-------------------------------------------------\n");
//	int tmp; scanf("%d",&tmp);
}

void printParents(node_t* node){

	printf("rodice %d:\n",nodeParentsN(node));
	for (int i = 0; i < nodeParentsN(node); i++){
		printNode(cacheFind(&node->parents[i]));
	}
	printf("rodice konec-------------------------------------------------\n");
//	int tmp; scanf("%d",&tmp);
}

