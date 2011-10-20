#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "config.h"
#include "reprezentation.h"

extern int timer;

void print_config(){
	printf("-------------------------------------\n");
	printf("N=%d, %d vlaken\n",N,THREADS); //TODO tady bude jestli hledam vyhravajici nebo prohravajici
	printf("velikost cache %d stavu\n",1<<CACHE_SIZE);
	printf("normovani pomoci %d s frekvenci %d \n",EVALUATION,NORMALIZATION_FREQUENCY);
	printf("-------------------------------------\n");
}

void binary_luint(luint n){
	for (int i = 0; i < 64; i++){
		if (i%5==0)
			printf(" ");
		printf("%d", !!(n&(1llu<<i)) );
	}
	printf("\n");
}

void binary_uint(uint n){
	for (int i = 0; i < 32; i++){
		if (i%5==0)
			printf(" ");
		printf("%d", !!(n&(1<<i)) );
	}
	printf("\n");
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


