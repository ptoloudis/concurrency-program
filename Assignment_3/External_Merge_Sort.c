/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 
// Number of Elements in Array or File
#define MAX 64
 
// number of threads
#define THREAD_MAX 4
 
// Array with Elements of Size MAX
int array[MAX];
int *status;

FILE *open_file(char *file_name, char *mode)
{
    FILE *fp;
    
    fp = fopen(file_name, mode);
    if(fp == NULL)
    {
        perror("File does not exist");
        exit(0);
    }
    return fp;
}

// Function for Merging 2 Parts
void merge(int low, int mid, int high)
{
    int left[mid - low + 1];
    int right[high - mid];
 
    // left_size is size of left part and right_size is size of right part
    int left_size = mid - low + 1;
    int right_size = high - mid;
    int i, j, k;

    // Storing Values in Left Part
    for (i = 0; i < left_size; i++)
    {
        left[i] = array[i + low];

    }
 
    // Storing Values in Right Part
    for (i = 0; i < right_size; i++)
    {
        right[i] = array[i + mid + 1];
    }
 
    k = low;
    i = 0; 
    j = 0;
 
    // Merge left and right in ascending order
    while ((i < left_size) && (j < right_size))
    {
        if (left[i] <= right[j])
            array[k++] = left[i++];
        else
            array[k++] = right[j++];
    }
 
    // Insert remaining values from left
    while (i < left_size) {
        array[k++] = left[i++];
    }
 
    // Insert remaining values from right
    while (j < right_size) {
        array[k++] = right[j++];
    }
}
 
// Merge sort function
void Merge_Sort(int low, int high)
{
    // Calculating mid point of array
    int mid = low + (high - low) / 2;
    if (low < high) {
 
        // Calling first half
        Merge_Sort(low, mid);
 
        // Calling second half
        Merge_Sort(mid + 1, high);
 
        // Merging the two halves
        merge(low, mid, high);
    }
}
 
// Thread function for multi-threading
void* merge_sort(void* argument)
{
    int thread_part = *(int*)argument;
    printf("Thread %d is running\n", thread_part);
 
    // Calculating low and high
    int low = thread_part * (MAX / 4);
    int high = (thread_part + 1) * (MAX / 4) - 1;
 
    // Evaluating mid point
    int mid = low + (high - low) / 2;
    if (low < high) {
        Merge_Sort(low, mid);
        Merge_Sort((mid + 1), high);
        merge(low, mid, high);
    }

    status[thread_part] = 1;
    for (size_t i = 0; i < 10000000; i++){ }
    
    return 0;
}
 

/*********************** MAIN ***********************/
int main(int argc, char *argv[])
{   
    FILE *fp;
    int i = 0;

    fp = open_file(argv[1], "r");

    while (fscanf(fp, "%d", &array[i]) != EOF)
    {
        i++;
    }
    fclose(fp);
    
    pthread_t threads[THREAD_MAX];
     
    status = (int*)malloc(sizeof(int) * THREAD_MAX);
    for ( i = 0; i < THREAD_MAX; i++)
    {
        status[i] = 0;
    }

    // Creating 4 Threads
    for (int i = 0; i < THREAD_MAX; i++)
    {
        printf("Starting Thread %d\n",i);

        pthread_create(&threads[i], NULL, merge_sort, &i);

        for (size_t j = 0; j < 100000; j++){}
    }
        
 
    while (1)
    {
        int flag = 0;
        for (i = 0; i < THREAD_MAX; i++)
        {
            if (status[i] == 1)
            {
                flag++;
            }
        }
        if (flag == 4)
        {
            break;
        }
    }
    
    // Merging the final 4 parts
    merge(0, (MAX / 2 - 1) / 2, MAX / 2 - 1);
    merge(MAX / 2, MAX/2 + (MAX-1-MAX/2)/2, MAX - 1);
    merge(0, (MAX - 1)/2, MAX - 1);

 
    // Displaying sorted array
    fp = open_file("output.txt", "w+");

    for ( i = 0; i < MAX; i++)
    {
        fprintf(fp, "%d\n", array[i]);
    }

    fclose(fp);
    
    free(status);
    
    return 0;
}