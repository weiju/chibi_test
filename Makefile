ifdef AMIGA
CC=vc +aos68k
CFLAGS=-DAMIGA -c99
else
CC=gcc
CFLAGS=-std=c99 -pedantic -ftest-coverage -fprofile-arcs
LDFLAGS=
endif

all: chibi_test

clean:
	rm -f *.o chibi_test *.gcda *.gcno

test: chibi_test
	./chibi_test

chibi_test: chibi_test.o chibi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

coverage: test
	gcovr -r . --xml-pretty -o coverage.xml
