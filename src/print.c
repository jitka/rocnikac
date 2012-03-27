#include <stdio.h>
#include "pnsearch.h"
#include "print.h"

void printNode(node_t* node){
//	printf("proof %d, disproof %d\n",node->proof, node->disproof);
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
		uint data0 = nodeNeighbour(node,i,0);
		uint data1 = nodeNeighbour(node,i,1);
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

void printChilds(node_t* node){

	if (!nodeExpanded(node))
		perror("nejsou deti");
	printf("potomci:\n");
	for (uint i = 0; i < nodeChildsN(node); i++) {
		if (node->childs[i] == NULL)
			printf("null\n");
		printNode(node->childs[i]);
	}
	printf("-------------------------------------------------------------------------\n");
	int tmp; scanf("%d",&tmp);
}

