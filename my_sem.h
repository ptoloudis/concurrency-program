#ifndef _HEADER_H
#define _HEADER_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

typedef struct my_sem
{
    struct ipc_perm sem_perm;  /* Ownership and permissions */
    time_t          sem_otime; /* Last semop time */
    time_t          sem_ctime; /* Last change time */
    unsigned long   sem_nsems; /* No. of semaphores in set */
}mysem_t;


int mysem_init(mysem_t *s, int n);
int mysem_down(mysem_t *s);
int mysem_up(mysem_t *s);
int mysem_destroy(mysem_t *s);

#endif