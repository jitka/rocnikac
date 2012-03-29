#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

ll_t* llNew(){
	return NULL;
}

void llAddNode(ll_t** where, node_t* node){
	ll_t* item = malloc(sizeof(ll_t));
	if (item == NULL)
		perror("malloc item");
	item->data = node;
	item->next = *where;
	*where = item;
}

node_t* llGetNode(ll_t** where){
	if (*where == NULL)
		return NULL;
	ll_t* item = *where;
	*where = item->next;
	node_t* data = item->data;
	free(item);
	return data;
}

node_t* llLastNode(ll_t** where){
	if (*where == NULL)
		return NULL;
	return (*where)->data;
}

int llGetLength(ll_t* item){
	int n = 0;
	while (item!=NULL){
		n++;
		item=item->next;
	}
	return n;
}

void llAddll(ll_t** where, ll_t* nodes){
	while (nodes != NULL){
		ll_t* item = malloc(sizeof(ll_t));
		if (item == NULL)
			perror("malloc item");
		item->data = nodes->data;
		item->next = *where;
		*where = item;
		nodes=nodes->next;
	}
}