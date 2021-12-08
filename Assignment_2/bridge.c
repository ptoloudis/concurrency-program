/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

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
}bridge_t;

pthread_mutex_t mutex;
pthread_cond_t red_arriving, blue_arriving, leaving;
bridge_t *current;
volatile int exit_flag;

void arriving_cars(enum color_t color2)
{
   pthread_mutex_lock(&mutex);
    if(color2 == red)
    {
        current->red_waiting ++;
        if(((current->cars_on_bridge >= current->capacity) || current->turn == 2) || current->blue_waiting != 0)
        {
            printf("Red Cars on the Bridge reached max capacity.\n");
            pthread_cond_wait(&red_arriving, &mutex);
        }

        current->turn = 1;
        current->red_waiting --;
        current->cars_on_bridge ++;
        if((current->red_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            pthread_cond_signal(&red_arriving);
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
            pthread_cond_wait(&blue_arriving, &mutex);
        }

        current->turn = 2;
        current->blue_waiting --;
        current->cars_on_bridge ++;
        if((current->blue_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            pthread_cond_signal(&blue_arriving);
        }
        // printf(Blue "Blue Cars arriving on the bridge.\n" Clear);
        printf("Blue Cars arriving on the bridge.\n");
        
    }
   pthread_mutex_unlock(&mutex);
    return;
}

void leaving_cars(enum color_t color2)
{
   pthread_mutex_lock(&mutex);
    if(exit_flag == 1 && current->blue_waiting == 0 && current->red_waiting == 0 && current->cars_on_bridge == 1)
    {
        pthread_cond_signal(&leaving);
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
                pthread_cond_signal(&blue_arriving);
            }
            current->turn = 0;
            current->cars_crossed = 0;
            
            return;
        } 
        
        if (current->cars_on_bridge < current->capacity)
        {
            pthread_cond_signal(&red_arriving);
        }
        
       pthread_mutex_unlock(&mutex);
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
                pthread_cond_signal(&red_arriving);
            }
            current->turn = 0;
            current->cars_crossed = 0;
           pthread_mutex_unlock(&mutex);
            return;
        } 
    
        if (current->cars_on_bridge < current->capacity )
        {
            pthread_cond_signal(&blue_arriving);
        }
    
       pthread_mutex_unlock(&mutex);
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
    int capacity, cars, time;
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

    // Initialize cond
    pthread_cond_init(&red_arriving, NULL);
    pthread_cond_init(&blue_arriving, NULL);
    pthread_cond_init(&leaving, NULL);
    pthread_mutex_init(&mutex, NULL);

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
       pthread_mutex_lock(&mutex);
        scanf("%d %c %d", &cars, &c, &time);
        if(cars < 0)
        {
            exit_flag = 1;
            pthread_cond_wait(&leaving, &mutex);
            break;
        }
       pthread_mutex_unlock(&mutex);
        if (c == 'r')
        {
            for(i = 0; i < cars; i++)
            {
                pthread_create(&bridge, NULL, Red_Cars, (void*)&current);
            } 
        }
        else
        {
            for(i = 0; i < cars; i++)
            {
                pthread_create(&bridge, NULL, Blue_Cars, (void*)&current);
            } 
        } 
        
        sleep(time);
    }

    sleep(10);
    // Destroy 
    pthread_cond_destroy(&red_arriving);
    pthread_cond_destroy(&blue_arriving);
    pthread_cond_destroy(&leaving);
    pthread_mutex_destroy(&mutex);
    free(current);
    return 0;
}
