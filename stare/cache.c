#include <stdio.h>
#include <pthread.h>

#include "config.h"
#include "types.h"

luint cache[4*(1<<CACHE_SIZE)];
uint hash_numbers[2][(N*(N+1))/2];

#if THREADS > 0
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#if THREADS > 0
#endif

static inline void put_into_cache_(luint m[4], int hash, int winner){
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


static inline int get_from_cache_(luint m[4], int hash){
	//vrati but jak to dopadlo nebo 42 ze to v cachy neni
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

void put_into_cache(luint m[4], int hash, int winner){
#if THREADS > 0
	if (pthread_mutex_lock(&mutex))
		perror("problem mutex");
#endif
	put_into_cache_(m, hash, winner);
#if THREADS > 0
	if (pthread_mutex_unlock(&mutex))
		perror("problem mutex");
#endif
}

int get_from_cache(luint m[4], int hash){
#if THREADS > 0
	if (pthread_mutex_lock(&mutex))
		perror("problem mutex");
#endif
	int ret = get_from_cache_(m, hash);
#if THREADS > 0
	if (pthread_mutex_unlock(&mutex))
		perror("problem mutex");
#endif
	return ret;
}

