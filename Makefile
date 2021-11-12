CC = gcc
CCFLAGS = -Wall -g 

# mysem: $(OBJ)
# 	$(CC) $(OBJ) -o FIFO_pipes -lpthread

mysem: mysem.c my_sem.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f*.o 