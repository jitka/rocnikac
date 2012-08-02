#ifndef CACHE_H
#define CACHE_H

#include "struct.h"

extern int cacheMiss;

void cacheInit();
void cacheInsert(node_t* node);
node_t* cacheFind(graph_t* graph); //vrati ukazatel na stejny graf nebo NULL pokud tam neni


#endif //CACHE_H
