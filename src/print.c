#include <stdio.h>
#include "nodefce.h"
#include "print.h"

void printNode(node_t* node){

	printf("turn %d, hash %d, proof %d, disproof %d\nexpanded ",nodeTurn(node),nodeHash(node),nodeProof(node),nodeDisproof(node));
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
		printf("OR\n");
	else 
		printf("AND\n");


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
				if ( nodeEdgeExist(node,i,j,c)){
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


void printChilds(node_t* node){

	if (!nodeExpanded(node)){
		printf("nejsou deti\n");
		return;
	}
	printf("potomci:\n");
	ll2FStart(&node->childs); 
	for (node_t* child; (child = ll2FGet(&node->childs)) != NULL; ll2FNext(&node->childs)){
		printNode(child);
	}
	printf("potomci konec------------------------------------------------------------------\n");
//	int tmp; scanf("%d",&tmp);
}

