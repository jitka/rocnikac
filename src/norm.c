#include <stdio.h>
#include <stdlib.h>
#include "norm.h"
#include "nodefce.h"
#include "print.h"


void norm(node_t* node){

	graph_t * graph = nodeGraph(node);
	int fce[N];
	int nodes[N];
	for (int v = 0; v < N; v++){
		fce[v] = 100000*nodeDegree(graph,v,0) + 100*nodeDegree(graph,v,1);
		nodes[v] = v;
	}
#ifdef NORM2
	int fce2[N];
	for (int i = 0; i < N; i++){
		fce2[i]=fce[i];
	}
	for (int i = 0; i < N; i++){
		for (int j = i+1; j < N; j++){
			if (nodeColorEdgeExist(graph,i,j,0)) {
				fce2[i] += fce[j]/10;
				fce2[j] += fce[i]/10;
			}
			if (nodeColorEdgeExist(graph,i,j,1)) {
				fce2[i] += fce[j]/100;
				fce2[j] += fce[i]/100;
			}
		}
	}
	for (int i = 0; i < N; i++){
		fce[i]=fce2[i];
	}
#endif //NORM2

//	for (int v = 0; v < N; v++){		printf("%d ",fce2[v]);	} printf(" lepsi \n");
//	for (int v = 0; v < N; v++){		printf("%d ",fce[v]);	} printf(" horsi \n");
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
			nodeChangeNodes(graph, i, min);
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

	graph_t newGraph; 	nodeEmptyGraph(&newGraph);
	for (int i = 0; i < N; i++){
		for (int j = i+1; j < N; j++){
			for (int c = 0; c < 2; c++){
				if (nodeColorEdgeExist(graph,i,j,c)) {
					nodeSetEdge(&newGraph,nodes2[i],nodes2[j],c);
				}
			}
		}
	}
	nodeCopyGraph(graph,&newGraph);

	//prohodi souradnice posledni hrany
	nodeSetLastEdge(node,nodes2[nodeLastEdgeI(node)],nodes2[nodeLastEdgeJ(node)]);
#endif

}
