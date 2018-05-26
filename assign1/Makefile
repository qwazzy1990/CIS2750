CFLAGS = -std=c11 -Wall -pedantic -Iinclude -g
CC = gcc
C = -c
src = ./src/
bin = ./bin/

all: list parser

Test:	
	$(CC) $(CFLAGS) src/LinkedListAPI.c src/GEDCOMparser.c src/GEDCOMutilities.c src/testMain.c -o bin/test.o

LinkedListAPI.o: src/LinkedListAPI.c
	$(CC) $(CFLAGS) $(C) src/LinkedListAPI.c -o bin/LinkedListAPI.o
 
GEDCOMparser.o: src/GEDCOMparser.c	
	$(CC) $(CFLAGS) $(C) src/GEDCOMparser.c -o bin/GEDCOMparser.o

GEDCOMutilities.o: src/GEDCOMutilities.c
	$(CC) $(CFLAGS) $(C) src/GEDCOMutilities.c -o bin/GEDCOMutilities.o

list: LinkedListAPI.o
	ar cr bin/liblist.a bin/LinkedListAPI.o

parser: GEDCOMparser.o GEDCOMutilities.o LinkedListAPI.o
	ar cr bin/libparse.a bin/GEDCOMutilities.o bin/GEDCOMparser.o bin/LinkedListAPI.o

test:	
	gcc -Wall -std=c11 -g src/GEDCOMutilities.c src/GEDCOMparser.c src/LinkedListAPI.c src/testMain.c -Iinclude -o bin/test

clean:
	rm -f bin/*.o bin/* .a
