#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "print.h"
#include "start.h"

//or node... na tahu je prvni hrac
//pozor lisim se v tom ze kdyz dojdu na konec nestandarte nastavim false!!
// to co pisu ma smysl pro N <= 9
//TODO radit listy podle heuristik
//zbytecne velka polozka cache
//MOZNE ZRYCHLENI
//rekurze v delete
//test v K4

#define true 1
#define false 0
#define CACHE_SIZE (1<<10)

node_t* root;
node_t* currentNode;
int numerOfNodes = 1;
uint hashNumbers[2][N][N];
ull cache[N*CACHE_SIZE];

void hashInit(){
	for (int i = 0; i < N; i++){
		for (int j = 0; j < i; j++){
			hashNumbers[0][i][j] = random() % CACHE_SIZE;
			hashNumbers[1][i][j] = random() % CACHE_SIZE;
			hashNumbers[0][j][i] = hashNumbers[0][i][j];
			hashNumbers[1][j][i] = hashNumbers[1][i][j];
		}
	}
}
/*
void cacheInsert(node_t* node){
	//tu mozna normalizovat
	//predelat na jiny format

	node = 0;
}
*/


void deleteChild(node_t* node){
/*	if (!node->expanded)
		return;
	node_t* child = node->child;
	while (child->brother != NULL){
		node_t* next;
		next = child->brother;
		deleteChild(child);
		free(child);
		child = next;
	} */
}


static inline void setProofAndDisproofNubers(node_t* node){
//nejdriv overit jestli neni value true nebo false
//pak pocitat ze synu
//a pak dat 1 1
	switch (node->value) {
	case UNKNOWN:
		if (node->expanded){
			if (node->type == OR){
				node_t* n = node->child;
				node->proof = n->proof;
				while (n->brother != NULL){
					n = n->brother;
					if (n->proof < node->proof)
						node->proof = n->proof;
				}

				n = node->child;
				node->disproof = n->disproof;
				while (n->brother != NULL){
					n = n->brother;
					node->disproof += n->disproof;
				}
			} else {
				node_t* n = node->child;
				node->proof = n->proof;
				while (n->brother != NULL){
					n = n->brother;
					node->proof += n->proof;
				}

				n = node->child;
				node->disproof = n->disproof;
				while (n->brother != NULL){
					n = n->brother;
					if (n->disproof < node->disproof)
						node->disproof = n->disproof;
				}
			}
			if (node->proof == 0){
				node->value = TRUE;
			} 
			if (node->disproof == 0){
				node->value = FALSE;
			}
		} else {
			node->proof = 1;
			node->disproof = 1;
			deleteChild(node);

		}
		break;
	case TRUE:
		node->proof = 0;
		node->disproof = INT_MAX;
		break;
	case FALSE:
		node->proof = INT_MAX;
		node->disproof = 0;
		break;
	}
}

static inline int testK4(ull *data, int i, int j, int color){
	//otestuje jestli po pridani hrany ij nevznikla K4
	uint tr; //jednicky jsou na tech pozicich kam vede hrana jak z i tak z je
	         //prvni vyhral pokud mezi dvema takovimi poziceme vede jeho hrana
	tr =  data[i+color] & data[j+color]; 
	for (int s = 0; s < N; s++)
		if (tr & (1<<s))
			for (int t = 0; t < N; t++)
				if (tr & (1<<t)){
					//staci overit jestli mezi s a t vede hrana
					if ( data[s+color] & (1<<t) )
						return true;

				}
	return false;
}

static inline node_t* createChild(node_t* node, int i, int j){
	//vytvori potomka obarvenim hrany i,j
	node_t* child = malloc(sizeof(node_t));
	if (child == NULL)
		perror("malloc child");
	numerOfNodes++;
	child->expanded = FALSE;
	child->turn = node->turn + 1;
	child->parent = node;

	if (node->type == OR) { //hraje prvni hrac
		child->type = AND;
		child->hash = node->hash ^ hashNumbers[0][i][j];
		//nevyhral prvni hrac?
		if (testK4(node->data,i,j,0)){
			child->value = TRUE;
			printf("prvni K4 %d %d\n",i,j);
			printNode(node);
		} else if ( child->turn == (N*(N-1))/2 ){
			child->value = FALSE;
		} else {
			child->value = UNKNOWN;
			for (int x = 0; x < N*2; x++){
				child->data[x] = node->data[x];
			}
			child->data[i] += 1<<j;
			child->data[j] += 1<<i;
		}
	} else {//hraje druhy
		child->type = OR;
		child->hash = node->hash ^ hashNumbers[1][i][j];
		//neprohral prvni hrac?
		if (testK4(node->data,i,j,N)){
			child->value = FALSE;
			printf("druhy K4 %d %d\n",i,j);
			printNode(node);
		} else if ( child->turn == (N*(N-1))/2 ){
			child->value = FALSE;
		} else {
			child->value = UNKNOWN;
			for (int x = 0; x < N*2; x++){
				child->data[x] = node->data[x];
			}
			child->data[i+N] += 1<<j;
			child->data[j+N] += 1<<i;
		}
	}
	return child;
}

static inline void developNode(node_t* node){
	//vytvori a ohodnoti potomky
	//TODO otestovat vypis viteztvi+proher, zbytek hotovo
	node_t * lastChild = NULL;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < i; j++)
			if ( !(node->data[i] & (1<<j)) && !(node->data[N+i] & (1<<j)) ){
//				printf("neco %d %d\n",i,j);
				//ij je hrana ktera jeste nema barvu
				node_t* child = createChild(node,i,j);
				setProofAndDisproofNubers(child);
//				printf("child %d %d\n",child->proof,child->disproof);
				//umistim potomka do stromu
				if (lastChild == NULL){
					node->child = child;
				} else {
					lastChild->brother = child;
				}
				lastChild = child;
			}
	if (lastChild == NULL){
		perror("nema syny");
	} else {
		lastChild->brother = NULL;
	}
	node->expanded = TRUE;
}

static inline node_t* updateAncenors(node_t* node){
	node_t* previousNode;
	int changed = true;
	while (node != NULL && changed){
		ull oldProof = node->proof;
		ull oldDisproof = node->disproof;
		setProofAndDisproofNubers(node);
		changed = (oldProof != node->proof) || (oldDisproof != node->disproof);
		previousNode = node;
		node = node->parent;
	}
	return previousNode;
}

static inline node_t* selectMostProving(node_t* node){
	while (node->expanded){
		node_t* n = node->child;
		if (node->type == OR){
			while (n->proof != node->proof){
				if (n->brother == NULL){
					perror("minimalni proof numer neni");
					//printf("ktere %d %d \n",node->turn,n->turn);
//					if (n
					exit(0);
				}
				n = n->brother;
			}
		} else {
			while (n->disproof != node->disproof){
				if (n->brother == NULL){
					perror("minimalni disproof numer neni");
				}
				n = n->brother;
			}
		}
		node = n;

	}
	return node;
}

void proofNuberSearch(){
	newTree();
	//printf("root %d %d\n",root->proof,root->disproof);

	currentNode = root;

	int counter = 0;
	while (root->proof > 0 && root->disproof > 0){
		counter++;
		node_t* mostProvingNode = selectMostProving(currentNode);
		developNode(mostProvingNode);
		currentNode = updateAncenors(mostProvingNode);
		if (counter % 10000000 == 0){
			printNode(mostProvingNode);
			printf("hotov node (%d) %llu %llu\n",mostProvingNode->turn,mostProvingNode->proof,mostProvingNode->disproof);
			printf("root %llu %llu\n",root->proof,root->disproof);
			printChild(mostProvingNode);
		}
	}

	switch (root->value) {
	case UNKNOWN:
		printf("divne\n");
		break;
	case TRUE:
		printf("prvni vyhraje\n");
		break;
	case FALSE:
		printf("druhy neprohraje\n");
		break;
	}
}

