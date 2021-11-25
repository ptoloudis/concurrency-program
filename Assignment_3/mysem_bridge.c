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
#define Red "\033[1;31m"
#define Blue "\033[1;34m"
#define Clear "\033[0m"
#define Green "\033[1;32m"

enum color_t {red, blue};

typedef struct bridge{
    int capacity;
    enum color_t color;
    int red_waiting; //Amount of Red Cars Waiting
    int blue_waiting; // Amount of Blue Cars Waiting
    int cars_on_bridge;
    int cars_crossed;
    int turn;
    mysem_t *mysem_array;
    mysem_t mutex; 
}bridge_t;

bridge_t *current;
volatile int exit_flag;

void arriving_cars(enum color_t color2)
{
    mysem_down(&current->mutex);
    if(color2 == red)
    {
        current->red_waiting ++;
        if(((current->cars_on_bridge >= current->capacity) || current->turn == 2) || current->blue_waiting != 0)
        {
            printf("Red Cars on the Bridge reached max capacity.\n");
            mysem_up(&current->mutex);
            mysem_down(&current->mysem_array[0]);
            mysem_down(&current->mutex);
        }

        current->turn = 1;
        current->red_waiting --;
        current->cars_on_bridge ++;
        if((current->red_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            mysem_up(&current->mysem_array[0]);
        }
        // printf( Red "Red Cars arriving on the bridge.\n" Clear);
        printf("Red Cars arriving on the bridge.\n");
    }
    else 
    {
        current->blue_waiting ++;
        if((current->cars_on_bridge >= current->capacity) || current->turn == 1 || current->red_waiting != 0)
        {
            printf("Blue Cars on the Bridge reached max capacity.\n");
            mysem_up(&current->mutex);
            mysem_down(&current->mysem_array[1]);
            mysem_down(&current->mutex);
        }

        current->turn = 2;
        current->blue_waiting --;
        current->cars_on_bridge ++;
        if((current->blue_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            mysem_up(&current->mysem_array[1]);
        }
        // printf(Blue "Blue Cars arriving on the bridge.\n" Clear);
        printf("Blue Cars arriving on the bridge.\n");
        
    }
    mysem_up(&current->mutex);
    return;
}

void leaving_cars(enum color_t color2)
{
    mysem_down(&current->mutex);
    if(exit_flag == 1 && current->blue_waiting == 0 && current->red_waiting == 0 && current->cars_on_bridge == 1)
    {
        mysem_up(&current->mysem_array[2]);
    }
    
    if(color2 == red)
    {
        //mysem_down(&current->mysem_array[0]);
        //printf(Green "Red Cars are leaving the Bridge.\n" Clear);
        printf("Red Cars are leaving the Bridge.\n");
        current->cars_on_bridge --;
        current->cars_crossed ++;
        if(current->cars_on_bridge == 0)
        {
            printf("Last Car on Bridge is Leaving.\n");
            if((current->blue_waiting != 0) || (current->cars_crossed == 2 * current->capacity))
            {
                current->turn = 2;
                mysem_up(&current->mysem_array[1]);
            }
            current->turn = 0;
            current->cars_crossed = 0;
            mysem_up(&current->mutex);
            return;
        } 
        
        if (current->cars_on_bridge < current->capacity)
        {
            mysem_up(&current->mysem_array[0]);
        }
        
        mysem_up(&current->mutex);
        sleep(1);
        return;
        
    }
    else 
    {
        //mysem_down(&current->mysem_array[1]);
        // printf(Green "Blue Cars are leaving the Bridge.\n" Clear);
        printf("Blue Cars are leaving the Bridge.\n");
        current->cars_on_bridge --;
        current->cars_crossed ++;
        if(current->cars_on_bridge == 0)
        {
            printf("Last Car on Bridge is Leaving.\n");
            if((current->red_waiting != 0) || (current->cars_crossed == 2 * current->capacity))
            {
                current->turn = 1;
                mysem_up(&current->mysem_array[0]);
            }
            current->turn = 0;
            current->cars_crossed = 0;
            mysem_up(&current->mutex);
            return;
        } 
    
        if (current->cars_on_bridge < current->capacity )
        {
            mysem_up(&current->mysem_array[1]);
        }
    
        mysem_up(&current->mutex);
        sleep(1);
        return;
    }
}

void *Red_Cars(void *argument)
{
    //printf("\033[1;31mRed Cars: \n\033[0m");
    arriving_cars( red);
    sleep(time_in_bridge);
    //printf("\033[1;31mRed Cars: \n\033[0m");
    leaving_cars(red);

    return NULL;
}

void *Blue_Cars(void *argument)
{
    //printf("\033[1;34mBlue Cars: \n\033[0m");
    arriving_cars(blue);
    sleep(time_in_bridge);
    //printf("\033[1;34mBlue Cars: \n\033[0m");
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
    current->cars_crossed = 0;
    current->turn = 0;
    exit_flag = 0;
    time = 0;
    
    // Create Threads    
    while(1)
    {
        scanf("%d %c %d", &cars, &c, &time);
        if(cars < 0)
        {
            exit_flag = 1;
            mysem_down(&current->mysem_array[2]);
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

    sleep(2);
    // Destroy the Semaphore
    free(current->mysem_array);
    mysem_destroy(&current->mutex);
    mysem_destroy(&current->mysem_array[0]);
    free(current);
    return 0;
}
