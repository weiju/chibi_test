ifdef AMIGA
CC=vc +aos68k
CFLAGS=-DAMIGA
else
CC=gcc
CFLAGS=
endif

all: chibi_test

clean:
	rm -f *.o chibi_test

chibi_test: chibi_test.o chibi.o
	$(CC) $(CFLAGS) -o $@ $^
