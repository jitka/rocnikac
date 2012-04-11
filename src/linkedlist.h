#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "struct.h"

void ll2New( ll2_t* ll);
int ll2Empty( ll2_t* ll);

node_t* ll2FirstNode( ll2_t* ll);
void ll2DelFirst( ll2_t* ll);
void ll2AddNodeEnd( ll2_t* ll, node_t* node);
void ll2AddNodeBegin( ll2_t* ll, node_t* node);
void ll2AddNodesEnd( ll2_t* ll, ll2_t* nodes);
void ll2Delete( ll2_t* ll, node_t* node);

void ll2FStart( ll2_t* ll);
void ll2FNext( ll2_t* ll);
node_t* ll2FGet( ll2_t* ll);
void ll2FDel( ll2_t* ll);

#endif

