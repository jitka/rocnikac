#include <stdlib.h>
#include <stdio.h>
#include "start.h"
#include "nodefce.h"
#include "linkedlist.h"

node_t* getRoot(){
	node_t* node = nodeNew();

	nodeEmptyGraph(node);

	nodeSetProof( node, 1);
	nodeSetDisproof( node, 1);
	nodeSetTurn( node, 0);
	nodeSetType( node, OR);
	nodeSetValue( node, UNKNOWN);
	
	return node;
}

node_t* readNode(){
	node_t* node = nodeNew();
	
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
	if (turn%2==0){
		nodeSetType( node, OR);
	} else {
		nodeSetType( node, AND);
	}


	return node;
}

ll2_t readNodes(){
	ll2_t nodes;
	ll2New(&nodes);
	scanf("[");
	while (true){
		node_t* node = readNode();
		if (nodeTurn(node) == 0)
			break;
		ll2AddNodeEnd(&nodes,node);
//		printNode(node);
		scanf(",");
	}
	scanf("]");
	return nodes;
}
