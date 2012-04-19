#include <stdio.h>
#include <stdlib.h>
#include "norm.h"
#include "nodefce.h"
#include "print.h"


void norm(node_t* node, int *u, int *v){

	int fce[N];
	int nodes[N];
	for (int v = 0; v < N; v++){
		fce[v] = 10000*nodeDegree(node,v,0) + 100*nodeDegree(node,v,1);
		nodes[v] = v;
	}
//	for (int v = 0; v < N; v++){		printf("%d ",fce[v]);	} printf(" pred \n");
//	printNode(node);

//	printf("%d %d\n",*u,*v);
//	printNode(node);
#ifdef NOVECHANGE //NEFUNGUJE JE POTREBA MENIT HASH I PRO SLOUPCE
 //TOHLE BY MELO BYT RYCHLEJSI
	for (int i = 0; i < N; i++){
		int min = i;
		for (int j = i+1; j < N; j++){
			if (fce[min] > fce[j])
				min = j;
		}
		if (i != min){
			int tmp = fce[i]; fce[i] = fce[min]; fce[min] = tmp;
			nodeChangeNodes(node, i, min);
		}
	}
#else
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
	int nodes2[N];
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (nodes[j]==i)
				nodes2[i]=j;
		}
	}
//	for (int v = 0; v < N; v++){	printf("%d ",nodes[v]);	} printf("\n");
//	for (int v = 0; v < N; v++){		printf("%d ",fce[v]);	} printf("\n");
//	printNode(node);	printf("--------------------------------------\n");

	node_t* newNode = malloc(sizeof(node_t)); nodeEmptyGraph(newNode);
	for (int i = 0; i < N; i++){
		for (int j = i+1; j < N; j++){
			for (int c = 0; c < 2; c++){
				if (nodeColorEdgeExist(node,i,j,c)) {
					nodeSetEdge(newNode,nodes2[i],nodes2[j],c);
				}
			}
		}
	}

	*u = nodes2[*u];
	*v = nodes2[*v];

	nodeCopyGraph(node,newNode);
//	printf("%d %d\n",*u,*v);
//	printNode(node);
	free(newNode);
#endif

#ifdef DEBUG
	for (int v = 0; v < N; v++){
		fce[v] = 10000*nodeDegree(node,v,0) + 100*nodeDegree(node,v,1);
	}
//	for (int v = 0; v < N; v++){		printf("%d ",fce[v]);	} printf("po \n");
//	printNode(node);
#endif //DEBUG

}
