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
    task_t *index;
    int size;
}scheduler_t;

// tuple 

#define SIZEOF_TS 100

#define D_FIELD 0 // data field
#define Q_FIELD 1 // query field

#define CHAR_TYPE 0
#define INT_TYPE 1
#define STR_TYPE 2
#define DOUBLE_TYPE 3

typedef enum { F, T } boolean;
typedef enum { IN, OUT } type_q;

typedef union INODE {
    char c, *cp;
    int i, *ip;
    char *s;
    double f, *fp;
}inode_t;

typedef struct INODE_LIST {
    int type_tag, field_tag;
    inode_t node;
    struct INODE_LIST *next;
}inode_list_t;

typedef struct TS_NODE {
    type_q type;
    boolean cv;  
    inode_list_t *inode;
    struct TS_NODE *next;
    struct TS_NODE *prev;
}ts_node_t;

typedef struct TS_LIST {
    boolean lock;
    ts_node_t *anode;
    struct TS_LIST *next;
}ts_list_t; 
 
 //************** Global Variables **************//
scheduler_t *scheduler_FIFO;
int thread_count, semaphore_count;
ucontext_t scheduler_context;

 /// function prototypes

int mythreads_init();
int mythreads_create(thr_t *thread, void(body)(void *), void *arguments);
int mythreads_yield();
int mythreads_join(thr_t *thread);
int mythreads_destroy(thr_t *thread);
void ts_init (void);
void ts_destroy (void);
int mythreads_tuple_out(char *fmt,...);
int mythreads_tuple_in(char *fmt,...);

#endif