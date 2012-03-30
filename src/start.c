#include <stdlib.h>
#include <stdio.h>
#include "start.h"
#include "nodefce.h"
#include "linkedlist.h"

node_t* getRoot(){
	node_t* node = malloc(sizeof(node_t));
	if (node == NULL)
		perror("malloc node");

	nodeEmptyGraph(node);

	nodeSetProof( node, 1);
	nodeSetDisproof( node, 1);
	nodeSetExpanded( node, false);
	nodeSetTurn( node, 0);
	nodeSetType( node, OR);
	nodeSetValue( node, UNKNOWN);
	
	node->parents = llNew();

	
	return node;
}

node_t* readNode(){
	node_t* node = malloc(sizeof(node_t));
	if (node == NULL)
		perror("malloc node");

	nodeEmptyGraph(node);
	
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

	nodeSetProof( node, 1);
	nodeSetDisproof( node, 1);
	nodeSetExpanded( node, false);
	nodeSetTurn( node, turn);
	nodeSetValue( node, UNKNOWN);
	node->parents = llNew();
	if (turn%2==0){
		nodeSetType( node, OR);
	} else {
		nodeSetType( node, AND);
	}


	return node;
}

ll_t* readNodes(){
	ll_t* nodes = llNew();
	scanf("[");
	while (true){
		node_t* node = readNode();
		if (nodeTurn(node) == 0)
			break;
		llAddNode(&nodes,node);
//		printNode(node);
		scanf(",");
	}
	scanf("]");
	return nodes;
}
