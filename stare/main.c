#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "types.h"
#include "config.h"
#include "cache.h"
#include "print.h"



void hash_init(){
	for (int i = 0; i < (N*(N+1))/2; i++){
		hash_numbers[0][i] = random()%(1<<CACHE_SIZE);
		hash_numbers[1][i] = random()%(1<<CACHE_SIZE);
	}
}

void reprezentation_init(){
#if N > 16
	//naplneni prevodnich tabulek mezi soustavami
	for (int i = 0; i < 32; i++)
		short_to_long[i] = (i%3) + (((i/3)%3)<<2) + ((i/9)<<4);
	for (int i = 0; i < 64; i++)
		long_to_short[i] = (i&3) + ((i>>2)&3)*3 + ((i>>4)&3)*9;
#endif
}


int main(){

	reprezentation_init();
	hash_init();

//	int x,y;
	luint m[4];
	for (int i=0; i<4; i++){
		m[i]=0llu;
	}

	print_config();
	printf("vyhral: %d\n",minimax(GREEN,m,0,0));
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
