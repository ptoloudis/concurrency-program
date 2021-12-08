/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//#define Train_Time 10

volatile int passenger_capacity;
volatile int exit_flag, exit_train;
pthread_mutex_t mutex;
pthread_cond_t arriving_passengers, leaving_passengers, train_ready, leaving;

typedef struct train{
    int total_passengers;
    int boarded_passengers;
    int waiting_passengers;
}train_t;

void *fill_train(void *ptr)
{
    train_t *tmp = (train_t *) ptr; 

    pthread_mutex_lock(&mutex);
    if(tmp->boarded_passengers >= passenger_capacity)  
    {
        // If maximum capacity reached stop getting on the train
        tmp->waiting_passengers ++;
        printf("Passenger wait to gets on the train\n");
        pthread_cond_wait(&arriving_passengers, &mutex);
        tmp->waiting_passengers --;
    }

    if((exit_flag == 1) && ((tmp->waiting_passengers) < passenger_capacity))
    {
        //Get of the park
        if(tmp->waiting_passengers == 0)
        {
            printf("Kill the train\n");
            exit_train = 1;
            pthread_cond_signal(&train_ready);
            pthread_cond_signal(&leaving);
        }
        else
        {
            pthread_cond_signal(&arriving_passengers);
        }
        pthread_mutex_unlock(&mutex);
        return 0;
    }

    // Get on the Train
    printf("Passenger gets ON the train \n");
    tmp->boarded_passengers ++;
    tmp->total_passengers --;
    if((tmp->waiting_passengers > 0) && (tmp->boarded_passengers < passenger_capacity)) 
    {
        pthread_cond_signal(&arriving_passengers);
    }
    pthread_mutex_unlock(&mutex);

    // If the train is full, start the train
    if(tmp->boarded_passengers == passenger_capacity)  
    {
        pthread_cond_signal(&train_ready);
    }

    pthread_cond_wait(&leaving_passengers, &mutex);
    printf("Passengers get OFF the train \n");
    tmp->boarded_passengers --;
    if (tmp->boarded_passengers != 0)
    {
        pthread_cond_signal(&leaving_passengers);
    }

    if((tmp->waiting_passengers > 0) && (tmp->boarded_passengers == 0))
    {
        pthread_cond_signal(&arriving_passengers);
    }
    if (exit_train == 1)
    {
        pthread_cond_signal(&leaving_passengers);
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

void *start_train_route(void *ptr)
{
    train_t *tmp = (train_t *) ptr; 

    while(1)
    {
        // Start the Train
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&train_ready, &mutex);
        if(exit_train == 1)
        {
            // Exit the program
            pthread_cond_signal(&leaving);
            pthread_mutex_unlock(&mutex);
            
            printf("The train is DEAD\n");
            return 0;
        }
        pthread_mutex_unlock(&mutex);
        
        if((tmp->waiting_passengers == 0) && tmp->boarded_passengers < passenger_capacity)
        {
            break;
        }

        printf("Train Starts Route\n");
        sleep(3);
        printf("Train Finished Route\n");

        // Get off the Train
        pthread_cond_signal(&leaving_passengers);

    }
    return 0;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    pthread_t train;
    int passengers, time, i;
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
    }

    // Initialize 
    pthread_mutex_init(&mutex, NULL);   
    pthread_cond_init(&arriving_passengers, NULL);
    pthread_cond_init(&leaving_passengers, NULL);
    pthread_cond_init(&train_ready, NULL);
    pthread_cond_init(&leaving, NULL);    

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
        for(i = 0; i < passengers; i++)
        {   
            ptr.total_passengers ++;
            pthread_create(&train, NULL, fill_train, (void*)&ptr);
        }
        sleep(time);
    }

    // Wait for the Threads to finish
    pthread_cond_signal(&leaving);
    sleep(2);
    
    //Destroy the Semaphore
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&arriving_passengers);
    pthread_cond_destroy(&leaving_passengers);
    pthread_cond_destroy(&train_ready);
    pthread_cond_destroy(&leaving);
    
    return 0;
}