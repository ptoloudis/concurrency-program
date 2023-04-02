CC = gcc
CCFLAGS = -Wall -g 
OBJ = FIFO_pipes.o my_pipe.o

askisi1-1: $(OBJ)
	$(CC) $(OBJ) -o FIFO_pipes -lpthread

my_pipe.o: my_pipe.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

FIFO_pipes.o: FIFO_pipes.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 
