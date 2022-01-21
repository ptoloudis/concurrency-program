/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

#define time_in_bridge 2
#define Red "\033[1;31m"
#define Blue "\033[1;34m"
#define Clear "\033[0m"

enum color_t {red, blue};

typedef struct bridge{
    int capacity;  // Total Capacity on the Bridge per Passing Time
    enum color_t color;
    int red_waiting; //Amount of Red Cars Waiting
    int blue_waiting; // Amount of Blue Cars Waiting
    int cars_on_bridge; // Amount of Cars on the Bridge
    int cars_crossed; // Amount of Cars Crossed the Bridge 
    int turn; // Which Car should cross the Bridge Red = 1 Blue = 2 Restart Counting = 0
}bridge_t;

bridge_t *current;
volatile int exit_flag;

void arriving_cars(enum color_t color)
{
    mythreads_tuple_in("%s","mutex");

    // Red Cars are Arriving but it's not their Turn to Cross
    if(color == red)
    {
        current->red_waiting ++;
        if(((current->cars_on_bridge >= current->capacity) || current->turn == 2) || current->blue_waiting != 0)
        {
            printf(Red "Red Cars: \n" Clear);
            printf("Red Cars on the Bridge reached max capacity.\n");
            mythreads_tuple_out("%s","mutex");
            mythreads_tuple_in("%s","red_waiting");
            mythreads_tuple_in("%s","mutex");
        }

        current->turn = 1;
        current->red_waiting --;
        current->cars_on_bridge ++;

        // Red Cars should Cross
        if((current->red_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            mythreads_tuple_out("%s","red_waiting");
        }

        printf(Red "Red Cars: \n" Clear);
        printf("Red Cars arriving on the bridge.\n");
    }
    // Blue Cars Arriving but it's not their Turn to Cross
    else 
    {
        current->blue_waiting ++;
        if((current->cars_on_bridge >= current->capacity) || current->turn == 1 || current->red_waiting != 0)
        {
            printf(Blue "Blue Cars: \n" Clear);
            printf("Blue Cars on the Bridge reached max capacity.\n");
            mythreads_tuple_out("%s","mutex");
            mythreads_tuple_in("%s","blue_waiting");
            mythreads_tuple_in("%s","mutex");
        }

        current->turn = 2;
        current->blue_waiting --;
        current->cars_on_bridge ++;

        // Blue Cars should Cross
        if((current->blue_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
              mythreads_tuple_out("%s","blue_waiting");
        }
        printf(Blue "Blue Cars: \n" Clear);
        printf("Blue Cars arriving on the bridge.\n");
        
    }

    mythreads_tuple_out("%s","mutex");
    
    return;
}

void leaving_cars(enum color_t color)
{
    mythreads_tuple_in("%s","mutex");
    if(exit_flag == 1 && current->blue_waiting == 0 && current->red_waiting == 0 && current->cars_on_bridge == 1)
    {
        mythreads_tuple_out("%s","exit_mtx");
    }   
    
    if(color == red)
    {
        printf(Red "Red Cars: \n" Clear);
        printf("Red Cars are leaving the Bridge.\n");
        current->cars_on_bridge --;
        current->cars_crossed ++;
        if(current->cars_on_bridge == 0)
        {
            printf("Last Car on Bridge is Leaving.\n");
            if((current->blue_waiting != 0) || (current->cars_crossed == 2 * current->capacity))
            {
                current->turn = 2;
                mythreads_tuple_out("%s","blue_waiting");
            }
            current->turn = 0;
            current->cars_crossed = 0;
            mythreads_tuple_out("%s","mutex");
            return;
        } 
        
        if (current->cars_on_bridge < current->capacity)
        {
            mythreads_tuple_out("%s","red_waiting");
        }
        
        mythreads_tuple_out("%s","mutex");
        sleep(1);
        return;
        
    }
    else 
    {
        printf(Blue "Blue Cars: \n" Clear);
        printf("Blue Cars are leaving the Bridge.\n");
        current->cars_on_bridge --;
        current->cars_crossed ++;
        if(current->cars_on_bridge == 0)
        {
            printf("Last Car on Bridge is Leaving.\n");
            if((current->red_waiting != 0) || (current->cars_crossed == 2 * current->capacity))
            {
                current->turn = 1;
                mythreads_tuple_out("%s","red_waiting");
            }
            current->turn = 0;
            current->cars_crossed = 0;
            mythreads_tuple_out("%s","mutex");
            return;
        } 
    
        if (current->cars_on_bridge < current->capacity )
        {
            mythreads_tuple_out("%s","blue_waiting");
        }
    
        mythreads_tuple_out("%s","mutex");
        sleep(1);
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

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    char c;
    int capacity, cars, time;
    int i, j;
    thr_t *bridge;

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

    mythreads_init();
    mythreads_tuple_out("%s","mutex");
    mythreads_tuple_out("%s","read");
    

    // Initialize Struct's Values
    current->blue_waiting = 0;
    current->red_waiting = 0;
    current->cars_on_bridge = 0;
    current->cars_crossed = 0;
    current->turn = 0;
    exit_flag = 0;
    time = 0;
    j = 0;
    bridge = (thr_t *) malloc(100*sizeof(thr_t) );
    
    // Create Threads    
    while(1)
    {
        mythreads_tuple_in("%s","read");
        scanf("%d %c %d", &cars, &c, &time);
        printf("%d %c %d\n", cars, c, time);
        mythreads_tuple_in("%s","mutex");
        if(cars < 0)
        {
            printf("EXIT FLAG\n");
            exit_flag = 1;
            if (current->cars_on_bridge == 0 && current->blue_waiting == 0 && current->red_waiting == 0)
            {
                mythreads_tuple_out("%s","mutex");
                break;
            }            
            mythreads_tuple_out("%s","mutex");
            mythreads_tuple_in("%s","exit_mtx");
            break;
        }
        mythreads_tuple_out("%s","mutex");

        if (c == 'r')
        {
            for(i = 0; i < cars; i++)
            {
                mythreads_create(&bridge[j+i], (void*)&Red_Cars, NULL);
            } 
        }
        else
        {
            for(i = 0; i < cars; i++)
            {
                mythreads_create(&bridge[j+i], (void*)&Blue_Cars, NULL);
            } 
        } 
        printf("all in\n");
        j = j + cars;
        sleep(time);
        mythreads_tuple_out("%s","read");
    }

    perror("hi");
    // Free Allocated Memory 
    free(current);
    for ( i = 0; i < j; i++)
    {
        mythreads_join(&bridge[i]);
        mythreads_destroy(&bridge[i]);
    }

    sleep(1);

    return 1;
}
