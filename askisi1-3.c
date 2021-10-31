/*
Name: Panagiotis Toloudis
Github: ptoloudis
Modified on: 30 October 2021 12:49:17
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define min_size 64;
int *status;
volatile int workers;
struct basik{
    FILE *input;
    int worker;
};
struct merger{
    char *arr;
    int low;
    int high;
    int worker;
};


int size_file(FILE* fl) {
    int j,size;
    
    j = fseek(fl, 0L, SEEK_END);
    if (j == -1) {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        return -1;
    }

    size = ftell(fl);
    if (size == -1) {
        ferror(fl);
        perror("no ftell argument file\n");
        fclose(fl);
        return -1;
    }

     j = fseek(fl, 0L, SEEK_SET);
    if (j == -1) {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        return -1;
    }

    if (size != 0) {
        return size;
    }
    return 0;
}

void * mergeSort(void * arg)
{   pthread_t tid;
    struct merger one,left,right;
    one= *(struct merger )arg;
    
    if (one.low < one.high) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = one.low + (one.high - l) / 2;
  
        left = one;
        left.high = m;
        right = one;
        right.low = m;
        workers 
        pthread_create(&tid,NULL,mergesort,(void *)&left);
        pthread_create(&tid,NULL,mergesort,(void *)&right);
  
        merge(arr, l, m, r);
    }
}

int main(int argc, char *argv[]){
    FILE *input;
    pthread_t tid;
    int size, i=1;

    workers = 0;
    input = fopen(argc[1],"rb");
    size = size_file(input);
    i = (size/2) +1;
    status = (int *) calloc (i,sizeof(int)); 
    if(status == NULL){
        perror("NO malloc in main for size > 64\n");
        exit(1);
    }
    if (size > min_size){ // if size >64 exetute external mergesort
        struct basik start;
        start.input = input;
        start.worker = 0;
        pthread_create(&tid,NULL,external,(void *)&start);
    }
    else { // if size <= 64 exetute mergesort
        struct merger start;
        start.high = size;
        start.low = 0;
        start.arr =  (char *) calloc (size,sizeof(char));
        fread(start.arr,size,sizeof(char),input);
        pthread_create(&tid,NULL,mergesort,(void *)&start);
    }

    while (status[0] != ){  }

    close(input);
    free(status);
    free(input);
    if(size <= min_size)
        free(start.arr);

    return 0;
}