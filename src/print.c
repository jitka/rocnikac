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

void printChilds(node_t* node){

	if (!node->expanded)
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

