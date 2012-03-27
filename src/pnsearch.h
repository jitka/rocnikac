#ifndef PNSEARCH_H
#define PNSEARCH_H

#define uint unsigned int
#define ull unsigned long long int 
#define N 5 

enum nodeType{ AND, OR };
enum nodeValue{ TRUE=1, FALSE=0, UNKNOWN=2 };

typedef struct node node_t;
struct node{
	ull proof;
	ull disproof;
	int expanded;
	int turn; //kolikaty je to tah = kolik je prave obarveno hran
	uint hash;
	node_t* parent;
	enum nodeType type;
	enum nodeValue value;
	ull data[N*2];
//	ull data2[3]; //TODO prepsat
	node_t* child;
	node_t* brother;
};

void proofNuberSearch();

#endif
