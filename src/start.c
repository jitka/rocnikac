#include <stdlib.h>
#include <stdio.h>
#include "pnsearch.h"

node_t* getRoot(){
	node_t* node = malloc(sizeof(node_t));
	if (node == NULL)
		perror("malloc node");

	nodeSetProof( node, 1);
	nodeSetDisproof( node, 1);
	nodeSetExpanded( node, false);
	nodeSetTurn( node, 0);
	nodeSetHash( node, 0);
	nodeSetType( node, OR);
	nodeSetValue( node, UNKNOWN);
	
	node->parent = NULL;

	nodeEmptyGraph(node);
	
	return node;
}

node_t* readNode(){
	node_t* node = malloc(sizeof(node_t));
	if (node == NULL)
		perror("malloc node");

	perror("todo");

	return node;
}
