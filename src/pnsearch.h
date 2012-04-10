#ifndef PNSEARCH_H
#define PNSEARCH_H

#include "struct.h"

void hashInit();
extern u32 hashNumbers[2][N][N]; //pro kazdou barvu a hranu
extern u32 hashNumbers2[2][N][1<<N]; //pro kazdou barvu, vrchol a sousedy
nodeValue_t proofNuberSearch(node_t* node);

#endif
