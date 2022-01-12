#ifndef HEADER_H
#define HEADER_H

#include <ucontext.h>

enum state {
    READY = 1,
    RUNNING = 2,
    BLOCKED = 0,
    FINISHED = -1 
};

typedef struct thread{

    int id;
    ucontext_t *thr;
    enum state state;

}thr_t;

typedef struct task{

    thr_t *thread;
    struct task *next;
}task_t;

typedef struct scheduler{

    task_t *head;
    task_t *tail;
    task_t *curr;
    int size;
}scheduler_t;

int mythreads_init();
int mythreads_create(thr_t *thread, void(body)(void *), void *arguments);
int mythreads_yield();
int mythreads_join(thr_t *thread);
int mythreads_destroy(thr_t *thread);
int mythreads_tuple_out(char *fmt,...);
int mythreads_tuple_in(char *fmt,...);

#endif