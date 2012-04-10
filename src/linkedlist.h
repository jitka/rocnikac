#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "struct.h"

void ll2New( ll2_t* ll);
int ll2Empty( ll2_t* ll);

node_t* ll2FirstNode( ll2_t* ll);
void ll2DelFirst( ll2_t* ll);
void ll2AddNodeEnd( ll2_t* ll, node_t* node);
void ll2AddNodesEnd( ll2_t* ll, ll2_t* nodes);
void ll2Delete( ll2_t* ll, node_t* node);

void ll2FStart( ll2_t* ll);
void ll2FNext( ll2_t* ll);
node_t* ll2FGet( ll2_t* ll);
void ll2FDel( ll2_t* ll);


ll_t* llNew();
int llEmpty(ll_t* ll);
void llDelete(ll_t** where, node_t* node);
void llAddll(ll_t** where, ll_t* nodes);

void llAddNode(ll_t** where, node_t* node);
node_t* llGetNode(ll_t** where); //kdyz neni vraci NULL
node_t* llLastNode(ll_t** where); //kdyz neni vraci NULL
void llDelete(ll_t** where, node_t* node);

int llGetLength(ll_t* where); //pomale! jen pro testovani!



#endif

