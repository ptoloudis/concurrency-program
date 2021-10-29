CC = gcc
CCFLAGS = -Wall -g
OBJ = test.o my_pipe.o

test: $(OBJ)
	$(CC) $(OBJ) -o test

my_pipe.o: my_pipe.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

test.o: test.c my_pipe.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 