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
    mysem_t mutex; 
}bridge_t;

void arriving_cars(bridge_t *ptr)
{
    mysem_down(&ptr->mutex);
    if(ptr->color == red)
    {
        if(ptr->cars_on_bridge < ptr->capacity)
        {
            ptr->cars_on_bridge ++;
            while((ptr->red_waiting > 0) && (ptr->cars_on_bridge < ptr->capacity))
            {
                ptr->red_waiting --;
                ptr->cars_on_bridge ++;
                mysem_up(&ptr->mysem_array[0]);
            }
            printf("Red Cars arriving on the bridge.\n");
            mysem_up(&ptr->mutex);
        }
        else if(ptr->cars_on_bridge == ptr->capacity)
        {
            ptr->red_waiting ++;
            printf("Red Cars on the Bridge reached max capacity.\n");
            mysem_up(&ptr->mutex);
            mysem_down(&ptr->mysem_array[0]);
        }
        else
        {
            mysem_up(&ptr->mutex);
            return;
        }
    }
    else if(ptr->color == blue)
    {
        if(ptr->cars_on_bridge < ptr->capacity)
        {
            ptr->cars_on_bridge ++;
            while((ptr->blue_waiting > 0) && (ptr->cars_on_bridge < ptr->capacity))
            {
                ptr->blue_waiting --;
                ptr->cars_on_bridge ++;
                mysem_up(&ptr->mysem_array[1]);
            }
            printf("Blue Cars arriving on the bridge.\n");
            mysem_up(&ptr->mutex);
        }
        else if(ptr->cars_on_bridge >= ptr->capacity)
        {
            ptr->blue_waiting ++;
            printf("Blue Cars on the Bridge reached max capacity.\n");
            mysem_up(&ptr->mutex);
            mysem_down(&ptr->mysem_array[1]);
        }
        else
        {
            mysem_up(&ptr->mutex);
            return;
        }
    }
    return;
}

void leaving_cars(bridge_t *ptr)
{
    mysem_down(&ptr->mutex);
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
                mysem_up(&ptr->mutex);
            }
            return;
        }
        else if(ptr->cars_on_bridge == 1)
        {
            printf("Last Car on Bridge is Leaving.\n");
            ptr->cars_on_bridge --;
            mysem_up(&ptr->mutex);
            return;
        }
    }
    else if(ptr->color == blue)
    {
        mysem_down(&ptr->mysem_array[1]);
        if(ptr->cars_on_bridge == ptr->capacity)
        {
            printf("Blue Cars are leaving the Bridge.\n");
            ptr->cars_on_bridge --;
            if(ptr->blue_waiting > 0)
            {
                ptr->blue_waiting --;
                ptr->cars_on_bridge ++;
                mysem_up(&ptr->mutex);
                mysem_up(&ptr->mysem_array[1]);
            }
            else
            {
                mysem_up(&ptr->mutex);
            }
            return;
        }
        else if(ptr->cars_on_bridge == 1)
        {
            printf("Last Car on Bridge is Leaving.\n");
            ptr->cars_on_bridge --;
            mysem_up(&ptr->mutex);
            return;
        }   
    }
    return;
}
void *Red_Cars(void *argument)
{
    bridge_t *ptr;

    ptr = (bridge_t *)argument;

    arriving_cars(ptr);
    sleep(2);
    leaving_cars(ptr);

    return NULL;
}

void *Blue_Cars(void *argument)
{
    bridge_t *ptr;

    ptr = (bridge_t *)argument;

    arriving_cars(ptr);
    sleep(2);
    leaving_cars(ptr);

    return NULL;
}

int main(int argc, char *argv[])
{
    bridge_t *current;
    int num_of_sem, semid, capacity, cars;
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
    
    // Create Threads    
    while(1)
    {
        scanf("%d %u %d", &cars, &current->color, &current->time);
        if(cars < 0)
        {
            break;
        }
        if (current->color == red)
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
        
        sleep(current->time);
    }
    return 0;
}
