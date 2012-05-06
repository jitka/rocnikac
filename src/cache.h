#ifndef CACHE_H
#define CACHE_H

#include "struct.h"

#ifdef DEBUG
extern int cacheMiss;
#endif //DEBUG
node_t* cache[CACHE_SIZE];

void cacheInsert(node_t* node);
node_t* cacheFind(graph_t* graph); //vrati ukazatel na stejny graf nebo NULL pokud tam neni
void cacheDelete(node_t* node);


#endif //CACHE_H
