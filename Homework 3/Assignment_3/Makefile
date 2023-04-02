CC = gcc
CCFLAGS = -Wall -g 

train: train.c
	$(CC) $(CCFLAGS)  train.c -o train -lpthread 

.PHONY: clean
clean:
	rm -f*.o 