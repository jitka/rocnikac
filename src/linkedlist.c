#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "print.h"

void ll2New( ll2_t* ll){
	ll->first = NULL;
	ll->last = NULL;
}

int ll2Empty( ll2_t* ll){
	return (ll->first == NULL);
}

ll_t* newItem( node_t* data, ll_t* prev, ll_t* next){
	ll_t* item = malloc(sizeof(ll_t));
	if (item == NULL)
		perror("malloc item");
	item->data = data;
	item->next = next;
	item->prev = prev;
	return item;
}

void ll2AddNodeEnd( ll2_t* ll, node_t* node){
	ll_t* item = newItem(node,ll->last,NULL);
	if (ll->last == NULL){
		if (ll->first != NULL)
			printf("blbost e\n");
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
		if (ll->last != NULL)
			printf("blbost b\n");
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
	node_t* node;
	while ((node = ll2FGet(nodes)) != NULL){
		ll2AddNodeEnd(ll,node);
		ll2FNext(nodes);
	}
}

void ll2Delete( ll2_t* ll, node_t* node){
	ll2FStart(ll);
	node_t* n;
	while ((n = ll2FGet(ll)) != NULL){
		if ( n == node ){
			ll2FDel(ll);
			return;
		}
	}
	perror("neni co mazat");
}


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

int llEmpty(ll_t* ll){
	return ll == NULL;
}

void llDelete(ll_t** where, node_t* node){
	if (*where == NULL)
		perror("mazu z niceho");
	ll_t* previous = *where;
	ll_t* item = (*where)->next;
	if (previous->data == node){
		free(previous);
		*where = item;
		return;	
	}
	while ( item != NULL){
		if (item->data == node){
			previous->next = item->next;
			free(item);
			return;
		}
		previous = item;
		item = item->next;	
	}
	perror("neni co");
	printNode(node);
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
