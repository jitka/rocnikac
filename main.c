#include <stdlib.h>
#include <stdio.h>
#define N 5
#define TRUE 1
#define FALSE 0
#define NONE 0
#define GREEN 1 //zeleny zacina
#define RED 2

int timer=0;

void print_adjacency_matrix(int m[N][N],char *text){
	char title[100];
	sprintf(title,"tmp/%d%s",timer++,text);
	FILE *F = fopen(title,"w");
	fprintf(F,"%d\n",N);
	for (int i=0; i<N; i++){
		for (int j=0; j<N; j++)
			fprintf(F,"%d ",m[i][j]+1);
		fprintf(F,"\n");
	}
	fclose(F);
}

void color_edge(int player, int m[N][N], int u, int v){
	m[u][v]=player;
	m[v][u]=player;
}

int has_four(int player, int m[N][N]){
	return TRUE;
}

int red_plays(int m[N][N]){
	return RED;
}

int green_plays(int m[N][N]){
	for (int i=0; i<N; i++)
		for (int j=i+1; j<N; j++)
			if (m[N][N]==NONE){
				color_edge(GREEN,m,i,j);
				if (has_four(GREEN,m)){
					color_edge(NONE,m,i,j);
					return GREEN;
				}
				if (red_plays(m)==GREEN)
					return GREEN;
			}
}

int main(){

	int m[N][N];
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			m[i][j]=NONE;
/*	
	print_adjacency_matrix(m,"");
	color_edge(GREEN,m,2,3);
	print_adjacency_matrix(m,"");
	color_edge(RED,m,4,1);
	print_adjacency_matrix(m,"");
*/

	//ma zelny=prvni zarucenou vyhru?
	if (green_plays(m) == GREEN)
		printf("ve hre na K%d ma prvni vyhravajici strategii\n",N);
	else
		printf("ve hre na K%d ma druhy neprohravajici strategii\n",N);
		
	return 0;
}
