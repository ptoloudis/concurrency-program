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

struct my_sem_t *sem_primes;
int *for_worker;
/*  > 0 :Number to be checked
    -1  :Number to be notified not working
    -2  :Number to be closed
*/
//int *status;
/*  0 = Worker is being Created
    1 = Available for Usage = down
    2 = Unavailable for Usage = up
    3 = To be closed = wait for all to be down 
    4 = Closed = when all down shut down the program
*/

/******************* PRIME NUMBER *******************/
void * prime_number(void *argument)
{
    int number, worker, i, j, flag;
    
    worker = *(int *)argument;;

    printf("Worker %d\n",worker); // Print the Threads we use as Workers 

    mysem_up(sem_primes[worker]); // Up the semaphore to be used by the Worker

    while (1)
    {                         
        mysem_down(sem_primes[worker]);      
        if (for_worker[worker] != -2) //add code
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

            // If the Number is Prime, print it 1 or print it 0
            if (flag)
            {
                printf("%d: 1\n", number);
            }     
            else
            {
                printf("%d: 0\n", number);
            } 
            
            for_worker[worker] = -1; // Set the status of the worker to be notified
            mysem_up(sem_primes[worker]);
        }   
        
             
    }
    
    // for (j = 0; j < 1000000; j++){} // Wait until all are printed 
    // printf("%d\n Seeeeeeee meee\n", count);
    // status[worker] = 4;   
    mysem_up(sem_primes[worker]); // Exit the Thread

    return 0;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    pthread_t test;
    int num_of_threads, i, j, number, flag, *sem_array, semid;

    // Check if the number of arguments is correct
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
    // status = (int *) malloc(num_of_threads * sizeof(int));
    // if (status == NULL)
    // {
    //     perror("No Malloc done to Status");
    //     exit(1);
    // }

    // Create the Semaphore
    semid = semget(IPC_PRIVATE, num_of_threads, 0666 | IPC_CREAT);
    if (semid == -1)
    {
        perror("ERROR: No semaphore created");
        exit(EXIT_FAILURE);
    }

    // Create the Semaphore Array
    sem_primes = (struct my_sem_t) malloc(num_of_threads * sizeof(struct my_sem_t));
    if(sem_primes == NULL)
    {
        perror("No Malloc done to Array Semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize the Semaphore Array and the Semaphore
    for ( i = 0; i < num_of_threads; i++)
    {
        sem_primes[i].sem_num = i;
        sem_primes[i].semid = semid;
        sem_primes[i].sem_val = 0;
        if (mysem_init(&sem_primes[i], 0) == -1)
        {
            perror("Error in mysem_init");
            exit(EXIT_FAILURE);
        }
        
    }
    
    // Malloc for the array of numbers to be checked
    for_worker = (int *) malloc(num_of_threads * sizeof(int));
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

    while(j != num_of_threads)   
    {   
        for (i = 0; i < num_of_threads; i++)
        {
            mysem_down(sem_primes[i]);
            //mysem_up(sem_primes[i]);
        }
    }

    while (1)
    {
        flag = 1;
        scanf("%d",&number);
        if (number == -1){
            for ( i = 0; i < num_of_threads; i++)
            {
                mysem_down(sem_primes[i]);
                for_worker[i]= -2; // Send the signal to close the program
                mysem_up(&sem_primes[i]);
                
            }
            break;
        }
        
        while (flag == 1)
        {
            for (i = 0; i < num_of_threads; i++)
            {
                if( for_worker[i] != -1)
                {
                    mysem_down(sem_primes[i]);
                    for_worker[i] = number; //Send the number to the Worker
                    mysem_up(&sem_primes[i]);
                    flag = 0;
                    break;
                }
            }  
        }
    }
    
    // Wait for all the Workers to finish
    for (i = 0; i < num_of_threads; i++)
    {
        mysem_down(&sem_primes[i]);
    }
    
    //for(i = 0; i < 1000000000; i++){}

    // Free All to Malloc and to Semaphore
    for (i = 0; i < num_of_threads; i++)
    {
        mysem_destroy(&sem_primes[i]);
    }
    free(for_worker);
    free(sem_primes);

    return 0;
}