#include <stdlib.h>
#include "setting.h"

u32 hashNumbers[2][N][N]; //pro kazdou barvu a hranu
u32 hashNumbers2[2][N][1<<N]; //pro kazdou barvu, vrchol a _mensi_ sousedy radky
//u32 hashNumbers3[2][N][1<<N]; //pro kazdou barvu, vrchol a _mensi_ sousedy sloupce
int count[1<<N];
u64 N1s;
u64 R1s;

void tabsInit(){
	for (int i = 0; i < N; i++){
		for (int j = 0; j < i; j++){
			hashNumbers[0][i][j] = random() % CACHE_SIZE;
			hashNumbers[0][j][i] = hashNumbers[0][i][j];
			hashNumbers[1][i][j] = random() % CACHE_SIZE;
			hashNumbers[1][j][i] = hashNumbers[1][i][j];
		}
	}
	for (int i = 0; i < N; i++){
		for (u32 s = 0; s < (1<<N); s++){
			int hash0 = 0;
			int hash1 = 0;
			for (int j = 0; j < i; j++){
				if ( s & (1<<j) ){
					hash0 ^= hashNumbers[0][i][j];
					hash1 ^= hashNumbers[1][i][j];
				}
			}
			hashNumbers2[0][i][s] = hash0;
			hashNumbers2[1][i][s] = hash1;
		}
	}

	for (uint m = 0; m < (1<<N); m++){
		int ones = 0;
		for (int i = 0; i < N; i++){
			if ( m & (1<<i) )
				ones++;
		}
		count[m] = ones;
	}


	N1s = ((1ULL<<N)-1ULL);
	R1s = 0;
	for (int i = 0; i < N; i++){
		R1s ^= (1<<i*N);
	}
}


