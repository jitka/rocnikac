#include <stdlib.h>
#include <stdio.h>
#define N 5
#define GREEN 1
#define RED 2

int timer=0;


void print_adjacency_matrix(int player, int m[N][N], char * text){
	char title[100];
       	if (player==GREEN)
		sprintf(title,"tmp/%d-green%s",timer,text);
       	if (player==RED)
		sprintf(title,"tmp/%d-red%s",timer,text);
	FILE *F = fopen(title,"w");
//	printf("%s\n",title);
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
	timer++;
}

int main(){

	int m[N][N]; //používá se ta část kde je první souřadnice vyšší
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			m[i][j]=0;
	
	print_adjacency_matrix(RED,m,"");
	color_edge(GREEN,m,2,3);
	print_adjacency_matrix(GREEN,m,"");
	color_edge(RED,m,4,1);
	print_adjacency_matrix(RED,m,"");

	return 0;
}
