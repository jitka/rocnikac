#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "node.h"


ll_t* llNew();
void llAddNode(ll_t** where, node_t* node);
node_t* llGetNode(ll_t** where); //kdyz neni vraci NULL
node_t* llLastNode(ll_t** where); //kdyz neni vraci NULL
int llGetLength(ll_t* where); //pomale! jen pro testovani!

#endif

