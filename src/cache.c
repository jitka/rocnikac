#include "cache.h"
#include "print.h"
#include "nodefce.h"

node_t** cache;

void cacheInit(){
	cache = calloc(sizeof(node_t),CACHE_SIZE);
	assert(cache != NULL);
}

void cacheInsert(node_t* node,int kdo){
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 position = ( nodeHash(node) + i ) % CACHE_SIZE;
		if ( cache[position] != NULL )
			continue;
//		cache[position].exist = true;
//		memcpy(&cache[position],node,sizeof(node_t));
		cache[position] = node;
//		graphCopy(nodeGraph(&cache[position]),nodeGraph(node));
//		printNode(&cache[position]);
//		printf("pozice puv %d skut %d\n",( nodeHash(node)  ) % CACHE_SIZE, position);
//		printf("plnim\n");
		return;
	}
#ifdef DEBUG
	cacheMiss++;
#endif //DEBUG
	if (kdo == 2)
		printf("cache insert neni misto\n");
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 position = ( nodeHash(node) + i ) % CACHE_SIZE;
		if ( 
				nodeCurrent(cache[position]) || 
				nodeCurrentChild(cache[position]) || 
//				nodeValue(&cache[position]) != UNKNOWN ||
				(nodeTurn(cache[position]) == nodeTurn(node)) )
			continue;
		//jeho detem ho odebrat za rodice
		//TODO testovat rodice
		node_t* old = cache[position];
		assert(old!=NULL);
		if (kdo == 2){
			printf("cache insert vyhazuji na %d toho:\n",position);
			printNode(old);
		}
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
		nodeDelete(old);
//		memcpy(&cache[position],node,sizeof(node_t));
		cache[position] = node;
		if (kdo == 2){
			printf("cache insert a davam tam toho:\n");
			printNode(cache[position]);
		}
		return;
	}

#ifdef DEBUG
	printf("neni kam dat %d\n",nodeHash(node));
#endif //DEBUG
}

node_t* cacheFind(graph_t* graph){ 
//	printf("cache: hledam\n");
	for (u32 i = 0; i < CACHE_PATIENCE; i++){
		u32 where = ( graphHash(graph) + i ) % CACHE_SIZE;
		if (cache[where] == NULL)
			continue;
		assert(graph != NULL);
		assert(cache[where] != NULL);
		assert(nodeGraph(cache[where]) != NULL);
		if ( graphCompare( nodeGraph(cache[where]), graph) ){
			return cache[where];
		}
		//printf("pozice puv %d skut %d\n",( graphHash(graph)  ) % CACHE_SIZE, where);
	}
//	printf("cache: neni tam\n");
	return NULL;
}

