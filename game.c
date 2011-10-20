#include "game.h"
#include "config.h"
#include "normalization.h"
#include "reprezentation.h"
#include "cache.h"

#define TRUE 1
#define FALSE 0

int timer=0;


static inline int next(int i){
	if (i==GREEN)
		return RED;
	if (i==RED)
		return GREEN;
	return 42;
}

static inline void copy_graph(luint m1[4], luint m2[4]){
	for (int i = 0; i < 4; i++)
		m1[i] = m2[i];
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

int minimax(int player, luint m[4], int depth, int hash);
int minimax_threat(int player, luint m[4], int depth, int hash, int u, int v);

int minimax_threat(int player, luint m[4], int depth, int hash, int u, int v){
	//minimax ale player musí hrat na hranu (u,v) ktera je volna
//	print_adjacency_matrix(m,"");

timer++;
	luint m2[4];
	int hash2;

	if ( NORMALIZATION_FREQUENCY > 0 && depth % NORMALIZATION_FREQUENCY == 0) 
		hash = normalization(m,hash);

	int winner;
	if ( (winner = get_from_cache(m,hash) ) != 42){
//	if ( depth % 3 == 1 && (winner = get_from_cache(m,hash) ) != 42 ) {
		//je v cachy
		return winner;
	}

	if (depth == (N*(N-1))/2 )
		//vse je obarvene remiza
		//TODO diky poctu zbylych hran a tomu jake jsou hrozby by mohlo jit skoncit driv
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
timer++;
	int max = -1;
	int min = 1;
	luint m2[4];
	int hash2 = 0;

	if ( NORMALIZATION_FREQUENCY > 0 && depth % NORMALIZATION_FREQUENCY == 0) 
		hash = normalization(m,hash);

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
