

CC=gcc
CFLAGS= -static -Wall -g -std=gnu99
SRC=$(shell find . -name "*.c")
INCLUDE= -I../dpc2sim/inc
LIBS= ../dpc2sim/lib/dpc2sim.a

BUILD:$(SRC)
	$(CC) $(CFLAGS) -o prefetcher $(SRC) $(INCLUDE) $(LIBS)
