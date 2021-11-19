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

enum color_t {red, blue};

typedef struct bridge{
    int capacity;
    int time;
    enum color_t color;
    int red_waiting; //Amount of Red Cars Waiting
    int blue_waiting; // Amount of Blue Cars Waiting
    int cars_on_bridge;
    mysem_t *mysem_array;
}bridge_t;

void arriving_cars(bridge_t *ptr)
{
    mysem_down(&ptr->mysem_array[0]);
    if(ptr->color == red)
    {
        mysem_down(&ptr->mysem_array[0]);
        if(ptr->cars_on_bridge < ptr->capacity)
        {
            ptr->cars_on_bridge ++;
            while((ptr->red_waiting > 0) && (ptr->cars_on_bridge < ptr->capacity))
            {
                ptr->red_waiting --;
                ptr->cars_on_bridge ++;
                mysem_up(&ptr->mysem_array[0]);
            }
        }
    }
    else if(ptr->color == blue)
    {
        mysem_down(&ptr->mysem_array[1]);
        if(ptr->cars_on_bridge < ptr->capacity)
        {
            ptr->cars_on_bridge ++;
            while((ptr->blue_waiting > 0) && (ptr->cars_on_bridge < ptr->capacity))
            {
                ptr->blue_waiting --;
                ptr->cars_on_bridge ++;
                mysem_up(&ptr->mysem_array[1]);
            }
        }
    }
    return;
}

void leaving_cars(bridge_t *ptr)
{
    mysem_down(&ptr->mysem_array[0]);
    if(ptr->color == red)
    {
        mysem_down(&ptr->mysem_array[0]);
        if(ptr->cars_on_bridge == ptr->capacity)
        {
            printf("Red Cars are leaving the Bridge.\n");
            ptr->cars_on_bridge --;
            if(ptr->red_waiting > 0)
            {
                ptr->red_waiting --;
                ptr->cars_on_bridge ++;
                mysem_up(&ptr->mysem_array[0]);
            }
            else
            {
                mysem_up(&ptr->mysem_array[0]);
            }
            return;
        }
        else if(ptr->cars_on_bridge == 1)
        {
            printf("Last Car on Bridge is Leaving.\n");
            ptr->cars_on_bridge --;
            return;
        }
    }
    return;
}
void *Red_Cars(void *argument)
{

    return 1;
}
void *Blue_Cars(void *argument)
{
    return 1;
}
int main(int argc, char *argv[])
{
    bridge_t *current;
    //mysem_t *mysem_array;
    int num_of_sem, semid;
    int i;

    if(argc != 2)
    {
        perror("ERROR : Not defiend amount of Capacity for the Bridge.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        current->capacity = atoi(argv[1]);
    }

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
    if(mysem_array == NULL)
    {
        perror("No Malloc done to Array Semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize the Semaphore Array, the Semaphore and the Array of Numbers
    for (i = 0; i < num_of_sem; i++)
    {   
        mysem_array[i].sem_num = i; // mysem_array[0] = Red Car      //mysem_array[1] = Blue Car
        mysem_array[i].semid = semid;
        mysem_array[i].sem_op = 0;
        if (mysem_init(&mysem_array[i], 0) == -1)
        {
            perror("Error in mysem_init");
            exit(EXIT_FAILURE);
        }
        
    }
    
    // // Create Threads
    // pthread_create(&train, NULL, start_train_route, (void*)&ptr);
    
    // while(1)
    // {
    //     scanf("%d", &passengers);
    //     if(passengers < 0)
    //     {
    //         break;
    //     }
    //     for(int i = 0; i < passengers; i++)
    //     {
    //         pthread_create(&train, NULL, fill_train, (void*)&ptr);
    //     }
    //     scanf("%d", &time);
    //     sleep(time);
    // }
    return 0;
}
