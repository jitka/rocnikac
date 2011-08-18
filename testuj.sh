
for i in `seq 50` 
do
	echo -e "#define N 5\n#define CACHE_SIZE 20\n#define NORMALIZATION_FREQUENCY $i" > config.h 
	make && time ./program-opt
done
