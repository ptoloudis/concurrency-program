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

mysem_t *sem_primes;
int *for_worker;
/*  > 0 :Number to be checked
    -1  :Number to be notified not working
    -2  :Number to be closed
*/

/******************* PRIME NUMBER *******************/
void * prime_number(void *argument)
{
    int number, worker, i, flag;
    
    worker = *(int *)argument;
    printf("Worker %d\n",worker); // Print the Threads we use as Workers 
    mysem_down(&sem_primes[worker]); // Up the semaphore to be used by the Worker

    while (1)
    {                     
        mysem_down(&sem_primes[worker]);     
        if (for_worker[worker] == -2)
        {
            break;
        } 
        
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
            printf("%d: 1 by worker %d\n", number,worker);
        }     
        else
        {
            printf("%d: 0 by worker %d\n", number,worker);
        } 
        
        for_worker[worker] = -1; // Set the status of the worker to be notified             
    }
     
    mysem_up(&sem_primes[worker]); // Exit the Thread
    //printf("Seeeeeeee meee %d\n",worker);  

    return 0;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    pthread_t test;
    int num_of_sem, i, number, flag, semid, num_of_threads;

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

    // Create the Semaphore
    num_of_sem = num_of_threads;
    semid = semget(IPC_PRIVATE, num_of_sem, 0666 | IPC_CREAT);
    if (semid == -1)
    {
        perror("ERROR: No semaphore created");
        exit(EXIT_FAILURE);
    }

    // Create the Semaphore Array
    sem_primes = (mysem_t *) malloc((num_of_sem) * sizeof(mysem_t));
    if(sem_primes == NULL)
    {
        perror("No Malloc done to Array Semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize the Semaphore Array and the Semaphore
    for ( i = 0; i < num_of_sem; i++)
    {   
        sem_primes[i].sem_num = i;
        sem_primes[i].semid = semid;
        sem_primes[i].sem_op = 0;
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

    for ( i = 0; i < num_of_threads; i++)
    {
        for_worker[i] = -1;
    }    

    
    // Creation of the Worker and Delay until All Workers are Ready to Work
    for ( i = 0; i < num_of_threads; i++)
    {
        pthread_create(&test,NULL,prime_number,(void *) &i);
        sleep(1); 
    }
  
    for (i = 0; i < num_of_threads; i++)
    {
        mysem_up(&sem_primes[i]);
    }

    while (1)
    {
        flag = 1;
        scanf("%d",&number);

        // Check if the number is -1, if it is, close the program
        if (number == -1){
            sleep(1);
            for ( i = 0; i < num_of_threads; i++)
            {
                sleep(1);
                for_worker[i] = -2; // Send the signal to close the program
                mysem_up(&sem_primes[i]);
            }
            break;
        }
        
        while (flag == 1)
        {
            for (i = 0; i < num_of_threads; i++)
            {
                if( for_worker[i] == -1)
                {
                    for_worker[i] = number; //Send the number to the Worker
                    mysem_up(&sem_primes[i]);
                    flag = 0;
                    break;
                }
            }  
        }
    }
    
    // Wait for all the Workers to finish
    for (i = 0 ; i < num_of_threads; i++)
    {
        mysem_down(&sem_primes[i]);
    }
    
    // Free All to Malloc and to Semaphore
    mysem_destroy(&sem_primes[0]);
    free(for_worker);
    free(sem_primes);

    return 0;
}
