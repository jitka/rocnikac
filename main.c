#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define N 14
#define TRUE 1
#define FALSE 0
#define GREEN 0
#define RED 1

typedef uint64_t luint;
int timer=0;


static inline void set_edge_color(int player, luint old[4], luint new[4], int u, int v){
	//zeleny 10 cerveny 01 zadna 00
	for (int i = 0; i < 4; i++)
		new[i]=old[i];
	if ( u < v ) {
		int tmp = v;
		v = u;
		u = tmp;
	}
	int pos = ((u*(u+1))/2 + v) * 2 + player;
//	printf("%d %d %d\n",u,v,pos);
	new[pos/64] |= (1ull)<<(pos%64);
}

static inline int get_edge_color(luint m[4], int u, int v){
	//return 0 zadna 1 zelena 2 cervena
	int pos;
	if (u > v){
		pos = ((u*(u+1))/2 + v) * 2;
	} else {
		pos = ((v*(v+1))/2 + u) * 2;
	}
	if ( m[pos/64] & (1llu<<(pos%64)) ){
		//hrana ma zelenou barvu
		return 1;
	} else if ( m[(pos+1)/64] & (1llu<<((pos+1)%64)) ){
		//hrana ma cervenou barvu
		return 2;
	} else {
		//neobarvena hrana
		return 0;
	}	
}

void print_adjacency_matrix(luint m[4],char *text){
	char title[100];
	sprintf(title,"tmp/%d%s",timer,text);
	FILE *F = fopen(title,"w");
	fprintf(F,"%d\n",N);
//	printf("nula: %llu\n",m[0]);
	for (int i=0; i<N; i++){
		for (int j=0; j<N; j++){
			fprintf(F,"%d ",get_edge_color(&m[0],i,j));
		}
		fprintf(F,"\n");
	}
	fclose(F);
}


int main(){

	luint m[4];
	luint m2[4];
	for (int i=0; i<4; i++){
		m[i]=0llu;
		m2[i]=0llu;
	}
	
	print_adjacency_matrix(m,"");

	set_edge_color(GREEN,m,m2,2,3);
	timer++;
	print_adjacency_matrix(m2,"");

	set_edge_color(RED,m2,m,4,2);
	timer++;
	print_adjacency_matrix(m,"");

	set_edge_color(RED,m2,m,12,4);
	timer++;
	print_adjacency_matrix(m,"");

	//ma zelny=prvni zarucenou vyhru?
/*	if (green_plays(m) == GREEN)
		printf("ve hre na K%d ma prvni vyhravajici strategii\n",N);
	else
		printf("ve hre na K%d ma druhy neprohravajici strategii\n",N);
		
	return 0;
*/
}
