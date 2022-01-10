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

pthread_mutex_t mutex;
pthread_cond_t red_arriving, blue_arriving, exit_mtx;


void arriving_cars(enum color_t color)
{
    pthread_mutex_lock(&mutex);

    // Red Cars are Arriving but it's not their Turn to Cross
    if(color == red)
    {
        current->red_waiting ++;
        if(((current->cars_on_bridge >= current->capacity) || current->turn == 2) || current->blue_waiting != 0)
        {
            printf(Red "Red Cars: \n" Clear);
            printf("Red Cars on the Bridge reached max capacity.\n");
            pthread_cond_wait(&red_arriving, &mutex);
        }

        current->turn = 1;
        current->red_waiting --;
        current->cars_on_bridge ++;

        // Red Cars should Cross
        if((current->red_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            pthread_cond_signal(&red_arriving);
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
            pthread_cond_wait(&blue_arriving, &mutex);
        }

        current->turn = 2;
        current->blue_waiting --;
        current->cars_on_bridge ++;

        // Blue Cars should Cross
        if((current->blue_waiting > 0) && (current->cars_on_bridge < current->capacity))
        {
            pthread_cond_signal(&blue_arriving);
        }
        printf(Blue "Blue Cars: \n" Clear);
        printf("Blue Cars arriving on the bridge.\n");
        
    }

    pthread_mutex_unlock(&mutex);
    
    return;
}

void leaving_cars(enum color_t color)
{
    pthread_mutex_lock(&mutex);
    if(exit_flag == 1 && current->blue_waiting == 0 && current->red_waiting == 0 && current->cars_on_bridge == 1)
    {
        pthread_cond_signal(&exit_mtx);
    }
    
    if(color == red)
    {
        printf(Red "Blue Cars: \n" Clear);
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
            pthread_mutex_unlock(&mutex);
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

    // Initialize Conditions
    pthread_cond_init(&red_arriving, NULL);
    pthread_cond_init(&blue_arriving, NULL);
    pthread_cond_init(&exit_mtx, NULL);
    pthread_mutex_init(&mutex, NULL);

    // Initialize Struct's Values
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
        pthread_mutex_lock(&mutex);
        if(cars < 0)
        {
            exit_flag = 1;
            if (current->cars_on_bridge == 0 && current->blue_waiting == 0 && current->red_waiting == 0)
            {
                pthread_mutex_unlock(&mutex);
                break;
            }            
            pthread_cond_wait(&exit_mtx, &mutex);
            pthread_mutex_unlock(&mutex);
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

    sleep(1);

    // Destroy Monitors
    pthread_cond_destroy(&red_arriving);
    pthread_cond_destroy(&blue_arriving);
    pthread_cond_destroy(&exit_mtx);
    pthread_mutex_destroy(&mutex);

    // Free Allocated Memory 
    free(current);
    return 0;
}
