ifdef AMIGA
CC=vc +aos68k
CFLAGS=-DAMIGA -c99
else
CC=gcc
CFLAGS=-std=c99 -pedantic -ftest-coverage -fprofile-arcs
LDFLAGS=
ts := $(shell /bin/date "+%Y%m%d%H%M%S")
endif

all: chibi_test

clean:
	rm -rf *.o chibi_test *.gcda *.gcno test-reports coverage.xml

test: chibi_test
	./chibi_test xml

chibi_test: chibi_test.o chibi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

coverage: test
	gcovr -r . --xml-pretty -o coverage.xml
