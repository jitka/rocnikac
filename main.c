#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "config.h"
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

luint cache[4*(1<<CACHE_SIZE)];
uint cache_numbers[2][(N*(N+1))/2];

static inline int next(int i){
	if (i==GREEN)
		return RED;
	if (i==RED)
		return GREEN;
	return 42;
}

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
	printf("\n");
}

static inline void binary_uint(uint n){
	for (int i = 0; i < 32; i++){
		if (i%5==0)
			printf(" ");
		printf("%d", !!(n&(1<<i)) );
	}
	printf("\n");
}


static inline int set_edge_color(int player, luint m[4], int hash, uint u, uint v){

	//poradove cislo hrany
	if ( u < v )
		swap_uint(&u,&v);
	int pos = (u*(u+1))/2 + v;

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
	//pos*2+player-1 je bit kde ma byt jednicka
	m[(pos*2+player-1)/64] |= (1llu)<<((pos*2+player-1)%64);
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
	if (timer>19)
		return;
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

static inline void evaluation_3(uint m[N][N],uint evaluation[N]){
	//zalozene na trojuhelinikach viz papir u pocitace
	uint green[N],red[N]; //sousedi
	uint g=0,r=0;
	for (int i=0; i<N; i++){
		evaluation[i] = 0u;
		//seznam sousedu danych barev
		for (int j=0; j<N; j++){
			if (j == i)
				break;
			if (m[i][j] == GREEN)
				green[g++] = j;
			if (m[i][j] == GREEN)
				red[r++] = j;
		}
		for (uint x = 0; x < g; x++)
			for (uint y = x+1; y < g; y++)
				switch (m[x][y]){
				case 0: 
					evaluation[i] += 1;
					break;
				case GREEN: 
					evaluation[i] += N;
					break;
				case RED:
					evaluation[i] += N*N;
					break;
				}
		for (uint x = 0; x < g; x++)
			for (uint y = 0; y < r; y++)
				switch (m[x][y]){
				case 0: 
					evaluation[i] += N*N*N;
					break;
				case GREEN: 
					evaluation[i] += N*N*N*N;
					break;
				case RED:
					evaluation[i] += N*N*N*N*N;
					break;
				}
		for (uint x = 0; x < r; x++)
			for (uint y = x+1; y < r; y++)
				switch (m[x][y]){
				case 0: 
					evaluation[i] += N*N*N*N*N*N;
					break;
				case GREEN: 
					evaluation[i] += N*N*N*N*N*N*N;
					break;
				case RED:
					evaluation[i] += N*N*N*N*N*N*N*N;
					break;
				}







	}
}

static inline int normalization(luint m[4]){
	//je potreba starat se i o cache
	//prevede m na matici sousednosti
	uint adjacency_matrix[N][N];
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			adjacency_matrix[i][j] = get_edge_color(&m[0],i,j);

	//ohodnoti vrcholy
	uint evaluation[N];
	evaluation_3(adjacency_matrix,evaluation);

/*	for (int i=0; i<N; i++)
		printf("%d ",evaluation[i]);
	printf("\n");
*/
	//tridit zaroven permutace a matici
	//TODO nebylo by tu rychlejsi nejdriv vymyslet na kolik nejmene
	//a ktera prehozeni to jde?
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
	int hash = 0;
	for (uint i=0; i<N; i++)
		for (uint j=0; j<N; j++)
			if (adjacency_matrix[i][j]>0)
				hash = set_edge_color(adjacency_matrix[i][j]-1,m,hash,i,j);
	return hash;
	
}

static inline void cache_init(){
	for (int i = 0; i < (N*(N+1))/2; i++){
		cache_numbers[0][i] = random()%(1<<CACHE_SIZE);
		cache_numbers[1][i] = random()%(1<<CACHE_SIZE);
	}
}

int threats(int player, luint m[4], int u, int v, int *x, int *y){
	//player pravhe hraje na u,v kolik tim udela hrozeb?
	//pokud jednu tak se jeji misto ulozi do x,y
	//hrozba vypada a) jako dva trojuhelniky mezi jejichz vrcholy neni obarvena hrana
	//nebo b) trojuhelnik prilepeny na stene jineho
	int third[N]; //ktere vrcholy spolu s u,v tvori barevny trojuhernik
	int count = 0;
	int threat = 0;
	for (int i = 0; i < N; i++){
		if (i==u || i==v)
			continue;
		if (get_edge_color(m,u,i) == player && get_edge_color(m,v,i) == player)
			third[count++]=i;
	}
	//a)
	for (int i = 0; i < count; i++)
		for (int j = i+1; j < count; j++)
			if (get_edge_color(m,third[i],third[j]) == 0){
				*x = third[i];
				*y = third[j];
				threat++;
			}
	//b)
	for (int i = 0; i < count; i++)
		for (int j = 0; j < N; j++){
			if (j == u || j == v || j == third[i])
				continue;
			if ( 		get_edge_color(m,third[i],j) == player &&
					get_edge_color(m,u,j) == player &&
					get_edge_color(m,v,j) == 0
					){
				*x = third[i];
				*y = j;
				threat++;
			}
			if ( 		get_edge_color(m,third[i],j) == player &&
					get_edge_color(m,v,j) == player &&
					get_edge_color(m,u,j) == 0
					){
				*x = third[i];
				*y = j;
				threat++;
			}
		}
	return threat;

}

int win(int player, luint m[4], int u, int v){
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

static inline int get_from_cache(luint m[4], int hash){
	//vrati but jak to dopodlo nebo 42 ze to v cachy neni
//	binary_luint(2);
//	binary_luint(~((luint)3<<62)); pro odstraneni vysledku
	if ( 		cache[hash] == m[0] &&
			cache[hash+1] == m[1] &&
			cache[hash+2] == m[2] &&
			( cache[hash+3] & (~((luint)3<<62)) ) == m[3]){
		//je v cachy
		int winner = (cache[hash+3] & ((luint)3<<62)) >> 62;
		if (winner == 2)
			winner = -1;
		return winner;
	}
	return 42;
}

static inline void put_into_cache(luint m[4], int hash, int winner){
	//prevest z 1zeleny vyhral 0remiza -1cerveny na
	//cislo hrace tedy 0nikdo 1zeleny 2cerveny
	if (winner == -1)
		winner = 2;
	//prekopiruju graf
	for (int i = 0; i < 4; i++)
		cache[hash+i]=m[i];
	//na konec pridam vysledek
	cache[hash+3] |= ((luint) winner) << 62;
}

int minimax(int player, luint m[4], int depth, int hash);
int minimax_threat(int player, luint m[4], int depth, int hash, int u, int v);

int minimax_threat(int player, luint m[4], int depth, int hash, int u, int v){
	//minimax ale player musí hrat na hranu (u,v) ktera je volna
//	print_adjacency_matrix(m,"");

	luint m2[4];
	int hash2;

	if ( NORMALIZATION_FREQUENCY > 0 && depth % NORMALIZATION_FREQUENCY == 0) 
		hash = normalization(m);

	int winner;
	if ( (winner = get_from_cache(m,hash) ) != 42){
//	if ( depth % 3 == 1 && (winner = get_from_cache(m,hash) ) != 42 ) {
		//je v cachy
		return winner;
	}

	if (depth == (N*(N-1))/2 )
		//vse je obarvene remiza
		return 0;

	int x,y;
	int t = threats(player,m,u,v,&x,&y);
	if (t > 1){
		if (player == GREEN)
			return 1;
		else
			return -1;
	}
	copy_graph(m2,m);
	hash2 = set_edge_color(player,m2,hash,u,v);

	if (t == 1){
		winner = minimax_threat(next(player),m2,depth+1,hash2,x,y);
	} else {
		winner = minimax(next(player),m2,depth+1,hash2);
	}

	if ( depth % 3 == 1 ) 
		put_into_cache(m2,hash2,winner);	
	return winner;
}


int minimax(int player, luint m[4], int depth, int hash){
	//1 zeleny vyhraje 
	//0 remiza obarvene bez k4 
	//-1 cerveny vyhraje
//	print_adjacency_matrix(m,"");
	int max = -1;
	int min = 1;
	luint m2[4];
	int hash2 = 0;

	if ( NORMALIZATION_FREQUENCY > 0 && depth % NORMALIZATION_FREQUENCY == 0) 
		hash = normalization(m);

	int winner;
	if ( (winner = get_from_cache(m,hash) ) != 42 && depth > 0){
//	if ( depth % 3 == 1 && (winner = get_from_cache(m,hash) ) != 42 ) {
		//je v cachy a zaroven neni prazdny
		return winner;
	}
	if ( depth == (N*(N-1))/2 )
		//vse je obarvene remiza
		return 0;

	for (uint i=0; i<N; i++)
		for (uint j=i+1; j<N; j++){
			if (get_edge_color(m,i,j) == 0){
				//pro vsechny neobarvene hrany (i,j)
				int x,y;
				int t = threats(player,m,i,j,&x,&y);
				if (t > 1){
					if (player == GREEN)
						return 1;
					else
						return -1;
				}
/*
				if (win(player,m,i,j)){
					if (player == GREEN)
						return 1;
					else
						return -1;
				}				
*/				copy_graph(m2,m);
				hash2 = set_edge_color(player,m2,hash,i,j);
				int tmp;
				if (t == 1){
					tmp = minimax_threat(next(player),m2,depth+1,hash2,x,y);
				} else {
					tmp = minimax(next(player),m2,depth+1,hash2);
				}
				if (tmp > max)
					max = tmp;
				if (tmp < min)
					min = tmp;
			}

				
		}
	if (player == GREEN)
		winner = max;
	else
		winner = min;

	if ( depth % 3 == 1 ) 
		put_into_cache(m2,hash2,winner);	
	return winner;
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

//	int x,y;
	luint m[4];
	for (int i=0; i<4; i++){
		m[i]=0llu;
	}

	printf("N=%d NORMALIZATION_FREQUENCY=%d vysl: %d\n",N,NORMALIZATION_FREQUENCY,minimax(GREEN,m,0,0));
/*	
	for (int i=0; i<4; i++){
		m[i]=0llu;
	}
	set_edge_color(GREEN,m,0,1,2);
//	set_edge_color(GREEN,m,0,0,4);
	set_edge_color(RED,m,0,4,2);
	set_edge_color(RED,m,0,0,1);
	set_edge_color(RED,m,0,0,3);
	set_edge_color(RED,m,0,5,4);
	set_edge_color(RED,m,0,4,3);
	set_edge_color(RED,m,0,0,2);
	set_edge_color(RED,m,0,0,5);
	print_adjacency_matrix(m,"");
//	printf("%d - ",threats(RED,m,2,5,&x, &y));
//	printf("%d %d\n",x,y);

	luint m2[4];
	for (int i=0; i<4; i++){
		m2[i]=0llu;
	}
	set_edge_color(RED,m2,0,1,4);
	set_edge_color(RED,m2,0,3,5);
	set_edge_color(RED,m2,0,2,4);
	set_edge_color(RED,m2,0,0,1);
	set_edge_color(RED,m2,0,5,3);
	set_edge_color(RED,m2,0,2,1);
	print_adjacency_matrix(m2,"");

	int x;
	put_into_cache(m,42,0);
	x=get_from_cache(m,42);
	printf("vysl %d\n",x);
	put_into_cache(m,2,1);
	x=get_from_cache(m,2);
	printf("vysl %d\n",x);
	put_into_cache(m,4,-1);
	x=get_from_cache(m,4);
	printf("vysl %d\n",x);
	put_into_cache(m,4,-1);
	x=get_from_cache(m,2);
	printf("vysl %d\n",x);
	put_into_cache(m,4,-1);
	x=get_from_cache(m2,4);
	printf("vysl %d\n",x);

	for (int i=0; i<4; i++){
		m[i]=0llu;
	}
	set_edge_color(RED,m,0,1,4);
	set_edge_color(RED,m,0,3,5);
	set_edge_color(RED,m,0,2,4);
	print_adjacency_matrix(m,"");
*/

	return 0;
}
