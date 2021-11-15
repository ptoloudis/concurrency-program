CC = gcc
CCFLAGS = -Wall -g 
OBJ = sem_pipes.o mysem_primes.o

mysem_primes: $(OBJ)
	$(CC) $(OBJ) -o mysem_primes -lpthread

mysem_primes.o: mysem_primes.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

sem_pipes.o: sem_pipes.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 