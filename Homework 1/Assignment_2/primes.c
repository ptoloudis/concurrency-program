/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

int *for_worker;
int *status;
/*  0 = Worker is being Created
    1 = Available for Usage
    2 = Unavailable for Usage
    3 = To be closed
    4 = Closed
*/

void * prime_number(void *argument)
{
    int number, worker, i, j, flag, count=0;
    
    worker = *(int *)argument;
    status[worker] = 1;

    printf("Worker %d\n",worker); // Print the Threads we use as Workers 

    while (status[worker] != 3)
    {                               //  Loop to Wait until Program Exits
        if (status[worker] == 2)
        {
            flag = 1;
            number = for_worker[worker];

            // Iterate from 2 to sqrt(n)
            for (i = 2; i <= sqrt(number); i++)
            {
                // If the Number is Divisible by any Number between 2 and n/2, it is not Prime
                if (number % i == 0)
                {
                    flag = 0;
                    break;
                }
            }

            if (number <= 1)
            {
                flag = 0; 
            }

            if (flag)
            {
                printf("%d: 1\n", number);
                count++;
            }     
            else
            {
                printf("%d: 0\n", number);
                count++;
            } 
            
            status[worker] = 1;
        }   
        
             
    }
    
    for (j = 0; j < 1000000; j++){} // Wait until all are printed 
    printf("%d\n Seeeeeeee meee\n", count);
    status[worker] = 4;   // Exit

    return 0;
}

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{
    pthread_t test;
    int n, i, j, number, flag;

    n = atoi(argv[1]);

    // Malloc for the all worker status and "table"
    status = (int *) malloc(n * sizeof(int));
    if (status == NULL)
    {
        perror("No Malloc done to Status");
        exit(1);
    }
    for ( i = 0; i < n; i++)
    {
        status[i]=0;
    }

    for_worker = (int *) malloc((2 *n) * sizeof(int));

    if (for_worker == NULL)
    {
        perror("No malloc done to Pipe_worker");
        exit(1);
    }

    // Creation of the Worker and Delay until All Workers are Ready to Work
    for ( i = 0; i < n; i++)
    {
        pthread_create(&test,NULL,prime_number,(void *) &i);
        for ( j = 0; j < 20000000; j++){} 
    }

    j=0;

    while(j != n)
    {
        for (i = 0; i < n; i++)
        {
            if(status[i]== 1)
            {
                j++;
            }
        }
    }

    while (1)
    {
        flag = 1;
        scanf("%d",&number);
        if (number == -1)
            break;
        
        while (flag == 1)
        {
            for (i = 0; i < n; i++)
            {
                if(status[i]== 1)
                {
                    for_worker[i] = number;
                    status[i] = 2;
                    flag = 0;
                    break;
                }
            }  
        }
    }
    
    // Notify Workers to Close and Wait all the Workers to be Closed
    for ( i = 0; i < n; i++)
    {
        status[i]=3;
    }

    while(j != n)
    {
        j=0;
        for (i = 0; i < n; i++)
        {
            if(status[i]== 4)
            {
                j++;
            }
        }
    }
    
    for(i = 0; i < 1000000000; i++){}

    // Free all Workers
    free(for_worker);
    free(status);

    return 0;
}