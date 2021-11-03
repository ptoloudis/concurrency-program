CC = gcc
CCFLAGS = -Wall -g 
OBJ = askisi1-1.o my_pipe.o

askisi1-1: $(OBJ)
	$(CC) $(OBJ) -o askisi1-1 -lpthread

my_pipe.o: my_pipe.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

askisi1-1.o: askisi1-1.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 