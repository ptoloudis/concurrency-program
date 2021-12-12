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

pthread_mutex_t mutex;
pthread_cond_t *cond;
int *for_worker;
/*  > 0 :Number to be checked
    -1  :Number to be closed
    -2  :Number to be notified not working
*/

/******************* PRIME NUMBER *******************/
void * prime_number(void *argument)
{
    int number, worker, i, flag;
    
    worker = *(int *)argument;
    printf("Worker %d\n",worker); // Print the Threads we use as Workers 
    pthread_mutex_lock(&mutex);
    //pthread_cond_wait(&cond[worker], &mutex);

    while (1)
    {                     
        pthread_cond_wait(&cond[worker], &mutex);     
        if (for_worker[worker] == -1)
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
        
        for_worker[worker] = -2; // Set the status of the worker to be notified             
    }
    printf("Seeeeeeee meee %d\n",worker);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond[worker]); // Exit the Thread 

    return 0;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    pthread_t test;
    int num_of_threads, i, number, flag;

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

    // Initialize the Mutex and the Condition Variable
    cond = (pthread_cond_t *)malloc((num_of_threads)*sizeof(pthread_cond_t));
    if (cond == NULL)
    {
        perror("ERROR : Not enough memory\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < num_of_threads; i++)
    {
        pthread_cond_init(&cond[i], NULL);
    }

    pthread_mutex_init(&mutex, NULL);  

    // Malloc for the array of numbers to be checked
    for_worker = (int *) malloc(num_of_threads * sizeof(int));
    if (for_worker == NULL)
    {
        perror("No malloc done to Pipe_worker");
        exit(1);
    }

    for ( i = 0; i < num_of_threads; i++)
    {
        for_worker[i] = -2;
    }    

    
    // Creation of the Worker and Delay until All Workers are Ready to Work
    for ( i = 0; i < num_of_threads; i++)
    {
        pthread_create(&test,NULL,prime_number,(void *) &i);
        sleep(0.5); 
    }

    // for ( i = 0; i < num_of_threads; i++)
    // {
    //     pthread_cond_signal(&cond[i]);
    // }

    //printf("hi\n");
    while (1)
    {
        flag = 1;
        scanf("%d",&number);

        // Check if the number is -1, if it is, close the program
        if (number == -1){
            sleep(1.5);
            for ( i = 0; i < num_of_threads; i++)
            {
                sleep(1);
                pthread_mutex_lock(&mutex);
                for_worker[i] = -1; // Send the signal to close the program
                pthread_cond_signal(&cond[i]);
                pthread_cond_wait(&cond[i], &mutex);
                pthread_mutex_unlock(&mutex);
            }
            break;
        }
        
        while (flag == 1)
        {
            for (i = 0; i < num_of_threads; i++)
            {
                if( for_worker[i] == -2)
                {
                    pthread_mutex_lock(&mutex);
                    for_worker[i] = number; //Send the number to the Worker
                    pthread_cond_signal(&cond[i]);
                    pthread_mutex_unlock(&mutex);
                    flag = 0;
                    break;
                }
            }  
        }
    }
    
    // Free All to Malloc and to Semaphore
    free(for_worker);
    pthread_cond_destroy(&cond[0]);
    pthread_mutex_destroy(&mutex);

    return 0;
}
