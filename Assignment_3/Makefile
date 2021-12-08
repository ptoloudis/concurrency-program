CC = gcc
CCFLAGS = -Wall -g 
OBJ = mysem_train.o mysem.o

mysem_train: $(OBJ)
	$(CC) $(OBJ) -o mysem_train -lpthread 

mysem.o: mysem.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

mysem_train.o: mysem_train.c my_sem.h
	$(CC) $(CCFLAGS) -c $<



.PHONY: clean
clean:
	rm -f*.o 
