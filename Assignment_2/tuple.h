#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
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

/* Function prototypes */
void ts_init (void);
void ts_destroy (void);
int out (char *tuple_mask, ...); 
int in (char *tuple_mask, ...); 