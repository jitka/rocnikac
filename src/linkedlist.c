#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

#ifdef DEBUG
#include "nodefce.h" 
#include "print.h"
#endif //DEBUG

void ll2New( ll2_t* ll){
	ll->first = NULL;
	ll->last = NULL;
}

int ll2Empty( ll2_t* ll){
	return (ll->first == NULL);
}

ll_t* newItem( node_t* data, ll_t* prev, ll_t* next){
	ll_t* item = malloc(sizeof(ll_t));
#ifdef DEBUG
	if (item == NULL)
		perror("malloc item");
#endif //DEBUG
	item->data = data;
	item->next = next;
	item->prev = prev;
	return item;
}

void ll2AddNodeEnd( ll2_t* ll, node_t* node){
	ll_t* item = newItem(node,ll->last,NULL);
	if (ll->last == NULL){
#ifdef DEBUG
		if (ll->first != NULL)
			printf("blbost e\n");
#endif //DEBUG
		ll->first = item;
		ll->last = item;
	} else {
		ll->last->next = item;
		ll->last = item;
	}

}

void ll2AddNodeBegin( ll2_t* ll, node_t* node){
	ll_t* item = newItem(node,NULL,ll->first);
	if (ll->first == NULL){
#ifdef DEBUG
		if (ll->last != NULL)
			printf("blbost b\n");
#endif //DEBUG
		ll->first = item;
		ll->last = item;
	} else {
		(ll->first)->prev = item;
		ll->first = item;
	}

}

node_t* ll2FirstNode( ll2_t* ll){
	if (ll->first == NULL)
		return NULL;
	else
		return ll->first->data;
}

void ll2DelFirst( ll2_t* ll){
	if (ll->first == NULL)
		perror("neni tu nic");
	else {
		if (ll->last == ll->first){
			ll->last = NULL;
		}
		ll_t* f = ll->first;
		ll->first = ll->first->next;
		free(f);
	}
}

void ll2FStart( ll2_t* ll){
	ll->current = ll->first;
}

void ll2FNext( ll2_t* ll){
	if (ll->current != NULL)
		ll->current = ll->current->next;
}

node_t* ll2FGet( ll2_t* ll){
	if (ll->current == NULL)
		return NULL;
	else {
		return ll->current->data;
	}
}

void ll2FDel( ll2_t* ll){
	ll_t* f = ll->current;
	if (f == NULL)
		return;
	if (f->prev == NULL){
		ll->first = f->next;
	} else {
		(f->prev)->next = f->next;
	}
	if (f->next == NULL){
		ll->last = f->prev;
		ll->current = NULL;
	} else {
		(f->next)->prev =  f->prev;
		ll->current = f->next;
	}
	free(f);
}

void ll2AddNodesEnd( ll2_t* ll, ll2_t* nodes){
	ll2FStart(nodes);
	for (node_t* n; (n = ll2FGet(nodes)) != NULL; ll2FNext(nodes)){
		ll2AddNodeEnd(ll,n);
	}
}

void ll2Delete( ll2_t* ll, node_t* node){
	ll2FStart(ll);
	for (node_t* n; (n = ll2FGet(ll)) != NULL; ll2FNext(ll)){
		if ( n == node ){
			ll2FDel(ll);
			return;
		}
	}
#ifdef DEBUG
	perror("neni co mazat");
#endif //DEBUG
}

int ll2Length( ll2_t* ll){
	ll2FStart(ll);
	int length = 0;
	for (node_t* n; (n = ll2FGet(ll)) != NULL; ll2FNext(ll)){
		length++;
	}
	return length;

}
