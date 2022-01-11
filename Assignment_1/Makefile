CC = gcc
CCFLAGS = -Wall -g 
OBJ = coroutine.o library.o

coroutines: $(OBJ)
	$(CC) $(OBJ) -o coroutines

library.o: library.c header.h
	$(CC) $(CCFLAGS) -c $<

coroutine.o: coroutine.c header.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 
