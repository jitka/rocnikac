CFLAGS=-std=gnu99  -Wall -W
OPTCFLAGS=$(CFLAGS) -O3 -march=native 
DBGCFLAGS=$(CFLAGS) -ggdb3


all: program-dbg program-opt

program-opt: main.c start.c print.c 
	$(CC) $(OPTCFLAGS) $(LDFLAGS) -o $@ $^

program-dbg: main.c start.c print.c
	$(CC) $(DBGCFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.s program-opt program-dbg tmp/*


