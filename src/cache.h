#ifndef CACHE_H
#define CACHE_H

#ifdef DEBUG
extern int cacheMiss;
#endif //DEBUG
node_t* cache[CACHE_SIZE];

static inline void cacheInsert(node_t* node);
static inline node_t* cacheFind(node_t* node); //vrati ukazatel na stejny graf nebo NULL pokud tam neni
static inline void cacheDelete(node_t* node);

//--------------------------------------------------------------

static inline void cacheInsert(node_t* node){
#ifdef DEBUG
/*	if (nodeTurn(node)==4){
		tmp42++;
		printf("%d\n",tmp42);
		printNode(node);
	} 
	*/
#endif //DEBUG
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] != NULL)
			continue;
		cache[where] = node;
		return;
	}
#ifdef DEBUG
	cacheMiss++;
	printf("neni kam dat %d\n",nodeHash(node));
#endif //DEBUG
}

static inline node_t* cacheFind(node_t* node){ 
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		if ( compareGraph( cache[where], node) ){
			return cache[where];
		}
	}
	return NULL;
}

static inline node_t* cacheFind2(graph_t* graph){ 
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( graph->hash + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		if ( compareNodeGraph( cache[where], graph) )
			return cache[where];
	}
	//printf("neni tam\n");
	return NULL;
}
static inline void cacheDelete(node_t* node){
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		if ( compareGraph( cache[where], node) ){
			cache[where] = NULL;
			return;
		}
	}
#ifdef DEBUG
	perror("neni v cachy");
#endif //DEBUG
}
#endif //CACHE_H
