#include <stdlib.h>
#include <stdio.h>
#include "pnsearch.h"

extern node_t* root;

node_t* newNode(){
	node_t* node = malloc(sizeof(node_t));
	if (node == NULL)
		perror("malloc node");

	node->proof = 1;
        node->disproof = 1;	
	node->expanded = FALSE;
	node->turn = 0;
	node->hash = 0;
	node->parent = NULL;
	node->type = OR;
	node->value = UNKNOWN;
	
	for (int i = 0; i < N*2; i++)
		node->data[i] = 0;
	return node;
}

void newTree(){
	root = newNode();
}

//TODO nacitani ze souboru
