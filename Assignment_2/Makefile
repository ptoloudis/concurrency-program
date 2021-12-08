CC = gcc
CCFLAGS = -Wall -g 
OBJ = mysem.o mysem_bridge.o

mysem_bridge: $(OBJ)
	$(CC) $(OBJ) -o mysem_bridge -lpthread 

mysem_bridge.o: mysem_bridge.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

mysem.o: mysem.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 
