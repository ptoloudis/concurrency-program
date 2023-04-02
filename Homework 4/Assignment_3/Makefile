CC = gcc
CCFLAGS = -Wall -g
OBJ = bridge.o library.o

bridge: $(OBJ)
	$(CC) $(OBJ) -o bridge

library.o: library.c header.h
	$(CC) $(CCFLAGS) -c $< 

bridge.o: bridge.c header.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 
