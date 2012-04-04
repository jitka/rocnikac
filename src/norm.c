#include <stdio.h>
#include "norm.h"

int count[1<<N];

void normInit(){

	for (uint m = 0; m < (1<<N); m++){
		int ones = 0;
		for (int i = 0; i < N; i++){
			if ( m & (1<<i) )
				ones++;
		}
		count[m] = ones;
	}

	printf("3-%d 0-%d 8-%d 15-%d",count[3],count[0],count[8],count[15]);
}

void norm(node_t** node){
	int degree[2][N];
	
}
