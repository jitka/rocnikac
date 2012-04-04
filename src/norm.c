#include <stdio.h>
#include <stdlib.h>
#include "nodefce.h"
#include "norm.h"
#include "print.h"

int count[1<<N];

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
	int degree[2][N];

//	printNode(node);
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

	for (int i = 0; i < N; i++){
		for (int j = i+1; j < N; j++){
			if (fce[i] > fce[j]){
				int pom = fce[i];
				fce[i] = fce[j];
				fce[j] = pom;
				pom = nodes[i];
				nodes[i] = nodes[j];
				nodes[j] = pom;
			}
		}	
	}
/*
	for (int v = 0; v < N; v++){
		printf("%d ",nodes[v]);
	} printf("\n");
	for (int v = 0; v < N; v++){
		printf("%d ",fce[v]);
	} printf("\n");
*/
	node_t* newNode = malloc(sizeof(node_t)); 
	nodeEmptyGraph(newNode);
	//tahle varianta se stara i o prehozeni hash

	for (int i = 0; i < N; i++){
		for (int j = i+1; j < N; j++){
			for (int c = 0; c < 2; c++){
				if (nodeEdgeColor(node,i,j,c)) {
					nodeSetEdge(newNode,nodes[i],nodes[j],c);
				}
			}
		}
	}
//	printNode(newNode);

	nodeCopyData(node,newNode);
//	printNode(node);

//	printf("--------------------------------------\n");
		
}
