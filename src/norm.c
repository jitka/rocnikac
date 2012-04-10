#include <stdio.h>
#include <stdlib.h>
#include "nodefce.h"
#include "pnsearch.h"
#include "norm.h"
#include "print.h"

int count[1<<N];
int TMP = 0;

void normInit(){
	for (uint m = 0; m < (1<<N); m++){
		int ones = 0;
		for (int i = 0; i < N; i++){
			if ( m & (1<<i) )
				ones++;
		}
		count[m] = ones;
	}

//	printf("3-%d 0-%d 8-%d 15-%d",count[3],count[0],count[8],count[15]);
}

void norm(node_t* node){
	TMP++;
	int degree[2][N];

	for (int c = 0; c < 2; c++){
		for (int v = 0; v < N; v++){
			degree[c][v] = count[ nodeNeighbour(node,v,c) ];
		}
	}


	int fce[N];
	int nodes[N];
	for (int v = 0; v < N; v++){
		fce[v] = 10*degree[0][v] + 1000*degree[1][v];
		nodes[v] = v;
	}
/* //TOHLE BY MELO BYT RYCHLEJSI
//	printNode(node);
	for (int i = 0; i < N; i++){
		int min = i;
		for (int j = i+1; j < N; j++){
			if (fce[min] > fce[j])
				min = j;
		}
		if (i != min)
			nodeChangeNodes(node, i, min);
	}
//	printNode(node);	printf("--------------------------------------\n");
*/

	//A TOHLE POMALEJSI
//	printNode(node);
	for (int i = 0; i < N; i++){
		for (int j = i+1; j < N; j++){
			if (fce[i] > fce[j]){
				int pom = fce[i];
				fce[i] = fce[j];
				fce[j] = pom;
				pom = nodes[i];
				nodes[i] = nodes[j];
				nodes[j] = pom;
//				nodeChangeNodes(node, i, j);
			}
		}	
	}
//	for (int v = 0; v < N; v++){	printf("%d ",nodes[v]);	} printf("\n");
//	for (int v = 0; v < N; v++){		printf("%d ",fce[v]);	} printf("\n");
//	printNode(node);	printf("--------------------------------------\n");

	node_t* newNode = malloc(sizeof(node_t)); nodeEmptyGraph(newNode);
	for (int i = 0; i < N; i++){
		for (int j = i+1; j < N; j++){
			for (int c = 0; c < 2; c++){
				if (nodeEdgeExist(node,i,j,c)) {
					nodeSetEdge(newNode,nodes[i],nodes[j],c);
				}
			}
		}
	}
	nodeCopyGraph(node,newNode);
//	printNode(newNode);
	free(newNode);

		
}
