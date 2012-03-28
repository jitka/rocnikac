#include <stdlib.h>
#include <stdio.h>
#include "pnsearch.h"

node_t* getRoot(){
	node_t* node = malloc(sizeof(node_t));
	if (node == NULL)
		perror("malloc node");

	nodeEmptyGraph(node);
	nodeSetHash( node, 0);

	nodeSetProof( node, 1);
	nodeSetDisproof( node, 1);
	nodeSetExpanded( node, false);
	nodeSetTurn( node, 0);
	nodeSetType( node, OR);
	nodeSetValue( node, UNKNOWN);
	
	node->parent = NULL;

	
	return node;
}

node_t* readNode(){
	node_t* node = malloc(sizeof(node_t));
	if (node == NULL)
		perror("malloc node");

	perror("todo");

	nodeEmptyGraph(node);
	nodeSetHash( node, 0);
	
	int u,v,turn=0;
	scanf("([");
	while (scanf("(%d,%d),",&u,&v)){
		nodeSetEdge(node,u,v,0);
		turn++;
	}
	scanf("],[");
	while (scanf("(%d,%d),",&u,&v)){
		nodeSetEdge(node,u,v,1);
		turn++;
	}
	scanf("])");
	printf("%d\n",turn);

	nodeSetProof( node, 1);
	nodeSetDisproof( node, 1);
	nodeSetExpanded( node, false);
	nodeSetTurn( node, turn);
	nodeSetValue( node, UNKNOWN);
	if (turn%2==0){
		nodeSetType( node, OR);
	} else {
		nodeSetType( node, AND);
	}


	return node;
}
