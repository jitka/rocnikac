# Spustit jako:
#   N=6 make varianta1
# a pak
#   ./program-opt
# nebo
#   ./program-dbg

N?=6
CFLAGS=-std=gnu99 -Wall -W -DN=$(N) $(OPTS)
OPTCFLAGS=$(CFLAGS) -O3 -march=native 
DBGCFLAGS=$(CFLAGS) -ggdb3


all: program-dbg program-opt

program-opt: main.c start.c print.c pnsearch.c linkedlist.c norm.c tabs.c stats.c cache.c 
	$(CC) $(OPTCFLAGS) $(LDFLAGS) -o $@ $^

program-dbg: main.c start.c print.c pnsearch.c linkedlist.c norm.c tabs.c stats.c cache.c
	$(CC) $(DBGCFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.s program-opt program-dbg tmp/*

varianta1:
	OPTS="-DDEBUG -DWEAK -DSTATS -DNORM2 -DTURNDDELETECHILDRENST=0 -DNOFREEK4 -DUPDATEANCESTORS2"\
	 make clean all
