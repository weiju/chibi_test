ifdef AMIGA
CC=vc +aos68k
CFLAGS=-DAMIGA -c99
else
CC=gcc
CFLAGS=-std=c99 -pedantic
endif

all: chibi_test

clean:
	rm -f *.o chibi_test

test: chibi_test
	./chibi_test

chibi_test: chibi_test.o chibi.o
	$(CC) $(CFLAGS) -o $@ $^
