/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "my_sem.h"

//#define Train_Time 10

volatile int passenger_capacity;
mysem_t *mysem_array;

typedef struct train{
    int total_passengers;
    int boarded_passengers;
    int waiting_passengers;
    int semid;
}train_t;

void *fill_train(void *ptr)
{
    train_t *tmp = (train_t *) ptr; 

    if((tmp->boarded_passengers < passenger_capacity) && (tmp->total_passengers != 0))
    {
        // Get on the Train
        printf("Passenger gets on the train \n");
        mysem_up(&mysem_array[1]);
        tmp->boarded_passengers ++;
        tmp->total_passengers --;
    }
    else
    {
        // If maximum capacity reached stop getting on the train
        mysem_down(&mysem_array[1]); //Wait
    }


    // tmp->boarded_passengers ++;
    // tmp->total_passengers --;
    mysem_down(&mysem_array[2]);

    tmp->boarded_passengers --;
    mysem_up(&mysem_array[2]);
    printf("Passengers get off the train \n");
    
    if(tmp->boarded_passengers == 0)
    {
        mysem_down(&mysem_array[3]);
    }
    return 0;
}
void *start_train_route(void *ptr)
{
    train_t *tmp = (train_t *) ptr; 

    while(1)
    {
        // Start the Train
        mysem_down(&mysem_array[0]);
        if((tmp->waiting_passengers == 0) && tmp->boarded_passengers < passenger_capacity)
        {
            break;
        }

        printf("Train Starts Route\n");
        sleep(2);
        printf("Train Finished Route\n");

        // Get off the Train
        mysem_up(&mysem_array[2]);

        // Get on the Train 
        mysem_up(&mysem_array[1]);
    }
    // Exit the program
    mysem_up(&mysem_array[3]);
    return 0;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    pthread_t train;
    int passengers, time, semid, num_of_sem, i;
    train_t ptr;

    if(argc != 2)
    {
        perror("ERROR : Not defiend amount of Threads to use\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        passenger_capacity = atoi(argv[1]);
    }

    // Create the Semaphore
    num_of_sem = 4;
    semid = semget(IPC_PRIVATE, num_of_sem, 0666 | IPC_CREAT);
    if (semid == -1)
    {
        perror("ERROR: No semaphore created");
        exit(EXIT_FAILURE);
    }

    // Create the Semaphore Array
    mysem_array = (mysem_t *) malloc((num_of_sem) * sizeof(mysem_t));
    if(mysem_array == NULL)
    {
        perror("No Malloc done to Array Semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize the Semaphore Array, the Semaphore and the Array of Numbers
    for (i = 0; i < num_of_sem; i++)
    {   
        mysem_array[i].sem_num = i;
        mysem_array[i].semid = semid;
        mysem_array[i].sem_op = 0;
        if (mysem_init(&mysem_array[i], 0) == -1)
        {
            perror("Error in mysem_init");
            exit(EXIT_FAILURE);
        }
        
    }
    
    // Create Threads
    pthread_create(&train, NULL, start_train_route, (void*)&ptr);
    
    while(1)
    {
        scanf("%d", &passengers);
        if(passengers < 0)
        {
            break;
        }
        for(int i = 0; i < passengers; i++)
        {
            pthread_create(&train, NULL, fill_train, (void*)&ptr);
        }
        scanf("%d", &time);
        sleep(time);
    }
    return 0;
}
