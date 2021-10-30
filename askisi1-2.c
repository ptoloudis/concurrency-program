/*
Name: Panagiotis Toloudis
Github: ptoloudis
Modified on: 29 October 2021 12:33:04
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

int *for_worker;
int *arr;
/*  0 = creative
    1 = not work
    2 = work
    3 = to close
    4 = close
*/

void *  prime_number(void *arg){
    int number, worker, i,j, flag;

    worker = *(int *)arg;
    arr[worker] = 1;

    while (arr[worker] != 3){   
        if (arr[worker] == 2){
            flag = 1;
            number = for_worker[worker];
            for (i = 2; i <= sqrt(number); i++) {
                if (number % i == 0) {
                    flag = 0;
                    break;
                }
            }

            if (number <= 1){
                flag = 0; }
            if (flag) 
                printf("%d: 1\n", number);
            else 
                printf("%d: 0\n", number);

            arr[worker] = 1;
        }        
    }
    
    for ( j = 0; j < 10000000; j++){   }
    arr[worker] = 4;    
    return 0;
}

int main(int argc, char *argv[]){
    pthread_t test;
    int n = atoi(argv[1]), i, j, number;

    arr = (int *) malloc(n * sizeof(int));
    if (arr == NULL){
        perror("No mallo to arr");
        exit(1);
    }
    for ( i = 0; i < n; i++){
        arr[i]=0;
    }
    for_worker = (int *) malloc((2 *n) * sizeof(int));
    if (for_worker == NULL){
        perror("No mallo to pipe_worker");
        exit(1);
    }

    for ( i = 0; i < n; i++){
        pthread_create(&test,NULL,prime_number,(void *) &i);
        for ( j = 0; j < 1000000; j++){   }
        
    }
    
    j=0;
    while(j != n){
        for (i = 0; i < n; i++){
            if(arr[i]== 1)
                j++;
        }
    }

// start time 
   clock_t start,end;
   double time;
   start = clock();

    while (1){
        scanf("%d",&number);
        if (number == -1)
            break;
        
        while (1)
        {
            for (i = 0; i < n; i++){
                if(arr[i]== 1)
                break;
            } 
            if(arr[i]== 1)
                break;
        }
        for_worker[i]= number;
        arr[i] = 2;
    }
    
// to close
    for ( i = 0; i < n; i++){
        arr[i]=3;
    }
    
    j=0;
    while(j != n){
        for (i = 0; i < n; i++){
            if(arr[i]== 4)
                j++;
        }
    }

//finish time
    end = clock();
    time = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%lf\n",time);

// free     
    free(for_worker);
    free(arr);

    return 0;
}