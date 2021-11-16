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
#include "my_sem.h"

#define SEM_MAX 0x01 //Binary

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}argument;

int mysem_init(mysem_t *s, int n)
{
    // Check if n is valid
    if (n < 0  || n > 1){
        printf("Semaphore number must be 0 or 1\n");
        return 0;
    }

    // Check  if semaphore is already initialized
    if (s->initialized == 1)
    {
        printf("Semaphore already initialized\n");
        return 0;
    }    
    
    //int i = 0;
    // // Create Semaphore to Initialize
    // s->semid = semget(IPC_PRIVATE, s->sem_num, S_IRWXU);
    // // Semaphore Already Exists
    // if(s->semid == -1)
    // {
    //     perror("ERROR: No semaphore created");
    //     exit(EXIT_FAILURE);
    // }

    // Initialize all Semaphores that have semid
    if(semctl(s->semid, s->sem_num, SETVAL, n)){
        perror("ERROR: Semaphore not initialized");
        exit(EXIT_FAILURE);
    }

    s->initialized = 1;
    
    return 1;
}

int mysem_down(mysem_t *s)
{
    // Check  if semaphore is not initialized
    if(s->initialized == 0)
    {
        return -1;
    }

    // Ιnitialize semaphore operation
    struct sembuf operation;
    int result;
    operation.sem_op = -1;
    operation.sem_flg = 0;
    operation.sem_num = s->sem_num;

    // Perform semaphore operation
    result = semop(s->semid, &operation, 1);
    if(result == -1)
    {
        perror("ERROR : Semaphore is not Decrement");
        exit(EXIT_FAILURE);
    }
    
    s->sem_op = -1;
    return 1;
}

int mysem_up(mysem_t *s)
{
    // Check  if semaphore is not initialized
    if(s->initialized == 0)
    {
        return -1;
    }

    // If semaphore is already incremented
    if(s->sem_op == 1){
        return 0;
    }

    // Ιnitialize semaphore operation
    struct sembuf operation;
    int result;
    operation.sem_op = 1;
    operation.sem_flg = 0;
    operation.sem_num = s->sem_num;

    // Perform semaphore operation
    result = semop(s->semid, &operation, 1);
    if(result == -1)
    {
        perror("ERROR : Semaphore is not Increment");
        exit(EXIT_FAILURE);
    }
    
    s->sem_op = 1;
    return 1;
}

int mysem_destroy(mysem_t *s)
{
    // Check  if semaphore is not initialized
    if(s->initialized == 0)
    {
        return -1;
    }

    // Destroy semaphore
    int result;   
    result = semctl(s->semid, s->sem_num, IPC_RMID);
    if(result == -1)
    {
        perror("ERROR : Semaphore not Destroyed");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}