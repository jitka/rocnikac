#ifndef PRINT_H
#define PRINT_H

#include "struct.h"

void printGraph(graph_t* graph);
void printNode(node_t* node);
void printValue(nodeValue_t value);
void printChildren(node_t* node);
void printParents(node_t* node);

#endif
