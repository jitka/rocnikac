#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define N 17
#define TRUE 1
#define FALSE 0
#define GREEN 0
#define RED 1


typedef uint64_t luint;
int timer=0;

#if N > 16
//5-ciferne cisla v dvojkove a 3-ciferne v trojkove soustave
uint short_to_long[32];
uint long_to_short[64];
#endif

static inline void swap_uint(uint *x, uint *y){
	uint tmp = *x;
	*x = *y;
	*y = tmp;
}

static inline void binary_luint(luint n){
	for (int i = 0; i < 64; i++){
		if (i%5==0)
			printf(" ");
		printf("%d", !!(n&(1llu<<i)) );
	}
}

static inline void binary_uint(uint n){
	for (int i = 0; i < 32; i++)
		printf("%d", !!(n&(1<<i)) );
}

static inline void set_edge_color(int player, luint old[4], luint new[4], uint u, uint v){
	//zeleny 10 cerveny 01 zadna 00
	for (int i = 0; i < 4; i++)
		new[i]=old[i];
	if ( u < v )
		swap_uint(&u,&v);
	int pos = (u*(u+1))/2 + v;
#if N > 16
	int pos5 = (pos/3)*5; //zacatek klicovich peti bitu
	uint o = 0; //tech pet klicovich bitu
	for (int i = 0; i < 5; i++)
		o += (uint) (1llu << i) * (!!( old[(pos5+i)/64] & (1llu<<((pos5+i)%64)) ));
	uint n; //prepsanych pet klicovych bitu
	n = long_to_short[ short_to_long[o] | (1llu << ((pos%3)*2+player)) ];
	for (int i = 0; i < 5; i++)
		new[(pos5+i)/64] |= ( (luint) !!(n & (1<<i)) ) * (1llu<<((pos5+i)%64)) ;
#else
	new[(pos*2+player)/64] |= (1llu)<<((pos*2+player)%64);
#endif
}

static inline int get_edge_color(luint m[4], int u, int v){
	//return 0 zadna 1 zelena 2 cervena
	int pos; //poradi dane hrany
	if (u > v){
		pos = (u*(u+1))/2 + v;
	} else {
		pos = (v*(v+1))/2 + u;
	}
#if N > 16
	int pos5 = (pos/3)*5; //zacatek klicovich peti bitu
	luint x = 0; //tech pet klicovich bitu
	for (int i = 0; i < 5; i++)
		x += (1llu<<i) * (!!( m[(pos5+i)/64] & (1llu<<((pos5+i)%64)) ));
	return ( ((luint) short_to_long[(uint) x]) >> ((pos%3)*2) ) & 3;
#else
	return ( m[(pos*2)/64] >> ((pos*2)%64) ) & 3;
#endif
}

void print_adjacency_matrix(luint m[4],char *text){
	char title[100];
	sprintf(title,"tmp/%d%s",timer,text);
	FILE *F = fopen(title,"w");
	fprintf(F,"%d\n",N);
	for (int i=0; i<N; i++){
		for (int j=0; j<N; j++){
			fprintf(F,"%d ",get_edge_color(&m[0],i,j));
		}
		fprintf(F,"\n");
	}
	fclose(F);
}

static inline void evaluation_1(uint m[N][N],uint evaluation[N]){
	m[0][0]=m[0][0];
	for (int i=0; i<N; i++)
		evaluation[i] = 0;
}

static inline void evaluation_2(uint m[N][N],uint evaluation[N]){
	for (int i=0; i<N; i++){
		evaluation[i] = 0u;
		for (int j=0; j<N; j++)
			evaluation[i] += m[i][j];

	}
}

static inline void normalization(luint m[4]){
	//prevede m na matici sousednosti
	uint adjacency_matrix[N][N];
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			adjacency_matrix[i][j] = get_edge_color(&m[0],i,j);

	//ohodnoti vrcholy
	uint evaluation[N];
	evaluation_2(adjacency_matrix,evaluation);

	for (int i=0; i<N; i++)
		printf("%d ",evaluation[i]);
	printf("\n");

	//tridit zaroven permutace a matici
	inline void swap(int x, int y){
		//sloupce
		for (int i=0; i<N; i++)
			swap_uint(&adjacency_matrix[i][x],&adjacency_matrix[i][y]);
		//radky
		for (int i=0; i<N; i++)
			swap_uint(&adjacency_matrix[x][i],&adjacency_matrix[y][i]);
		swap_uint(&evaluation[x], &evaluation[y]);
	}
	int unsorted = TRUE;
	for (int i = 0; i<N && unsorted; i++){
		unsorted = FALSE;
		for (int j=i+1; j<N; j++){
			if (evaluation[i] > evaluation[j])
				swap(i,j);
			if (evaluation[j-1] > evaluation[j])
				unsorted = TRUE;
		}
	}
	
	
	//prevest zpet
	for (int i=0; i<4; i++)
		m[i]=0llu;
	for (uint i=0; i<N; i++)
		for (uint j=0; j<N; j++)
			if (adjacency_matrix[i][j]>0)
				set_edge_color(adjacency_matrix[i][j]-1,&m[0],&m[0],i,j);
	
}

int main(){

#if N > 16
	//naplneni prevodnich tabulek mezi soustavami
	for (int i = 0; i < 32; i++)
		short_to_long[i] = (i%3) + (((i/3)%3)<<2) + ((i/9)<<4);
	for (int i = 0; i < 64; i++)
		long_to_short[i] = (i&3) + ((i>>2)&3)*3 + ((i>>4)&3)*9;
#endif

	luint m[4];
	luint m2[4];
	for (int i=0; i<4; i++){
		m[i]=0llu;
		m2[i]=0llu;
	}
	
	print_adjacency_matrix(m,"");

	set_edge_color(GREEN,m,m2,6,2);
	timer++;
	print_adjacency_matrix(m2,"");
	
	set_edge_color(RED,m2,m,4,2);
	timer++;
	print_adjacency_matrix(m,"");

	set_edge_color(RED,m2,m,12,2);
	timer++;
	print_adjacency_matrix(m,"");

	normalization(m);
	timer++;
	print_adjacency_matrix(m,"");

	return 0;
}
