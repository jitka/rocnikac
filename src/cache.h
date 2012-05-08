#ifndef CACHE_H
#define CACHE_H

#include "struct.h"

#ifdef DEBUG
extern int cacheMiss;
#endif //DEBUG

void cacheInit();
void cacheInsert(node_t* node,int kdo);
node_t* cacheFind(graph_t* graph); //vrati ukazatel na stejny graf nebo NULL pokud tam neni


#endif //CACHE_H
