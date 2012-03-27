#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "print.h"
#include "pnsearch.h"
#include "start.h"

#define true 1
#define false 0
#define CACHE_SIZE (1<<10)

// or node... na tahu je prvni hrac

node_t* currentNode;
int numerOfNodes = 1; //abych vedela kolik zeru pameti

//--------------------------CACHE-----------------------
uint hashNumbers[2][N][N]; //pro kazdou parvu a hranuo
//ull cache[N*CACHE_SIZE]; //TODO to bude jinak velike

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

//--------------------------PN-SEARCH-----------------------
static inline void deleteChild(node_t* node){
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
	nodeProof(node); //nedela nic, neotravuje
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

				uint min = nodeProof(n);
				while (n->brother != NULL){
					n = n->brother;
					if (nodeProof(n) < min)
						min = nodeProof(n);
				}
				nodeSetProof( node, min);

				n = node->child;
				uint sum = nodeDisproof(n);
				while (n->brother != NULL){
					n = n->brother;
					sum += nodeDisproof(n);
				}
				nodeSetDisproof( node, sum);

			} else {
				node_t* n = node->child;

				uint sum = nodeProof(n);
				while (n->brother != NULL){
					n = n->brother;
					sum += nodeProof(n);
				}
				nodeSetProof( node, sum);

				n = node->child;
				uint min = nodeDisproof(n);
				while (n->brother != NULL){
					n = n->brother;
					if (nodeDisproof(n) < min)
						min = nodeDisproof(n);
				}
				nodeSetDisproof( node, min);
			}
			if (nodeProof(node) == 0){
				node->value = TRUE;
			} 
			if (nodeDisproof(node) == 0){
				node->value = FALSE;
			}
		} else {
			nodeSetProof(node,1);
			nodeSetDisproof(node,1);
			deleteChild(node);

		}
		break;
	case TRUE:
		nodeSetProof(node,0);
		nodeSetDisproof(node,INT_MAX);
		break;
	case FALSE:
		nodeSetProof(node,INT_MAX);
		nodeSetDisproof(node,0);
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
		uint oldProof = nodeProof(node);
		uint oldDisproof = nodeDisproof(node);
		setProofAndDisproofNubers(node);
		changed = (oldProof != nodeProof(node)) || (oldDisproof != nodeDisproof(node));
		previousNode = node;
		node = node->parent;
	}
	return previousNode;
}

static inline node_t* selectMostProving(node_t* node){
	while (node->expanded){
		node_t* n = node->child;
		if (node->type == OR){
			while (nodeProof(n) != nodeProof(node)){
				if (n->brother == NULL){
					perror("minimalni proof numer neni");
					//printf("ktere %d %d \n",node->turn,n->turn);
//					if (n
					exit(0);
				}
				n = n->brother;
			}
		} else {
			while (nodeDisproof(n) != nodeDisproof(node)){
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

void proofNuberSearch(node_t* root){
	//printf("root %d %d\n",root->proof,root->disproof);

	currentNode = root;

	int counter = 0;
	while (nodeProof(root) > 0 && nodeDisproof(root) > 0){
		counter++;
		node_t* mostProvingNode = selectMostProving(currentNode);
		developNode(mostProvingNode);
		currentNode = updateAncenors(mostProvingNode);
		if (counter % 10000000 == 0){
			printNode(mostProvingNode);
			printf("hotov node (%d) %u %u\n",mostProvingNode->turn,nodeProof(mostProvingNode),nodeDisproof(mostProvingNode));
			printf("root %u %u\n",nodeProof(root),nodeDisproof(root));
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

