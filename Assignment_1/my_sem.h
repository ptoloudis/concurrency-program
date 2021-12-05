/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#ifndef _HEADER_H
#define _HEADER_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

typedef struct mysem
{
    int semid;
    int initialized; //If semaphore is initialized
    int sem_num; //Amount of semaphores in group
    int sem_op;  //Operation of semaphore is up or down
    // int sem_flag;
}mysem_t;

int mysem_init(mysem_t *s, int n);
int mysem_down(mysem_t *s);
int mysem_up(mysem_t *s);
int mysem_destroy(mysem_t *s);

#endif