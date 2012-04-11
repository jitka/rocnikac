#include <stdio.h>
#include "nodefce.h"
#include "graphfce.h"
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
		u32 data0 = nodeNeighbour(node,i,0);
		u32 data1 = nodeNeighbour(node,i,1);
		for (int j = 0; j < N; j++){
			if (data0 & (1<<j)){
				printf("0 ");
			} else if (data1 & (1<<j)){
				printf("1 ");
			} else {
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

