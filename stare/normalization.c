#include "normalization.h"
#include "reprezentation.h"

#define swap_uint(a,b) { uint tmp = (a); b = (a); a = (tmp); }
#define TRUE 1
#define FALSE 0

#if EVALUATION == 1
static inline void evaluation_fce(uint m[N][N],uint evaluation[N]){
	//vsechny vrcholy jsou si rovny
	m[0][0]=m[0][0];
	for (int i=0; i<N; i++)
		evaluation[i] = 0;
}
#endif 

#if EVALUATION == 2
static inline void evaluation_fce(uint m[N][N],uint evaluation[N]){
	//vraci stupne vrcholu
	for (int i=0; i<N; i++){
		evaluation[i] = 0u;
		for (int j=0; j<N; j++)
			evaluation[i] += m[i][j];

	}

}
#endif 

#if EVALUATION == 3
static inline void evaluation_fce(uint m[N][N],uint evaluation[N]){
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
#endif 


#if EVALUATION != 0
uint normalization(luint m[4], uint hash){
	//je potreba starat se i o cache
	//prevede m na matici sousednosti
	uint adjacency_matrix[N][N];
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			adjacency_matrix[i][j] = get_edge_color(&m[0],i,j);

	//ohodnoti vrcholy
	uint evaluation[N];
	evaluation_fce(adjacency_matrix,evaluation);

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
			swap_uint(adjacency_matrix[i][x],adjacency_matrix[i][y]);
		//radky
		for (int i=0; i<N; i++)
			swap_uint(adjacency_matrix[x][i],adjacency_matrix[y][i]);
		swap_uint(evaluation[x], evaluation[y]);
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
	hash = 0U;
	for (uint i=0; i<N; i++)
		for (uint j=0; j<N; j++)
			if (adjacency_matrix[i][j]>0)
				hash = set_edge_color(adjacency_matrix[i][j]-1,m,hash,i,j);
	return hash;
	
}
#endif

