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
volatile int exit_flag, exit_train;
mysem_t mtx;
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

    mysem_down(&mtx);
    if(tmp->boarded_passengers >= passenger_capacity)  
    {
        // If maximum capacity reached stop getting on the train
        tmp->waiting_passengers ++;
        printf("Passenger wait to gets on the train\n");
        mysem_up(&mtx);
        mysem_down(&mysem_array[1]); // Wait for the train to be empty
        mysem_down(&mtx);
        tmp->waiting_passengers --;
    }

    if((exit_flag == 1) && ((tmp->waiting_passengers) < passenger_capacity))
    {
        //Get of the park
        if(tmp->waiting_passengers == 0)
        {
            printf("Kill the train\n");
            exit_train = 1;
            mysem_up(&mysem_array[0]);
            mysem_up(&mysem_array[2]);
        }
        else
        {
            mysem_up(&mysem_array[1]);
        }
        mysem_up(&mtx);
        return 0;
    }

    // Get on the Train
    printf("Passenger gets ON the train \n");
    tmp->boarded_passengers ++;
    tmp->total_passengers --;
    if((tmp->waiting_passengers > 0) && (tmp->boarded_passengers < passenger_capacity)) 
    {
        mysem_up(&mysem_array[1]);
    }
    mysem_up(&mtx);

    // If the train is full, start the train
    if(tmp->boarded_passengers == passenger_capacity)  
    {
        mysem_up(&mysem_array[0]);
    }

    mysem_down(&mysem_array[2]);
    mysem_down(&mtx);
    printf("Passengers get OFF the train \n");
    tmp->boarded_passengers --;
    if (tmp->boarded_passengers != 0)
    {
        mysem_up(&mysem_array[2]);
    }

    mysem_up(&mtx);
    if((tmp->waiting_passengers > 0) && (tmp->boarded_passengers == 0))
    {
        mysem_up(&mysem_array[1]);
    }
    if (exit_train == 1)
    {
        mysem_up(&mysem_array[2]);
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
    
        mysem_down(&mtx);
        if(exit_train == 1)
        {
            // Exit the program
            mysem_up(&mysem_array[3]);
            mysem_up(&mtx);
            printf("The train is DEAD\n");
            return 0;
        }
        mysem_up(&mtx);
        
        if((tmp->waiting_passengers == 0) && tmp->boarded_passengers < passenger_capacity)
        {
            break;
        }

        printf("Train Starts Route\n");
        sleep(3);
        printf("Train Finished Route\n");

        // Get off the Train
        mysem_up(&mysem_array[2]);
    }
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
        exit_train = 0;
        exit_flag = 0;
        ptr.total_passengers = 0;
        ptr.boarded_passengers = 0;
        ptr.waiting_passengers = 0;
        ptr.semid = 0;
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

    // Initialize the Semaphore Array, the Semaphore
    for (i = 0; i < num_of_sem; i++)
    {   
        mysem_array[i].sem_num = i;
        mysem_array[i].semid = semid;
        mysem_array[i].sem_op = 0;
        if (mysem_init(&mysem_array[i], 0) == -1)
        {
            perror("Error in mysem_init in Semaphore");
            exit(EXIT_FAILURE);
        }
        
    }
    
    // Create and Initialize the Mutex
    mtx.semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (mtx.semid == -1)
    {
        perror("ERROR: No Mutex created");
        exit(EXIT_FAILURE);
    }

    mtx.sem_num = 0;
    mtx.sem_op = 0;
    mtx.initialized = 0;   
    if (mysem_init(&mtx, 1) == -1)
    {
        perror("Error in mysem_init in Mutex");
        exit(EXIT_FAILURE);
    }

    // Create Threads
    pthread_create(&train, NULL, start_train_route, (void*)&ptr);
    
    while(1)
    {
        scanf("%d %d", &passengers,&time);
        if(passengers < 0)
        {
            exit_flag = 1;
            pthread_create(&train, NULL, fill_train, (void*)&ptr); // killer
            break;
        }
        for(int i = 0; i < passengers; i++)
        {   
            ptr.total_passengers ++;
            pthread_create(&train, NULL, fill_train, (void*)&ptr);
        }
        sleep(time);
    }

    // Wait for the Threads to finish
    mysem_down(&mysem_array[3]);
    sleep(2);
    
    //Destroy the Semaphore
    mysem_destroy(&mysem_array[0]);
    mysem_destroy(&mtx);
    free(mysem_array);

    return 0;
}