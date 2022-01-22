/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ucontext.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "header.h"

#define DEFAULT_STACK_SIZE 64000
#define DEFAULT_ARGUMENTS 0x1
#define CLK_PERIOD 2


/*******************Scheduler Functions********************/
ucontext_t terminate, join;
sigset_t sig_set;
scheduler_t *scheduler_FIFO;

static int signal_scheduler();

//Create a CPU Clock
void create_timer(){
  struct itimerval timer = { {0} };
  
  //Timer Configuration
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 500000;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 500000;
  
  setitimer(ITIMER_REAL, &timer, NULL);
}

//Block Incoming Signals
void block_sigalarm(){  
  sigemptyset(&sig_set);
  sigaddset(&sig_set, SIGALRM);
  if(sigprocmask(SIG_BLOCK, &sig_set, NULL) < 0){
    perror("Block Signal");
    exit(-1);
  }
}

//Unblock Signals
void unblock_sigalarm(){ 
  if(sigprocmask(SIG_UNBLOCK, &sig_set, NULL) < 0){
    perror("Unblock Signal");
    exit(-1);
  }
}

// Terminate the last context and go to main
static void terminate_context(){
  getcontext(&terminate);
  
  if(scheduler_FIFO->size >= 2){
    scheduler_FIFO->index->thread->state = FINISHED;
    scheduler_FIFO->size--;
    setcontext(scheduler_FIFO->head->thread->thr);  //Transfer Control Back to main()
  }
}

// Input to the scheduler
static bool scheduler_enqueue(thr_t *thread){
  task_t *task = (task_t *) malloc(sizeof(task_t));
  if(!task)
    return false;
  
  //Assign Pointers
  if(!scheduler_FIFO->head || !scheduler_FIFO->tail){
    scheduler_FIFO->head = scheduler_FIFO->tail = scheduler_FIFO->index = task;
  }
  
  task->next = scheduler_FIFO->head;
  scheduler_FIFO->tail->next = task;
  scheduler_FIFO->tail = task;
  
  //Initialize Data
  task->thread = thread;
  
  scheduler_FIFO->size++;

  return true;
}

// Make the swap to the next context
static void scheduler(int signum){
  //Signal Safe with Block and Unblock
  block_sigalarm();

  //Current Running Context
  scheduler_FIFO->curr = scheduler_FIFO->index;
  
  do {
    scheduler_FIFO->index = scheduler_FIFO->index->next;
  } while (scheduler_FIFO->index->thread->state != READY);
  
  printf("\033[0;33mSaving %d Running %d\033[0m\n", scheduler_FIFO->curr->thread->id, scheduler_FIFO->index->thread->id);
  

  if(scheduler_FIFO->size >= 2){
    swapcontext(scheduler_FIFO->curr->thread->thr, scheduler_FIFO->index->thread->thr);
  }
  unblock_sigalarm();
}

// Call the scheduler
static int signal_scheduler(){
  
  //Run Scheduler
  signal(SIGALRM, scheduler);
  if(kill(getpid(), SIGALRM) < 0)
    return -1;

  return 0;
}


/*******************Thread Functions********************/

int id;
thr_t  *nxt ;
scheduler_t *schedr;

//Initialize the Thread
int mythreads_init(){

    thr_t *main;
    struct sigaction act = {{ 0 }};

    //Use thread_count to Specify IDs
    thread_count = 0;

  //Create Scheduler
    scheduler_FIFO = (scheduler_t *) malloc(sizeof(scheduler_t));
    if(!scheduler_FIFO)
        return -1;

    scheduler_FIFO->head = NULL;
    scheduler_FIFO->tail = NULL;
    scheduler_FIFO->index = NULL;
  
    //Thread: main()
    main = (thr_t *) malloc(sizeof(thr_t));
    if(!main)
        return -1;
  
    main->id = thread_count++;
    main->state = READY;
    main->thr = (ucontext_t *) malloc(sizeof(ucontext_t));
 
    if(!scheduler_enqueue(main))
        return -1;
  
  //Listen for Context Termination
    terminate_context();


  //Activate Scheduler Timer
    act.sa_handler = scheduler;
    if(sigaction(SIGALRM, &act, NULL) < 0){
        perror("Timer Set Error");
        exit(-1);
    }

    //Activate Scheduler
    create_timer();

    id = 0;
    ts_init();
    schedr = (scheduler_t *) malloc(sizeof(scheduler_t));
    if (schedr == NULL) {
        return -1;
    }
    schedr->head = NULL;
    schedr->tail = NULL;
    schedr->curr = NULL;
    schedr->size = 0; 
    schedr->index = NULL;
    return 0;
}

//Create a Thread
int mythreads_create(thr_t *thread, void(body)(void *), void *arguments){

    //ucontext_t *tmp;
    thread->thr=(ucontext_t *) malloc (sizeof(ucontext_t));
    if (getcontext(thread->thr)) {
      return 0;
    }
    thread->thr->uc_stack.ss_sp = malloc(DEFAULT_STACK_SIZE);
    thread->thr->uc_stack.ss_size = DEFAULT_STACK_SIZE;
    thread->thr->uc_stack.ss_flags = 0;
    thread->thr->uc_link = 0;
    thread->state = READY;
    thread->id = id;
    id++;
    makecontext(thread->thr, (void *)body, 1, arguments);

     //Schedule Context as a Scheduler Job
    if(!scheduler_enqueue(thread))
        return -1;

    return 0;
}

//Yield Thread By Calling Scheduler To Assign The Next Job
int mythreads_yield(){
    block_sigalarm();
    printf("Thread Yield\n");
    if(signal_scheduler() < 0)
        return -1;
    unblock_sigalarm();
    return 0;
}

// Wait to Join a Thread
int mythreads_join(thr_t *thread){

    while(thread->state != FINISHED){ }
    
    return 0;
}

//Terminate the Thread and Free Memory
int mythreads_destroy(thr_t *thread){
    task_t *previous, *current;
  
    for(previous = scheduler_FIFO->head, current = scheduler_FIFO->head->next; current != scheduler_FIFO->head; previous = current, current = current->next){
        current = current->next;
        free(previous->next);
        previous->next = current;
    }
    unblock_sigalarm();
    return 0;

}

/*******************Tuple Space Function****************/

ts_list_t tuplespace[SIZEOF_TS];

// Create and initialize the hash table of tuplespace
void ts_init(){ 
  int i;
  for(i=0;i<SIZEOF_TS;i++){
    tuplespace[i].lock = F;
    tuplespace[i].anode = NULL;
  }
}

// Free the memory of a node
void free_node(inode_list_t *inode){ 
  inode_list_t *nxt;
  for(nxt=inode;nxt->next!=NULL;nxt=nxt->next){ // Free the memory of the node
    free(nxt);
  }
  free(nxt);
}

// Destroy the hash table of tuplespace
void ts_destroy(){ 
  int i;
  ts_node_t *temp;
  ts_node_t *temp2;

  for(i=0;i<SIZEOF_TS;i++){
    temp = tuplespace[i].anode;
    while(temp != NULL){
      temp2 = temp;
      free_node(temp2->inode);
      temp = temp->next;
      free(temp2);
    }
  }
}

/* In the match function, we check if the tuple mask is a match with the tuple in the tuplespace.
 * If the tuple mask is a match, we return 1, otherwise we return 0.
 * The match function is used in the in and out functions.
 * 
 * Check the type of the tuple mask and the tuple in the tuplespace.
 * Check the all posible cases of the type of the tuple mask and the tuple in the tuplespace.
*/ 

int match(inode_list_t *inode1, inode_list_t *inode2)
{
  inode_list_t *ptr1, *ptr2;

  if ((inode1 == NULL) || (inode2 == NULL)) return 0;

  for (ptr1 = inode1, ptr2 = inode2; ((ptr1 != NULL) && (ptr2 != NULL));
        ptr1 = ptr1->next, ptr2 = ptr2->next) {

    if (ptr1->field_tag == D_FIELD) {
        if (ptr2->field_tag == D_FIELD) {
          switch (ptr1->type_tag) {
            case CHAR_TYPE:
              if ((ptr2->type_tag != CHAR_TYPE) ||
                  (ptr1->node.c != ptr2->node.c)) return 0;
              break;
            case STR_TYPE:
              if ((ptr2->type_tag != STR_TYPE) ||
                  strcmp(ptr1->node.s, ptr2->node.s)) return 0;
              break;
            case INT_TYPE:
              if ((ptr2->type_tag != INT_TYPE) ||
                  (ptr1->node.i != ptr2->node.i)) return 0;
              break;
            case DOUBLE_TYPE:
              if ((ptr2->type_tag != DOUBLE_TYPE) ||
                  (ptr1->node.f != ptr2->node.f)) return 0;
              break;
            default:
              perror( "Shit!\n");
              exit(-1);
              break;
          }
        } else { /* ptr2->field_tag == Q_FIELD */
          switch (ptr1->type_tag) {
            case CHAR_TYPE:
              if (ptr2->type_tag != CHAR_TYPE) return 0;
              break;
            case STR_TYPE:
              if (ptr2->type_tag != STR_TYPE) return 0;
              break;
            case INT_TYPE:
              if (ptr2->type_tag != INT_TYPE) return 0;
              break;
            case DOUBLE_TYPE:
              if (ptr2->type_tag != DOUBLE_TYPE) return 0;
              break;
            default:
              perror( "Shit!\n");
              exit(-1);
              break;
          }
        }
    } else { /* Q_FIELD */
        if (ptr2->field_tag == D_FIELD) {
          switch (ptr1->type_tag) {
            case CHAR_TYPE:
              if (ptr2->type_tag != CHAR_TYPE) return 0;
              break;
            case STR_TYPE:
              if (ptr2->type_tag != STR_TYPE) return 0;
              break;
            case INT_TYPE:
              if (ptr2->type_tag != INT_TYPE) return 0;
              break;
            case DOUBLE_TYPE:
              if (ptr2->type_tag != DOUBLE_TYPE) return 0;
              break;
            default:
              perror( "Shit!\n");
              exit(-1);
              break;
          }
        } else {
          return 0; /* Q_FIELD shouldn't match another Q_FIELD */
        }
    }
  }

  if ((ptr1 == NULL) && (ptr2 == NULL)) return 1;
  else return 0;
}

/* In the bind val fuction we bind the value of the tuple mask to the tuple in the tuplespace.
 * The bind val function is used in the in and out functions.
 *
*/

void bind_val(inode_list_t *inode1, inode_list_t *inode2)
{
    inode_list_t *ptr1, *ptr2;

    if ((inode1 == NULL) || (inode2 == NULL)) return;

    for (ptr1 = inode1, ptr2 = inode2; (ptr1 != NULL) && (ptr2 != NULL);ptr1 = ptr1->next, ptr2 = ptr2->next) {

      if (ptr1->field_tag == D_FIELD) {
        if (ptr2->field_tag == Q_FIELD) {
          switch (ptr1->type_tag) {
          case CHAR_TYPE:
            *ptr2->node.cp = ptr1->node.c;
            break;
          case STR_TYPE:
            strcpy(ptr2->node.s, ptr1->node.s);
            break;
          case INT_TYPE:
            *ptr2->node.ip = ptr1->node.i;
            break;
          case DOUBLE_TYPE:
            *(ptr2->node.fp) = ptr1->node.f;
            break;
          default:
            perror( "Shit!\n");
            exit(-1);
            break;
          }
        } 
      } 
      else { /* Q_FIELD */
        if (ptr2->field_tag == D_FIELD) {
          switch (ptr1->type_tag) {
          case CHAR_TYPE:
            *ptr1->node.cp = ptr2->node.c;
            break;
          case STR_TYPE:
            strcpy(ptr1->node.s, ptr2->node.s);
            break;
          case INT_TYPE:
            *ptr1->node.ip = ptr2->node.i;
            break;
          case DOUBLE_TYPE:
            *ptr1->node.fp = ptr2->node.f;
            break;
          default:
            perror( "Shit!\n");
            exit(-1);
            break;
          }
        }
      }
    }
}

/* In the Out faction we check if the tuple mask is a match with the tuple in the tuplespace.
 * If the tuple mask is a match and the bind val, we remove the tuple from the tuplespace,
 * otherwise we do nothing.
 * 
*/

int mythreads_tuple_out (char *fsm, ...) 
{ 
  va_list tuple_list; 
  char *mask_ptr; 
  int len, hash, i;
  ts_node_t *tsnode, *tsptr;
  inode_list_t *inode , *node_ptr;


  len = (strlen(fsm) / 2);
  hash = 0;

  tsnode = (ts_node_t *)(malloc(sizeof( ts_node_t)));
  tsnode->type = OUT;
  tsnode->cv = F;
  tsnode->inode = NULL;
  tsnode->next = NULL;
  tsnode->prev = NULL;

  node_ptr = NULL;

  va_start(tuple_list, fsm); 

  for (mask_ptr = fsm, i = 1; *mask_ptr; mask_ptr+=2, i++) 
  { 
    if (*mask_ptr == '%') 
    {
      switch (mask_ptr[1]) {
        case 's': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.s = va_arg(tuple_list, char *); 
          inode->type_tag = STR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 3 * i;
          break;
        case 'd': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.i = va_arg(tuple_list, int); 
          inode->type_tag = INT_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 5 * i;
          break;
        case 'f': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.f = va_arg(tuple_list, double); 
          inode->type_tag = DOUBLE_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 7 * i;
          break;
        case 'c': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.c = (char) va_arg(tuple_list, int); 
          inode->type_tag = CHAR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 11 * i;
          break;
        default:
          perror( "Unknown type of an out.\n");
          exit(-1);
        }  
    } 
    else {
      if (*mask_ptr == '?') {
        perror( "Cannot do a '?' on a out!");
        exit(-1);
      }
    }
      
    if (tsnode->inode == NULL) {
      tsnode->inode = node_ptr = inode;
    } 
    else {
      node_ptr->next = inode;
      node_ptr = inode;
    }
    
  } 

  va_end(tuple_list); 

  /* Enter it into the tuple space */

  len = (len + hash) % SIZEOF_TS;
  while (tuplespace[len].lock == T){  }// Wait to enter the tuple space
  
  tuplespace[len].lock = T;
  
  for (tsptr = tuplespace[len].anode; tsptr != NULL; tsptr = tsptr->next) {

    if (match(tsnode->inode, tsptr->inode)) {
      if (tsptr->type == IN) {
        bind_val(tsnode->inode, tsptr->inode);
        tsptr->cv = T;
        tuplespace[len].lock = F;
        free_node(tsnode->inode);
        free(tsnode);
        return 0;
      }
    }
  }

  if (tuplespace[len].anode != NULL) {
    tuplespace[len].anode->prev = tsnode;
  }
  tsnode->next = tuplespace[len].anode;
  tuplespace[len].anode = tsnode;
  tuplespace[len].lock = F;
  return 0;
}

/* In the In faction we check if the tuple mask is a match with the tuple in the tuplespace.
 * If the tuple mask is a match and the bind val, we remove the tuple from the tuplespace,
 * Else wait to be notified.
 *
*/

int mythreads_tuple_in (char *fsm, ...)
{ 
  va_list tuple_list; 
  char *mask_ptr; 
  int len, hash, i;
  inode_list_t *inode, *node_ptr;
  ts_node_t *tsnode, *ptsptr, *tsptr;

  len = (strlen(fsm) / 2);
  hash = 0;
  
  tsnode = (ts_node_t *)(malloc(sizeof(ts_node_t)));
  tsnode->type = IN;
  tsnode->cv = F;
  tsnode->inode = NULL;
  tsnode->next = NULL;
  tsnode->prev = NULL;

  node_ptr = NULL;

  va_start(tuple_list, fsm); 

  for (mask_ptr = fsm, i = 1; *mask_ptr; mask_ptr+=2, i++) 
  { 
    if (*mask_ptr == '%') 
    {
      switch (mask_ptr[1]){ 
        case 's': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.s = va_arg(tuple_list, char *); 
          inode->type_tag = STR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 3 * i;
          break;
        case 'd': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.i = va_arg(tuple_list, int); 
          inode->type_tag = INT_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 5 * i;
          break;
        case 'f': 
          inode =  (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.f = va_arg(tuple_list, double); 
          inode->type_tag = DOUBLE_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 7 * i;
          break;
        case 'c': 
          inode =  (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.c = (char) va_arg(tuple_list, int); 
          inode->type_tag = CHAR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 11 * i;
          break;
        default:
          perror( "Unknown type of an out.\n");
          exit(-1);
        } 
    } else {
      if (*mask_ptr == '?') 
      {
        switch (mask_ptr[1]){ 
        case 's': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.s = va_arg(tuple_list, char *); 
          inode->type_tag = STR_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 3 * i;
          break;
        case 'd': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.ip = va_arg(tuple_list, int *); 
          inode->type_tag = INT_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 5 * i;
          break;
        case 'f': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.f = va_arg(tuple_list, double); 
          inode->type_tag = DOUBLE_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 7 * i;
          break;
        case 'c': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
         inode->node.c = (char) va_arg(tuple_list, int); 
          inode->type_tag = CHAR_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 11 * i;
          break;
        default:
          perror( "Unknown type of an in.\n");
          exit(-1);
        }
      } else {
          perror( "Don't know what it is - in ?!");
          exit(-1);
      }
    }

    if (tsnode->inode == NULL) {
      tsnode->inode = node_ptr = inode;
    } else {
      node_ptr->next = inode;
      node_ptr = inode;
    }
  } 

  va_end(tuple_list); 

  /* Attempt to find it in the tuple space */

  len = (len + hash) % SIZEOF_TS;
  tuplespace[len].lock = T;
  
  ptsptr = NULL;

  for (tsptr = tuplespace[len].anode; tsptr != NULL; tsptr = tsptr->next) {

    if (match(tsnode->inode, tsptr->inode)) {
      if (tsptr->type == OUT) {
        bind_val(tsnode->inode, tsptr->inode);

        /* Delink the out tuple */
        if (ptsptr == NULL) {
          tuplespace[len].anode = tsptr->next;
        } else {
          ptsptr->next = tsptr->next;
        }

        tuplespace[len].lock = F;
        free_node(tsnode->inode);
        free_node(tsptr->inode);
        free(tsptr);
        free(tsnode);

        return 2;
      } 
    }

    ptsptr = tsptr;
  }

  if (tuplespace[len].anode != NULL) {
    tuplespace[len].anode->prev = tsnode;
  }
  tsnode->next = tuplespace[len].anode;
  tuplespace[len].anode = tsnode;
  
  tuplespace[len].lock = F;
  while (tsnode->cv == F){}
  while (tuplespace[len].lock == T){}
  tuplespace[len].lock = T;
  
  /* Delink the out tuple */
  if (tsnode->prev == NULL) {
    tuplespace[len].anode = tsnode->next;
    if (tsnode->next != NULL) {
      tsnode->next->prev = NULL;
    }
  } else {
    tsnode->prev->next = tsnode->next;
  }
  free_node(tsnode->inode);
  free(tsnode);
  tuplespace[len].lock = F;
  return 1;
}