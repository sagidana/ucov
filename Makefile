CC=gcc
CFLAGS=-g -ggdb -fsanitize-coverage=trace-pc
LIBS=

all: main

trace.o: trace.c
	$(CC) -c -o trace.o trace.c

main: main.c trace.o
	$(CC) -o main main.c trace.o $(CFLAGS)

clean:
	rm -f *.o main

