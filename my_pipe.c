/*
Name: Panagiotis Toloudis
Github: ptoloudis
Modified on: 28 October 2021 14:18:40
*/

#include <stdio.h>
#include <stdlib.h>
#include "my_pipe.h"

struct my_pipe *arr;
int many_pipe;

struct my_pipe{
    char *str_p;
    int size_p;
    int valid_p; // 1 is valid_p, 0 is not valid_p
    int write_p; // 0 to not write_p, 1 to write_p, 2 is close 
    int output_p;
    int input_p; 
};

void pipe_init(){
    arr = NULL;
    many_pipe = 0;
}

void pipe_free(){
    for (size_t i = 0; i < many_pipe; i++){
        if (arr[i].str_p != NULL){
            free(arr[i].str_p);
        }    
    }
    free(arr);    
}

int pipe_open(int size_p){
    int i,j;

    i = many_pipe;
    for (j = 0; j < i; j++){
        if (arr[j].valid_p == 0){
            break;
        }
    }
    if ((i == 0) || (arr[j].valid_p != 0)){
        many_pipe++;
        arr = (struct my_pipe *) realloc (arr,many_pipe * sizeof(struct my_pipe));
        i = j;
    }
    
    
    if (arr == NULL){
        perror("malloc str_puct\n");
        exit(1);
    }
    arr[i].str_p = (char *) malloc (size_p *sizeof(char));
    if (arr[i].str_p == NULL){
        perror("malloc array\n");
        exit(1);
    }
    arr[i].size_p =  size_p;
    arr[i].valid_p = 1;
    arr[i].output_p= 0;
    arr[i].input_p = 0;
    arr[i].write_p = 0;
    return i;
}

int pipe_write(int p, char c){
    if (arr[p].write_p == 2){
        return -1;
    }
    
    arr[p].write_p = 1;
    int input_p = arr[p].input_p;
    int output_p = arr[p].output_p;
    int size_p = arr[p].size_p;
    while ((input_p == size_p) && (output_p == 0)){ } ///alagi
    
    if (input_p == size_p){
        arr[p].input_p = 0;
    }
    
    arr[p].str_p[input_p] = c;
    arr[p].input_p++;
    arr[p].write_p = 0;

    return 1;
}

int pipe_writeDone(int p){
    if (arr[p].write_p == 2){
        return -1;
    }
    arr[p].write_p = 2;
    return 1;
}

int pipe_read(int p, char *c){
    if (arr[p].valid_p == 0)
        return -1;
    
    if (arr[p].input_p == arr[p].output_p){
        if (arr[p].write_p == 2)
            return 0;
    }
    
    while ((arr[p].input_p == arr[p].output_p)){ }
    
    if (arr[p].output_p == arr[p].size_p){
        arr[p].output_p = 0;
    }
    
    int out = arr[p].output_p;
    *c = arr[p].str_p[out];
    arr[p].output_p++;
    return 1;   
}

void pipe_close(int p){
    free(arr[p].str_p);
    arr[p].str_p = NULL;
    arr[p].valid_p =0;
}