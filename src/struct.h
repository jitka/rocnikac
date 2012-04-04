#ifndef NODE_H
#define NODE_H

#include <limits.h>
#define uchar unsigned int //8-bit
#define uint unsigned int //32-bit
#define ull unsigned long long int //64-bit 
#define MAXPROOF (INT_MAX/1000)
#define true 1
#define false 0
#define N 6 //funguje pro N 5..8 aby sla matice sousedu ulozit 

typedef enum { AND, OR } nodeType_t;
typedef enum { TRUE=1, FALSE=0, UNKNOWN=2 } nodeValue_t;
typedef enum { RED=0, BLUE=1 } color;

typedef struct node node_t;

typedef struct linkedListNode ll_t;
struct linkedListNode{
	node_t* data;
	ll_t* next;
};

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
	ll_t* parents;

	ull graph[2];
};

extern uint hashNumbers[2][N][N]; //pro kazdou parvu a hranuo

#endif
