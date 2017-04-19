CC=gcc
CFLAGS=-Wall -O2 -lm -lpthread -I.

samples:: sampleLP sampleILP sample-runtime

sample-runtime: sample-runtime.c annealing.o

sampleLP: sampleLP.c annealing.o

sampleILP: sampleILP.c annealing.o

annealing.o: annealing.c annealing.h

clean: 
	rm -f *.o sampleLP sampleILP sample-runtime
