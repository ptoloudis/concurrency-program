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

enum gender_t {female, male};

typedef struct WC{
    int capacity;  // Total Capacity on the Bridge per Passing Time
    enum gender_t gender;
    int women_waiting; //Amount of Red Cars Waiting
    int men_waiting; // Amount of Blue Cars Waiting
    int people_in_WC; // Amount of Cars on the Bridge
    int people_crossed; // Amount of Cars Crossed the Bridge 
    int turn; // Which Car should cross the Bridge Red = 1 Blue = 2 Restart Counting = 0
}WC_t;

WC_t *current;
volatile int exit_flag;

pthread_mutex_t mutex;
pthread_cond_t women_arriving, men_arriving, exit_mtx;


void arriving_people(enum gender_t gender)
{
    pthread_mutex_lock(&mutex);

    // Red Cars are Arriving but it's not their Turn to Cross
    if(gender == female)
    {
        current->women_waiting ++;
        if(((current->people_in_WC >= current->capacity) || current->turn == 2) || current->men_waiting != 0)
        {
            printf(Red "Women: \n" Clear);
            printf("Red Cars on the Bridge reached max capacity.\n");
            pthread_cond_wait(&women_arriving, &mutex);
        }

        current->turn = 1;
        current->women_waiting --;
        current->people_in_WC ++;

        // Red Cars should Cross
        if((current->women_waiting > 0) && (current->people_in_WC < current->capacity))
        {
            pthread_cond_signal(&women_arriving);
        }

        printf(Red "Women: \n" Clear);
        printf("Red Cars arriving on the bridge.\n");
    }
    // Blue Cars Arriving but it's not their Turn to Cross
    else 
    {
        current->men_waiting ++;
        if((current->people_in_WC >= current->capacity) || current->turn == 1 || current->women_waiting != 0)
        {
            printf(Blue "Men: \n" Clear);
            printf("Blue Cars on the Bridge reached max capacity.\n");
            pthread_cond_wait(&men_arriving, &mutex);
        }

        current->turn = 2;
        current->men_waiting --;
        current->people_in_WC ++;

        // Blue Cars should Cross
        if((current->men_waiting > 0) && (current->people_in_WC < current->capacity))
        {
            pthread_cond_signal(&men_arriving);
        }
        printf(Blue "Men: \n" Clear);
        printf("Blue Cars arriving on the bridge.\n");
        
    }

    pthread_mutex_unlock(&mutex);
    
    return;
}

void leaving_people(enum gender_t gender)
{
    pthread_mutex_lock(&mutex);
    if(exit_flag == 1 && current->men_waiting == 0 && current->women_waiting == 0 && current->people_in_WC == 1)
    {
        pthread_cond_signal(&exit_mtx);
    }
    
    if(gender == female)
    {
        printf(Red "Women: \n" Clear);
        printf("Red Cars are leaving the Bridge.\n");
        current->people_in_WC --;
        current->people_crossed ++;
        if(current->people_in_WC == 0)
        {
            printf("Last Car on Bridge is Leaving.\n");
            if((current->men_waiting != 0) || (current->people_crossed == 2 * current->capacity))
            {
                current->turn = 2;
                pthread_cond_signal(&men_arriving);
            }
            current->turn = 0;
            current->people_crossed = 0;
            pthread_mutex_unlock(&mutex);
            return;
        } 
        
        if (current->people_in_WC < current->capacity)
        {
            pthread_cond_signal(&women_arriving);
        }
        
        pthread_mutex_unlock(&mutex);
        sleep(1);
        return;
        
    }
    else 
    {
        printf(Blue "Men: \n" Clear);
        printf("Blue Cars are leaving the Bridge.\n");
        current->people_in_WC --;
        current->people_crossed ++;
        if(current->people_in_WC == 0)
        {
            printf("Last Car on Bridge is Leaving.\n");
            if((current->women_waiting != 0) || (current->people_crossed == 2 * current->capacity))
            {
                current->turn = 1;
                pthread_cond_signal(&women_arriving);
            }
            current->turn = 0;
            current->people_crossed = 0;
            pthread_mutex_unlock(&mutex);
            return;
        } 
    
        if (current->people_in_WC < current->capacity )
        {
            pthread_cond_signal(&men_arriving);
        }
    
        pthread_mutex_unlock(&mutex);
        sleep(1);
        return;
    }
}

void *Women(void *argument)
{
    arriving_people(female);
    sleep(time_in_bridge);
    leaving_people(female);

    return NULL;
}

void *Men(void *argument)
{
    arriving_people(male);
    sleep(time_in_bridge);
    leaving_people(male);

    return NULL;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    char c;
    int capacity, people, time;
    int i;
    pthread_t WC;

    if(argc != 2)
    {
        perror("ERROR : Not defiend amount of Capacity for the Bridge.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        capacity = atoi(argv[1]);
    }

    current = (WC_t *) malloc(sizeof(WC_t));
    if(current == NULL)
    {
        perror("No Malloc done to Array Semaphore");
        exit(EXIT_FAILURE);
    }
    current->capacity = capacity;

    // Initialize Conditions
    pthread_cond_init(&women_arriving, NULL);
    pthread_cond_init(&men_arriving, NULL);
    pthread_cond_init(&exit_mtx, NULL);
    pthread_mutex_init(&mutex, NULL);

    // Initialize Struct's Values
    current->men_waiting = 0;
    current->women_waiting = 0;
    current->people_in_WC = 0;
    current->people_crossed = 0;
    current->turn = 0;
    exit_flag = 0;
    time = 0;
    
    // Create Threads    
    while(1)
    {
        scanf("%d %c %d", &people, &c, &time);
        pthread_mutex_lock(&mutex);
        if(people < 0)
        {
            exit_flag = 1;
            if (current->people_in_WC == 0 && current->men_waiting == 0 && current->women_waiting == 0)
            {
                pthread_mutex_unlock(&mutex);
                break;
            }            
            pthread_cond_wait(&exit_mtx, &mutex);
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        if (c == 'f')
        {
            for(i = 0; i < people; i++)
            {
                pthread_create(&WC, NULL, Women, (void*)&current);
            } 
        }
        else
        {
            for(i = 0; i < people; i++)
            {
                pthread_create(&WC, NULL, Men, (void*)&current);
            } 
        } 
        sleep(time);
    }

    sleep(1);

    // Destroy Monitors
    pthread_cond_destroy(&women_arriving);
    pthread_cond_destroy(&men_arriving);
    pthread_cond_destroy(&exit_mtx);
    pthread_mutex_destroy(&mutex);

    // Free Allocated Memory 
    free(current);
    return 0;
}
