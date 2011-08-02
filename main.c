#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define N 6
#define CACHE_SIZE 20//logaritmus velikosti 20..33M 25..1G
#define TRUE 1
#define FALSE 0
#define GREEN 1
#define RED 2


typedef uint64_t luint;
int timer=0;

#if N > 16
//5-ciferne cisla v dvojkove 
//a 3-ciferne v trojkove soustave zapsane do sesti bitu
uint short_to_long[32];
uint long_to_short[64];
#endif

luint cache[1<<CACHE_SIZE];
uint cache_numbers[2][(N*(N+1))/2];

static inline void swap_uint(uint *x, uint *y){
	uint tmp = *x;
	*x = *y;
	*y = tmp;
}

static inline void copy_graph(luint m1[4], luint m2[4]){
	for (int i = 0; i < 4; i++)
		m1[i] = m2[i];
}

static inline void binary_luint(luint n){
	for (int i = 0; i < 64; i++){
		if (i%5==0)
			printf(" ");
		printf("%d", !!(n&(1llu<<i)) );
	}
}

static inline void binary_uint(uint n){
	for (int i = 0; i < 32; i++){
		if (i%5==0)
			printf(" ");
		printf("%d", !!(n&(1<<i)) );
	}
}


static inline int set_edge_color(int player, luint m[4], int hash, uint u, uint v){
	//poradove cislo hrany
	if ( u < v )
		swap_uint(&u,&v);
	int pos = (u*(u+1))/2 + v;
	printf("%d-%d pos%d\n",u,v,pos);
	//zeleny 10 cerveny 01 zadna 00
#if N > 16
	int pos5 = (pos/3)*5; //zacatek klicovich peti bitu
	uint o = 0; //tech pet klicovich bitu
	for (int i = 0; i < 5; i++)
		o += (uint) (1llu << i) * (!!( m[(pos5+i)/64] & (1llu<<((pos5+i)%64)) ));
	uint n; //prepsanych pet klicovych bitu
	n = long_to_short[ short_to_long[o] | (1llu << ((pos%3)*2+player)) ];
	for (int i = 0; i < 5; i++)
		m[(pos5+i)/64] |= ( (luint) !!(n & (1<<i)) ) * (1llu<<((pos5+i)%64)) ;
#else
	m[(pos*2+player)/64] |= (1llu)<<((pos*2+player)%64);
#endif

	//nova hodnota hasovaci fce TODO T
	return hash ^ cache_numbers[player-1][pos];
}

static inline int get_edge_color(luint m[4], uint u, uint v){
	//return 0 zadna 1 zelena 2 cervena
	
	//poradove cislo hrany
	if ( u < v )
		swap_uint(&u,&v);
	int pos = (u*(u+1))/2 + v;

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
	sprintf(title,"tmp/%d%s",timer++,text);
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
	//vsechny vrcholy jsou si rovny
	m[0][0]=m[0][0];
	for (int i=0; i<N; i++)
		evaluation[i] = 0;
}

static inline void evaluation_2(uint m[N][N],uint evaluation[N]){
	//vraci stupne vrcholu
	for (int i=0; i<N; i++){
		evaluation[i] = 0u;
		for (int j=0; j<N; j++)
			evaluation[i] += m[i][j];

	}
}

static inline void normalization(luint m[4]){
	//TODO netestovane
	//prevede m na matici sousednosti
	uint adjacency_matrix[N][N];
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			adjacency_matrix[i][j] = get_edge_color(&m[0],i,j);

	//ohodnoti vrcholy
	uint evaluation[N];
	evaluation_2(adjacency_matrix,evaluation);

/*	for (int i=0; i<N; i++)
		printf("%d ",evaluation[i]);
	printf("\n");
*/
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
				set_edge_color(adjacency_matrix[i][j]-1,m,0,i,j);
	
}

static inline void cache_init(){
	for (int i = 0; i < (N*(N+1))/2; i++){
		cache_numbers[0][i] = random()%(1<<CACHE_SIZE);
		cache_numbers[1][i] = random()%(1<<CACHE_SIZE);
	}
}

int number_of_threat(int player,luint m[4], int u, int v){
	//TODO netestovane, zatim nepouzivane, melo by to ale vracet i kde
	player++; //aby cislo hrace odpovidalo barve jeho hrany
	int third[N]; //ktere vrcholy spolu s u,v tvori barevny troj
	int count = 0;
	for (int i = 0; i < N; i++){
		if (i==u || i==v)
			continue;
		if (get_edge_color(m,u,i) == player && get_edge_color(m,v,i) == player)
			third[count++]=i;
	}
	int threat = 0;
	while (count > 0){
		//z K4 chybi ta naproti
		for (int i = 0; i < count-1; i++)
			if (get_edge_color(m,third[i],third[count]) == 0){
				//druhy tam musi hrat
				threat++;
			}
		//z K4 chybi hrana na boku
		for (int i = 0; i < N; i++){
			if (i==third[count])
				continue;
			if (get_edge_color(m,u,i) == player 
			   && get_edge_color(m,third[count],i) == player
			   && get_edge_color(m,v,i) == 0)
				threat++;
			if (get_edge_color(m,v,i) == player 
			   && get_edge_color(m,third[count],i) == player
			   && get_edge_color(m,u,i) == 0)
				threat++;
		}
		count--;
	}
	return threat;
}

int win(int player, luint m[4], int u, int v){
	//TODO netestovane, pouziva se
	player++; //aby cislo hrace odpovidalo barve jeho hrany
	int third[N]; //ktere vrcholy spolu s u,v tvori barevny trojuhernik
	int count = 0;
	for (int i = 0; i < N; i++){
		if (i==u || i==v)
			continue;
		if (get_edge_color(m,u,i) == player && get_edge_color(m,v,i) == player)
			third[count++]=i;
	}
	for (int i = 0; i < count; i++)
		for (int j = i+1; j < count; j++)
			if (get_edge_color(m,third[i],third[j]) == player)
				return TRUE;
	return FALSE;
}

int minimax(int player, luint m[4], int depth, int hash){
	//TODO netestovane
	int max = -1;
	int min = 1;
	luint m2[4];
	int hash2;
	if (depth == (N*(N-1))/2 )
		return 0;
	for (uint i=0; i<N; i++)
		for (uint j=i+1; j<N; j++){
			if (get_edge_color(m,i,j) == 0){
//	print_adjacency_matrix(m,"");
//	normalization(m);
//	timer++;
				//nepouzita hrana zkusim ji
				for (int i = 0; i < 4; i++)
					m2[i]=m[i];
				//TODO mam hrozbu? kolik?
				hash2 = set_edge_color(player,m2,hash,i,j);
				if (win(player,m2,i,j)){
					if (player == GREEN)
						return 1;
					else
						return -1;
				}
				int tmp = minimax((player+1)%2,m2,depth+1,hash);
				if (tmp > max)
					max = tmp;
				if (tmp < min)
					min = tmp;
			}

				
		}
	switch (player) {
		case GREEN: return min;
			    break;
		case RED: return max;
			  break;
	}
	return 42;
	//retur 1 zeleny vyhraje 0 remiza obarvene bez k4 -1 cerveny vyhraje
}

int main(){

#if N > 16
	//naplneni prevodnich tabulek mezi soustavami
	for (int i = 0; i < 32; i++)
		short_to_long[i] = (i%3) + (((i/3)%3)<<2) + ((i/9)<<4);
	for (int i = 0; i < 64; i++)
		long_to_short[i] = (i&3) + ((i>>2)&3)*3 + ((i>>4)&3)*9;
#endif
	cache_init();

	luint m[4];
	for (int i=0; i<4; i++){
		m[i]=0llu;
	}

//	printf("vysl %d\n",minimax(GREEN,m,0,0));


	set_edge_color(GREEN,m,0,1,2);
//	set_edge_color(RED,m,0,4,2);
//	set_edge_color(RED,m,0,0,1);
//	set_edge_color(RED,m,0,0,3);
//	set_edge_color(RED,m,0,5,4);
//	set_edge_color(RED,m,0,0,6);
	set_edge_color(RED,m,0,4,3);
	print_adjacency_matrix(m,"pred");

//	normalization(m);
//	print_adjacency_matrix(m,"po");

	return 0;
}
