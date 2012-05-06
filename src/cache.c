#include "cache.h"
#include "print.h"
#include "nodefce.h"

void cacheInsert(node_t* node){
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 position = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[position] != NULL)
			continue;
		cache[position] = node;
		return;
	}
#ifdef DEBUG
	cacheMiss++;
#endif //DEBUG
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 position = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (nodeCurrent(cache[position]) || nodeCurrentChild(cache[position]) || (nodeTurn(cache[position]) == nodeTurn(node)) )
			continue;
		//jeho detem ho odebrat za rodice
		//TODO testovat rodice
		node_t* old = cache[position];
		assert(old!=NULL);
		for (int i = 0; i < nodeChildrenN(old); i++){
			node_t* child = cacheFind(&old->children[i]);
			if (child == NULL)
				continue;
			int where = 0;
			for(int j = 0; j < nodeParentsN(child); j++){
				assert(&child->parents[j]!=NULL);
				if ( graphCompare( nodeGraph(old), &child->parents[j]) ){
					continue;
				} else {
					child->parents[where] = child->parents[j];
					where++;
				}

			}
			nodeSetParentN(child,where);
		}
		free(old);
		cache[position] = node;
		return;
	}

#ifdef DEBUG
	printf("neni kam dat %d\n",nodeHash(node));
#endif //DEBUG
}

node_t* cacheFind(graph_t* graph){ 
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( graphHash(graph) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		if ( graphCompare( nodeGraph(cache[where]), graph) )
			return cache[where];
	}
	//printf("neni tam\n");
	return NULL;
}

void cacheDelete(node_t* node){
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( nodeHash(node) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		if ( graphCompare( nodeGraph(cache[where]), nodeGraph(node)) ){
			cache[where] = NULL;
			return;
		}
	}
#ifdef DEBUG
	perror("neni v cachy");
#endif //DEBUG
}
