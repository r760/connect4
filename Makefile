# makefile for connect4
#
# make connect4

CC=gcc
CFLAGS=-lncurses

clean:
	rm -rf *.o
	rm -rf connect4

connect4: connect4.c connect4.h
	$(CC) $(CFLAGS) connect4.c -o connect4