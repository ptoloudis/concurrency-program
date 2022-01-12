#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include "header.h"

#define DEFAULT_STACK_SIZE 64000
#define DEFAULT_ARGUMENTS 0x1


int id;
thr_t  *nxt ;
scheduler_t *scheduler;

int mythreads_init(){

    id = 0;
    scheduler = (scheduler_t *) malloc(sizeof(scheduler_t));
    if (scheduler == NULL) {
        return -1;
    }
    scheduler->head = NULL;
    scheduler->tail = NULL;
    scheduler->curr = NULL;
    scheduler->size = 0; 
    return 0;
}

int mytreads_create(thr_t *thread, void(body)(void *), void *arguments){

    getcontext(&(thread->thr));
    thread->thr.uc_stack.ss_sp = malloc(DEFAULT_STACK_SIZE);
    thread->thr.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    thread->thr.uc_stack.ss_flags = 0;
    thread->thr.uc_link = 0;
    thread->state = READY;
    thread->id = id;
    id++;
    if(scheduler->head == NULL){
        scheduler->head = thread;
        scheduler->tail = thread;
        scheduler->curr = thread;
        scheduler->size = 1;
    }
    else{
        scheduler->tail->next = thread;
        scheduler->tail = thread;
        scheduler->size++;
    }

    makecontext(&(thread->thr), (void *)body, 1, arguments);
    return 0;
}

int mythreads_join(thr_t *thread){

    while(thread->state != FINISHED){
      mythreads_yield();
    }
    
    return 0;
}


int mythreads_destroy(thr_t *thread){

    free(thread->thr.uc_stack.ss_sp);
    return 0;

}

int mythreads_tuple_out(char *fmt, ...){

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return 0;
}