#ifndef PNSEARCH_H
#define PNSEARCH_H

#define uint unsigned int
#define ull unsigned long long int 
#define N 5 

enum nodeType{ AND, OR };
enum nodeValue{ TRUE=1, FALSE=0, UNKNOWN=2 };

typedef struct node node_t;
struct node{
	ull proof2;
	ull disproof2;
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
	//pocet synu, otcu-snizovat pri delete, 
	node_t** childs;
	uint childsNumber;
};

void proofNuberSearch(node_t* node);
void hashInit();

//--------------------------NODE----------------------

//expanded
static inline uint nodeExpanded(node_t * node){
	return node->expanded;
}

//turn
static inline uint nodeTurn(node_t * node){
	return node->turn;
}

//hash
static inline uint nodeHash(node_t * node){
	return node->hash;
}

//childsNumber
static inline uint nodeChildsN(node_t * node){
	return node->childsNumber;
}
static inline void nodeSetChildsN(node_t * node, uint v){
	node->childsNumber = v;
}

//proof
static inline uint nodeProof(node_t * node){
	return node->proof2;
}
static inline void nodeSetProof(node_t * node, uint proof){
	node->proof2 = proof;
}

//disproof
static inline uint nodeDisproof(node_t * node){
	return node->disproof2;
}
static inline void nodeSetDisproof(node_t * node, uint disproof){
	node->disproof2 = disproof;
}

#endif
