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
int *status;
/*  0 = creative
    1 = not work
    2 = work
    3 = to close
    4 = close
*/

void * prime_number(void *arg){
    int number, worker, i,j, flag;

    worker = *(int *)arg;
    status[worker] = 1;

    printf("Worker %d\n",worker);// 
    while (status[worker] != 3){    //  loop to wait to0 go to exit
        if (status[worker] == 2){
            flag = 1;
            number = for_worker[worker];
            // Iterate from 2 to sqrt(n)
            for (i = 2; i <= sqrt(number); i++) {
                // If n is divisible by any number between
                // 2 and n/2, it is not prime
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

            status[worker] = 1;
        }        
    }
    
    printf("bey %d\n",worker);
    for ( j = 0; j < 10000000; j++){   } // wait to print all 
    status[worker] = 4;   //exit
    return 0;
}

int main(int argc, char *argv[]){
    pthread_t test;
    int n = atoi(argv[1]), i, j, number;

    // malloc for the all worker status and "table"
    status = (int *) malloc(n * sizeof(int));
    if (status == NULL){
        perror("No mallo to status");
        exit(1);
    }
    for ( i = 0; i < n; i++){
        status[i]=0;
    }
    for_worker = (int *) malloc((2 *n) * sizeof(int));
    if (for_worker == NULL){
        perror("No mallo to pipe_worker");
        exit(1);
    }

    //creative the worker and wait all to ready to work
    for ( i = 0; i < n; i++){
        pthread_create(&test,NULL,prime_number,(void *) &i);
        for ( j = 0; j < 20000000; j++){   } 
    }
    j=0;
    while(j != n){
        for (i = 0; i < n; i++){
            if(status[i]== 1)
                j++;
        }
    }

// start time and start to work
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
                if(status[i]== 1)
                break;
            } 
            if(status[i]== 1)
                break;
        }
        for_worker[i]= number;
        status[i] = 2;
    }
    
// notify workers to close and wait all the worker to close
    for ( i = 0; i < n; i++){
        status[i]=3;
    }
    j=0;
    while(j != n){
        for (i = 0; i < n; i++){
            if(status[i]== 4)
                j++;
        }
    }

//finish time
    end = clock();
    time = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%lf\n",time);

// free all pointer   
    free(for_worker);
    free(status);

    return 0;
}