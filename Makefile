CC = gcc
CCFLAGS = -Wall -g 

WC: WC.c
	$(CC)  WC.c -o WC -lpthread 

.PHONY: clean
clean:
	rm -f*.o 
