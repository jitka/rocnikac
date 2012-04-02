#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "struct.h"

ll_t* llNew();
void llAddNode(ll_t** where, node_t* node);
node_t* llGetNode(ll_t** where); //kdyz neni vraci NULL
node_t* llLastNode(ll_t** where); //kdyz neni vraci NULL
int llGetLength(ll_t* where); //pomale! jen pro testovani!
void llAddll(ll_t** where, ll_t* nodes);
void llDelete(ll_t** where, node_t* node);
void llDeletell(ll_t* ll);



#endif

