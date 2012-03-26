#include <stdio.h>
#include "nodeStruct.h"
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
		for (int j = 0; j < N; j++){
			if (node->data[i] & (1<<j)){
				printf("1 ");
			} else if (node->data[i+N] & (1<<j)){
				printf("2 ");
			} else {
				printf("  ");
			}
		}
		printf("|\n");
	}
	printf("\n");
				
}

void printChild(node_t* node){
	if (!node->expanded)
		perror("nejsou deti");
	node_t* n = node->child;
	printf("potomci:\n");
	printNode(n);
	while (n->brother != NULL){
		n = n->brother;
		printNode(n);
	}
	printf("-------------------------------------------------------------------------\n");
	int tmp;
	scanf("%d",&tmp);
}
