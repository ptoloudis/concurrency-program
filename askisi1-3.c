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
struct basik{//to external
    FILE *input;
    FILE *out;
    int worker;
};
struct merger{// to mergeSort
    char *arr;
    int low;
    int high;
    int worker;
    int status;
};


int size_file(FILE* fl) {
    int j,size;
    
    j = fseek(fl, 0L, SEEK_END); //go to the end file
    if (j == -1) {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        exit(1);
    }

    size = ftell(fl); //tell the size about the file
    if (size == -1) {
        ferror(fl);
        perror("no ftell argument file\n");
        fclose(fl);
        exit(1);
    }

    j = fseek(fl, 0L, SEEK_SET); //go to the start file
    if (j == -1) {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        exit(1);
    }

    if (size != 0) {
        return size;
    }
    return 0;
}

void merge(int arr[], int l, int m, int r) // merge the arr
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
  
    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
  
    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void *mergeSort(void * arg){   
    struct merger one, test; //test is the topical struct 
    one = *(struct merger )arg;
    
    if (one.low < one.high) {
        pthread_t tid;
        int m = one.low + (one.high - l) / 2; // Same as (l+r)/2, but avoids overflow for large low and high
        test = one;      
        // first sorting left arr
        test.status = ;
        test.worker = (one.worker*2)+1;
        test.high = m;
        pthread_create(&tid,NULL,mergesort,(void *)&test);
        while (test.status!=  ){ }

        // second sorting right arr
        test.status = ;
        test.worker = test.worker + 1;
        test.high = one.high;
        test.low = m+1;
        pthread_create(&tid,NULL,mergesort,(void *)&test);
        while (test.status!=  ){ }

        // next merge and return
        merge(test.arr, one.low, m, one.high);
        one.status = ;
    }
}

int main(int argc, char *argv[]){
    FILE *input, *out;
    pthread_t tid;
    int size, i=1;

    workers = 0;
    input = fopen(argc[1],"rb");
    out = fopen(out.txt,"wb+");
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
        start.out = out;
        start.worker = 0;
        pthread_create(&tid,NULL,external,(void *)&start);
    }
    else { // if size <= 64 exetute mergesort
        struct merger start;
        start.high = size;
        start.worker = 0;
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