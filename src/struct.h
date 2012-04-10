#ifndef NODE_H
#define NODE_H

#include <limits.h>
#include "setting.h"
#define uchar unsigned int //8-bit
#define uint unsigned int //32-bit
#define ull unsigned long long int //64-bit 
#define true 1
#define false 0

typedef enum { AND, OR } nodeType_t;
typedef enum { TRUE=1, FALSE=0, UNKNOWN=2 } nodeValue_t;
typedef enum { RED=0, BLUE=1 } color;

typedef struct node node_t;

typedef struct linkedListNode ll_t;
struct linkedListNode{
	node_t* data;
	ll_t* next;
	ll_t* prev;
};

typedef struct {
	ll_t * first;
	ll_t * last;
	ll_t * current;
} ll2_t;

typedef struct {
	ull g[2];
} graph_t;

struct node{
	uint proof2;
	uint disproof;

	uchar data;// bity:
	           // 0 expanded
		   // 1 type
		   // 2,3 value

	uchar turn; //kolikaty je to tah = kolik je prave obarveno hran
	uchar childsNumber;

	uint hash;

	node_t** childs;
	ll2_t parents;

	ull graph[2];
	
	graph_t graph2;
};

extern uint hashNumbers[2][N][N]; //pro kazdou parvu a hranuo

#endif
