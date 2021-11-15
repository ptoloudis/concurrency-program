CC = gcc
CCFLAGS = -Wall -g 
OBJ = sem_pipes.o my_pipe.o

sem_pipes: $(OBJ)
	$(CC) $(OBJ) -o sem_pipes -lpthread

mysem.o: mysem.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

sem_pipes.o: sem_pipes.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 