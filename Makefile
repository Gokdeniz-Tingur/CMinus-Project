#
# Makefile for C-
# Gnu C Version
#

CC = gcc

CFLAGS = -Wall

OBJS = main.o util.o scan.o parse.o

cminus: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o cminus

main.o: main.c globals.h util.h scan.h parse.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

scan.o: scan.c scan.h util.h globals.h
	$(CC) $(CFLAGS) -c scan.c

parse.o: parce.c parse.h globals.h scan.h util.h
	$(CC) $(CFLAGS) -c parse.c

clean:
	-rm cminus $(OBJS)

run: cminus
	./cminus $(CURDIR)/gcd.cm

test: cminus
	./cminus $(CURDIR)/$(CM_FILE)

all: cminus
