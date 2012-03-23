#include <stdlib.h>
#include <stdio.h>
#include "nodeStruct.h"

extern node_t* root;

node_t* newNode(){
	node_t* node = malloc(sizeof(node_t));
	if (root == NULL)
		perror("malloc root");

	root->proof = 1;
        root->disproof = 1;	
	root->expanded = FALSE;
	root->turn = 0;
	root->hash = 0;
	root->parent = NULL;
	root->type = OR;
	root->value = UNKNOWN;
	
	for (int i = 0; i < N*2; i++)
		root->data[i] = 0;

void newTree(){
	root = newNode();
}

void generateStartPosition(int depth){
//	node_t nodes[1000];
//	int used = 1;
	int red = depth / 2;

	for (int i = 0; i < depth; i++){
		for (u = 0; u < N; u++){
			for (int v = 0; (v < u) && ((u*n+v) < N*(N-1)/2 - i); v++){
			}
		}

	}
}

