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

#define time_in_bridge 2

enum color_t {red, blue};

typedef struct bridge{
    int capacity;
    enum color_t color;
    int red_waiting; //Amount of Red Cars Waiting
    int blue_waiting; // Amount of Blue Cars Waiting
    int cars_on_bridge;
    mysem_t *mysem_array;
    mysem_t mutex; 
}bridge_t;

bridge_t *current;

void arriving_cars(enum color_t color2)
{
    mysem_down(&current->mutex);
    if(color2 == red)
    {
        current->red_waiting ++;
        printf("car on brige: %d\n", current->cars_on_bridge);
        if(current->cars_on_bridge >= current->capacity)
        {
            printf("Red Cars on the Bridge reached max capacity.\n");
            mysem_up(&current->mutex);
            mysem_down(&current->mysem_array[0]);
            mysem_down(&current->mutex);
        }

        if((current->red_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            current->red_waiting --;
            current->cars_on_bridge ++;
            mysem_up(&current->mysem_array[0]);
        }
        printf("Red Cars arriving on the bridge.\n");
        mysem_up(&current->mutex);
    }
    else if(color2 == blue)
    {
        current->blue_waiting ++;
        if(current->cars_on_bridge >= current->capacity)
        {
            printf("Blue Cars on the Bridge reached max capacity.\n");
            mysem_up(&current->mutex);
            mysem_down(&current->mysem_array[1]);
            mysem_down(&current->mutex);
        }

        if((current->blue_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            current->blue_waiting --;
            current->cars_on_bridge ++;
            mysem_up(&current->mysem_array[1]);
        }
        printf("Blue Cars arriving on the bridge.\n");
        mysem_up(&current->mutex);
    }
    return;
}

void leaving_cars(enum color_t color2)
{
    mysem_down(&current->mutex);
    if(color2 == red)
    {
        mysem_down(&current->mysem_array[0]);
        if(current->cars_on_bridge == 1)
        {
            printf("Last Car on Bridge is Leaving.\n");
            current->cars_on_bridge --;
            mysem_up(&current->mutex);
            return;
        } 
        
        printf("Red Cars are leaving the Bridge.\n");
        current->cars_on_bridge --;
        if(current->red_waiting > 0)
        {
            current->cars_on_bridge --;
        }
        
        mysem_up(&current->mysem_array[0]);
        mysem_up(&current->mutex);
            
        return;
        
    }
    else if(color2 == blue)
    {
        mysem_down(&current->mysem_array[1]);
        if(current->cars_on_bridge == 1)
        {
            printf("Last Car on Bridge is Leaving.\n");
            current->cars_on_bridge --;
            mysem_up(&current->mutex);
            return;
        } 
        
        printf("Blue Cars are leaving the Bridge.\n");
        current->cars_on_bridge --;
        if(current->blue_waiting > 0)
        {
            current->blue_waiting ++;
            current->cars_on_bridge --;
    
        }
        
        mysem_up(&current->mysem_array[1]);
        mysem_up(&current->mutex);
            
        return;
    }
}

void *Red_Cars(void *argument)
{
    arriving_cars( red);
    sleep(time_in_bridge);
    leaving_cars(red);

    return NULL;
}

void *Blue_Cars(void *argument)
{

    arriving_cars(blue);
    sleep(time_in_bridge);
    leaving_cars(blue);

    return NULL;
}

int main(int argc, char *argv[])
{
    char c;
    int num_of_sem, semid, capacity, cars, time;
    int i;
    pthread_t bridge;

    if(argc != 2)
    {
        perror("ERROR : Not defiend amount of Capacity for the Bridge.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        capacity = atoi(argv[1]);
    }

    current = (bridge_t *) malloc(sizeof(bridge_t));
    if(current == NULL)
    {
        perror("No Malloc done to Array Semaphore");
        exit(EXIT_FAILURE);
    }
    current->capacity = capacity;

    // Create the Semaphore
    num_of_sem = 2;
    semid = semget(IPC_PRIVATE, num_of_sem, 0666 | IPC_CREAT);
    if (semid == -1)
    {
        perror("ERROR: No semaphore created");
        exit(EXIT_FAILURE);
    }

    // Create the Semaphore Array
    current->mysem_array = (mysem_t *) malloc((num_of_sem) * sizeof(mysem_t));
    if(current->mysem_array == NULL)
    {
        perror("No Malloc done to Array Semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize the Semaphore Array, the Semaphore and the Array of Numbers
    for (i = 0; i < num_of_sem; i++)
    {   
        current->mysem_array[i].sem_num = i; // mysem_array[0] = Red Car      //mysem_array[1] = Blue Car
        current->mysem_array[i].semid = semid;
        current->mysem_array[i].sem_op = 0;
        if (mysem_init(&current->mysem_array[i], 0) == -1)
        {
            perror("Error in mysem_init");
            exit(EXIT_FAILURE);
        }
        
    }

    // Cheating the mutex
    current->mutex.semid = semget(IPC_PRIVATE, num_of_sem, 0666 | IPC_CREAT);
    if (current->mutex.semid == -1)
    {
        perror("ERROR: No semaphore created");
        exit(EXIT_FAILURE);
    }

    //Initialize the mutex
    current->mutex.sem_num = 0;
    current->mutex.sem_op = 0;
    if (mysem_init(&current->mutex, 1) == -1)
    {
        perror("Error in mysem_init");
        exit(EXIT_FAILURE);
    }

    // Initialize the struct
    current->blue_waiting = 0;
    current->red_waiting = 0;
    current->cars_on_bridge = 0;
    time = 0;
    
    // Create Threads    
    while(1)
    {
        scanf("%d %c %d", &cars, &c, &time);
        if(cars < 0)
        {
            break;
        }

        if (c == 'r')
        {
            for(int i = 0; i < cars; i++)
            {
                pthread_create(&bridge, NULL, Red_Cars, (void*)&current);
            } 
        }
        else
        {
            for(int i = 0; i < cars; i++)
            {
                pthread_create(&bridge, NULL, Blue_Cars, (void*)&current);
            } 
        } 
        
        sleep(time);
    }

    sleep(4);
    return 0;
}
