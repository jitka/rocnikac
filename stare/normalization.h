#include "types.h"
#include "config.h"


#if EVALUATION == 0
static inline uint normalization(luint m[4], uint hash){
	(void) m;
	return hash;
}

#else
uint normalization(luint m[4], uint hash);

#endif
