#ifndef REPREZENTATION_H_
#define REPREZENTATION_H_

#include "cache.h" //hash_numbers

#if N > 16
//5-ciferne cisla v dvojkove 
//a 3-ciferne v trojkove soustave zapsane do sesti bitu
uint short_to_long[32];
uint long_to_short[64];
#endif

static inline int set_edge_color(int player, luint m[4], int hash, uint u, uint v){

	if ( u < v ){
		luint tmp = u;
		u=v;
		v=tmp;
	}
	//poradove cislo hrany
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
	return hash ^ hash_numbers[player-1][pos];
}

static inline int get_edge_color(luint m[4], uint u, uint v){
	//return 0 zadna 1 zelena 2 cervena
	
	if ( u < v ){
		luint tmp = u;
		u=v;
		v=tmp;
	}
	//poradove cislo hrany
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


#endif

