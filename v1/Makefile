

CC=gcc
CFLAGS= -O3 -g -std=gnu99
SRC=$(shell find . -name "*.c")
INCLUDE= -I.
#LIBS=

BUILD:$(SRC)
	$(CC) $(CFLAGS) -o prefetcher $(SRC) $(INCLUDE) $(LIBS)
