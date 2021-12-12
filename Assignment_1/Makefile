CC = gcc
CCFLAGS = -Wall -g

primes: primes.c
	$(CC) $(CCFLAGS) primes.c -o primes -lpthread -lm

.PHONY: clean
clean:
	rm -f*.o 
