CC=gcc
COMMON_CFLAGS=-fPIC
CFLAGS=-O0 -g -ggdb -fsanitize-coverage=trace-pc
LIBS=

all: lib.so main

main: main.c lib.so
	$(CC) -o main main.c

lib.so: lib.c ucov.o
	$(CC) -o lib.so lib.c ucov.o $(CFLAGS) $(COMMON_CFLAGS) -shared

ucov.o: ucov.c
	$(CC) -c -o ucov.o ucov.c $(COMMON_CFLAGS)

clean:
	rm -f lib.so *.o

