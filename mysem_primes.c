/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include "my_sem.h"

int *for_worker;
int *status;
/*  0 = Worker is being Created
    1 = Available for Usage = down
    2 = Unavailable for Usage = up
    3 = To be closed = wait for all to be down 
    4 = Closed = when all down shut down the program
*/

void * prime_number(void *argument)
{
    int number, worker, i, j, flag, count=0;
    
    worker = *(int *)argument;
    status[worker] = 1;

    printf("Worker %d\n",worker); // Print the Threads we use as Workers 

    while (status[worker] != 3)
    {                               //  Loop to Wait until Program Exits
        if (status[worker] == 2)
        {
            flag = 1;
            number = for_worker[worker];

            // Iterate from 2 to sqrt(n)
            for (i = 2; i <= sqrt(number); i++)
            {
                // If the Number is Divisible by any Number between 2 and n/2, it is not Prime
                if (number % i == 0)
                {
                    flag = 0;
                    break;
                }
            }

            if (number <= 1)
            {
                flag = 0; 
            }

            if (flag)
            {
                printf("%d: 1\n", number);
                count++;
            }     
            else
            {
                printf("%d: 0\n", number);
                count++;
            } 
            
            status[worker] = 1;
        }   
        
             
    }
    
    // for (j = 0; j < 1000000; j++){} // Wait until all are printed 
    // printf("%d\n Seeeeeeee meee\n", count);
    status[worker] = 4;   // Exit

    return 0;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    pthread_t test;
    int num_of_threads, i, j, number, flag, *sem_array, semid;

    if(argc != 2)
    {
        perror("ERROR : Not defiend amount of Threads to use\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        num_of_threads = atoi(argv[1]);
    }

    // Malloc for the all worker status and "table"
    status = (int *) malloc(num_of_threads * sizeof(int));
    if (status == NULL)
    {
        perror("No Malloc done to Status");
        exit(1);
    }

    // Semaphores
    sem_array = (int *) malloc(num_of_threads * sizeof(int));
    if(!sem_array)
    {
        exit(EXIT_FAILURE);
    }
    for ( i = 0; i < num_of_threads; i++)
    {
        //status[i]=0;
        sem_array[i] = 0;
    }
    semid = mysem_init(sem_array, num_of_threads);
    
    //Workers
    for_worker = (int *) malloc((2 * num_of_threads) * sizeof(int));

    if (for_worker == NULL)
    {
        perror("No malloc done to Pipe_worker");
        exit(1);
    }

    // Creation of the Worker and Delay until All Workers are Ready to Work
    for ( i = 0; i < num_of_threads; i++)
    {
        pthread_create(&test,NULL,prime_number,(void *) &i);
        for ( j = 0; j < 20000000; j++){} 
    }

    j=0;

    while(j != num_of_threads)
    {
        for (i = 0; i < num_of_threads; i++)
        {
            if(status[i]== 1)
            {
                j++;
            }
        }
    }

    while (1)
    {
        flag = 1;
        scanf("%d",&number);
        if (number == -1)
            break;
        
        while (flag == 1)
        {
            for (i = 0; i < num_of_threads; i++)
            {
                if(status[i]== 1)
                {
                    for_worker[i] = number;
                    status[i] = 2;
                    flag = 0;
                    break;
                }
            }  
        }
    }
    
    // Notify Workers to Close and Wait all the Workers to be Closed
    for ( i = 0; i < num_of_threads; i++)
    {
        status[i]=3;
    }

    while(j != num_of_threads)
    {
        j=0;
        for (i = 0; i < num_of_threads; i++)
        {
            if(status[i]== 4)
            {
                j++;
            }
        }
    }
    
    //for(i = 0; i < 1000000000; i++){}

    // Free all Workers
    free(for_worker);
    free(status);

    return 0;
}