#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <my_sem.h>

int mysem_init(mysem_t *s, int *n)
{
    int semid;
    int i;

    // Create Semaphore to Initialize
    semid = semget(IPC_PRIVATE, s, S_IRWXU);
    // Semaphore Already Exists
    if(semid == -1)
    {
        perror("ERROR: No semaphore created");
        exit(EXIT_FAILURE);
    }

    // Initialize all Semaphores that have semid
    for(i = 0; i < s; i++)
    {
        if(semctl(semid, i, SETVAL, n[i] != 0))
        {
            perror("ERROR: Semaphore Initialization not Done");
            exit(EXIT_FAILURE);
        }
    }
    return semid;
}