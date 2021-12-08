CC = gcc
CCFLAGS = -Wall -g 
OBJ = mysem_primes.o mysem.o

mysem_primes: $(OBJ)
	$(CC) $(OBJ) -o mysem_primes -lpthread -lm

mysem_primes.o: mysem_primes.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

mysem.o: mysem.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 