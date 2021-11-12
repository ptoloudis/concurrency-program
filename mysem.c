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
    int i = 0;

    // Create Semaphore to Initialize
    s->semid = semget(IPC_PRIVATE, s->sem_num, S_IRWXU);
    // Semaphore Already Exists
    if(s->semid == -1)
    {
        perror("ERROR: No semaphore created");
        exit(EXIT_FAILURE);
    }

    // Initialize all Semaphores that have semid
    for(i = 0; i < s->sem_num; i++)
    {
        if(semctl(s->semid, i, SETVAL, 1))
        {
            perror("ERROR: Semaphore Initialization not Done");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

int mysem_down(mysem_t *s)
{
    struct sembuf operation;
    int i, result;

    if(s->initialized == 0)
    {
        return -1;
    }

    operation.sem_op = -1;
    operation.sem_flg = 0;

    for(i = 0; i < s->sem_num; i++)
    {
        operation.sem_num = i;
        result = semop(s->semid, &operation, 1);
        if(result == -1)
        {
            perror("ERROR : Semaphore is not Decrement");
            exit(EXIT_FAILURE);
        }
    }
    s->sem_op = 0;
    return 1;
}


int mysem_up(mysem_t *s)
{
    struct sembuf operation;
    int i, result;

    if(s->initialized == 0)
    {
        return -1;
    }

    if(s->sem_op == 1){
        return 0;
    }

    operation.sem_op = 1;
    operation.sem_flg = 0;

    for(i = 0; i < s->sem_num; i++)
    {
        operation.sem_num = i;
        result = semop(s->semid, &operation, 1);
        if(result == -1)
        {
            perror("ERROR : Semaphore is not Increment");
            exit(EXIT_FAILURE);
        }
    }
    s->sem_op = 1;
    return 1;
}


int mysem_destroy(mysem_t *s)
{
    int result, i;

    if(s->initialized == 0)
    {
        return -1;
    }

    for(i = 0; i < s->sem_num; i++)
    {
        result = semctl(s->semid, i, IPC_RMID);
        if(result == -1)
        {
            perror("ERROR : Semaphore not Destroyed");
            exit(EXIT_FAILURE);
        }
    }
    return 1;
}